
#include "nmea.h"

#include <iostream>

#include "../external/minmea/minmea.h" // GitHub MINMEA Project

// Constructor
nmea::nmea(std::string message)
{
    this-> message = message;
}

NMEAResult nmea::parseNMEASentance()
{
    NMEAResult result{};

    // Switch Statement to Handle Diff Sentences
    switch (minmea_sentence_id(message.c_str(), false))
    {
        case MINMEA_SENTENCE_GGA:
        {
            minmea_sentence_gga GGA{}; // empty struct which Valid GGA sentences values will be passed to.
            result.sentence_type = NMEASentenceEnum::GGA;

            // Validate the Sentence | Validate CheckSum
            if (!minmea_check(message.c_str(), true))  // This means that the sentence is invalid. Invalid Counter Increases inside UDPReceiver
            {
                result.valid = false;
                return result;
            }

            if (minmea_parse_gga(&GGA, message.c_str())) {
                printGGA(GGA);
                // Returns valid GGA Sentence
                result.valid = true;
                result.GGA = GGA;
                return result;
            }
            break;
        }
        case MINMEA_SENTENCE_RMC:
            break;
        case MINMEA_UNKNOWN:
            break;
        case MINMEA_SENTENCE_GBS:
            break;
        case MINMEA_SENTENCE_GLL:
            break;
        case MINMEA_SENTENCE_GSA:
            break;
        case MINMEA_SENTENCE_GST:
            break;
        case MINMEA_SENTENCE_GSV:
            break;
        case MINMEA_SENTENCE_VTG:
            break;
        case MINMEA_SENTENCE_ZDA:
            break;
        case MINMEA_INVALID:
            break;
    }

    return result;
}

minmea_sentence_gga nmea::nmeaGGA()
{
    // NMEA Sentence
    minmea_sentence_gga GGA{}; // empty struct

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