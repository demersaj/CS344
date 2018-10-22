cmake_minimum_required(VERSION 3.10)
project(Program2 C)

#use C90 standards
set(CMAKE_C_STANDARD 90)

add_executable(buildrooms demersa.buildrooms.c)

add_executable(adventure demersa.adventure.c)