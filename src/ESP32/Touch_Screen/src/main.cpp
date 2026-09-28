#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  tft.init();
  tft.setRotation(1); // Landscape mode (320x240)
  tft.fillScreen(TFT_BLACK); // Clear screen

  // Set text color (white text, black background)
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  // Method 1: Using drawString (left-aligned)
  tft.drawString("Hello CYD", 10, 10, 4); // Text, X, Y, Font

  // Method 2: Using drawCentreString (centered)
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.drawCentreString("Centered Text", 160, 60, 4); // X is screen center

  // Method 3: Using setCursor and print
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setCursor(20, 100);
  tft.print("Using print()");

  // Displaying numbers
  tft.drawNumber(1234, 20, 140, 4);
  tft.drawFloat(3.14159, 2, 20, 170, 4); // Value, Decimals, X, Y, Font
}

int counter;

void loop() {
	counter++;
  	// Your main code here
	delay(10);
  	tft.drawString("Counter: " + String(counter), 10, 10, 4); // Text, X, Y, Font
}