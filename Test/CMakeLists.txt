# Unlike the libraries above, this builds an actual runnable program —
# tests need to be something you can execute, not just link into other code.

# Compile Counter_Test.cpp into a binary called "UDP_Tests".
# (UDP_tests.h gets pulled in automatically via #include inside
# Counter_Test.cpp — you never need to list header files separately here.)
add_executable(UDP_Tests UDP_Tests/Counter_Test.cpp)

# Lets Counter_Test.cpp do #include "UDP_tests.h" without a relative path.
# PRIVATE here (not PUBLIC) because nothing ever links against UDP_Tests
# afterward — it's the final product, so there's no "downstream" target
# that would need this setting passed along.
target_include_directories(UDP_Tests PRIVATE UDP_Tests)

# Everything UDP_Tests needs to compile and run:
target_link_libraries(UDP_Tests
        PRIVATE
        UDP        # the code under test — also pulls in NMEA + minmea transitively
        NMEA       # listed explicitly too, in case Counter_Test.cpp uses NMEA directly
        gtest      # the GoogleTest framework itself
        gtest_main # provides main() for us automatically, so we don't write our own runner
)

# Pulls in CMake's built-in helper functions for GoogleTest integration.
include(GoogleTest)

# After UDP_Tests is built, scan it and register each individual TEST(...)
# as its own separate CTest entry — so `ctest`, or CLion's test runner,
# can run and report each test individually rather than as one big blob.
gtest_discover_tests(UDP_Tests) # Actual Directory of my tests Test/UDP_TESTS