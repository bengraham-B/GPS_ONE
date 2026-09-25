add_library(NMEA STATIC nmea.cpp)
target_include_directories(UDP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(NMEA PUBLIC minmea)