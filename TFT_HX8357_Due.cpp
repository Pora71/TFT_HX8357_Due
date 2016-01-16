/***************************************************
  Arduino TFT graphics library targetted at the Due
  board when used with one of these two display shields
  from Banggood China:
  
  3.2 Inch 320 X 480 TFT LCD Display Module Support Arduino Mega2560
  http://www.banggood.com/3_2-Inch-320-X-480-TFT-LCD-Display-Module-Support-Arduino-Mega2560-p-963574.html

  or:

  3.0 Inch 320 X 480 TFT LCD Display Module Support Arduino Mega2560
  http://www.banggood.com/3_0-Inch-320-X-480-TFT-LCD-Display-Module-Support-Arduino-Mega2560-p-963573.html

  These displays are also available with a Mega board:
  
  http://www.banggood.com/Mega2560-R3-Board-With-USB-Cable-3_2-Inch-TFT-LCD-Display-Module-p-965164.html
  http://www.banggood.com/Mega2560-R3-Board-With-USB-Cable-3_0-Inch-TFT-LCD-Display-Module-p-967224.html

  Dispay port to Mega and Due pin mapping:

	TFT	Mega	Due
	D15	PA7	D.6
	D14	PA6	D.3
	D13	PA5	D.2
	D12	PA4	D.1
	D11	PA3	D.0
	D10	PA2	A.15
	D9	PA1	A.14
	D8	PA0	B.26

	D7	PC7	D.9
	D6	PC6	A.7
	D5	PC5	D.10
	D4	PC4	C.1
	D3	PC3	C.2
	D2	PC2	C.3
	D1	PC1	C.4
	D0	PC0	C.5
		
	RS	PD7	C.6
	WR	PG2	C.7
	CS	PG1	C.8
	RST	PG0	C.9

  This library has been derived from the Adafruit_GFX
  library and the associated driver library. See text
  at the end of this file.

  This is a standalone library that contains the
  hardware driver, the graphics funtions and the
  proportional fonts.

  This library also contains a set of fonts for fast
  rendering. The larger fonts are Run Length Encoded
  to reduce their size.

  Added 2/1/16
  This library now includes the full font set of custom
  fonts from the GFX library.

  16/1/16
  Converted to library for Due

  Created by Bodmer, latest copy here:
  https://github.com/Bodmer

 ****************************************************/

#include "TFT_HX8357_Due.h"

//#include <avr/pgmspace.h>

/***************************************************************************************
** Function name:           TFT_HX8357_Due
** Description:             Constructor
***************************************************************************************/
TFT_HX8357_Due::TFT_HX8357_Due(int16_t w, int16_t h)
{
  _cs   = 40;
  _rs   = 38;
  _rst  = 41;
  _wr   = 39;
  _fcs  = 44; //Unused FLASH chip select

  pinMode(_rst, OUTPUT);
  digitalWrite(_rst, HIGH);

  pinMode(_rs, OUTPUT);
  pinMode(_cs, OUTPUT);
  pinMode(_wr, OUTPUT);

  digitalWrite(_rs, HIGH);

  digitalWrite(_cs, HIGH);

  digitalWrite(_wr, HIGH);

  pinMode(_fcs, OUTPUT);
  digitalWrite(_fcs, HIGH); // Stop line floating


  // Set 16 bit port pins to output on Due
  pinMode(29, OUTPUT);
  pinMode(28, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);

  _width    = w;
  _height   = h;
  rotation  = 0;
  cursor_y  = cursor_x  = 0;
  textfont  = 1;
  textsize  = 1;
  textcolor   = 0xFFFF;
  textbgcolor = 0x0000;
  pixelfg  = 0xFFFF;
  pixelbg  = 0x0000;
  padX = 0;
  textwrap  = true;
  textdatum = TL_DATUM; // Top left text datum is default

  fontsloaded = 0;

#ifdef LOAD_GLCD
  fontsloaded = 0x0002;  // Bit 1 set
#endif

#ifdef LOAD_FONT2
  fontsloaded |= 0x0004; // Bit 2 set
#endif

#ifdef LOAD_FONT4
  fontsloaded |= 0x0010; // Bit 4 set
#endif

#ifdef LOAD_FONT6
  fontsloaded |= 0x0040; // Bit 6 set
#endif

#ifdef LOAD_FONT7
  fontsloaded |= 0x0080; // Bit 7 set
#endif

#ifdef LOAD_FONT8
  fontsloaded |= 0x0100; // Bit 8 set
#endif

#ifdef LOAD_GFXFF
  gfxFont   = NULL; // Set the font to the GLCD
#endif
}

/***************************************************************************************
** Function name:           writecommand
** Description:             Send an 8 bit command to the TFT
***************************************************************************************/
void TFT_HX8357_Due::writecommand(uint8_t c)
{
SETUP_CS_L;
RS_L;
hi_byte(0);
lo_byte(c);
WR_STB;
RS_H;
SETUP_CS_H;
}

/***************************************************************************************
** Function name:           writedata
** Description:             Send a 8 bit data value to the TFT
***************************************************************************************/
void TFT_HX8357_Due::writedata(uint8_t c)
{
SETUP_CS_L;
hi_byte(c>>8);
lo_byte(c);
WR_STB;
SETUP_CS_H;
}

/***************************************************************************************
** Function name:           begin
** Description:             Included for backwards compatibility
***************************************************************************************/
void TFT_HX8357_Due::begin(void)
{
 init();
}

/***************************************************************************************
** Function name:           init
** Description:             Reset, then initialise the TFT display registers
***************************************************************************************/
void TFT_HX8357_Due::init(void)
{
  // toggle RST low to reset
    digitalWrite(_rst, HIGH);
    delay(10);
    digitalWrite(_rst, LOW);
    delay(10);
    digitalWrite(_rst, HIGH);
    delay(10);

#ifndef HX8357C

// Configure HX8357-B display
    writecommand(0x11);
    delay(20);
    writecommand(0xD0);
    writedata(0x07);
    writedata(0x42);
    writedata(0x18);

    writecommand(0xD1);
    writedata(0x00);
    writedata(0x07);
    writedata(0x10);

    writecommand(0xD2);
    writedata(0x01);
    writedata(0x02);

    writecommand(0xC0);
    writedata(0x10);
    writedata(0x3B);
    writedata(0x00);
    writedata(0x02);
    writedata(0x11);

    writecommand(0xC5);
    writedata(0x08);

    writecommand(0xC8);
    writedata(0x00);
    writedata(0x32);
    writedata(0x36);
    writedata(0x45);
    writedata(0x06);
    writedata(0x16);
    writedata(0x37);
    writedata(0x75);
    writedata(0x77);
    writedata(0x54);
    writedata(0x0C);
    writedata(0x00);

    writecommand(0x36);
    writedata(0x0a);

    writecommand(0x3A);
    writedata(0x55);

    writecommand(0x2A);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0x3F);

    writecommand(0x2B);
    writedata(0x00);
    writedata(0x00);
    writedata(0x01);
    writedata(0xDF);

    delay(120);
    writecommand(0x29);

    delay(25);
// End of HX8357-B display configuration

#else

// HX8357-C display initialisation

	writecommand(0xB9); // Enable extension command
	writedata(0xFF);
	writedata(0x83);
	writedata(0x57);
	delay(50);
	
	writecommand(0xB6); //Set VCOM voltage
	writedata(0x2C);    //0x52 for HSD 3.0"
	
	writecommand(0x11); // Sleep off
	delay(200);
	
	writecommand(0x35); // Tearing effect on
	writedata(0x00);    // Added parameter

	writecommand(0x3A); // Interface pixel format
	writedata(0x55);    // 16 bits per pixel

	//writecommand(0xCC); // Set panel characteristic
	//writedata(0x09);    // S960>S1, G1>G480, R-G-B, normally black

	//writecommand(0xB3); // RGB interface
	//writedata(0x43);
	//writedata(0x00);
	//writedata(0x06);
	//writedata(0x06);

	writecommand(0xB1); // Power control
	writedata(0x00);
	writedata(0x15);
	writedata(0x0D);
	writedata(0x0D);
	writedata(0x83);
	writedata(0x48);
	
	
	writecommand(0xC0); // Does this do anything?
	writedata(0x24);
	writedata(0x24);
	writedata(0x01);
	writedata(0x3C);
	writedata(0xC8);
	writedata(0x08);
	
	writecommand(0xB4); // Display cycle
	writedata(0x02);
	writedata(0x40);
	writedata(0x00);
	writedata(0x2A);
	writedata(0x2A);
	writedata(0x0D);
	writedata(0x4F);
	
	writecommand(0xE0); // Gamma curve
	writedata(0x00);
	writedata(0x15);
	writedata(0x1D);
	writedata(0x2A);
	writedata(0x31);
	writedata(0x42);
	writedata(0x4C);
	writedata(0x53);
	writedata(0x45);
	writedata(0x40);
	writedata(0x3B);
	writedata(0x32);
	writedata(0x2E);
	writedata(0x28);
	
	writedata(0x24);
	writedata(0x03);
	writedata(0x00);
	writedata(0x15);
	writedata(0x1D);
	writedata(0x2A);
	writedata(0x31);
	writedata(0x42);
	writedata(0x4C);
	writedata(0x53);
	writedata(0x45);
	writedata(0x40);
	writedata(0x3B);
	writedata(0x32);
	
	writedata(0x2E);
	writedata(0x28);
	writedata(0x24);
	writedata(0x03);
	writedata(0x00);
	writedata(0x01);

	writecommand(0x36); // MADCTL Memory access control
	writedata(0x48);
	delay(20);

	writecommand(0x21); //Display inversion on
	delay(20);

	writecommand(0x29); // Display on
	
	delay(120);
#endif

#ifdef KEEP_CS_LOW
  SETUP_CS_L;
#endif
}

/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle outline
***************************************************************************************/
void TFT_HX8357_Due::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = - r - r;
  int16_t x = 0;

  drawPixel(x0  , y0 + r, color);
  drawPixel(x0  , y0 - r, color);
  drawPixel(x0 + r, y0  , color);
  drawPixel(x0 - r, y0  , color);

  while (x < r) {
    if (f >= 0) {
      r--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    drawPixel(x0 + x, y0 + r, color);
    drawPixel(x0 - x, y0 + r, color);
    drawPixel(x0 + x, y0 - r, color);
    drawPixel(x0 - x, y0 - r, color);
    drawPixel(x0 + r, y0 + x, color);
    drawPixel(x0 - r, y0 + x, color);
    drawPixel(x0 + r, y0 - x, color);
    drawPixel(x0 - r, y0 - x, color);
  }
}

/***************************************************************************************
** Function name:           drawCircleHelper
** Description:             Support function for circle drawing
***************************************************************************************/
void TFT_HX8357_Due::drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -r -r;
  int16_t x     = 0;

  while (x < r) {
    if (f >= 0) {
      r--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + r, color);
      drawPixel(x0 + r, y0 + x, color);
    }
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - r, color);
      drawPixel(x0 + r, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - r, y0 + x, color);
      drawPixel(x0 - x, y0 + r, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - r, y0 - x, color);
      drawPixel(x0 - x, y0 - r, color);
    }
  }
}

/***************************************************************************************
** Function name:           fillCircle
** Description:             draw a filled circle
***************************************************************************************/
void TFT_HX8357_Due::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  drawFastVLine(x0, y0 - r, r + r + 1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

/***************************************************************************************
** Function name:           fillCircleHelper
** Description:             Support function for filled circle drawing
***************************************************************************************/
// Used to do circles and roundrects
void TFT_HX8357_Due::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -r - r;
  int16_t x     = 0;

  delta++;
  while (x < r) {
    if (f >= 0) {
      r--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      drawFastVLine(x0 + x, y0 - r, r + r + delta, color);
      drawFastVLine(x0 + r, y0 - x, x + x + delta, color);
    }
    if (cornername & 0x2) {
      drawFastVLine(x0 - x, y0 - r, r + r + delta, color);
      drawFastVLine(x0 - r, y0 - x, x + x + delta, color);
    }
  }
}

/***************************************************************************************
** Function name:           fillScreen
** Description:             Clear the screen to defined colour
***************************************************************************************/
void TFT_HX8357_Due::fillScreen(uint16_t color)
{
  fillRect(0, 0, _width, _height, color);
}

/***************************************************************************************
** Function name:           drawRect
** Description:             Draw a rectangle outline
***************************************************************************************/
// Draw a rectangle
void TFT_HX8357_Due::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  drawFastHLine(x, y, w, color);
  drawFastHLine(x, y + h - 1, w, color);
  drawFastVLine(x, y, h, color);
  drawFastVLine(x + w - 1, y, h, color);
}

/***************************************************************************************
** Function name:           drawRoundRect
** Description:             Draw a rounded corner rectangle outline
***************************************************************************************/
// Draw a rounded rectangle
void TFT_HX8357_Due::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  drawFastHLine(x + r  , y    , w - r - r, color); // Top
  drawFastHLine(x + r  , y + h - 1, w - r - r, color); // Bottom
  drawFastVLine(x    , y + r  , h - r - r, color); // Left
  drawFastVLine(x + w - 1, y + r  , h - r - r, color); // Right
  // draw four corners
  drawCircleHelper(x + r    , y + r    , r, 1, color);
  drawCircleHelper(x + w - r - 1, y + r    , r, 2, color);
  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  drawCircleHelper(x + r    , y + h - r - 1, r, 8, color);
}

/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a rounded corner filled rectangle
***************************************************************************************/
// Fill a rounded rectangle
void TFT_HX8357_Due::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  // smarter version
  fillRect(x + r, y, w - r - r, h, color);

  // draw four corners
  fillCircleHelper(x + w - r - 1, y + r, r, 1, h - r - r - 1, color);
  fillCircleHelper(x + r    , y + r, r, 2, h - r - r - 1, color);
}

/***************************************************************************************
** Function name:           drawTriangle
** Description:             Draw a triangle outline using 3 arbitrary points
***************************************************************************************/
// Draw a triangle
void TFT_HX8357_Due::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

/***************************************************************************************
** Function name:           fillTriangle 
** Description:             Draw a filled triangle using 3 arbitrary points
***************************************************************************************/
// Fill a triangle - uses faster Bressenham method (original Adafruit function overflowed on big triangles!)
void TFT_HX8357_Due::fillTriangle (int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3, uint16_t c)
{
	int16_t t1x,t2x,y,minx,maxx,t1xp,t2xp;
	bool changed1 = false;
	bool changed2 = false;
	int16_t signx1,signx2,dx1,dy1,dx2,dy2;
	uint16_t e1,e2;
     // Sort vertices
	if (y1>y2) { swap(y1,y2); swap(x1,x2); }
	if (y1>y3) { swap(y1,y3); swap(x1,x3); }
	if (y2>y3) { swap(y2,y3); swap(x2,x3); }

	t1x=t2x=x1; y=y1;   // Starting points

	dx1 = x2 - x1; if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
	dy1 = y2 - y1;
 
	dx2 = x3 - x1; if(dx2<0) { dx2=-dx2; signx2=-1; } else signx2=1;
	dy2 = y3 - y1;
	
	if (dy1 > dx1) {   // swap values
        swap(dx1,dy1);
		changed1 = true;
	}
	if (dy2 > dx2) {   // swap values
        swap(dy2,dx2);
		changed2 = true;
	}
	
	e2 = dx2>>1;
    // Flat top, just process the second half
    if(y1==y2) goto next;
    e1 = dx1>>1;
	
	for (uint16_t i = 0; i < dx1;) {
		t1xp=0; t2xp=0;
		if(t1x<t2x) { minx=t1x; maxx=t2x; }
		else		{ minx=t2x; maxx=t1x; }
        // process first line until y value is about to change
		while(i<dx1) {
			i++;			
			e1 += dy1;
	   	   	while (e1 >= dx1) {
				e1 -= dx1;
   	   	   	   if (changed1) t1xp=signx1;//t1x += signx1;
				else          goto next1;
			}
			if (changed1) break;
			else t1x += signx1;
		}
	// Move line
	next1:
        // process second line until y value is about to change
		while (1) {
			e2 += dy2;		
			while (e2 >= dx2) {
				e2 -= dx2;
				if (changed2) t2xp=signx2;//t2x += signx2;
				else          goto next2;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}
	next2:
		if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
		if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
	   	drawFastHLine(minx, y, maxx-minx, c);    // Draw line from min to max points found on the y
		// Now increase y
		if(!changed1) t1x += signx1;
		t1x+=t1xp;
		if(!changed2) t2x += signx2;
		t2x+=t2xp;
    	     y += 1;
		if(y==y2) break;
		
   }
	next:
	// Second half
	dx1 = x3 - x2; if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
	dy1 = y3 - y2;
	t1x=x2;
 
	if (dy1 > dx1) {   // swap values
        swap(dy1,dx1);
		changed1 = true;
	} else changed1=false;
	
	e1 = dx1>>1;
	
	for (uint16_t i = 0; i<=dx1; i++) {
		t1xp=0; t2xp=0;
		if(t1x<t2x) { minx=t1x; maxx=t2x; }
		else		{ minx=t2x; maxx=t1x; }
	    // process first line until y value is about to change
		while(i<dx1) {
    		e1 += dy1;
	   	   	while (e1 >= dx1) {
				e1 -= dx1;
   	   	   	   	if (changed1) { t1xp=signx1; break; }//t1x += signx1;
				else          goto next3;
			}
			if (changed1) break;
			else   	   	  t1x += signx1;
			if(i<dx1) i++;
		}
	next3:
        // process second line until y value is about to change
		while (t2x!=x3) {
			e2 += dy2;
	   	   	while (e2 >= dx2) {
				e2 -= dx2;
				if(changed2) t2xp=signx2;
				else          goto next4;
			}
			if (changed2)     break;
			else              t2x += signx2;
		}	   	   
	next4:

		if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
		if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
	   	drawFastHLine(minx, y, maxx-minx, c);    // Draw line from min to max points found on the y
		// Now increase y
		if(!changed1) t1x += signx1;
		t1x+=t1xp;
		if(!changed2) t2x += signx2;
		t2x+=t2xp;
       	y += 1;
		if(y>y3) return;
	}
}

/***************************************************************************************
** Function name:           drawBitmap
** Description:             Draw an image stored in an array on the TFT
***************************************************************************************/
void TFT_HX8357_Due::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;

  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++ ) {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        drawPixel(x + i, y + j, color);
      }
    }
  }
}

/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position
***************************************************************************************/
void TFT_HX8357_Due::setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position and font
***************************************************************************************/
void TFT_HX8357_Due::setCursor(int16_t x, int16_t y, uint8_t font)
{
  textfont = font;
  cursor_x = x;
  cursor_y = y;
}

/***************************************************************************************
** Function name:           setTextSize
** Description:             Set the text size multiplier
***************************************************************************************/
void TFT_HX8357_Due::setTextSize(uint8_t s)
{
  if (s>7) s = 7; // Limit the maximum size multiplier so byte variables can be used for rendering
  textsize = (s > 0) ? s : 1; // Don't allow font size 0
}

/***************************************************************************************
** Function name:           setTextFont
** Description:             Set the font for the print stream
***************************************************************************************/
void TFT_HX8357_Due::setTextFont(uint8_t f)
{
  textfont = (f > 0) ? f : 1; // Don't allow font 0
#ifdef LOAD_GFXFF
  gfxFont = NULL;
#endif
}

/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground colour (background is transparent)
***************************************************************************************/
void TFT_HX8357_Due::setTextColor(uint16_t c)
{
  // For 'transparent' background set the bg the same as fg
  setTextColor(c, c);
}

/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground and background colour
***************************************************************************************/
void TFT_HX8357_Due::setTextColor(uint16_t c, uint16_t b)
{
  textcolor   = c;
  textbgcolor = b;
  fgColor(c);
  bgColor(b);
}

/***************************************************************************************
** Function name:           setTextWrap
** Description:             Define if text should wrap at end of line
***************************************************************************************/
void TFT_HX8357_Due::setTextWrap(boolean w)
{
  textwrap = w;
}

/***************************************************************************************
** Function name:           setTextDatum
** Description:             Set the text position reference datum
***************************************************************************************/
void TFT_HX8357_Due::setTextDatum(uint8_t d)
{
  textdatum = d;
}

/***************************************************************************************
** Function name:           setTextPadding
** Description:             Define padding width (aids erasing old text and numbers)
***************************************************************************************/
void TFT_HX8357_Due::setTextPadding(uint16_t x_width)
{
  padX = x_width;
}

/***************************************************************************************
** Function name:           getRotation
** Description:             Return the rotation value (as used by setRotation())
***************************************************************************************/
uint8_t TFT_HX8357_Due::getRotation(void)
{
  return rotation;
}

/***************************************************************************************
** Function name:           width
** Description:             Return the pixel width of display (per current rotation)
***************************************************************************************/
// Return the size of the display (per current rotation)
int16_t TFT_HX8357_Due::width(void)
{
  return _width;
}

/***************************************************************************************
** Function name:           height
** Description:             Return the pixel height of display (per current rotation)
***************************************************************************************/
int16_t TFT_HX8357_Due::height(void)
{
  return _height;
}

/***************************************************************************************
** Function name:           textWidth
** Description:             Return the width in pixels of a string in a given font
***************************************************************************************/
int16_t TFT_HX8357_Due::textWidth(char *string, int16_t font)
{
  int16_t str_width  = 0;
  char uniCode;
  const uint8_t *widthtable;

  if (font>1 && font<9)
  {
  widthtable = (const uint8_t *)pgm_read_word( &(fontdata[font].widthtbl ) ) - 32; //subtract the 32 outside the loop

    while (*string)
    {
      uniCode = *(string++);
      str_width += pgm_read_byte( widthtable + uniCode); // Normally we need to subract 32 from uniCode
    }
  }

  else
  {

#ifdef LOAD_GFXFF
    if(gfxFont) // New font
    {
      while (*string)
      {
        uniCode = *(string++);
        uniCode -= pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph  = &(((GFXglyph *)pgm_read_word(&gfxFont->glyph))[uniCode]);
        // If this is not the  last character then use xAdvance
        if (*string) str_width += pgm_read_byte(&glyph->xAdvance);
        // Else use the offset plus width since this can be bigger than xAdvance
        else str_width += ((int8_t)pgm_read_byte(&glyph->xOffset) + pgm_read_byte(&glyph->width));
      }
    }
    else
#endif
    {
#ifdef LOAD_GLCD
      while (*string++) str_width += 6;
#endif
    }
  }
  return str_width * textsize;
}

/***************************************************************************************
** Function name:           fontsLoaded
** Description:             return an encoded 16 bit value showing the fonts loaded
***************************************************************************************/
// Returns a value showing which fonts are loaded (bit N set =  Font N loaded)

uint16_t TFT_HX8357_Due::fontsLoaded(void)
{
  return fontsloaded;
}


/***************************************************************************************
** Function name:           fontHeight
** Description:             return the height of a font (yAdvance for free fonts)
***************************************************************************************/
int16_t TFT_HX8357_Due::fontHeight(int16_t font)
{
#ifdef LOAD_GFXFF
  if (font==1)
  {
    if(gfxFont) // New font
    {
      return pgm_read_byte(&gfxFont->yAdvance) * textsize;
    }
  }
#endif
  return pgm_read_byte( &fontdata[font].height ) * textsize;
}

/***************************************************************************************
** Function name:           drawChar
** Description:             draw a single character in the Adafruit GLCD or GFX font
***************************************************************************************/
void TFT_HX8357_Due::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
  if ((x >= _width)            || // Clip right
      (y >= _height)           || // Clip bottom
      ((x + 6 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
    return;

#ifdef LOAD_GLCD
//>>>>>>>>>>>>>>>>>>
#ifdef LOAD_GFXFF
  if(!gfxFont) { // 'Classic' built-in font
#endif
//>>>>>>>>>>>>>>>>>>

  boolean fillbg = (bg != color);

  if ((size==1) && fillbg)
  {
    setAddrWindow(x, y, x+5, y+8);

    fgColor(color); bgColor(bg); // Setup the port values

    byte column[5];
    const uint8_t *flash_address = (const uint8_t *)font + c * 5;
    column[0] = pgm_read_byte(flash_address++);
    column[1] = pgm_read_byte(flash_address++);
    column[2] = pgm_read_byte(flash_address++);
    column[3] = pgm_read_byte(flash_address++);
    column[4] = pgm_read_byte(flash_address++);

    byte mask = 0x1;

    for (int8_t j = 0; j < 8; j++) {
      if (column[0] & mask) fgWrite();
      else bgWrite();WR_STB;
      if (column[1] & mask) fgWrite();
      else bgWrite();WR_STB;
      if (column[2] & mask) fgWrite();
      else bgWrite();WR_STB;
      if (column[3] & mask) fgWrite();
      else bgWrite();WR_STB;
      if (column[4] & mask) fgWrite();
      else bgWrite();WR_STB;
      mask <<= 1;
      bgWrite();WR_STB; // Last pixel
    }
    CS_H;
  }
  else
  {
    const uint8_t *flash_address = (const uint8_t *)font + c * 5; 
    for (int8_t i = 0; i < 6; i++ ) {
      uint8_t line;
      if (i == 5)
        line = 0x0;
      else
        line = pgm_read_byte(flash_address + i);

      if (size == 1) // default size
      {
        for (int8_t j = 0; j < 8; j++) {
          if (line & 0x1) drawPixel(x + i, y + j, color);
          line >>= 1;
        }
      }
      else {  // big size
        for (int8_t j = 0; j < 8; j++) {
          if (line & 0x1)  fillRect(x + i * size, y + j * size, size, size, color);
          else if (fillbg) fillRect(x + i * size, y + j * size, size, size, bg);
          line >>= 1;
        }
      }
    }
  }

//>>>>>>>>>>>>>>>>>>>>>>>>>>>
#ifdef LOAD_GFXFF
  } else { // Custom font
#endif
//>>>>>>>>>>>>>>>>>>>>>>>>>>>
#endif // LOAD_GLCD

#ifdef LOAD_GFXFF
//>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling drawChar()
    // directly with 'bad' characters of font may cause mayhem!

    c -= pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph  = &(((GFXglyph *)pgm_read_word(&gfxFont->glyph))[c]);
    uint8_t  *bitmap = (uint8_t *)pgm_read_word(&gfxFont->bitmap);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t  w  = pgm_read_byte(&glyph->width),
             h  = pgm_read_byte(&glyph->height),
             xa = pgm_read_byte(&glyph->xAdvance);
    int8_t   xo = pgm_read_byte(&glyph->xOffset),
             yo = pgm_read_byte(&glyph->yOffset);
    uint8_t  xx, yy, bits, bit=0;
    int16_t  xo16, yo16;

    if(size > 1) {
      xo16 = xo;
      yo16 = yo;
    }

    // Todo: Add character clipping here

    // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
    // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
    // has typically been used with the 'classic' font to overwrite old
    // screen contents with new data.  This ONLY works because the
    // characters are a uniform size; it's not a sensible thing to do with
    // proportionally-spaced fonts with glyphs of varying sizes (and that
    // may overlap).  To replace previously-drawn text when using a custom
    // font, use the getTextBounds() function to determine the smallest
    // rectangle encompassing a string, erase the area with fillRect(),
    // then draw new text.  This WILL infortunately 'blink' the text, but
    // is unavoidable.  Drawing 'background' pixels will NOT fix this,
    // only creates a new set of problems.  Have an idea to work around
    // this (a canvas object type for MCUs that can afford the RAM and
    // displays supporting setAddrWindow() and pushColors()), but haven't
    // implemented this yet.

// Here we have 3 versions of the same function just for evaluation purposes
// Comment out the next two #defines to revert to the slower Adafruit implementation

// If FAST_LINE is defined then the free fonts are rendered using horizontal lines
// this makes rendering fonts 2-5 times faster. Particularly good for large fonts.
// This is an elegant solution since it still uses generic functions present in the
// stock library.

// If FAST_SHIFT is defined then a slightly faster (at least for AVR processors)
// shifting bit mask is used

// Free fonts don't look good when the size multiplier is >1 so we couls remove
// code if this is not wanted and speed things up

#define FAST_HLINE
#define FAST_SHIFT
//FIXED_SIZE is an option in User_Setup.h that only works with FAST_LINE enabled

#ifdef FIXED_SIZE
    x+=xo; // Save 88 bytes of FLASH
    y+=yo;
#endif

#ifdef FAST_HLINE

  #ifdef FAST_SHIFT
    uint16_t hpc = 0; // Horizontal foreground pixel count
    for(yy=0; yy<h; yy++) {
      for(xx=0; xx<w; xx++) {
        if(bit == 0) {
          bits = pgm_read_byte(&bitmap[bo++]);
          bit  = 0x80;
        }
        if(bits & bit) hpc++;
        else {
          if (hpc) {
#ifndef FIXED_SIZE
            if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
            else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
#else
            drawFastHLine(x+xx-hpc, y+yy, hpc, color);
#endif
            hpc=0;
          }
        }
        bit >>= 1;
      }
      // Draw pixels for this line as we are about to increment yy
          if (hpc) {
#ifndef FIXED_SIZE
            if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
            else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
#else
            drawFastHLine(x+xx-hpc, y+yy, hpc, color);
#endif
            hpc=0;
          }
    }
  #else
    uint16_t hpc = 0; // Horizontal foreground pixel count
    for(yy=0; yy<h; yy++) {
      for(xx=0; xx<w; xx++) {
        if(!(bit++ & 7)) {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        if(bits & 0x80) hpc++;
        else {
          if (hpc) {
            if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
            else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
            hpc=0;
          }
        }
        bits <<= 1;
      }
      // Draw pixels for this line as we are about to increment yy
      if (hpc) {
        if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
        else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
        hpc=0;
      }
    }
  #endif

#else
    for(yy=0; yy<h; yy++) {
      for(xx=0; xx<w; xx++) {
        if(!(bit++ & 7)) {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        if(bits & 0x80) {
          if(size == 1) {
            drawPixel(x+xo+xx, y+yo+yy, color);
          } else {
            fillRect(x+(xo16+xx)*size, y+(yo16+yy)*size, size, size, color);
          }
        }
        bits <<= 1;
      }
    }
#endif
#endif


#ifdef LOAD_GLCD
  #ifdef LOAD_GFXFF
  } // End classic vs custom font
  #endif
#endif

}

/***************************************************************************************
** Function name:           setWindow
** Description:             define an area to receive a stream of pixels
***************************************************************************************/
// Function to set the address window, for use in sketches

void TFT_HX8357_Due::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
  setAddrWindow(x0, y0, x1, y1);
  CS_H;
}

/***************************************************************************************
** Function name:           setAddrWindow
** Description:             define an area to rexeive a stream of pixels
***************************************************************************************/
// This is a private function, use setWindow() in sketches

void TFT_HX8357_Due::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  if (x0>x1) swap(x0,x1);
  if (y0>y1) swap(y0,y1);
  //if((x1 >= _width) || (y1 >= _height)) return;
  CS_L;
  RS_L; addrCmd(HX8357_CASET); WR_STB; RS_H;
  lo_byte(x0>>8); WR_STB;
  lo_byte(x0); WR_STB;
  lo_byte(x1>>8); WR_STB;
  lo_byte(x1); WR_STB;
  RS_L; addrCmd(HX8357_PASET); WR_STB; RS_H;
  lo_byte(y0>>8); WR_STB;
  lo_byte(y0); WR_STB;
  lo_byte(y1>>8); WR_STB;
  lo_byte(y1); WR_STB;
  RS_L; addrCmd(HX8357_RAMWR); WR_STB; RS_H;
}

/***************************************************************************************
** Function name:           drawPixel
** Description:             push a single pixel at an arbitrary position
***************************************************************************************/

void TFT_HX8357_Due::drawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  // Faster range checking, possible because x and y are unsigned
  if ((x >= _width) || (y >= _height)) return;

  if (color!=pixelfg) fgColor(color);

  CS_L;
  RS_L; addrCmd(HX8357_CASET); WR_STB; RS_H;
  lo_byte(x>>8); WR_STB;
  lo_byte(x); WR_STB;
  lo_byte(x>>8); WR_STB;
  lo_byte(x); WR_STB;
  RS_L; addrCmd(HX8357_PASET); WR_STB; RS_H;
  lo_byte(y>>8); WR_STB;
  lo_byte(y); WR_STB;
  lo_byte(y>>8); WR_STB;
  lo_byte(y); WR_STB;
  RS_L; addrCmd(HX8357_RAMWR); WR_STB; RS_H;
  fgWrite(); WR_STB;
  CS_H;
}

/***************************************************************************************
** Function name:           pushColor
** Description:             push a single pixel
***************************************************************************************/
void TFT_HX8357_Due::pushColor(uint16_t color)
{
  if (color!=pixelfg) fgColor(color);
  CS_L;
  fgWrite();
  WR_STB;
  CS_H;
}

/***************************************************************************************
** Function name:           pushColor
** Description:             push a single colour to "len" pixels
***************************************************************************************/
void TFT_HX8357_Due::pushColor(uint16_t color, uint16_t len)
{
  if (color!=pixelfg) fgColor(color);
  CS_L;
  fgWrite();
  while(len--) {WR_L;WR_H;}
  CS_H;
}

/***************************************************************************************
** Function name:           pushColors
** Description:             push an aray of pixels for BMP image drawing
***************************************************************************************/
// Sends an array of 16-bit color values to the TFT; used
// externally by BMP examples.  Assumes that setWindow() has
// previously been called to define the bounds.  Max 255 pixels at
// a time (BMP examples read in small chunks due to limited RAM).

void TFT_HX8357_Due::pushColors(uint16_t *data, uint8_t len)
{
  CS_L;
  while(len--) {
    uint16_t color = *data++;
    lo_byte(color);
    hi_byte(color >> 8);
    WR_STB; 
  }
  CS_H;
}

/***************************************************************************************
** Function name:           pushColors
** Description:             push an aray of pixels for 16 bit raw image drawing
***************************************************************************************/
// Assumes that setWindow() has previously been called

void TFT_HX8357_Due::pushColors(uint8_t *data, uint16_t len)
{
  CS_L;
  while(len--) {
    hi_byte(*data++);
    lo_byte(*data++);
    WR_STB; 
  }
  CS_H;
}

/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
***************************************************************************************/

// Bresenham's algorithm - thx wikipedia - speed enhanced by Bodmer to use
// an eficient FastH/V Line draw routine for line segments of 2 pixels or more
// enhanced further using code from Xark and Spellbuilder (116 byte penalty)

void TFT_HX8357_Due::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  boolean steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx = x1 - x0, dy = abs(y1 - y0);;


  int16_t err = dx>>1, ystep=-1, xs=x0, dlen=0;
  if (y0 < y1) ystep = 1;

  if (color!=pixelfg) fgColor(color);

  CS_L;
  // Split into steep and not steep for FastH/V separation
  if(steep) {
    for (; x0<=x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;

        RS_L; addrCmd(HX8357_CASET); WR_STB; RS_H;
        lo_byte(y0>>8); WR_STB;
        lo_byte(y0);    WR_STB;
        lo_byte(y0>>8); WR_STB;
        lo_byte(y0);    WR_STB;
        RS_L; addrCmd(HX8357_PASET); WR_STB; RS_H;
        lo_byte(xs>>8); WR_STB;
        lo_byte(xs);    WR_STB;
        lo_byte(x1>>8); WR_STB;
        lo_byte(x1);    WR_STB;
        RS_L; addrCmd(HX8357_RAMWR); WR_STB; RS_H;

        fgWrite();

        while (dlen)   { dlen--;WR_L;WR_H;}

        y0 += ystep; xs=x0+1;
      }
    }
    if(dlen) drawFastVLine(y0, xs, dlen, color);
  }
  else
  {
    for (; x0<=x1; x0++) {
      dlen++;
      err -= dy;
      if (err < 0) {
        err += dx;

        RS_L; addrCmd(HX8357_CASET); WR_STB; RS_H;
        lo_byte(xs>>8); WR_STB;
        lo_byte(xs); WR_STB;
        lo_byte(x1>>8); WR_STB;
        lo_byte(x1); WR_STB;
        RS_L; addrCmd(HX8357_PASET); WR_STB; RS_H;
        lo_byte(y0>>8); WR_STB;
        lo_byte(y0); WR_STB;
        lo_byte(y0>>8); WR_STB;
        lo_byte(y0); WR_STB;
        RS_L; addrCmd(HX8357_RAMWR); WR_STB; RS_H;

        fgWrite();

        while(dlen){ dlen--;WR_L;WR_H;}

        y0 += ystep; xs=x0+1;
      }
    }
    if(dlen) drawFastHLine(xs, y0, dlen, color);
  }
  CS_H;
}

/***************************************************************************************
** Function name:           drawFastVLine
** Description:             draw a vertical line
***************************************************************************************/
void TFT_HX8357_Due::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
#ifdef CLIP_CHECK
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height)) return;
  if ((y + h - 1) >= _height) h = _height - y;
#endif

  CS_L;
  RS_L; addrCmd(HX8357_CASET); WR_STB; RS_H;
  lo_byte(x>>8); WR_STB;
  lo_byte(x); WR_STB;
  lo_byte(x>>8); WR_STB;
  lo_byte(x); WR_STB;
  RS_L; addrCmd(HX8357_PASET); WR_STB; RS_H;
  lo_byte(y>>8); WR_STB;
  lo_byte(y); WR_STB;
  y+=h-1; lo_byte(y>>8); WR_STB;
  lo_byte(y); WR_STB;
  RS_L; addrCmd(HX8357_RAMWR); WR_STB; RS_H;

  if (color!=pixelfg) fgColor(color);
  fgWrite();

  while (h>15) { h-=16;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
  }
  while (h--) { WR_L;WR_H;}
  CS_H;
}

/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
***************************************************************************************/
void TFT_HX8357_Due::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
#ifdef CLIP_CHECK
  // Rudimentary clipping
  if ((x >= _width) || (y >= _height)) return;
  if ((x + w - 1) >= _width)  w = _width - x;
#endif


  CS_L;
  RS_L; addrCmd(HX8357_CASET); WR_STB; RS_H;
  lo_byte(x>>8); WR_STB;
  lo_byte(x); WR_STB;
  x+=w-1; lo_byte(x>>8); WR_STB;
  lo_byte(x); WR_STB;
  RS_L; addrCmd(HX8357_PASET); WR_STB; RS_H;
  lo_byte(y>>8); WR_STB;
  lo_byte(y); WR_STB;
  lo_byte(y>>8); WR_STB;
  lo_byte(y); WR_STB;
  RS_L; addrCmd(HX8357_RAMWR); WR_STB; RS_H;

  if (color!=pixelfg) fgColor(color);
  fgWrite();

  while (w>15) { w-=16;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
  }
  while(w--){ WR_L;WR_H;}
  CS_H;
}

/***************************************************************************************
** Function name:           fillRect
** Description:             draw a filled rectangle
***************************************************************************************/
void TFT_HX8357_Due::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
#ifdef CLIP_CHECK
  // rudimentary clipping (drawChar w/big text requires this)
  if ((x >= _width) || (y >= _height) || (w==0) || (h==0)) return;
  if ((x + w - 1) >= _width)  w = _width  - x;
  if ((y + h - 1) >= _height) h = _height - y;
#endif

  setAddrWindow(x, y, x+w-1, y+h-1);

  if (color!=pixelfg) fgColor(color);
  fgWrite();

  uint32_t hw= h*w;

  while (hw>47) { hw-=48; // Screen width/10
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
  }

  while (hw>15) { hw-=16;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
    WR_STB;WR_STB;WR_STB;WR_STB;
  }

  while(hw--) { WR_L;WR_H;}
  CS_H;
}

/***************************************************************************************
** Function name:           color565
** Description:             convert three 8 bit RGB levels to a 16 bit colour value
***************************************************************************************/
uint16_t TFT_HX8357_Due::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/***************************************************************************************
** Function name:           setRotation
** Description:             rotate the screen orientation m = 0-3 or 4-7 for BMP drawing
***************************************************************************************/

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_BGR 0x08
#define MADCTL_RGB 0x00
#define MADCTL_SS  0x02
#define MADCTL_GS  0x01

#ifdef MIRROR

void TFT_HX8357_Due::setRotation(uint8_t m)
{
  writecommand(HX8357_MADCTL);
  rotation = m % 4;
  switch (rotation) {
   case 0: // Portrait
     writedata(MADCTL_BGR | MADCTL_SS);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
     break;
   case 1: // Landscape (Portrait + 90)
     writedata(MADCTL_MV | MADCTL_BGR);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
  case 2: // Inverter portrait
    writedata( MADCTL_BGR | MADCTL_GS);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
    break;
   case 3: // Inverted landscape
     writedata(MADCTL_MV | MADCTL_BGR | MADCTL_SS | MADCTL_GS);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
  }
}

#else

void TFT_HX8357_Due::setRotation(uint8_t m)
{
  writecommand(HX8357_MADCTL);
  rotation = m % 8;
  switch (rotation) {
   case 0: // Portrait
     writedata(MADCTL_BGR | MADCTL_MX);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
     break;
   case 1: // Landscape (Portrait + 90)
     writedata(MADCTL_BGR | MADCTL_MV);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
  case 2: // Inverter portrait
    writedata( MADCTL_BGR | MADCTL_MY);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
    break;
   case 3: // Inverted landscape
     writedata(MADCTL_BGR | MADCTL_MV | MADCTL_MX | MADCTL_MY);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
   case 4: // Portrait
     writedata(MADCTL_BGR | MADCTL_MX | MADCTL_MY);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
     break;
   case 5: // Landscape (Portrait + 90)
     writedata(MADCTL_BGR | MADCTL_MV | MADCTL_MX);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
  case 6: // Inverter portrait
    writedata( MADCTL_BGR);
     _width  = HX8357_TFTWIDTH;
     _height = HX8357_TFTHEIGHT;
    break;
   case 7: // Inverted landscape
     writedata(MADCTL_BGR | MADCTL_MV | MADCTL_MY);
     _width  = HX8357_TFTHEIGHT;
     _height = HX8357_TFTWIDTH;
     break;
  }
}

#endif
/***************************************************************************************
** Function name:           invertDisplay
** Description:             invert the display colours i = 1 invert, i = 0 normal
***************************************************************************************/
void TFT_HX8357_Due::invertDisplay(boolean i)
{
  writecommand(i ? HX8357_INVON : HX8357_INVOFF);
}

/***************************************************************************************
** Function name:           write
** Description:             draw characters piped through print class
***************************************************************************************/
size_t TFT_HX8357_Due::write(uint8_t utf8)
{
  if (utf8 == '\r') return 1;

  uint8_t uniCode = utf8;        // Work with a copy
  if (utf8 == '\n') uniCode+=22; // Make it a valid space character to stop errors

  uint16_t width = 0;
  uint16_t height = 0;

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
  //Serial.print((char) uniCode); // Debug line sends all printed TFT text to serial port
  //Serial.println(uniCode, HEX); // Debug line sends all printed TFT text to serial port
  //delay(5);                     // Debug optional wait for serial port to flush through
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  if(!gfxFont) {
#endif
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#ifdef LOAD_FONT2
  if (textfont == 2)
  {
      width = pgm_read_byte(widtbl_f16 + uniCode-32);
      height = chr_hgt_f16;
      // Font 2 is rendered in whole byte widths so we must allow for this
      width = (width + 6) / 8;  // Width in whole bytes for font 2
                                // Should be + 7 but must allow for font width change
      width = width * 8;        // Width converted back to pixels
  }
  #ifdef LOAD_RLE
  else
  #endif
#endif


#ifdef LOAD_RLE
  {
      // Uses the fontinfo struct array to avoid lots of 'if' or 'switch' statements
      // This is more convenient for the RLE fonts
      // Yes, these lines are needlessly executed when textfont = 1
      width = pgm_read_byte( pgm_read_word( &(fontdata[textfont].widthtbl ) ) + uniCode-32 );
      height= pgm_read_byte( &fontdata[textfont].height );
  }
#endif

#ifdef LOAD_GLCD
  if (textfont==1)
  {
      width =  6;
      height = 8;
  }
#else
  if (textfont==1) return 0;
#endif

  height = height * textsize;

  if (utf8 == '\n') {
    cursor_y += height;
    cursor_x  = 0;
  }
  else
  {
    if (textwrap && (cursor_x + width * textsize >= _width))
    {
      cursor_y += height;
      cursor_x = 0;
    }
    cursor_x += drawChar(uniCode, cursor_x, cursor_y, textfont);
  }

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef LOAD_GFXFF
  } // Custom GFX font
  else
  {

    if(utf8 == '\n') {
      cursor_x  = 0;
      cursor_y += (int16_t)textsize *
                  (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if(uniCode != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if((uniCode >= first) && (uniCode <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        uint8_t   c2    = uniCode - pgm_read_byte(&gfxFont->first);
        GFXglyph *glyph = &(((GFXglyph *)pgm_read_word(&gfxFont->glyph))[c2]);
        uint8_t   w     = pgm_read_byte(&glyph->width),
                  h     = pgm_read_byte(&glyph->height);
        if((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset);
          if(textwrap && ((cursor_x + textsize * (xo + w)) >= _width)) {
            // Drawing character would go off right edge; wrap to new line
            cursor_x  = 0;
            cursor_y += (int16_t)textsize *
                        (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
          }
          drawChar(cursor_x, cursor_y, uniCode, textcolor, textbgcolor, textsize);
        }
        cursor_x += pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize;
      }
    }

  }
#endif // LOAD_GFXFF
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

  return 1;
}

/***************************************************************************************
** Function name:           drawChar
** Description:             draw a unicode onto the screen
***************************************************************************************/
// To be streamlined at some point
int16_t TFT_HX8357_Due::drawChar(uint16_t uniCode, int16_t x, int16_t y, int16_t font)
{

  if (font==1)
  {
#ifdef LOAD_GLCD
  #ifndef LOAD_GFXFF
    drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
    return 6 * textsize;
  #endif
#else
  #ifndef LOAD_GFXFF
    return 0;
  #endif
#endif

#ifdef LOAD_GFXFF
      drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
      if(!gfxFont) { // 'Classic' built-in font
  #ifdef LOAD_GLCD
        return 6 * textsize;
  #else
        return 0;
  #endif
      }
      else
      {
        uint8_t first = pgm_read_byte(&gfxFont->first);
        if((uniCode >= first) && (uniCode <= (uint8_t)pgm_read_byte(&gfxFont->last)))
        {
          uint8_t   c2    = uniCode - pgm_read_byte(&gfxFont->first);
          GFXglyph *glyph = &(((GFXglyph *)pgm_read_word(&gfxFont->glyph))[c2]);
          return pgm_read_byte(&glyph->xAdvance) * textsize;
        }
        else
        {
          return 0;
        }
      }
#endif
  }

  uint16_t width  = 0;
  uint16_t height = 0;
  const uint8_t *flash_address = 0;

  uniCode -= 32;

  if(textcolor != pixelfg) fgColor(textcolor);

#ifdef LOAD_FONT2
  if (font == 2)
  {
      // This is 20us faster than using the fontdata structure
      flash_address = (const uint8_t *)pgm_read_word(&chrtbl_f16[uniCode]);
      width = pgm_read_byte(widtbl_f16 + uniCode);
      height = chr_hgt_f16;
  }
  #ifdef LOAD_RLE
  else
  #endif
#endif

#ifdef LOAD_RLE
  {
      // This is slower than above but is more convenient for the RLE fonts
      flash_address = (const uint8_t *)pgm_read_word( pgm_read_word( &(fontdata[font].chartbl ) ) + uniCode*sizeof(void *) );
      width = pgm_read_byte( pgm_read_word( &(fontdata[font].widthtbl ) ) + uniCode );
      height= pgm_read_byte( &fontdata[font].height );
  }
#endif

  int16_t w = width;
  byte line = 0;

  byte tl = textcolor;
  byte th = textcolor >> 8;
  byte bl = textbgcolor;
  byte bh = textbgcolor >> 8;

#ifdef LOAD_FONT2 // chop out 962 bytes of code if we do not need it
  if (font == 2) {
    w = w + 6; // Should be + 7 but we need to compensate for width increment
    w = w / 8;
    if (x + width * textsize >= _width) return width * textsize ;

    if (textcolor == textbgcolor || textsize != 1) {
      int16_t pX      = 0;
      int16_t pY      = y;

      for (int16_t i = 0; i < height; i++)
      {
        if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize, textbgcolor);

        for (int16_t k = 0; k < w; k++)
        {
          line = pgm_read_byte(flash_address + w * i + k);
          if (line) {
            if (textsize == 1) {
              pX = x + k * 8;
              if (line & 0x80) drawPixel(pX, pY, textcolor);
              if (line & 0x40) drawPixel(pX + 1, pY, textcolor);
              if (line & 0x20) drawPixel(pX + 2, pY, textcolor);
              if (line & 0x10) drawPixel(pX + 3, pY, textcolor);
              if (line & 0x08) drawPixel(pX + 4, pY, textcolor);
              if (line & 0x04) drawPixel(pX + 5, pY, textcolor);
              if (line & 0x02) drawPixel(pX + 6, pY, textcolor);
              if (line & 0x01) drawPixel(pX + 7, pY, textcolor);
            }
            else {
              pX = x + k * 8 * textsize;
              if (line & 0x80) fillRect(pX, pY, textsize, textsize, textcolor);
              if (line & 0x40) fillRect(pX + textsize, pY, textsize, textsize, textcolor);
              if (line & 0x20) fillRect(pX + 2 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x10) fillRect(pX + 3 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x08) fillRect(pX + 4 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x04) fillRect(pX + 5 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x02) fillRect(pX + 6 * textsize, pY, textsize, textsize, textcolor);
              if (line & 0x01) fillRect(pX + 7 * textsize, pY, textsize, textsize, textcolor);
            }
          }
        }
        pY += textsize;
      }
    }
    else
      // Faster drawing of characters and background using block write
    {

      setAddrWindow(x, y, (x + w * 8) - 1, y + height - 1);

      if(textbgcolor!=pixelbg) bgColor(textbgcolor);

      for(int16_t i=0; i<height; i++)
      {
        for (int16_t k = 0;k < w; k++)
        { 
        line = pgm_read_byte(flash_address+w*i+k);
        if(line&0x80) fgWrite();
        else bgWrite(); WR_STB;
        if(line&0x40) fgWrite();
        else bgWrite(); WR_STB;
        if(line&0x20) fgWrite();
        else bgWrite(); WR_STB;
        if(line&0x10) fgWrite();
        else bgWrite(); WR_STB;
        if(line&0x08) fgWrite();
        else bgWrite(); WR_STB;
        if(line&0x04) fgWrite();
        else bgWrite(); WR_STB;
        if(line&0x02) fgWrite();
        else bgWrite(); WR_STB;
        if(line&0x01) fgWrite();
        else bgWrite(); WR_STB;
        }
      }
      CS_H;
    }
  }

  #ifdef LOAD_RLE
  else
  #endif
#endif  //FONT2

#ifdef LOAD_RLE  //674 bytes of code
  // Font is not 2 and hence is RLE encoded
  {

    w *= height; // Now w is total number of pixels in the character

    if ((textsize != 1) || (textcolor == textbgcolor)) {
      if (textcolor != textbgcolor) fillRect(x, y, width * textsize, textsize * height, textbgcolor);
      int16_t px = 0, py = y, tpy = y; // To hold character block start and end column and row values
      int16_t pc = 0; // Pixel count
      byte np = textsize * textsize; // Number of pixels in a drawn pixel

      // We don't need background here
      if(textcolor != pixelfg) fgColor(textcolor);
      //if(textbgcolor!=pixelbg) bgColor(textbgcolor);

      byte tnp = 0; // Temporary copy of np for while loop
      byte ts = textsize - 1; // Temporary copy of textsize
      // 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
      // w is total number of pixels to plot to fill character block
      while (pc < w)
      {
        line = pgm_read_byte(flash_address);
        flash_address++; // 20 bytes smaller by incrementing here
        if (line & 0x80) {
          line &= 0x7F;
          line++;
          if (ts) {
            px = x + textsize * (pc % width); // Keep these px and py calculations outside the loop as they are slow
            py = y + textsize * (pc / width);
          }
          else {
            px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
            py = y + pc / width;
          }
          
          while (line--) { // In this case the while(line--) is faster
            pc++; // This is faster than putting pc+=line before while()
            setAddrWindow(px, py, px + ts, py + ts);
            fgWrite();
            if (ts) {
              tnp = np;
              // textsize > 1 so pixel count will be a multiple of 4
              while (tnp) { tnp-=4; WR_STB;WR_STB;WR_STB;WR_STB; }
            }
            else { WR_L; WR_H; }

            px += textsize;

            if (px >= (x + width * textsize))
            {
              px = x;
              py += textsize;
            }
          }
        }
        else {
          line++;
          pc += line;
        }
      }
      CS_H;
    }
    else // Text colour != background && textsize = 1
         // so use faster drawing of characters and background using block write
    {

      if(textbgcolor!=pixelbg) bgColor(textbgcolor);

      setAddrWindow(x, y, x + width - 1, y + height - 1);

      // Maximum font size is equivalent to 180x180 pixels in area
      while (w > 0)
      {
        line = pgm_read_byte(flash_address++);
        if (line & 0x80) {
          line &= 0x7F;
          fgWrite();
        }
        else {
          bgWrite();
        }
        line++; w -= line;
        while(line>3){ line-=4; WR_STB; WR_STB; WR_STB; WR_STB; }
        while(line)  { line--;  WR_STB;}
      }
      CS_H;
    }
  }
  // End of RLE font rendering
#endif
  return width * textsize;    // x +
}

/***************************************************************************************
** Function name:           drawString
** Description :            draw string with padding if it is defined
***************************************************************************************/
int16_t TFT_HX8357_Due::drawString(char *string, int16_t poX, int16_t poY, int16_t font)
{
  int16_t sumX = 0;
  uint8_t padding = 1, baseline = 0;
  uint16_t cwidth = 0;
  uint16_t cheight = 0;

#ifdef LOAD_GFXFF
  if (font == 1) {
    if(gfxFont) {
      cheight = glyph_ab * textsize;
      poY += cheight; // Adjust for baseline datum of free fonts
      baseline = cheight;
      padding =101; // Different padding method used for Free Fonts

      // We need to make an adjustment for the botom of the string (eg 'y' character)
      if ((textdatum == BL_DATUM) || (textdatum == BC_DATUM) || (textdatum == BR_DATUM)) {
        cheight += glyph_bb * textsize;
      }
    }
  }
#endif

  if (textdatum || padX)
  {
    // Find the pixel width of the string in the font
    cwidth  = textWidth(string, font);

    // If it is not font 1 (GLCD or free font) get the basline and pixel height of the font
    if (font!=1) {
      baseline = pgm_read_byte( &fontdata[font].baseline ) * textsize;
      cheight = fontHeight(font);
    }

    switch(textdatum) {
      case TC_DATUM:
        poX -= cwidth/2;
        padding += 1;
        break;
      case TR_DATUM:
        poX -= cwidth;
        padding += 2;
        break;
      case ML_DATUM:
        poY -= cheight/2;
        //padding += 0;
        break;
      case MC_DATUM:
        poX -= cwidth/2;
        poY -= cheight/2;
        padding += 1;
        break;
      case MR_DATUM:
        poX -= cwidth;
        poY -= cheight/2;
        padding += 2;
        break;
      case BL_DATUM:
        poY -= cheight;
        //padding += 0;
        break;
      case BC_DATUM:
        poX -= cwidth/2;
        poY -= cheight;
        padding += 1;
        break;
      case BR_DATUM:
        poX -= cwidth;
        poY -= cheight;
        padding += 2;
        break;
      case L_BASELINE:
        poY -= baseline;
        //padding += 0;
        break;
      case C_BASELINE:
        poX -= cwidth/2;
        poY -= baseline;
        //padding += 1;
        break;
      case R_BASELINE:
        poX -= cwidth;
        poY -= baseline;
        padding += 2;
        break;
    }
    // Check coordinates are OK, adjust if not
    if (poX < 0) poX = 0;
    if (poX+cwidth>_width)   poX = _width - cwidth;
    if (poY < 0) poY = 0;
    if (poY+cheight-baseline>_height) poY = _height - cheight;
  }

#ifdef LOAD_GFXFF
  if ((font == 1) && (gfxFont) && (textcolor!=textbgcolor))
    {
      cheight = (glyph_ab + glyph_bb) * textsize;
      fillRect(poX, poY - glyph_ab * textsize, cwidth, cheight, textbgcolor);
      padding -=100;
    }
#endif

  while (*string) sumX += drawChar(*(string++), poX+sumX, poY, font);

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Switch on debugging for the padding areas
//#define PADDING_DEBUG

#ifndef PADDING_DEBUG
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  if((padX>cwidth) && (textcolor!=textbgcolor))
  {
    int16_t padXc = poX+cwidth;
#ifdef LOAD_GFXFF
    if ((font == 1) && (gfxFont))
    {
      poY -= glyph_ab * textsize;
    }
#endif
    switch(padding) {
      case 1:
        fillRect(padXc,poY,padX-cwidth,cheight, textbgcolor);
        break;
      case 2:
        fillRect(padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);
        padXc = (padX-cwidth)>>1;
        if (padXc>poX) padXc = poX;
        fillRect(poX - padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);
        break;
      case 3:
        if (padXc>padX) padXc = padX;
        fillRect(poX + cwidth - padXc,poY,padXc-cwidth,cheight, textbgcolor);
        break;
    }
  }


#else

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv DEBUG vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// This is debug code to show text (green box) and blanked (white box) areas
// It shows that the padding areas are being correctly sized and positioned

  if((padX>sumX) && (textcolor!=textbgcolor))
  {
    int16_t padXc = poX+sumX; // Maximum left side padding
#ifdef LOAD_GFXFF
    if ((font == 1) && (gfxFont)) poY -= glyph_ab;
#endif
    drawRect(poX,poY,sumX,cheight, TFT_GREEN);
    switch(padding) {
      case 1:
        drawRect(padXc,poY,padX-sumX,cheight, TFT_WHITE);
        break;
      case 2:
        drawRect(padXc,poY,(padX-sumX)>>1, cheight, TFT_WHITE);
        padXc = (padX-sumX)>>1;
        if (padXc>poX) padXc = poX;
        drawRect(poX - padXc,poY,(padX-sumX)>>1,cheight, TFT_WHITE);
        break;
      case 3:
        if (padXc>padX) padXc = padX;
        drawRect(poX + sumX - padXc,poY,padXc-sumX,cheight, TFT_WHITE);
        break;
    }
  }
#endif
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ DEBUG ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

return sumX;
}

/***************************************************************************************
** Function name:           drawCentreString
** Descriptions:            draw string centred on dX
***************************************************************************************/
int16_t TFT_HX8357_Due::drawCentreString(char *string, int16_t dX, int16_t poY, int16_t font)
{
  byte tempdatum = textdatum;
  int16_t sumX = 0;
  textdatum = TC_DATUM;
  sumX = drawString(string, dX, poY, font);
  textdatum = tempdatum;
  return sumX;
}

/***************************************************************************************
** Function name:           drawRightString
** Descriptions:            draw string right justified to dX
***************************************************************************************/
int16_t TFT_HX8357_Due::drawRightString(char *string, int16_t dX, int16_t poY, int16_t font)
{
  byte tempdatum = textdatum;
  int16_t sumX = 0;
  textdatum = TR_DATUM;
  sumX = drawString(string, dX, poY, font);
  textdatum = tempdatum;
  return sumX;
}

/***************************************************************************************
** Function name:           drawNumber
** Description:             draw a long integer
***************************************************************************************/
int16_t TFT_HX8357_Due::drawNumber(long long_num, int16_t poX, int16_t poY, int16_t font)
{
  char str[12];
  ltoa(long_num, str, 10);
  return drawString(str, poX, poY, font);
}

/***************************************************************************************
** Function name:           drawFloat
** Descriptions:            drawFloat, prints 7 non zero digits maximum
***************************************************************************************/
// Adapted to assemble and print a string, this permits alignment relative to a datum
// looks complicated but much more compact and actually faster than using print class
int16_t TFT_HX8357_Due::drawFloat(float floatNumber, int16_t dp, int16_t poX, int16_t poY, int16_t font)
{
  char str[14];               // Array to contain decimal string
  uint8_t ptr = 0;            // Initialise pointer for array
  int8_t  digits = 1;         // Count the digits to avoid array overflow
  float rounding = 0.5;       // Round up down delta

  if (dp > 7) dp = 7; // Limit the size of decimal portion

  // Adjust the rounding value
  for (uint8_t i = 0; i < dp; ++i) rounding /= 10.0;

  if (floatNumber < -rounding)    // add sign, avoid adding - sign to 0.0!
  {
    str[ptr++] = '-'; // Negative number
    str[ptr] = 0; // Put a null in the array as a precaution
    digits = 0;   // Set digits to 0 to compensate so pointer value can be used later
    floatNumber = -floatNumber; // Make positive
  }

  floatNumber += rounding; // Round up or down

  // For error put ... in string and return (all TFT_HX8357_Due library fonts contain . character)
  if (floatNumber >= 2147483647) {
    strcpy(str, "...");
    return drawString(str, poX, poY, font);
  }
  // No chance of overflow from here on

  // Get integer part
  uint32_t temp = (uint32_t)floatNumber;

  // Put integer part into array
  ltoa(temp, str + ptr, 10);

  // Find out where the null is to get the digit count loaded
  while ((uint8_t)str[ptr] != 0) ptr++; // Move the pointer along
  digits += ptr;                  // Count the digits

  str[ptr++] = '.'; // Add decimal point
  str[ptr] = '0';   // Add a dummy zero
  str[ptr + 1] = 0; // Add a null but don't increment pointer so it can be overwritten

  // Get the decimal portion
  floatNumber = floatNumber - temp;

  // Get decimal digits one by one and put in array
  // Limit digit count so we don't get a false sense of resolution
  uint8_t i = 0;
  while ((i < dp) && (digits < 9)) // while (i < dp) for no limit but array size must be increased
  {
    i++;
    floatNumber *= 10;       // for the next decimal
    temp = floatNumber;      // get the decimal
    ltoa(temp, str + ptr, 10);
    ptr++; digits++;         // Increment pointer and digits count
    floatNumber -= temp;     // Remove that digit
  }
  
  // Finally we can plot the string and return pixel length
  return drawString(str, poX, poY, font);
}

/***************************************************************************************
** Function name:           setFreeFont
** Descriptions:            Sets the GFX free font to use
***************************************************************************************/

#ifdef LOAD_GFXFF

void TFT_HX8357_Due::setFreeFont(const GFXfont *f) {
  //textdatum = L_BASELINE;
  textfont = 1;
  gfxFont = (GFXfont *)f;

  // Save above baseline (for say H)  and below baseline (for y tail) heights 
  uint16_t uniCode = FF_HEIGHT - pgm_read_byte(&gfxFont->first);
  GFXglyph *glyph1  = &(((GFXglyph *)pgm_read_word(&gfxFont->glyph))[uniCode]);
  glyph_ab = -pgm_read_byte(&glyph1->yOffset);

  uniCode = FF_BOTTOM - pgm_read_byte(&gfxFont->first);
  GFXglyph *glyph2  = &(((GFXglyph *)pgm_read_word(&gfxFont->glyph))[uniCode]);
  glyph_bb = pgm_read_byte(&glyph2->height) + (int8_t)pgm_read_byte(&glyph2->yOffset);
}

#else

// Alternative to setTextFont() so we don't need two different named functions
void TFT_HX8357_Due::setFreeFont(uint8_t font) {
  setTextFont(font);
}

#endif


// #####################################################################################
//  Unfortunately the Due bits allocated to the 16 bit bus are inconveniently scattered
//  around different port bits so we have to jump through a few coding hoops to improve
//  performance.  The following functions attempt to speed things up a bit.
// #####################################################################################


/***************************************************************************************
** Function name:           Setup high byte on 16 bit TFT bus
** Descriptions:            Sets or clears the bits on the bus
***************************************************************************************/

void TFT_HX8357_Due::hi_byte(byte hi)
{
  // Set or clear bits (convenient but may not be fastest way)
  //                                     Port.bit
  if(hi&0x80) REG_PIOD_SODR = 0x1 << 6;  // D.6
  else        REG_PIOD_CODR = 0x1 << 6;

/* Legacy code not used
  if(hi&0x40) REG_PIOD_SODR = 0x1 << 3;  // D.3
  else        REG_PIOD_CODR = 0x1 << 3;
  if(hi&0x20) REG_PIOD_SODR = 0x1 << 2;  // D.2
  else        REG_PIOD_CODR = 0x1 << 2;
  if(hi&0x10) REG_PIOD_SODR = 0x1 << 1;  // D.1
  else        REG_PIOD_CODR = 0x1 << 1;
  if(hi&0x08) REG_PIOD_SODR = 0x1 << 0;  // D.0
  else        REG_PIOD_CODR = 0x1 << 0;
*/
  // Finally have a nice 4 bit group D.3-0, so marginally quicker way to do above
  // Initially done more as an excercise to manipulate groups of bits on Due
  REG_PIOD_OWER = 0x0000000F; // Enable selected bits for writing
  REG_PIOD_OWDR = 0xFFFFFFF0; // Disable all other bits
  REG_PIOD_ODSR = hi>>3;      // Now write the bits, value is masked automatically

  if(hi&0x04) REG_PIOA_SODR = 0x1 << 15; // A.15
  else        REG_PIOA_CODR = 0x1 << 15;
  if(hi&0x02) REG_PIOA_SODR = 0x1 << 14; // A.14
  else        REG_PIOA_CODR = 0x1 << 14;
  if(hi&0x01) REG_PIOB_SODR = 0x1 << 26; // B.26
  else        REG_PIOB_CODR = 0x1 << 26;
}

/***************************************************************************************
** Function name:           Setup low byte on 16 bit TFT bus
** Descriptions:            Sets or clears the bits on the bus
***************************************************************************************/

void TFT_HX8357_Due::lo_byte(byte lo)
{
  // Set or clear bits                   Port.bit
  if(lo&0x80) REG_PIOD_SODR = 0x1 << 9;  // D.9
  else        REG_PIOD_CODR = 0x1 << 9;
  if(lo&0x40) REG_PIOA_SODR = 0x1 << 7;  // A.7
  else        REG_PIOA_CODR = 0x1 << 7;
  if(lo&0x20) REG_PIOD_SODR = 0x1 << 10; // D.10
  else        REG_PIOD_CODR = 0x1 << 10;
  if(lo&0x10) REG_PIOC_SODR = 0x1 << 1;  // C.1
  else        REG_PIOC_CODR = 0x1 << 1;
  if(lo&0x08) REG_PIOC_SODR = 0x1 << 2;  // C.2
  else        REG_PIOC_CODR = 0x1 << 2;
  if(lo&0x04) REG_PIOC_SODR = 0x1 << 3;  // C.3
  else        REG_PIOC_CODR = 0x1 << 3;
  if(lo&0x02) REG_PIOC_SODR = 0x1 << 4;  // C.4
  else        REG_PIOC_CODR = 0x1 << 4;
  if(lo&0x01) REG_PIOC_SODR = 0x1 << 5;  // C.5
  else        REG_PIOC_CODR = 0x1 << 5;

}

/***************************************************************************************
** Function name:           Pre-calculate port values for 16 bit colour write
** Descriptions:            Sets foreground values, used to speed up text rendering
***************************************************************************************/

void TFT_HX8357_Due::fgColor(uint16_t c)
{
  fgA = 0; fgB = 0; fgC = 0; fgD = 0;
  pixelfg = c;

  // High byte                   Port.bit
  if(c&0x8000) fgD |= 0x1 << 6;  // D.6
  if(c&0x4000) fgD |= 0x1 << 3;  // D.3
  if(c&0x2000) fgD |= 0x1 << 2;  // D.2
  if(c&0x1000) fgD |= 0x1 << 1;  // D.1
  if(c&0x0800) fgD |= 0x1 << 0;  // D.0
  if(c&0x0400) fgA |= 0x1 << 15; // A.15
  if(c&0x0200) fgA |= 0x1 << 14; // A.14
  if(c&0x0100) fgB |= 0x1 << 26; // B.26

  // Low byte                   Port.bit
  if(c&0x0080) fgD |= 0x1 << 9;  // D.9
  if(c&0x0040) fgA |= 0x1 << 7;  // A.7
  if(c&0x0020) fgD |= 0x1 << 10; // D.10
  if(c&0x0010) fgC |= 0x1 << 1;  // C.1
  if(c&0x0008) fgC |= 0x1 << 2;  // C.2
  if(c&0x0004) fgC |= 0x1 << 3;  // C.3
  if(c&0x0002) fgC |= 0x1 << 4;  // C.4
  if(c&0x0001) fgC |= 0x1 << 5;  // C.5

}

/***************************************************************************************
** Function name:           Pre-calculate port values for 16 bit colour write
** Descriptions:            Sets background values, used to speed up text rendering
***************************************************************************************/

void TFT_HX8357_Due::bgColor(uint16_t c)
{
  bgA = 0; bgB = 0; bgC = 0; bgD = 0;
  pixelbg = c;

  // High byte                   Port.bit
  if(c&0x8000) bgD |= 0x1 << 6;  // D.6
  if(c&0x4000) bgD |= 0x1 << 3;  // D.3
  if(c&0x2000) bgD |= 0x1 << 2;  // D.2
  if(c&0x1000) bgD |= 0x1 << 1;  // D.1
  if(c&0x0800) bgD |= 0x1 << 0;  // D.0
  if(c&0x0400) bgA |= 0x1 << 15; // A.15
  if(c&0x0200) bgA |= 0x1 << 14; // A.14
  if(c&0x0100) bgB |= 0x1 << 26; // B.26

  // Low byte                    Port.bit
  if(c&0x0080) bgD |= 0x1 << 9;  // D.9
  if(c&0x0040) bgA |= 0x1 << 7;  // A.7
  if(c&0x0020) bgD |= 0x1 << 10; // D.10
  if(c&0x0010) bgC |= 0x1 << 1;  // C.1
  if(c&0x0008) bgC |= 0x1 << 2;  // C.2
  if(c&0x0004) bgC |= 0x1 << 3;  // C.3
  if(c&0x0002) bgC |= 0x1 << 4;  // C.4
  if(c&0x0001) bgC |= 0x1 << 5;  // C.5

}

/***************************************************************************************
** Function name:           Write the common control words CASET, PASET and RAMWR
** Descriptions:            Used to improve performance
***************************************************************************************/

void TFT_HX8357_Due::addrCmd(uint8_t cmd)
{
  //                |       |       |       |         Ruler for byte MS bits 31, 23, 15 and 7
  //                                        v         Marker for register bits used
  REG_PIOA_OWER = 0b00000000000000000000000010000000; // Enable selected bits for writing
  REG_PIOA_OWDR = 0b11111111111111111111111101111111; // Disable all other bits
  REG_PIOA_ODSR = 0b00000000000000000000000000000000; // Now write the bits, value is masked automatically

  //                |       |       |       |         Ruler for byte MS bits 31, 23, 15 and 7
  //                                     vv           Marker for register bits used
  REG_PIOD_OWER = 0b00000000000000000000011000000000; // Enable selected bits for writing
  REG_PIOD_OWDR = 0b11111111111111111111100111111111; // Disable all other bits
  REG_PIOD_ODSR = 0b00000000000000000000010000000000; // Now write the bits, value is masked automatically

  //                                       |       |       |       |         Ruler for bits 31, 23, 15 and 7
  //                                                                 vvvvv   Marker for bits used
  REG_PIOC_OWER =                        0b00000000000000000000000000111110; // Enable selected bits for writing
  REG_PIOC_OWDR =                        0b11111111111111111111111111000001; // Disable all other bits
  if (cmd==HX8357_CASET) REG_PIOC_ODSR = 0b00000000000000000000000000010100; // Now write the CASET bits
  if (cmd==HX8357_PASET) REG_PIOC_ODSR = 0b00000000000000000000000000110100; // Now write the RASET bits
  if (cmd==HX8357_RAMWR) REG_PIOC_ODSR = 0b00000000000000000000000000001100; // Now write the RAMWR bits
}

/***************************************************************************************
** Function name:           Write foreground colour to 16 bit bus
** Descriptions:            Used to improve performance
***************************************************************************************/

void TFT_HX8357_Due::fgWrite(void)
{
  //                |       |       |       |         Ruler for byte MS bits 31, 23, 15 and 7
  //                                vv      v         Marker for register bits used
  REG_PIOA_OWER = 0b00000000000000001100000010000000; // Enable selected bits for writing
  REG_PIOA_OWDR = 0b11111111111111110011111101111111; // Disable all other bits
  REG_PIOA_ODSR = fgA;                                // Now write the bits, value is masked automatically

  //                     v                            Marker for register bits used
  REG_PIOB_OWER = 0b00000100000000000000000000000000; // Enable selected bits for writing
  REG_PIOB_OWDR = 0b11111011111111111111111111111111; // Disable all other bits
  REG_PIOB_ODSR = fgB;                                // Now write the bits, value is masked automatically

  //                                          vvvvv   Marker for register bits used
  REG_PIOC_OWER = 0b00000000000000000000000000111110; // Enable selected bits for writing
  REG_PIOC_OWDR = 0b11111111111111111111111111000001; // Disable all other bits
  REG_PIOC_ODSR = fgC;                                // Now write the bits, value is masked automatically

  //                                     vv  v  vvvv  Marker for register bits used
  REG_PIOD_OWER = 0b00000000000000000000011001001111; // Enable selected bits for writing
  REG_PIOD_OWDR = 0b11111111111111111111100110110000; // Disable all other bits
  REG_PIOD_ODSR = fgD;                                // Now write the bits, value is masked automatically
}

/***************************************************************************************
** Function name:           Write background colour to 16 bit bus
** Descriptions:            Used to improve performance
***************************************************************************************/

void TFT_HX8357_Due::bgWrite(void)
{
  //                |       |       |       |         Ruler for byte MS bits 31, 23, 15 and 7
  //                                vv      v         Marker for register bits used
  REG_PIOA_OWER = 0b00000000000000001100000010000000; // Enable selected bits for writing
  REG_PIOA_OWDR = 0b11111111111111110011111101111111; // Disable all other bits
  REG_PIOA_ODSR = bgA;                                // Now write the bits, value is masked automatically

  //                     v                            Marker for register bits used
  REG_PIOB_OWER = 0b00000100000000000000000000000000; // Enable selected bits for writing
  REG_PIOB_OWDR = 0b11111011111111111111111111111111; // Disable all other bits
  REG_PIOB_ODSR = bgB;                                // Now write the bits, value is masked automatically

  //                                          vvvvv   Marker for register bits used
  REG_PIOC_OWER = 0b00000000000000000000000000111110; // Enable selected bits for writing
  REG_PIOC_OWDR = 0b11111111111111111111111111000001; // Disable all other bits
  REG_PIOC_ODSR = bgC;                                // Now write the bits, value is masked automatically

  //                                     vv  v  vvvv  Marker for register bits used
  REG_PIOD_OWER = 0b00000000000000000000011001001111; // Enable selected bits for writing
  REG_PIOD_OWDR = 0b11111111111111111111100110110000; // Disable all other bits
  REG_PIOD_ODSR = bgD;                                // Now write the bits, value is masked automatically
}


/***************************************************************************************
***************************************************************************************/

// No getTextBounds() as we will only use the GFX fonts for static screen information


//   ORIGINAL LIBRARY HEADER
/*
  This library has been derived from the Adafruit_GFX
  library and the associated driver library. See text
  at the end of this file.

  This library is NOT maintained by Adafruit,
  please do not contact them for support!

*/
/***************************************************
  This is our library for the Adafruit  HX8357 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

 ****************************************************/

/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/