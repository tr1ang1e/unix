#!/bin/bash

set -euo pipefail

# ----------------- build local ----------------- #

buildDir=".tmp"
if [ ! -d $buildDir ]; 
then
  mkdir $buildDir
fi

buildCommand="cd $buildDir 
  && cmake .. 
  && cmake --build .
  && cd -"
eval $buildCommand

# ----------------- build remote ---------------- #

# to reduce ssh connection time
# enable master connection =
# (create and) edit ~/.ssh/config
# with following content:
#
# Host *
#     ConstrolMaster auto
#     ControlPath ~/.ssh/master-<user>@<host>:<path>

remoteHost="$1"
remoteDir="/root/projects/stevens"

# can't use '&&' inside remote command because SSH tool
# requires the ';' should be used as commands separator

remoteBuildCommand="cd $remoteDir/research;
  rm -rf $buildDir 2>/dev/null;
  mkdir $buildDir 2>/dev/null;
  cd $buildDir;
  cmake ..;
  cmake --build .;
  cd -;
  cd ../;
  rm -r -f research;
  rm -r -f lib"

tar -cf - -C ../ research lib | ssh $remoteHost "tar -xf - -C $remoteDir; $remoteBuildCommand"
