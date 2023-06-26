#!/bin/bash

set -euo pipefail

# --------------- args checking ----------------- #

print_help()
{
    echo "Following arguments are available:
    *no-arg* .......... build basic locally
    rem [HOSTRY,PRI] .. build basic locally and remotely
    rch ............... run research build script
    test .............. build test.c file"
}

build_test()
{
    SOURCES="test.c"
    HEADERS="./lib/"
    LIB="__lib"
    LIBSDIR="./lib"
    OUTPUT="__t"

    gcc $SOURCES -o $OUTPUT -I $HEADERS -l $LIB -L $LIBSDIR
}

if [[ $# -gt 2 ]];
then
    echo "Wrong arguments number"
    exit 1
fi

basic_local=true
basic_remote=false
    # remote defaults
    remoteHost="root@185.186.246.53"
    remoteDir="/root/projects/stevens"
research=false

if [ -n "${1-}" ]
then
    case $1 in

        "--help")
            print_help
            exit 0
            ;;
        
        "rem") 
            basic_remote=true 
            ;;

        "rch")
            basic_local=false
            research=true
            ;;

        "test")
            build_test
            exit 0
            ;;

        *)
            echo "Uncknown argument: $1"
            print_help
            exit 0
            ;;

    esac
fi

if [ -n "${2-}" ] && $basic_remote
then
    case $2 in

        "HOSTRY")
            remoteHost="root@185.186.246.53"
            ;;

        "RPI")
            remoteHost=""root@""$(dig +short $2)""
            ;;
        
        *)
            echo "Uncknown argument: $2"
            print_help
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
    researchBuildCommand="./build.sh $remoteHost"
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

# To reduce ssh connection time'
#
# 1.  
#    enable master connection =
#    (create and) edit ~/.ssh/config
#    with following content:
#
# Host *
#    ConstrolMaster auto
#    ControlPath ~/.ssh/master-<user>@<host>:<path>
#
# 2.
#    and then start SSH session in 
#    a separate terminal window

if $basic_remote
then

    echo "Building basic remotely..."


    # can't use '&&' inside remote command because SSH tool
    # requires the ';' should be used as commands separator

    # useful command:
    #       find ./lib ! -name "*.a" ! -name "lib" -type f,d -delete
    # - find all files and dirs with names '*.a' and 'lib'
    # - revert selection
    # - delete final selection

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
