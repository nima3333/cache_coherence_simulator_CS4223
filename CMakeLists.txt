cmake_minimum_required(VERSION 3.17)
project(multicore)

set(CMAKE_CXX_STANDARD 17)

add_executable(multicore main.cpp Bus.cpp Bus.h BusMessage.cpp BusMessage.h)