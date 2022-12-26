cmake_minimum_required(VERSION 3.10)
project(hw4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
add_executable(hw4 main.cpp)

find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
target_link_libraries(hw4 ${MPI_CXX_LIBRARIES})

FIND_PACKAGE(Boost 1.47.0 COMPONENTS mpi serialization REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(hw4 ${Boost_LIBRARIES})
