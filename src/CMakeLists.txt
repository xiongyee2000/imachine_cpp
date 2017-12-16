cmake_minimum_required(VERSION 2.6)

add_definitions(-std=c++11)

project(imachine)
#set global option. path of executable file
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#set global option. path of library 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(org/jinsha/imachine)
add_subdirectory(examples)