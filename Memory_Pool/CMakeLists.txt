cmake_minimum_required(VERSION 3.25)
project(Memory_Pool)

set(CMAKE_CXX_STANDARD 17)

add_executable(Memory_Pool main.cpp MemoryPool.hpp StackAlloc.hpp)
