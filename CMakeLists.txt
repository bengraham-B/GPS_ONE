cmake_minimum_required(VERSION 4.3)
project(GPS_ONE)

set(CMAKE_CXX_STANDARD 26)

add_executable(GPS_ONE
        main.cpp
        external/minmea/minmea.c
        UDP/UDP_Receiver.cpp
        UDP/UDP_Receiver.h
)

target_include_directories(GPS_ONE PRIVATE external/minmea)
target_link_libraries(GPS_ONE m)