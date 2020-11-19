# cmake version to be used
cmake_minimum_required( VERSION 3.19 )

# project name
project( web_client )

# flags

# files

# include
include_directories( ./include ./src )

# target
add_executable( web_client ./src/main.cpp )