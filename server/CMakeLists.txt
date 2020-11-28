# cmake version to be used
cmake_minimum_required( VERSION 3.19 )

# project name
project( web_server )
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
find_package (Threads)
# flags

# files

# include
include_directories( ./include ./src )

# target
add_executable( web_server ./src/main.cpp )
target_link_libraries (web_server ${CMAKE_THREAD_LIBS_INIT})