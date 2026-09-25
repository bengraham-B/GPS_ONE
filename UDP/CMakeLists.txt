# Same pattern — UDP/ becomes its own library.

add_library(UDP STATIC UDP_Receiver.cpp)

# Lets anything linking UDP do #include "UDP_Receiver.h" directly.
target_include_directories(UDP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# UDP_Receiver.cpp includes both minmea.h and nmea.h, and calls into the
# nmea class directly — so UDP depends on NMEA.
# We only need to list NMEA here (not minmea) because NMEA already
# PUBLIC-links minmea, so UDP inherits minmea automatically through NMEA.
# This chain is: UDP -> NMEA -> minmea.
target_link_libraries(UDP PUBLIC NMEA)