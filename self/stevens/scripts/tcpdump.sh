#!/bin/sh


# man 8 tcpdump
# print out TCP packets content


# options
iface="-i eth0"
verbose="-v"

# filters
host="host 185.186.246.53"      # remote host
port="port 1111"                # local port


sudo tcpdump $iface $verbose $host and $port
