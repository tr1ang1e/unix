#!/bin/sh

while true
do
    # clear
    cat /proc/net/tcp | grep :0457      # 1111 port in host byte order, hex
    sleep 1
done
