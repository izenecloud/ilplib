#!/bin/bash

# build path
PROJECT_HOME="`dirname $0`/.."
BUILD_PATH="${PROJECT_HOME}/build/temp"

# build type
BUILD_TYPE=release

if [ "$1" = "clean" ]
then
if test -d $BUILD_PATH
then rm -r $BUILD_PATH
fi

# remove all lib files
rm -fr $PROJECT_HOME/lib/lib*

# remove all executable files
ls $PROJECT_HOME/bin/langid_* | grep -v langid_build_model.sh | xargs rm -fr
rm -fr $PROJECT_HOME/bin/test_* $PROJECT_HOME/bin/demo-server
elif [ "$1" = "" ] || [ "$1" = "debug" ] || [ "$1" = "release" ] || [ "$1" = "profile" ]
then
if [ "$1" != "" ]
then
BUILD_TYPE=$1
fi

echo "build type:" $BUILD_TYPE

if [ -d $BUILD_PATH ]
then
echo $BUILD_PATH directory exists
else
echo make directory $BUILD_PATH
mkdir $BUILD_PATH
fi

cd $BUILD_PATH

echo "generating Makefiles for GCC"
cmake -G "Unix Makefiles" -DCMAKE_COMPILER_IS_GNUCXX=1 -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_LANGID_DEBUG_PRINT=1 ../../source/
make all
else
echo "usage: $0 [debug|release|profile|clean]"
exit 1
fi
