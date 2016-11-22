/*
*  RaspGFX V0.1  
*  file: RaspiGFX.cpp
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

#include "RaspiGFX.h"

/*
*  Default constructor.
*/
RaspiGFX::RaspiGFX(FBTftParam* prm) : Framebuffer(prm) {
   textSize = 1;
   gfxFont = NULL;
   wrap = false;
}

/*
*  Experimental.
*/
void RaspiGFX::setRotation(int16_t r) {

   switch(r) {
      case 0:
         fbTftParam->rotate = 0; break;
      case 90:
         fbTftParam->rotate = 90; break;  
      case 180:
         fbTftParam->rotate = 180; break;  
      case 270:
         fbTftParam->rotate = 270; break;
      default: return;
   }

   deleteModule();
   initModule();
   getFramebuffer();
}

/*
*  Exposes current width.
*/
int16_t RaspiGFX::getWidth() {
   return width;
}

/*
*  Exposes current height.
*/
int16_t RaspiGFX::getHeight() {
   return height;
}

/*
*  This is used to 'plot' a pixel in given color.
*  Works correctly only with a 16bpp color depth.
*/
void RaspiGFX::putPixel(uint16_t x, uint16_t y, uint16_t clr) {

   if((x < 0) || (x >= width) || (y < 0) || (y >= height)) return;

   // calculates the pixel's byte offset inside the buffer
   unsigned int pix_offset = x * BYTES_PER_PIXEL + y * fix_info.line_length;

   // his is about the same as 'fbp[pix_offset] = value'
   *((unsigned short*)(fbp + pix_offset)) = clr;
}

/*
*  Fills the screen with given color.
*/
void RaspiGFX::fillScreen(uint16_t clr) {
   fillRect(0, 0, width, height, clr);
}

/*
*  Draws a line in given color.
*  (Uses Bresenham's line algorithm).
*/
void RaspiGFX::drawLine(int x0, int y0, int x1, int y1, int clr) {
   int dx = x1 - x0;
   dx = (dx >= 0) ? dx : -dx; // abs()
   int dy = y1 - y0;
   dy = (dy >= 0) ? dy : -dy; // abs()
   int sx;
   int sy;

   if(x0 < x1)
      sx = 1;
   else
      sx = -1;
   if(y0 < y1)
      sy = 1;
   else
      sy = -1;

   int err = dx - dy;
   int e2;
   int done = 0;

   while(!done) {
      putPixel(x0, y0, clr);
      if ((x0 == x1) && (y0 == y1)) {
         done = 1;
      }
      else {
         e2 = 2 * err;
         if (e2 > -dy) {
            err = err - dy;
            x0 = x0 + sx;
         }
         if (e2 < dx) {
            err = err + dx;
            y0 = y0 + sy;
         }
      }
   }
}

/*
*  Optimized horizontal line-drawing. 
*  Avoids angular calculation.
*/
void RaspiGFX::drawFastHLine(uint16_t x0, uint16_t y0, uint16_t w, uint16_t clr) {
   uint16_t wx = x0 + w;
   while(x0 <= wx) {
      putPixel(x0++, y0, clr);
   }
}

/*
*  Optimized vertical line-drawing. 
*  Avoids angular calculation.
*/
void RaspiGFX::drawFastVLine(uint16_t x0, uint16_t y0, uint16_t h, uint16_t clr) {
   uint16_t hy = y0 + h;
   while(y0 <= hy) {
      putPixel(x0, y0++, clr);
   }
}

/*
*  Draws a rectangle outline in given clr.
*/
void RaspiGFX::drawRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t clr) {
   drawFastHLine(x0, y0, w, clr);     // top
   drawFastVLine(x0, y0, h, clr);     // left
   drawFastHLine(x0, y0 + h, w, clr); // bottom
   drawFastVLine(x0 + w, y0, h, clr); // right
}

/*
*  Fills a rectangle with given clr.
*/
void RaspiGFX::fillRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t clr) {
   for(uint16_t y = 0; y < h; y++) {
      for(uint16_t x = 0; x < w; x++) {
         putPixel(x0 + x, y0 + y, clr);
      }
   }
}

/* 
*  Draws a rounded corners rectangle 
*  outline with given radius and given clr.
*  (Uses Bresenham's circle algorithm).
*/
void RaspiGFX::drawRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t r, uint16_t clr) {
   // radius should not be wider than 
   // half the width or half the height
   r = w / 2 < r ? w / 2 : r;
   r = h / 2 < r ? h / 2 : r;
   // repositions rectangle
   x0 = x0 + r;
   y0 = y0 + r;
   int x = r;
   int y = 0;
   int radiusError = 1 - x;

   while(x >= y) {
      // top left
      putPixel(-y + x0, -x + y0, clr);
      // upper middle left
      putPixel(-x + x0, -y + y0, clr);
      // lower middle left
      putPixel(-x + x0, y + y0 + h - 2 * r, clr);
      // bottom left
      putPixel(-y + x0, x + y0 + h - 2 * r - 1, clr);
      // top right
      putPixel(w + y + x0 - (2 * r) - 1, -x + y0, clr);
      // upper middle right
      putPixel(w + x + x0 - (2 * r) - 1, -y + y0, clr);
      // lower middle right
      putPixel(w + x + x0 - (2 * r) - 1, y + y0 + h - 2 * r, clr);       
      // bottom right
      putPixel(w + y + x0 - (2 * r) - 1, x + y0 + h - 2 * r - 1, clr);

      y++;
      if(radiusError < 0) {
         radiusError += 2 * y + 1;
      } else {
         x--;
         radiusError += 2 * (y - x + 1);
      }
   }

   //top
   for(x = x0; x < w + x0 - 2 * r - 1; x++)
      putPixel(x, y0 - r, clr);
   // right
   for(y = y0; y < y0 + h - 2 * r; y++)
      putPixel(x0 + w - r - 1, y, clr);
   //bottom
   for(x = x0; x < w + x0 - 2 * r; x++)
      putPixel(x, y0 + h - r - 1, clr);
   // left
   for(y = y0; y < y0 + h - 2 * r; y++)
      putPixel(x0 - r, y, clr);
}

/* 
*  Draws a rounded corners rectangle with 
*  given radius and given color.
*  (Uses Bresenham's circle algorithm).
*/
void RaspiGFX::fillRoundRect(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint16_t r, uint16_t clr) {
   // radius should not be wider than 
   // half the width or half the height
   r = w / 2 < r ? w / 2 : r;
   r = h / 2 < r ? h / 2 : r;
   // repositions rectangle
   x0 = x0 + r;
   y0 = y0 + r;
   int x = r;
   int y = 0, i = 0;
   int radiusError = 1 - x;

   while(x >= y) {

      // top left
      for(i = -y + x0; i < (w + y + x0 - (2 * r)); i++) 
         putPixel(i, -x + y0, clr);
      // upper middle left
      for(i = -x + x0; i < (w + x + x0 - (2 * r)); i++) 
         putPixel(i, -y + y0, clr);
      // lower middle left
      for(i = -x + x0; i < (w + x + x0 - (2 * r)); i++) 
         putPixel(i, (h - 2 * r) + y + y0, clr);
      // bottom left
      for(i = -y + x0; i < (w + y + x0 - (2 * r)); i++) 
         putPixel(i, (h - 2 * r) + x + y0 - 1, clr);

      y++;
      if(radiusError < 0) {
         radiusError += 2 * y + 1;
      } else {
         x--;
         radiusError += 2 * (y - x + 1);
      }
   }
   // middle rect
   for(y = y0; y < (h + y0 - 2 * r); y++) {
      for(x = x0 - r; x < w + x0 - r; x++) {
         putPixel(x, y, clr);
      }
   }
}

/* 
*  Draws a triangle 
*  outline in given color.
*  (Uses Bresenham's line algorithm).
*/
void RaspiGFX::drawTriangle(uint16_t x0, uint16_t y0, 
                            uint16_t x1, uint16_t y1, 
                            uint16_t x2, uint16_t y2, 
                            uint16_t clr) {

   drawLine(x0, y0, x1, y1, clr);
   drawLine(x1, y1, x2, y2, clr);
   drawLine(x2, y2, x0, y0, clr);
}

/* 
*  Draws a polygon with given points array
*  and given color.
*/
void RaspiGFX::drawPolygon(point16 points[], int16_t len, uint16_t clr) {

   for(int16_t i = 0; i < len; i++) {
      if(i > 0) {
         point16 p1 = points[i - 1];
         point16 p2 = points[i];
         drawLine(p1.getX(), p1.getY(), p2.getX(), p2.getY(), clr);
      }
   }
}

/* 
*  Fills a polygon with given points array
*  and given color.
*/
void RaspiGFX::fillPolygon(point16 points[], int16_t len, uint16_t clr) {

   for(int16_t i = 0; i < len; i++) {
      if(i > 1) {
         point16 p1 = points[i - 2];
         point16 p2 = points[i - 1];
         point16 p3 = points[i];
         fillTriangle(p1.getX(), p1.getY(), p2.getX(), p2.getY(), p3.getX(), p3.getY(), clr);
      }
   }
}

/* 
*  Fills a triangle in given color. 
*  Credit to Adafruit_GFX library for this.
*  (https://github.com/adafruit/Adafruit-GFX-Library).
*/
void RaspiGFX::fillTriangle(uint16_t x0, uint16_t y0, 
                            uint16_t x1, uint16_t y1, 
                            uint16_t x2, uint16_t y2, 
                            uint16_t clr) {

   int16_t a, b, y, last;

   // Sort coordinates by Y order (y2 >= y1 >= y0)
   if(y0 > y1) {
      swap_int16_t(y0, y1); 
      swap_int16_t(x0, x1);
   }
   if(y1 > y2) {
      swap_int16_t(y2, y1); 
      swap_int16_t(x2, x1);
   }
   if(y0 > y1) {
      swap_int16_t(y0, y1); 
      swap_int16_t(x0, x1);
   }

   // Handle awkward all-on-same-line case as its own thing
   if(y0 == y2) { 
      a = b = x0;
      if(x1 < a)      
         a = x1;
      else if(x1 > b) 
         b = x1;
      if(x2 < a)      
         a = x2;
      else if(x2 > b) 
         b = x2;
      drawFastHLine(a, y0, b - a + 1, clr);
      return;
   }

   // Upper part of triangle:
   // find scanline crossings for segments 0-1 and 0-2. 
   // If y1=y2 (flat-bottomed triangle), the scanline y1
   // is included here (and second loop will be skipped, avoiding a /0
   // error there), otherwise scanline y1 is skipped here and handled
   // in the second loop...which also avoids a /0 error here if y0=y1
   // (flat-topped triangle).
   if(y1 == y2) 
      last = y1;   // Include y1 scanline
   else         
      last = y1 - 1; // Skip it

   for(y = y0; y <= last; y++) {
      a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
      b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);

      if(a > b) 
         swap_int16_t(a,b);
      drawFastHLine(a, y, b - a + 1, clr);
   }

   // Lower part of triangle:
   // find scanline crossings for segments 0-2 and 1-2.  
   //This loop is skipped if y1=y2.
   for(; y <= y2; y++) {
      a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
      b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);

      if(a > b) 
         swap_int16_t(a, b);
      drawFastHLine(a, y, b - a + 1, clr);
   }
}

/* 
*  Draws a circle outline in given clr.
*  (Uses Bresenham's circle algorithm).
*/
void RaspiGFX::drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t clr)
{
   int x = r;
   int y = 0;
   int radiusError = 1 - x;

   while(x >= y) {
      // top left
      putPixel(-y + x0, -x + y0, clr);
      // top right
      putPixel(y + x0, -x + y0, clr);
      // upper middle left
      putPixel(-x + x0, -y + y0, clr);
      // upper middle right
      putPixel(x + x0, -y + y0, clr);
      // lower middle left
      putPixel(-x + x0, y + y0, clr);
      // lower middle right
      putPixel(x + x0, y + y0, clr);
      // bottom left
      putPixel(-y + x0, x + y0, clr);
      // bottom right
      putPixel(y + x0, x + y0, clr);

      y++;
      if(radiusError < 0) {
         radiusError += 2 * y + 1;
      } else {
         x--;
         radiusError += 2 * (y - x + 1);
      }
   }
}

/* 
*  Draws a filled circle in given color.
*  (Uses Bresenham's circle algorithm).
*/
void RaspiGFX::fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t clr) {
   int x = r;
   int y = 0;
   int radiusError = 1 - x;

   while(x >= y) {
      // top
      drawLine(-y + x0, -x + y0, y + x0, -x + y0, clr);
      // upper middle
      drawLine(-x + x0, -y + y0, x + x0, -y + y0, clr);
      // lower middle
      drawLine(-x + x0, y + y0, x + x0, y + y0, clr);
      // bottom 
      drawLine(-y + x0, x + y0, y + x0, x + y0, clr);

      y++;
      if(radiusError < 0) {
         radiusError += 2 * y + 1;
      } else {
         x--;
         radiusError += 2 * (y - x + 1);
      }
   }
}

/* 
*  Draws given byte array bitmap
*  in given color.
*/
void RaspiGFX::drawBitmap(const uint8_t *bitmap, 
                          int16_t x, int16_t y,
                          int16_t w, int16_t h, 
                          uint16_t clr) {

   int16_t i, j, byteWidth = (w + 7) / 8;
   uint8_t byte;

   for(j = 0; j < h; j++) {
      for(i = 0; i < w; i++) {
         if(i & 7) 
            byte <<= 1;
         else 
            byte = bitmap[j * byteWidth + i / 8];
         if(byte & 0x80) 
            putPixel(x + i, y + j, clr);
      }
   }
}

/* 
*  Draws given byte array bitmap
*  in given color and given background.
*/
void RaspiGFX::drawBitmap(const uint8_t *bitmap, 
                          int16_t x, int16_t y,
                          int16_t w, int16_t h, 
                          uint16_t clr, 
                          uint16_t bg) {

   int16_t i, j, byteWidth = (w + 7) / 8;
   uint8_t byte;

   for(j = 0; j < h; j++) {
      for(i = 0; i < w; i++) {
         if(i & 7) 
            byte <<= 1;
         else 
            byte = bitmap[j * byteWidth + i / 8];
         if(byte & 0x80) 
            putPixel(x + i, y + j, clr);
         else            
            putPixel(x + i, y + j, bg);
      }
   }
}

/* 
*  Returns current position coordinates.
*/
point16 RaspiGFX::getCursorPosition() {
   return point16(cursor_x, cursor_y);
}

/* 
*  Sets current cursor position.
*/
void RaspiGFX::setCursorPosition(int16_t x, int16_t y) {
   cursor_x = x;
   cursor_y = y;
}

/* 
*  Sets current font. The given one 
*  will be used for the next string printing.
*/
void RaspiGFX::setFont(const GFXfont *f) {
   if(f) { // Font struct pointer passed in?
      if(!gfxFont) { // And no current font struct?
         // Switching from classic to new font behavior.
         // Move cursor pos down 6 pixels so it's on baseline.
         cursor_y += 6;
      }
   } else if(gfxFont) { // NULL passed.  Current font struct defined?
      // Switching from new to classic font behavior.
      // Move cursor pos up 6 pixels so it's at top-left of char.
      cursor_y -= 6;
   }
   gfxFont = (GFXfont *)f;
}

/* 
*  Sets text size.
*/
void RaspiGFX::setTextSize(uint8_t s) {
   textSize = (s > 0) ? s : 1;
}

/* 
*  Sets text to given color.
*/
void RaspiGFX::setTextColor(uint16_t textClr) {
   textColor = textClr;
}

/* 
*  Sets text to given color,
*  and text background to given color.
*/
void RaspiGFX::setTextColor(uint16_t textClr, uint16_t txtBgClr) {
   textColor = textClr;
   textBgColor = txtBgClr;
}

/* 
*  Sets wether text should be wrapped.
*/
void RaspiGFX::setTextWrap(bool w) {
   wrap = w;
}

/* 
*  Prints the given string, character after character.
*/
void RaspiGFX::print(const char *str) {
   while(*str) {
      putChar(*str++);
   }
}

/* 
*  Prints the given string, character after character.
*  This overloaded version also modifies current 
*  text color and current text background color.
*  It also sets the current cursor position.
*/
void RaspiGFX::print(const char *str, 
                     int16_t x, 
                     int16_t y, 
                     uint16_t clr, 
                     uint16_t txtBgClr) {
   setCursorPosition(x, y);
   setTextColor(clr, txtBgClr);
   while(*str) {
      putChar(*str++);
   }
}

/* 
*  Puts the given char, using default font or if set,
*  a custom font, taking account of text wrapping and
*  screen size.
*  Credit to Adafruit_GFX library for this.
*  (https://github.com/adafruit/Adafruit-GFX-Library).
*/
void RaspiGFX::putChar(uint8_t c) {

   if(!gfxFont) { // built-in font

      if(c == '\n') {
         cursor_y += textSize * 8;
         cursor_x  = 0;
      } else if(c == '\r') {
         // skip
      } else {
         if(wrap && ((cursor_x + textSize * 6) >= width)) { // Heading off edge?
            cursor_x = 0;            // Reset x to zero
            cursor_y += textSize * 8; // Advance y one line
         }
         drawChar(cursor_x, cursor_y, c, textColor, textBgColor, textSize);
         cursor_x += textSize * 6;
      }

   } else { // custom font

      if(c == '\n') {

         cursor_x  = 0;
         cursor_y += (int16_t)textSize * gfxFont->yAdvance;

      } else if(c != '\r') {

         uint8_t first = gfxFont->first;

         if((c >= first) && (c <= gfxFont->last)) {

            uint8_t c2 = c - gfxFont->first;
            GFXglyph *glyph = &gfxFont->glyph[c2];
            uint8_t w = glyph->width,
            h = glyph->height;

            if((w > 0) && (h > 0)) { // Is there an associated bitmap?
               int16_t xo = glyph->xOffset;
               if(wrap && ((cursor_x + textSize * (xo + w)) >= width)) {
                  // Drawing character would go off right edge; wrap to new line
                  cursor_x  = 0;
                  cursor_y += (int16_t)textSize * gfxFont->yAdvance;
               }
               drawChar(cursor_x, cursor_y, c, textColor, textBgColor, textSize);
            }
            cursor_x += glyph->xAdvance * textSize;
         }
      }
   }
}

/* 
*  Draws the given char, using default font or if set,
*  a custom font (text size must be 1). The character should
*  be previously filtered by putChar() to eliminate newlines,
*  returns, non-printable characters, etc. Calling drawChar()
*  directly with 'bad' characters of font may cause mayhem!
*  Credit to Adafruit_GFX library for this.
*  (https://github.com/adafruit/Adafruit-GFX-Library).
*/
void RaspiGFX::drawChar(int16_t x, int16_t y, 
                        unsigned char c,
                        uint16_t clr, uint16_t txtBgClr, 
                        uint8_t size) {

   if(!gfxFont) { // 'Classic' built-in font

      if((x >= width) ||            // Clip right
         (y >= height) ||           // Clip bottom
         ((x + 6 * size - 1) < 0) || // Clip left
         ((y + 8 * size - 1) < 0))   // Clip top
         return;

      if(c >= 176) c++; // Handle 'classic' charset behavior

      for(int8_t i = 0; i < 6; i++) {
         uint8_t line;
         if(i < 5) 
            line = *(gfxDefaultFont + (c * 5) + i);
         else
            line = 0x0;
         for(int8_t j = 0; j < 8; j++, line >>= 1) {
            if(line & 0x1) {
               if(size == 1) 
                  putPixel(x + i, y + j, clr);
               else 
                  fillRect(x + (i * size), y + (j * size), size, size, clr);
            } else if(txtBgClr != clr) {
               if(size == 1) 
                  putPixel(x + i, y + j, txtBgClr);
               else          
                  fillRect(x + i * size, y + j * size, size, size, txtBgClr);
            }
         }
      }

   } else { // Custom font

      c -= gfxFont->first;
      GFXglyph *glyph = &gfxFont->glyph[c];
      uint8_t *bitmap = (uint8_t *)gfxFont->bitmap;

      uint16_t bo = glyph->bitmapOffset;
      uint8_t w = glyph->width,
      h = glyph->height;
      int8_t xo = glyph->xOffset,
      yo = glyph->yOffset;
      uint8_t xx = 0, yy = 0, bits = 0, bit = 0;
      int16_t xo16 = 0, yo16 = 0;

      if(size > 1) {
         xo16 = xo;
         yo16 = yo;
      }

      for(yy = 0; yy < h; yy++) {
         for(xx = 0; xx < w; xx++) {
            if(!(bit++ & 7)) {
               bits = bitmap[bo++];
            }
            if(bits & 0x80) {
               if(size == 1) {
                  putPixel(x + xo + xx, y + yo + yy, clr);
               } else {
                  fillRect(x + (xo16+xx) * size, y + (yo16+yy) * size, size, size, clr);
               }
            }
            bits <<= 1;
         }
      }
   } 
}

/* 
*  Provides bounds of given text. Useful for
*  filling the area when drawing new text on
*  the same position is required.
*  Credit to Adafruit_GFX library for this.
*  (https://github.com/adafruit/Adafruit-GFX-Library).
*
*  NOTE: currently this does not work correctly
*  with built in font.
*/
void RaspiGFX::getTextBounds(const char *str, 
                             int16_t x, int16_t y,
                             int16_t *x1, int16_t *y1, 
                             uint16_t *w, uint16_t *h, 
                             uint8_t * glyphWidths) {

   uint8_t c; // Current character

   *x1 = x;
   *y1 = y;
   *w  = *h = 0;

   if(gfxFont) {

      GFXglyph *glyph;
      uint8_t first = gfxFont->first,
              last  = gfxFont->last,
              gw, gh, xa;
      int8_t xo, yo;
      int16_t minx = width, miny = height, maxx = -1, maxy = -1,
              gx1, gy1, gx2, gy2, ts = textSize,
              ya = ts * gfxFont->yAdvance;

      while((c = *str++)) {
         if(c != '\n') { // Not a newline
            if(c != '\r') { // Not a carriage return, is normal char
               if((c >= first) && (c <= last)) { // Char present in current font
                  c    -= first;
                  glyph = &gfxFont->glyph[c];
                  gw    = glyph->width;

                  gh    = glyph->height;
                  xa    = glyph->xAdvance;
                  xo    = glyph->xOffset;
                  yo    = glyph->yOffset;

                  if(wrap && ((x + (((int16_t)xo + gw) * ts)) >= width)) {
                     // Line wrap
                     x  = 0;  // Reset x to 0
                     y += ya; // Advance y by 1 line
                  }
                  gx1 = x   + xo * ts;
                  gy1 = y   + yo * ts;
                  gx2 = gx1 + gw * ts - 1;
                  gy2 = gy1 + gh * ts - 1;
                  if(gx1 < minx) minx = gx1;
                  if(gy1 < miny) miny = gy1;
                  if(gx2 > maxx) maxx = gx2;
                  if(gy2 > maxy) maxy = gy2;
                  x += xa * ts;

                  if(glyphWidths != NULL) 
                     *glyphWidths++ = xa * ts;
               }
            } // Carriage return = do nothing
         } else { // Newline
            x  = 0;  // Reset x
            y += ya; // Advance y by 1 line
         }
      }
      // End of string
      *x1 = minx;
      *y1 = miny;
      if(maxx >= minx) *w  = maxx - minx + 1;
      if(maxy >= miny) *h  = maxy - miny + 1;

   } else { // Default font

      uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines

      while((c = *str++)) {
         if(c != '\n') { // Not a newline
            if(c != '\r') { // Not a carriage return, is normal char
               if(wrap && ((x + textSize * 6) >= width)) {
                  x  = 0;            // Reset x to 0
                  y += textSize * 8; // Advance y by 1 line
                  if(lineWidth > maxWidth) 
                     maxWidth = lineWidth; // Save widest line
                  lineWidth  = textSize * 6; // First char on new line
               } else { // No line wrap, just keep incrementing X
                  lineWidth += textSize * 6; // Includes interchar x gap
               }
               if(glyphWidths != NULL) 
                  *glyphWidths++ = textSize * 6;
            } // Carriage return = do nothing
         } else { // Newline
            x = 0;            // Reset x to 0
            y += textSize * 8; // Advance y by 1 line
            if(lineWidth > maxWidth) 
               maxWidth = lineWidth; // Save widest line
            lineWidth = 0;     // Reset lineWidth for new line
         }
      }
      // End of string
      if(lineWidth) 
         y += textSize * 8; // Add height of last (or only) line
      *w = maxWidth - 1;               // Don't include last interchar x gap
      *h = y - *y1;
   } // End classic vs custom font
}







