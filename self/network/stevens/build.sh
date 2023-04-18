#!/bin/bash

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

# args parsing

if [[ $# -gt 1 ]];
then
  echo "Wrong arguments number"
  exit 1
fi

if [[ $# -eq 0 ]];
then
  exit 0
fi

if [[ "$1" != "remote" ]];
then
  echo "Uncknown argument: $1"
  exit 1
fi

# to reduce ssh connection time
# enable master connection =
# (create and) edit ~/.ssh/config
# with following content:
#
# Host *
#     ConstrolMaster auto
#     ControlPath ~/.ssh/master-<user>@<host>:<path>

remoteHost="root@185.186.246.53"
remoteDir="/root/projects/stevens"

# can't use '&&' inside remote command because SSH tool
# requires the ';' should be used as commands separator

remoteBuildCommand="cd $remoteDir;
  mkdir $buildDir 2>/dev/null;
  cd $buildDir;
  cmake ..;
  cmake --build .;
  cd -;
  rm -r -f $buildDir;
  rm -r -f lib;
  rm *.c *.txt"

tar -cf - client.c server.c lib CMakeLists.txt | ssh $remoteHost "tar -xf - -C $remoteDir; $remoteBuildCommand"
