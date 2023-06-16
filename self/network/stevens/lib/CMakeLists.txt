SET (LIB_SOURCES
    ${LIB_DIR}/cli/cli.c
    ${LIB_DIR}/common/utils.c
    ${LIB_DIR}/error/error.c
    ${LIB_DIR}/io/read.c
    ${LIB_DIR}/io/write.c
    ${LIB_DIR}/process/process.c
    ${LIB_DIR}/socket/socket.c
)

ADD_LIBRARY(__lib STATIC ${LIB_SOURCES})
TARGET_INCLUDE_DIRECTORIES(__lib PRIVATE ${LIB_DIR})
SET_TARGET_PROPERTIES(__lib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})

TARGET_COMPILE_DEFINITIONS(__lib PRIVATE DEBUG)     # debug messages on or off
TARGET_COMPILE_DEFINITIONS(__lib PRIVATE TRACE)     # trace messages on or off
