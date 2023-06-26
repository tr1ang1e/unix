#!/bin/sh


# man 8 tcpdump
# print out TCP packets content


# options
iface="-i eth0"
verbose="-v"

# run
port="port 1111"      # local port
sudo tcpdump $iface $verbose $port

# if host filter is required
# host="host 185.186.246.53"      # remote host
# sudo tcpdump $iface $verbose $host and $port
