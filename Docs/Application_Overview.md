# Application Overview

## Overview

### High Level Overview
 1. The GPS_ONE C++ application runs as a UDP Server in which ```NMEA``` data will be streamed to it.
    1. Multiple Receiving Ports will be used.
 2. The incoming ```NMEA``` data is verified against the CheckSum and then parsed depending on its sentence (GGA< RMC, ect). Only moving to the next step with the valid data.
 3. The valid data is sent over UDP to a destination. 

<hr/>

## Application Diagram
```
GPS_ONE/
├── CMakeLists.txt          (top-level: wires everything together)
├── main.cpp
├── external/
│   └── minmea/
│       └── CMakeLists.txt  (wraps the third-party minmea lib)
├── NMEA/
│   ├── CMakeLists.txt
│   ├── nmea.cpp
│   └── nmea.h
├── UDP/
│   ├── CMakeLists.txt
│   ├── UDP_Receiver.cpp
│   └── UDP_Receiver.h
└── Test/
    ├── CMakeLists.txt
    └── UDP_Tests/
        ├── Counter_Test.cpp
        └── UDP_tests.h
```
<hr/>

## Projects

### Executable
 - ```main.cpp``` - Running the Application

### Libraries
 - ```UDP``` - Responsible for handling UDP streaming for both incoming and outgoing.
 - ```NMEA``` - Responsible for verifying and parsing the NMEA data

### Testing
 - ```Tests```

<hr/>

## CMakeLists.txt

### Separate Projects
 - Each directory which contains a ```CMakeLists.txt``` File will be hence treated as its own project.

<hr/>