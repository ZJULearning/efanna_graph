cmake_minimum_required(VERSION 2.8)

project(efanna2e)
include_directories(${PROJECT_SOURCE_DIR}/include)
#OpenMP
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(FATAL_ERROR "no OpenMP supprot")
endif()

#OpenBLAS
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/module)
find_package(OpenBLAS REQUIRED)
include_directories(${OpenBLAS_INCLUDE_DIR})
set(BLAS_LIB ${OpenBLAS_LIB})

#external libraries like faiss
include_directories(${PROJECT_SOURCE_DIR}/extern_libraries)
link_directories(${PROJECT_SOURCE_DIR}/extern_libraries/faiss/lib)

add_definitions (-std=c++11 -O3 -lboost -march=native -Wall -DINFO)

add_subdirectory(src)
add_subdirectory(tests)
