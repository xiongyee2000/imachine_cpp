cmake_minimum_required(VERSION 2.8)
project(imachine_ut)

#option of build mode
option(DEBUG_mode "ON for debug or OFF for release" ON)

#set global option. path of executable file
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

#set global option. path of library 
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(GTEST_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/gtest-1.8.0/googletest/include)
set(GMOCK_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/gtest-1.8.0/googlemock/include)
set(GMOCK_LIB_PATH     ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/gtest-1.8.0/build/googlemock)
set(GTEST_LIB_PATH     ${CMAKE_CURRENT_SOURCE_DIR}/../../3rdParty/gtest-1.8.0/build/googlemock/gtest)

add_definitions(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")

set(src_dir )
aux_source_directory(. src_dir)

add_definitions(-DMOCK_UT)

include_directories(${GTEST_INCLUDE_PATH})
include_directories(${GMOCK_INCLUDE_PATH})

link_directories(
	${GTEST_LIB_PATH}
	${GMOCK_LIB_PATH}
) 

add_executable(imachine_ut  ${src_dir} 
)


target_link_libraries(imachine_ut
libgtest.a
libgtest_main.a
libgmock.a
libgmock_main.a
pthread
gcov
)
