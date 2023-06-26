#!/bin/sh

'''
    get info about socket state, timers. etc
    reading /proc/net/tcp is depricated in favor of 'tcp_diag'
    for info see: https://www.kernel.org/doc/Documentation/networking/proc_net_tcp.txt

    IP:port output format is:
        - hex
        - IP = net byte order
        - port = host byte order
    To print from application and compare with output use:
    printf("%08X:%04X", <ip>, <port>)
'''

while true
do
    # clear
    cat /proc/net/tcp | grep :0457      # 1111 port in host byte order, hex
    sleep 1
done
