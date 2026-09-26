# This folder becomes its own library — like its own small project that
# other parts of the codebase (UDP, and eventually GPS_ONE itself) can
# reference and build on top of.

# Create a library target named "NMEA", compiled from nmea.cpp.
# STATIC = it gets compiled into a .a file and baked directly into
# whatever links against it, rather than being loaded separately at runtime.
add_library(NMEA STATIC nmea.cpp)

# Anyone who links against NMEA should automatically be able to
# #include "nmea.h" without knowing this folder's path.
# PUBLIC = both this target AND anything that links it inherit the setting.
target_include_directories(NMEA PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# nmea.cpp includes minmea.h and calls minmea functions directly
# (minmea_check, minmea_parse_gga, etc), so NMEA depends on minmea.
# NOTE: "minmea" below must exactly match whatever target name is
# defined inside external/minmea/CMakeLists.txt — confirm that file's
# add_library(...) line before relying on this.
# PUBLIC means anything that links NMEA (like UDP, or GPS_ONE) automatically
# also gets minmea's include path and link requirements passed along —
# this is called a "transitive dependency," and it's why we don't have
# to manually re-link minmea everywhere NMEA gets used.
target_link_libraries(NMEA PUBLIC minmea)