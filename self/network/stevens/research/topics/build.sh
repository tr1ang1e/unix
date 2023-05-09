#!/bin/sh

SOURCES="topic.c ../../lib/lib.c"
HEADERS="../../lib/"
OUTPUT="__topic"

gcc $SOURCES -o $OUTPUT  -I $HEADERS