#!/bin/bash

projectRootDir=$(pwd)

function init_env()
{
    #remove old directory
    rm -rf ${projectRootDir}/3rdParty/gtest-1.8.0/build
    mkdir -p ${projectRootDir}/3rdParty/gtest-1.8.0/build
    rm -rf ${projectRootDir}/ut_build
    mkdir -p ${projectRootDir}/ut_build
}

function process_build()
{   
   #./build gtest 
    cd  ${projectRootDir}/3rdParty/gtest-1.8.0/build
    cmake ..
    make

    cd ${projectRootDir}/ut_build
    cmake ${projectRootDir}/unit_test/src
    make
    cd ${projectRootDir}
}

init_env
process_build

