cmake_minimum_required(VERSION 3.13)

project(server)

set(CXX_STANDARD_REQUIRED 20)

add_executable(scan_service scan.cpp server.cpp main.cpp)