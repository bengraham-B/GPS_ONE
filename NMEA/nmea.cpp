//
// Created by ben on 2026/09/24.
//

#include "nmea.h"

#include <iostream>

#include "../external/minmea/minmea.h" // GitHub NMEA Project

// Constructor
nmea::nmea(std::string message)
{
    this-> message = message;
}

minmea_sentence_gga nmea::nmeaGGA()
{
    // NMEA Sentence
    minmea_sentence_gga GGA{}; // empty struct

    // Receive Sentence from constructor and ensure length is not zero
    if (const std::size_t message_length = std::size(message); message_length == 0) {
        return minmea_sentence_gga{};
    }

    // Validate Sentence | Validate CheckSum
    if (!minmea_check(message.c_str(), true))
    {
        std::cout <<""<<std::endl;
        std::cout << "INVALID" <<std::endl;
        std::cout <<""<<std::endl;
        return minmea_sentence_gga{};
    }

    // Only Parses messages which Checksum is valid
    if (minmea_parse_gga(&GGA, message.c_str())) {
        // Parsing Succeeded
        std::cout << "GGA: " <<message <<std::endl;

        // Print to screen
        printGGA(GGA);
        return GGA;
    }


    return GGA; // Returns empty
}

void nmea::printGGA(const minmea_sentence_gga &GGA)
{
    std::cout << GGA.time.hours << "h" << GGA.time.minutes << ":" << GGA.time.seconds << " | " << minmea_tocoord(&GGA.latitude) << " | " << std::endl;
}