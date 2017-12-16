cmake_minimum_required(VERSION 2.6)

set (SOURCE_FILES )
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#add_library(imachine STATIC ${SOURCE_FILES})
add_library(imachine SHARED ${SOURCE_FILES})
