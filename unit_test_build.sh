#!/bin/bash

projectRootDir=$(pwd)

function init_env()
{
    #remove old directory
    rm -rf ${projectRootDir}/3rdParty/gtest-1.8.0/build
    mkdir -p ${projectRootDir}/3rdParty/gtest-1.8.0/build
    rm -rf ${projectRootDir}/unit_test_build
    mkdir -p ${projectRootDir}/unit_test_build
    #mkdir ${projectRootDir}/unit_test_build/bin
}

function process_build()
{   
   #./build gtest 
    cd  ${projectRootDir}/3rdParty/gtest-1.8.0/build
    cmake ..
    make

    cd ${projectRootDir}/unit_test_build
    cmake ${projectRootDir}/unit_test/src
    make
    cd ${projectRootDir}
}

init_env
process_build

