cmake_minimum_required(VERSION 3.29)
project(LTL C)

set(CMAKE_C_STANDARD 23)

include_directories(.)

add_executable(LTL main.c
        ltl/lock.c
        ltl/thread.c
        ltl/thread_list.c
        ltl/linux/linux.c
        ltl/windows/windows.c)

# Windows Build

#set(CMAKE_SYSTEM_NAME  Windows)
#set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
#set(CMAKE_C_FLAGS      "${CMAKE_CXX_FLAGS} -static")