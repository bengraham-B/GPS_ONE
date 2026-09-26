#include <string>
#include <iostream>
#include <vector> // Equivalent to C# Lists

#include "nmea.h"
#include "UDP_Receiver.h"
#include "gtest/gtest.h"

using namespace std;

/* TODO
 * Test that Checksum Validation counter for GGA is working correctly
 * Some will be invalid on purpose to verify they are be counted as invalid.
 */
const vector<string> GGA_to_test = {
    "$GPGGA,120000.00,3355.4940,S,01825.4460,E,1,08,0.9,15.0,M,20.0,M,,*44",
    "$GPGGA,120010.00,3355.4640,S,01825.4880,E,1,08,0.9,16.0,M,20.0,M,,*4B",
    "$GPGGA,120020.00,3355.4340,S,01825.5300,E,1,07,1.0,17.0,M,20.0,M,,*49",
    "$GPGGA,120030.00,3355.4040,S,01825.5720,E,1,09,0.8,18.0,M,20.0,M,,*45",
    "$GPGGA,120040.00,3355.3740,S,01825.6140,E,1,08,0.9,19.0,M,20.0,M,,*40",
    "$GPGGA,120050.00,3355.3440,S,01825.6560,E,2,10,0.7,20.0,M,20.0,M,,*4A",
    "$GPGGA,120100.00,3355.3140,S,01825.6980,E,1,08,0.9,21.0,M,20.0,M,,*4C",
    "$GPGGA,120110.00,3355.2840,S,01825.7400,E,1,06,1.2,22.0,M,20.0,M,,*46",
    "$GPGGA,120120.00,3355.2540,S,01825.7820,E,1,08,0.9,23.0,M,20.0,M,,*43",
    "$GPGGA,120130.00,3355.2240,S,01825.8240,E,1,08,0.9,24.0,M,20.0,M,,*FF", // Invalid
};

TEST(NMEACounter_Tests, test_GGA_sentence_validity)
{
    int NMEASentenceCounter = 0;
    int invalidNMEASentenceCounter = 0;

    for (int i = 0; i < GGA_to_test.size(); i++)
    {
        nmea NMEA(GGA_to_test[i]);
        NMEAResult result = NMEA.parseNMEASentance();

        if (!result.valid)
        {
            invalidNMEASentenceCounter++;
        }

        if (result.valid && result.sentence_type==NMEASentenceEnum::GGA)
        {
            NMEASentenceCounter++;
        }
    }

    // Compare counter params to the function counters
    EXPECT_EQ(NMEASentenceCounter, 9);
    EXPECT_EQ(invalidNMEASentenceCounter, 1);
}
