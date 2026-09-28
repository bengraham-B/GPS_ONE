#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting TFT Test...");

    tft.init();
    tft.setRotation(1); // Landscape mode (320x240)
    tft.fillScreen(TFT_BLACK);

    // Set text color and size
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    // Draw text at a specific position
    tft.setCursor(20, 100);
    tft.println("Hello PlatformIO!");

    // Or draw a centered string
    tft.drawCentreString("CYD Test", 160, 150, 4);

    Serial.println("TFT Initialized!");
}

void loop() {
    // Nothing here for this test
    delay(1000);
}