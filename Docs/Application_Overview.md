# Application Overview

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