# cmake version to be used
cmake_minimum_required( VERSION 3.19 )

# project name
project( web_server )

# flags

# files

# include
include_directories( ./include ./src )

# target
add_executable( web_server ./src/main.cpp )