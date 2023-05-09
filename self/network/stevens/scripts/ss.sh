#!/bin/sh


# man 8 ss
# dump socket statistics (similar to netstat)


# options
<< options
    a = all sockets
    O = one line
    m = memory
    p = show process using socket
options
options=aOmp

# filters
sport="sport = 1111"
dport="dport = 1111"


ss -$options $sport or $dport
