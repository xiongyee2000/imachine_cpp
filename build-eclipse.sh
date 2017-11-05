#!/bin/bash

set -e

script_dir=$(cd `dirname $0`; pwd)

echo "==== Generating Eclipse project files ===="

rm -rf $script_dir/eclipse_project
mkdir -p $script_dir/eclipse_project

cd eclipse_project
cmake -DCMAKE_BUILD_TYPE=Debug -G"Eclipse CDT4 - Unix Makefiles" -D_ECLIPSE_VERSION=4.6.3 ../src
#make -j

cd ..

echo "==== Eclipse project files generated successfully in eclipse_project directory ===="
