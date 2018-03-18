#!/bin/bash

projectRootDir=$(pwd)

function init_env()
{
    #remove old directory
    rm -rf ${projectRootDir}/3rdParty/gtest-1.8.0/build
    mkdir -p ${projectRootDir}/3rdParty/gtest-1.8.0/build
    rm -rf ${projectRootDir}/eclipse_ut_project
    mkdir -p ${projectRootDir}/eclipse_ut_project
}

function process_build()
{   
   #./build gtest 
    cd  ${projectRootDir}/3rdParty/gtest-1.8.0/build
    cmake ..
    make

    cd ${projectRootDir}/eclipse_ut_project
    cmake ${projectRootDir}/unit_test/src  -DCMAKE_BUILD_TYPE=Debug -G"Eclipse CDT4 - Unix Makefiles" -D_ECLIPSE_VERSION=4.6.3  
    make
    cd ${projectRootDir}
}

init_env
process_build

