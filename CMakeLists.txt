#
# Risc-V interpretator implementation
# if you want see some more you may check github.com/Exactlywb
# (c) Frolov Daniil
#

cmake_minimum_required(VERSION 3.8)

project("RISC-V")

set(EXEC_FILE "interpretator")

set(HEADERS

    interpret/interpret.h

)

set(SOURCES

    main.c
    interpret/interpret.c

)

#compiler settings
add_compile_options(

    -Wall
    -Wextra

    -Wsign-conversion
    -Wdouble-promotion

    -Wfloat-equal
    -Wbool-compare

    -Wdiv-by-zero

)

#executable files
add_executable(${EXEC_FILE} ${SOURCES})
