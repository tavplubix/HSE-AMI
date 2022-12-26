set(CMAKE_VERBOSE_MAKEFILE 1)
cmake_minimum_required(VERSION 3.8)
project(PDC)

#set(CMAKE_CXX_STANDARD 14)
#find_package(Threads REQUIRED)
#
#set(SOURCE_FILES phils.cpp)
#add_executable(Phils ${SOURCE_FILES})
#target_link_libraries(Phils Threads::Threads)
#
#find_package(OpenMP)
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()
#
#add_executable(Datagen datagen.cpp)
#add_executable(KMeans kmeans.cpp)


find_package(CUDA QUIET REQUIRED)
set(CUDAHOSTCXX /usr/bin/gcc-4.8)
set(
        CUDA_NVCC_FLAGS
        "--compiler-bindir /usr/bin/gcc-4.8; --compiler-options --std=c++11"
)
set(CUDA_PROPAGATE_HOST_FLAGS OFF)
set(CMAKE_CUDA_STANDARD 98)
cuda_add_executable(CUDA_MatMul matmul.cu)
set_property(TARGET CUDA_MatMul PROPERTY CUDA_STANDARD 98)




