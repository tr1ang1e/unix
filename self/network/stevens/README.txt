All examples are commented according
to the following rule:

/*
    :: <protocol>
    :: <description>
    :: <version>
        - <marks>

    $ <execute>                 // <marks>

    # ------------------------------------------- #

    <comments> 
*/


All clients require servers to communicate with,
unless otherwise is not specified explicitely. To 
be sure client and server apps are compatible, 
see versions. Versions format: <major>.<minor>
    major - must be the same at client and server side
    minor - indicates enhancements which doesn't broke compatibility


Git commit messages are done in format:
    stevens / c=<version> s=<version> / <comment>
