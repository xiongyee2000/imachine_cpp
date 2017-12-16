cmake_minimum_required(VERSION 2.6)

set (SOURCE_FILES )
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../../)
target_include_directories(imachine PUBLIC
../../lib
)

add_executable(atm ${SOURCE_FILES})
target_link_libraries(atm imachine)
