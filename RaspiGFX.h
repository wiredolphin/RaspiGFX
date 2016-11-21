/*
*  RaspGFX V0.1  
*  file: RaspiGFX.h
*  
*  This file is part of the RaspiGFX project, a graphic library
*  for the use of small TFT LCD display modules (like the ili9340)
*  on Raspberry. This software contains portions of code produced
*  by Adafruit Industries. (For the original code and related license
*  see https://github.com/adafruit/Adafruit-GFX-Library)
*
*  Copyright (C) 2016 Vince Damiani
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef _RASPIGFX_H_
#define _RASPIGFX_H_

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <system_error>

#include "Framebuffer.h"
#include "RGB565.h"
#include "Point.h"
#include "gfxdfont.h"

#ifndef swap_int16_t
#define swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#define PROGMEM

// default framebuffer palette
typedef enum {
	BLACK        = 0x0000,
	BLUE         = 0x001F,
	GREEN        = 0x07E0,
	CYAN         = 0x07FF,
	RED          = 0xF800,
	MAGENTA      =	0xF81F,
	YELLOW       = 0xFFE0,
	WHITE        = 0xFFFF
} COLOR_INDEX_T;




typedef struct { // Data stored PER GLYPH
	uint16_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint8_t   first, last; // ASCII extents
	uint8_t   yAdvance;    // Newline distance (y axis)
} GFXfont;

typedef Point<int16_t> point16;

class RaspiGFX : public Framebuffer {
private:
	GFXfont * gfxFont;
	const int8_t BYTES_PER_PIXEL = 2;
	uint16_t
		textColor,
		textBgColor;
	int16_t
  		cursor_x, 
	  	cursor_y,
	  	textSize;
	bool wrap;
public:
	RaspiGFX(FBTftParam* prm);
	void putPixel(uint16_t x, uint16_t y, uint16_t clr),
		setRotation(int16_t r),
		fillScreen(uint16_t clr),
		drawLine(int x0, int y0, int x1, int y1, int clr),
		drawFastHLine(uint16_t x0, uint16_t y0, uint16_t w, uint16_t clr),
		drawFastVLine(uint16_t x0, uint16_t y0, uint16_t h, uint16_t clr),	
		drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t clr),
		fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t clr),
		drawRoundRect(uint16_t x0, uint16_t y0, 
			uint16_t w, uint16_t h, uint16_t r, uint16_t clr),
		fillRoundRect(uint16_t x0, uint16_t y0, 
			uint16_t w, uint16_t h, uint16_t r, uint16_t clr),
		drawTriangle(uint16_t x0, uint16_t y0, 
			uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t clr),
		fillTriangle(uint16_t x0, uint16_t y0, 
			uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t clr),
		fillPolygon(point16 points[], int16_t len, uint16_t clr),
		drawPolygon(point16 points[], int16_t len, uint16_t clr),
		drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t clr),
		fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t clr),
		drawBitmap(const uint8_t * bitmap, int16_t x, int16_t y, 
			int16_t w, int16_t h, uint16_t clr),
		drawBitmap(const uint8_t * bitmap, int16_t x, int16_t y, 
			int16_t w, int16_t h, uint16_t clr, uint16_t bg),
		setFont(const GFXfont *f),
		setTextSize(uint8_t s),
		setTextColor(uint16_t textClr),
		setTextColor(uint16_t textClr, uint16_t textBgClr),
		drawChar(int16_t x, int16_t y, unsigned char c, 
			uint16_t color, uint16_t textBgClr, uint8_t size),
		putChar(uint8_t c),
		setTextWrap(bool w),
		print(const char *str),
		print(const char *str, int16_t x, int16_t y, uint16_t clr, uint16_t textBgClr),
		setCursorPosition(int16_t x, int16_t y),
		getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, 
			uint16_t *w, uint16_t *h, uint8_t * glyphWidths = NULL);
	point16 getCursorPosition(void);
	int16_t 
		getWidth(),
		getHeight();
};





#endif








