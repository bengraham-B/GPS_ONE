#ifndef GPS_ONE_NMEA_H
#define GPS_ONE_NMEA_H

#pragma once // prevents the file from being included twice by accident
#include <string>
#include "../external/minmea/minmea.h"

enum class NMEASentenceEnum
{
    INVALID = -1,
    UNKNOWN = 0,
    GGA
};

struct NMEAResult
{
    minmea_sentence_gga GGA{};
    NMEASentenceEnum sentence_type {NMEASentenceEnum::UNKNOWN};
    bool valid = true;
};

class nmea {
public:
    nmea(std::string message); // Constructor
    minmea_sentence_gga nmeaGGA();
    static void printGGA(const minmea_sentence_gga &GGA);
    NMEAResult parseNMEASentance();
    NMEAResult result;

private:
    std::string message; // Passed in constructor

};


#endif //GPS_ONE_NMEA_H
