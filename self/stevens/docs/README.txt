All examples are commented according
to the following rule:

/*
    :: <protocol>
    :: <description>
    :: <version>
        - <marks>

    $ <execute>                 // <marks>

    # ------------------------------------------- #

    // Comment section.
    // If the same comment is required for both
    //   client and server sides, it will be placed
    //   only on the client side
*/

                * * *

All clients require servers to communicate with,
unless otherwise is not specified explicitely. To 
be sure client and server apps are compatible, 
see versions. Versions format: <major>.<minor>
    major - must be the same at both client and server sides
    minor - indicates enhancements which doesn't broke compatibility

Git commit messages are done in format:
    if source code is changed:      "stevens / c=<version> s=<version> / <comment>"
    if common changes:              "stevens / <comment>"
    if learning some topic:         "stevens / <topic> : <comment>"

                * * *

Request for comments (RFC):
    UDP = RFC 768
    TCP = RFC 793, 1323, 2581, 2988, 3390
    SCTP = RFC 3286 (introduction), 2960, 3309

                * * *

Problems and possible solutions:

    bind() error: Address already in use
        $ netstat -tulpn  /  $ lsof -i:<port>
        $ kill <pid>
