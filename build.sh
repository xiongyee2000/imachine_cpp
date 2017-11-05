#!/bin/bash

set -e

script_dir=$(cd `dirname $0`; pwd)

RELEASE_BUILD=1
CMAKE_OPTIONS=""

# --- usage ---
function usage()
{
cat <<- EOT
    Usage: $0 [options]

    Options:
        -r|release       release build (default)
        -d|debug         debug build
        -h|help          usage help

    Example:
        ./build.sh -d
EOT
}
# --- end of usage() ---

#---------------------------------#
#  Handle command line arguments  #
#---------------------------------#
while getopts "rdh" opt
do
    case $opt in
        r|release)
            RELEASE_BUILD=1
            ;;

        d|debug)
            RELEASE_BUILD=0
            ;;

        h|help)
            usage
            exit 0
            ;;

        \?)
            usage
            exit 1
            ;;

    esac    # --- end of case ---
done

if [[ $RELEASE_BUILD -eq 1 ]]; then
    CMAKE_OPTIONS="$CMAKE_OPTIONS: -DCMAKE_BUILD_TYPE=Release"
else
    CMAKE_OPTIONS="$CMAKE_OPTIONS: -DCMAKE_BUILD_TYPE=Debug"
fi

echo "==== Starting build ===="

rm -rf $script_dir/build
rm -rf $script_dir/dist
mkdir -p $script_dir/build
mkdir -p $script_dir/dist

dist_dir=$script_dir/dist
mkdir -p $dist_dir
mkdir -p $dist_dir/include
mkdir -p $dist_dir/lib
mkdir -p $dist_dir/bin

cd build
cmake $CMAKE_OPTIONS ../src
make -j

cp -v ./fsme/src/libimachine-dyn.so $dist_dir/lib/libimachine.so
cp -v ./fsme/src/libimachine-static.a $dist_dir/lib/libimachine.a
cp -v ../src/fsme/header/fsme.h $dist_dir/include
cp -v ./example/imachine_example $dist_dir/bin

cd ..

echo "==== Build finished successfully ===="
