/*
 Example to show how text padding and setting the datum works.

 Drawing a numbers at a location can leave the remains of previous numbers.
 for example drawing 999 then 17 may display as:
    179 for left datum
    or
    917 for right datum.

 By setting the correct text padding width and setting a background colour then
 the plotted numbers will overprint old values.  The padding width must be set
 to be large enough to cover the old text.

 This sketch shows drawing numbers and text both with and without padding.
 Unpadded text and numbers plot in red.
 Padded text and numbers plot in green.

 Padding works with all plotting datums set be setTextDatum()

 The height of the padded area is set automatically by the font used.
 
  #########################################################################
  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
  #########################################################################
 */

#include <TFT_HX8357_Due.h> // Hardware-specific library

TFT_HX8357_Due tft = TFT_HX8357_Due();       // Invoke custom library

unsigned long drawTime = 0;

void setup(void) {
  tft.begin();
  tft.setRotation(1);
}

void loop() {
  int x = 240;
  int y = 60;
  byte decimal_places = 1;
  byte font = 8;
  
  tft.fillScreen(HX8357_BLACK);

  header("Right datum padding demo");

  tft.setTextColor(HX8357_RED, HX8357_BLUE);

  tft.setTextDatum(TR_DATUM); // Top Right is datum, so decimal point stays in same place
                              // any datum could be used

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // The value on screen will be wrong as not all digitas are over-printed
  
  tft.setTextPadding(0); // Setting to zero switches off padding

  // Print all numbers from 49.9 to 0.0 in font 8 to show the problem
  
  for (int i = 499; i >= 0; i--) {
    tft.drawFloat(i/10.0, decimal_places, x, y, font);
  }

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Now set padding width to be 3 digits plus decimal point wide in font 8
  // Padding height is set automatically, all numeric digits are the same width
  // in fonts 1 to 8. The value on screen will now be correct as all digits are over-printed

  int padding = tft.textWidth("99.9", font); // get the width of the text in pixels
  tft.setTextColor(HX8357_GREEN, HX8357_BLUE);
  tft.setTextPadding(padding);

  for (int i = 499; i >= 0; i--) {
    tft.drawFloat(i/10.0, decimal_places, x, y + 140, font); // Use 1 decimal place
  }

  delay(5000);

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Now use integers
  // The value on screen will be wrong as not all digitas are over-printed
  
  tft.fillScreen(HX8357_BLACK);

  header("Left datum padding demo");

  tft.setTextColor(HX8357_RED, HX8357_BLUE);

  tft.setTextDatum(TL_DATUM); // Top Left is datum
                              // any datum could be used

  tft.setTextPadding(0); // Setting to zero switches off padding

  // Print all numbers from 49.9 to 0.0 in font 8 to show the problem
  
  for (int i = 499; i >= 0; i--) {
    tft.drawNumber(i, x, y, font);
  }

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Now set padding width to be 3 digits plus decimal point wide in font 8
  // Padding height is set automatically, all numeric digits are the same width
  // in fonts 1 to 8
  // The value on screen will now be correct as all digits are over-printed

  padding = tft.textWidth("999", font); // get the width of the text in pixels
  tft.setTextColor(HX8357_GREEN, HX8357_BLUE);
  tft.setTextPadding(padding);

  for (int i = 499; i >= 0; i--) {
    tft.drawNumber(i, x, y + 140, font); // Use 1 decimal place
  }

  delay(5000);

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Now use integers with a centred datum
  // The value on screen will be wrong as not all digitas are over-printed
  
  tft.fillScreen(HX8357_BLACK);

  header("Centre datum padding demo");
  
  tft.setTextColor(HX8357_RED, HX8357_BLUE);

  tft.setTextDatum(TC_DATUM); // Top Centre is datum
                              // any datum could be used

  tft.setTextPadding(0); // Setting to zero switches off padding

  // Print all numbers from 49.9 to 0.0 in font 8 to show the problem
  
  for (int i = 499; i >= 0; i--) {
    tft.drawNumber(i, x, y, font);
  }

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Now set padding width to be 3 digits plus decimal point wide in font 8
  // Padding height is set automatically, all numeric digits are the same width
  // in fonts 1 to 8
  // The value on screen will now be correct as all digits are over-printed

  padding = tft.textWidth("999", font); // get the width of the text in pixels
  tft.setTextColor(HX8357_GREEN, HX8357_BLUE);
  tft.setTextPadding(padding);

  for (int i = 499; i >= 0; i--) {
    tft.drawNumber(i, x, y + 140, font); // Use 1 decimal place
  }

  delay(5000);

  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // Now use text over-printing by setting the padding value
  // Previous text is not wiped by a shorter string

  tft.fillScreen(HX8357_LIGHTGREY);

  header("Centred datum text padding demo");
  
  tft.setTextSize(2); // Any text size muliplier will work
  
  tft.setTextColor(HX8357_RED, HX8357_BLUE);

  tft.setTextDatum(TC_DATUM); // Top Centre is datum
                              // any datum could be used

  tft.setTextPadding(0); // Setting to zero switches off padding

  tft.drawString("Quick brown", x, y, 4);
  delay(2000);
  tft.drawString("fox", x, y, 4);

  delay(2000);
  
  // Now set padding width to longest string
  // Previous text will automatically be wiped by a shorter string!
  font = 4;

  padding = tft.textWidth("Quick brown", font); // get the width of the widest text in pixels
                                                // could set this to any number up to screen width
  tft.setTextPadding(padding);

  tft.setTextColor(HX8357_GREEN, HX8357_BLUE);
  
  tft.drawString("Quick brown", x, y+80, font);
  delay(2000);
  tft.drawString("fox", x, y+80, font);

  delay(5000);
}

// Print the header for a display screen
void header(char *string)
{
  tft.setTextSize(1);
  tft.setTextColor(HX8357_MAGENTA, HX8357_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(string, 240, 10, 4);
 
}




