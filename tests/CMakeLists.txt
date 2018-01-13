set(CMAKE_CXX_STANDARD 11)

add_executable(test_nndescent test_nndescent.cpp)
target_link_libraries(test_nndescent ${PROJECT_NAME})

add_executable(test_nndescent_refine test_nndescent_refine.cpp)
target_link_libraries(test_nndescent_refine ${PROJECT_NAME})

add_executable(test_kdtree_graph test_kdtree_graph.cpp)
target_link_libraries(test_kdtree_graph ${PROJECT_NAME})

add_executable(test_faiss_graph test_faiss_graph.cpp)
target_link_libraries(test_faiss_graph ${PROJECT_NAME} faiss ${BLAS_LIB})
