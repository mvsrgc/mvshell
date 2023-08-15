cmake_minimum_required(VERSION 3.26)
project(mvshell C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(mvshell
        shell.c
        shell.h
        tokenize.c
        tokenize.h
        utils.c
        utils.h)
