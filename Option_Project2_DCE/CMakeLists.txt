cmake_minimum_required (VERSION 3.0)

project (DEAD_CODE_ELIMINATION)

find_package(LLVM REQUIRED CONFIG)

execute_process (COMMAND llvm-config --cxxflags --ldflags OUTPUT_VARIABLE LLVM_CONFIG_N)

include_directories(${LLVM_INCLUDE_DIRS})

string (REPLACE "\n" " " LLVM_CONFIG "${LLVM_CONFIG_N}")

set (CMAKE_CXX_FLAGS "-O3 -g ${LLVM_CONFIG}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")


include_directories (include)
add_library (RangeAnalysis SHARED
RangeAnalysis/RangeAnalysis.cpp
RangeAnalysis/vSSA.cpp
RangeAnalysis/ClientRA.cpp
)



