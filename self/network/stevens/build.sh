#!/bin/bash

buildDir=".tmp"
if [ ! -d $buildDir ]; then
  mkdir $buildDir
fi

cd $buildDir
cmake ..
cmake --build .
cd -
