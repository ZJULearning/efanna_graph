//
// Created by 付聪 on 2017/6/26.
//

#ifndef EFANNA2E_INDEX_PQ_H
#define EFANNA2E_INDEX_PQ_H
#include <faiss/AutoTune.h>
#include "util.h"
#include "parameters.h"
#include "neighbor.h"
#include "index.h"
#include <cassert>

namespace efanna2e {
class IndexPQ : public Index {
 public:
  explicit IndexPQ(const size_t dimension, const size_t n, Metric m, Index *initializer);


  virtual ~IndexPQ();

  virtual void Save(const char *filename)override;
  virtual void Load(const char *filename)override;


  virtual void Build(size_t n, const float *data, const Parameters &parameters) override;

  virtual void Search(
      const float *query,
      const float *x,
      size_t k,
      const Parameters &parameters,
      unsigned *indices) override;

 protected:

  Index *initializer_;
  faiss::Index* index;
  void compute_gt_for_tune(const float* q,
                           const unsigned nq,
                          const unsigned k,
                           unsigned *gt);
};
}


#endif //EFANNA2E_INDEX_PQ_H
