cmake_minimum_required(VERSION 3.2)

# Project name and version
project(fuzzy_ner VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(${CMAKE_SOURCE_DIR}/edlib/edlib/include)
link_directories(${CMAKE_SOURCE_DIR}/edlib/build/lib)

# Use the OpenMP package
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# Build a shared library
add_library(fuzzy_ner_lib SHARED fuzzy_ner.cpp)
target_link_libraries(fuzzy_ner_lib PRIVATE -ledlib)

add_subdirectory(pybind11)
pybind11_add_module(fuzzy_ner fuzzy_ner_wrapper.cpp)
target_link_libraries(fuzzy_ner PUBLIC fuzzy_ner_lib)



