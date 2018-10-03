EFANNA\_graph: an Extremely Fast Approximate Nearest Neighbor graph construction Algorithm framework 
============
EFANNA is a ***flexible*** and ***efficient*** library for approximate nearest neighbor search (ANN search) on large scale data. It implements the algorithms of our paper [EFANNA : Extremely Fast Approximate Nearest Neighbor Search Algorithm Based on kNN Graph](http://arxiv.org/abs/1609.07228).    
This project (efanna_graph) contains only the ***approximate nearest neighbor graph construction*** part in our [EFANNA](http://arxiv.org/abs/1609.07228) paper. 
The reasons are as follows:
* Some advanced graph based ANN search algorithms (e.g., [HNSW](https://github.com/searchivarius/nmslib), [NSG](https://github.com/ZJULearning/nsg)) make search with Efanna almost meaningless.
* But the approximate kNN graph construction part in Efanna is still interesting and important.
* The idea of kNN graph construction in Efanna is very simple. It just combines KD-tree and nndescent. The KD-tree part can be repleced by other kNN graph initilization algorithms. To make the algorithm framework more flexible, we decompose the nndescent part and KD-tree initilization part.
* We also add an example to use Faiss to build an initial graph and use nndescent to refine the graph.


How To Complie    
-------
Case 1: If the dataset is very large (more than 4M points), you may want to use faiss to build a graph for initilization. Then you need to downlad the faiss package and compile with it. 

    cd efanna_graph/extern_libraries/
    git clone https://github.com/facebookresearch/faiss.git
    cd faiss/

  Please see the documentation of [faiss](https://github.com/facebookresearch/faiss) for how to compile the faiss.


Case 2: If you don't need the faiss for initilization, you can compile efanna_graph without the faiss support.

    cd efanna_graph/
    rm include/efanna2e/index_pq.h src/index_pq.cpp
    comment the last two lines in tests/CMakeLists.txt

Then go to the root directory of efanna_graph and make.    

	cd efanna_graph/
	cmake .
    make

How To Use    
------

* kNN graph building with nndescent:    

		cd tests/   
		./test_nndescent data_file save_graph K L iter S R

 Meaning of the parameters:    

    **data_file** is the path of the origin data.
    **save_graph** is the path of the kNN graph to be saved.
    **K** is the 'K' of kNN graph.
    **L** is the parameter controlling the graph quality, larger is more accurate but slower, no smaller than K.
    **iter** is the parameter controlling the iteration times, iter usually < 30.
    **S** is the parameter contollling the graph quality, larger is more accurate but slower.
    **R** is the parameter controlling the graph quality, larger is more accurate but slower.


* kNN graph initilization with KD-tree and refine by nndescent (efanna):    

		cd tests/   
        ./test_kdtree_graph data_file nTrees mLevel K saving_graph
        ./test_nndescent_refine data_file init_graph save_graph K L iter S R

 Meaning of the parameters:    

    **nTrees** is the number of trees used to build the graph (larger is more accurate but slower)
    **mLevel** conquer-to-depeth (smaller is more accurate but slower) 
    **K** is the 'K' of kNN graph.
    **saveing_graph** is the path of the kNN graph to be saved.
    **init_graph** is the graph built by test_kdtree_graph, same with **saveing\_graph** here.

	
* kNN graph initilization with faiss and refine by nndescent:    

		cd tests/   
        ./test_faiss_graph data_file IndexKey SearchKey K saving_graph
        ./test_nndescent_refine data_file init_graph save_graph K L iter S R

 Meaning of the parameters(from left to right):    

    **IndexKey** is the parameter in faiss to build the index.
    **SearchKey** is the parameter in faiss to search with the index.

   Please see the documentation of [faiss](https://github.com/facebookresearch/faiss) for how to set these two parameters.



Output format
------
Suppose the database has N points, and numbered from 0 to N-1. You want to build an approximate kNN graph. The graph can be regarded as a N * k Matrix. The saved kNN graph binary file saves the matrix by row. The first 4 bytes of each row saves the int value of k, next 4 bytes saves the value of M and next 4 bytes saves the float value of the norm of the point. Then it follows k*4 bytes, saving the indices of the k nearest neighbors of respective point. The N rows are saved continuously without seperating characters.   

Input of EFANNA
------
Because there is no unified format for input data, users may need to write input function to read your own data. You may imitate the input function in our sample code (sample/efanna\_efanna\_index\_buildgraph.cc) to load the data into our matrix.
To use SIMD instruction optimization, you should pay attention to the data alignment problem of SSE / AVX instruction.  

Benchmark data set
------
* [SIFT1M and GIST1M](http://corpus-texmex.irisa.fr/)

        ./test_nndescent gist_base.fvecs gist.100NN.graph 100 120 10 15 100

    With the above command, one can build a 100NN graph on GIST1M with around 96% accuracy in 960 seconds on a i7-4790K cpu with 8 threads.

        ./test_nndescent sift_base.fvecs sift.50NN.graph 50 70 8 10 100

    With the above command, one can build a 50NN graph on SIFT1M with around 90% accuracy in 72 seconds on a i7-4790K cpu with 8 threads.

        ./test_nndescent sift_base.fvecs sift.50NN.graph 50 70 10 10 50

    With the above command, one can build a 50NN graph on SIFT1M with around 97% accuracy in 106 seconds on a i7-4790K cpu with 8 threads.

Acknowledgment
------
The implemnetation of nndescent is taken from [Kgraph](http://www.kgraph.org). They proposed the nndescent algorithm. Many thanks to them for inspiration.

