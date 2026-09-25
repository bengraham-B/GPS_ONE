add_executable(UDP_Tests UDP_Tests/Counter_Test.cpp)
target_include_directories(UDP_Tests PRIVATE UDP_Tests)
target_link_libraries(UDP_Tests
    PRIVATE
        UDP
        NMEA
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(UDP_Tests)

#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
#)
#
## For Windows: Prevent overriding the parent project's compilier Linker settings
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)
#
#add_executable(UDP_Tests UDP_Tests/Counter_Test.cpp)
#
#target_include_directories(UDP_Tests PRIVATE UDP_Tests)
#target_link_libraries(UDP_Tests
#    PRIVATE
#        GPS_ONE
#        gtest
#        gtest_main
#)
#
#include(GoogleTest)
#gtest_discover_tests(UDP_Tests)