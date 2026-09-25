add_library(UDP STATIC UDP_Receiver.h)
target_include_directories(UDP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(UDP PUBLIC NMEA)