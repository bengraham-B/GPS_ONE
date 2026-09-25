# Minimum CMake version required to understand this file's syntax.
cmake_minimum_required(VERSION 4.3)

# Names the overall project "GPS_ONE" and sets up the compilers CMake will use.
project(GPS_ONE)

# Old third-party CMakeLists.txt files (like minmea's) sometimes declare
# a minimum CMake version older than 3.5, which modern CMake refuses to
# interpret. This tells CMake to treat any such old declaration as if it
# said 3.5, without needing to edit the third-party file itself.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Compile everything in this project using the C++26 standard.
set(CMAKE_CXX_STANDARD 26)

# --- Pull in GoogleTest automatically from GitHub ---
# FetchContent lets CMake download a dependency's source at configure time,
# instead of you having to install it system-wide yourself.
include(FetchContent)

# Declare WHAT we want (a specific tagged release, as a zip) and WHERE to
# get it. This doesn't download anything yet — just registers the request.
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

# Actually download it (if not already cached) and run its own CMakeLists.txt,
# which defines the "gtest" and "gtest_main" targets used in Test/CMakeLists.txt.
FetchContent_MakeAvailable(googletest)

# --- Bring in each sub-project ---
# add_subdirectory tells CMake "also read the CMakeLists.txt inside this
# folder, and add whatever targets it defines to the overall build."
# Order matters: minmea's CMakeLists.txt must run before NMEA's (since NMEA
# links against the "minmea" target), and NMEA before UDP, etc.
add_subdirectory(external/minmea)   # already set up and working — untouched
add_subdirectory(NMEA)
add_subdirectory(UDP)

# --- The actual application ---
# main.cpp is now the ONLY source file compiled directly into GPS_ONE —
# all the real logic lives in the UDP/NMEA/minmea libraries above.
# Like your C# analogy: main.cpp is Program.cs, just referencing the
# other "projects" rather than containing their code itself.
add_executable(GPS_ONE main.cpp)

# Link the app against UDP, which transitively drags in NMEA and minmea
# thanks to the PUBLIC links set up in their own CMakeLists.txt files.
# PRIVATE here because nothing else links against the GPS_ONE executable
# itself — it's the final product, not something else builds on top of.
target_link_libraries(GPS_ONE PRIVATE UDP)

# --- Testing ---
# Turns on CMake/CTest's test-tracking machinery for the whole project.
# Must come before add_subdirectory(Test) so gtest_discover_tests() inside
# Test/CMakeLists.txt has testing infrastructure ready to register into.
enable_testing()

# Pull in Test/CMakeLists.txt, which builds the UDP_Tests executable.
add_subdirectory(Test)