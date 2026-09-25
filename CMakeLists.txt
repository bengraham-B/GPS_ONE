cmake_minimum_required(VERSION 4.3)
project(GPS_ONE)

set(CMAKE_CXX_STANDARD 26)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(external/minmea)
add_subdirectory(NMEA)
add_subdirectory(UDP)

add_executable(GPS_ONE main.cpp)
target_link_libraries(GPS_ONE PRIVATE UDP NMEA)

enable_testing()
add_subdirectory(Test)