//
// Created by 付聪 on 2017/6/26.
//
#include <efanna2e/index_pq.h>
#include <efanna2e/exceptions.h>
#include <efanna2e/parameters.h>
#include <faiss/index_io.h>
#include <faiss/index_factory.h>
#include <omp.h>

namespace efanna2e {
IndexPQ::IndexPQ(const size_t dimension, const size_t n, Metric m, Index *initializer):Index(dimension, n, m),
     initializer_{initializer}{}


IndexPQ::~IndexPQ(){}

void IndexPQ::Save(const char *filename){
  faiss::write_index(index, filename);
}
void IndexPQ::Load(const char *filename){
  index = faiss::read_index(filename);
}

void IndexPQ::compute_gt_for_tune(const float* q,
                         const unsigned nq,
                        const unsigned k,
                         unsigned *gt){
#pragma omp parallel for
  for(unsigned i=0; i<nq; i++){
    std::vector<Neighbor> res;
    for(unsigned j=0; j<nd_; j++){
      float dist = distance_->compare(q + i * dimension_, data_ + j * dimension_, dimension_);
      res.push_back(Neighbor(j, dist, true));
    }
    std::partial_sort(res.begin(), res.begin()+k, res.end());
    for(unsigned j=0; j<k; j++){
      gt[i * k + j] = res[j].id;
    }
  }

}

void IndexPQ::Build(size_t n, const float *data, const Parameters &parameters) {
  const std::string pq_index_key = parameters.Get<std::string>("pq_index_key");
  data_ = data;
  index = faiss::index_factory(dimension_, pq_index_key.c_str());
  index->train(nd_, data_);
  index->add(nd_, data_);

  unsigned sample_num = 100;
  float* sample_queries = new float[dimension_ * sample_num];
  std::vector<unsigned> tmp(sample_num);
  std::mt19937 rng;
  GenRandom(rng, tmp.data(), sample_num, nd_);

  for(unsigned i=0; i<tmp.size(); i++){
    unsigned id = tmp[i];
    memcpy(sample_queries + i * dimension_, data_ + id * dimension_, dimension_ * sizeof(float));
  }

  unsigned k = 10;
  faiss::Index::idx_t *gt = new faiss::Index::idx_t[k * sample_num];//ground truth
  unsigned * gt_c = new unsigned[k * sample_num];
  compute_gt_for_tune(sample_queries, sample_num, k, gt_c);
  for(unsigned i=0; i<k*sample_num; i++){
    gt[i] = gt_c[i];
  }
  delete [] gt_c;
  std::string selected_params;

  faiss::IntersectionCriterion crit(sample_num, k);
  crit.set_groundtruth (k, nullptr, gt);
  crit.nnn = k; // by default, the criterion will request only 1 NN

  std::cout<<"Preparing auto-tune parameters\n";

  faiss::ParameterSpace params;
  params.initialize(index);
  faiss::OperatingPoints ops;
  params.explore (index, sample_num, sample_queries, crit, &ops);
  for (size_t i = 0; i < ops.optimal_pts.size(); i++) {
    if (ops.optimal_pts[i].perf > 0.5) {
      selected_params = ops.optimal_pts[i].key;
      break;
    }
  }

  std::cout<<"best parameters auto-tuned: "<<selected_params<<std::endl;
  has_built = true;
}

void IndexPQ::Search(
    const float *query,
    const float *x,
    size_t k,
    const Parameters &parameters,
    unsigned *indices) {
  std::string search_key = parameters.Get<std::string>("pq_search_key");
  faiss::ParameterSpace f_params;
  f_params.set_index_parameters(index, search_key.c_str());

  faiss::Index::idx_t *Ids = new faiss::Index::idx_t[k];
  float *Dists = new float[k];

  index->search(1, query, k, Dists, Ids);
  for(unsigned i=0; i<k; i++){
    indices[i] = Ids[i];
  }
}
}
