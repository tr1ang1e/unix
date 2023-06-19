#!/bin/sh

# get info about socket state, timers. etc
# reading /proc/ne/tcp is depricated in favor of 'tcp_diag'
# for info see: https://www.kernel.org/doc/Documentation/networking/proc_net_tcp.txt

while true
do
    # clear
    cat /proc/net/tcp | grep :0457      # 1111 port in host byte order, hex
    sleep 1
done
