# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.1...3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(MergeSort CXX)

include(common.cmake)
add_definitions(-D_PARALLEL_SORT_)
set_common_project_settings(tbb)
add_executable (MergeSort "src/mergesort.cpp") 
target_link_libraries(MergeSort  TBB::tbb Threads::Threads)
