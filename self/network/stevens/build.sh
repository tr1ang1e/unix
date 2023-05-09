#!/bin/bash

set -euo pipefail

# --------------- args checking ----------------- #

if [[ $# -gt 1 ]];
then
    echo "Wrong arguments number"
    exit 1
fi

basic_local=true
basic_remote=false
research=false

if [[ $# -eq 1 ]];
then

    case $1 in

        "rem") 
            basic_remote=true 
            ;;

        "rch")
            basic_local=false
            research=true
            ;;

        *)
            echo "Uncknown argument: $1"
            echo "Following are available:
                      = build basic locally
                  rem = build basic locally and remotely
                  rch = run research build script"
            exit 0
            ;;

    esac 

fi

# --------------- build research ---------------- #
# always build locally and remotely

if $research
then

    echo "Building research examples..."
    
    cd research
    researchBuildCommand="./build.sh"
    eval $researchBuildCommand

    exit 0

fi

# ----------------- build local ----------------- #
# build anyway doesn't matter if remote is required

if $basic_local
then

    echo "Building basic locally..."

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

fi

# ----------------- build remote ---------------- #
# build only if remote is required

if $basic_remote
then

    echo "Building basic remotely..."

    # to reduce ssh connection time
    # 
    # 1.  
    #     enable master connection =
    #     (create and) edit ~/.ssh/config
    #     with following content:
    #
    # Host *
    #     ConstrolMaster auto
    #     ControlPath ~/.ssh/master-<user>@<host>:<path>
    #
    # 2.
    #     and then start SSH session in 
    #     a separate terminal window

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

fi
