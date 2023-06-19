#!/bin/sh

# simple script for local build only

SOURCES="topic.c"
HEADERS="../../lib/"
LIB="__lib"
LIBSDIR="../../lib"
OUTPUT="__topic"

gcc $SOURCES -o $OUTPUT -I $HEADERS -l $LIB -L $LIBSDIR
