# UDP IN GPS ONE

## Overview
 - UDP is used in this application so that NMEA data can be streamed from any device to the IP address the C++ application is running on.
 - Furthermore, the NMEA data will then be sent over UDP to destination, which will be a dotnet server.

### Projects
 - The UDP library is found in the UDP directory.
 - Treated as its own project | Has its own CMakeLists.txt file.
 - the ```UDP.h``` will contain all the declarations for both sending and receiving

<hr/>

## Receiving
- Currently, the application is only listening over 1 port which will be expanded to a range of ports.
- The main.cpp in the root of the C++ application, the UDP_Receiver runs in a while loop which handles the income UDP stream

<hr/>

## Sending
 - The sending of UDP will be placed in a method which will be called when a stream needs to be sent to the destination.

### Destinations
 - ```Dotnet Web Server```: This server will then over UDP send it to the Client side applications.

### Multi-casting
 - At this point we will only be sending to 1 destination.

<hr/>