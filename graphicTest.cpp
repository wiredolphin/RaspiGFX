/*
*  RaspGFX V0.1  
*  file: graphicTest.cpp
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

#include <cstdlib>
#include "RaspiGFX.h"

// Include some font.
#include "BitmapFonts/FreeMono18pt7b.h"
#include "BitmapFonts/FreeMono24pt7b.h"
#include "BitmapFonts/FreeMonoBold24pt7b.h"
#include "BitmapFonts/FreeSans24pt7b.h"
#include "BitmapFonts/DigitalWF24pt7b.h"
#define DELAY 800000


void randomColorBarsRight(RaspiGFX rgfx) {  
    srand(time(NULL));
    for(int16_t i = 0; i < rgfx.getWidth(); i += 20) {
        rgfx.fillRect(i, 0, 20, rgfx.getHeight(), RGB565(rand() % 0xffff));
        usleep(100000);
    }
}

void randomColorBarsLeft(RaspiGFX rgfx) {
    srand(time(NULL));
    for(int16_t i = rgfx.getWidth() - 20; i >= 0; i -= 20) {
        rgfx.fillRect(i, 0, 20, rgfx.getHeight(), RGB565(rand() % 0xffff));
        usleep(100000);
    }
}

void typeWriting(RaspiGFX rgfx, int16_t speed, int16_t x, int16_t y, const char * str, int16_t clr) {
    int16_t len = strlen(str) + 1;
    char buffer[len];
    for(int16_t i = 1; i < len; i++) {
        memcpy(buffer, &str[0], i);
        buffer[i] = '\0';
        rgfx.print(buffer, x, y, clr, BLACK);
        usleep(speed * 1000);
    } 
}

void countdownAndRotate(RaspiGFX& rgfx, int16_t rotate) {
    int16_t x1, y1;
    uint16_t w, h;
    rgfx.setFont(&DigitalWF24pt7b);
    char countBuffer[3];
    for(int8_t i = 3; i > 0; i--) {
        snprintf(countBuffer, 3, "%.2d", i);
        rgfx.print(countBuffer, 40, 80, WHITE, BLACK); 
        usleep(1000000);
        // clear the previous text
        rgfx.getTextBounds(countBuffer, 40, 80, &x1, &y1, &w, &h);
        rgfx.fillRect(x1, y1, w, h, BLACK);
    }
    rgfx.print("00", 40, 80, WHITE, BLACK);
    rgfx.setFont(NULL);
    rgfx.print("Rotating the screen...", 40, 120, WHITE, BLACK); 
    usleep(DELAY * 3);
    rgfx.setRotation(rotate);
}

int main(int argc, char* argv[]) {

    int16_t i, x1, y1;
    uint16_t w, h;

    // Prepare fbtft module parameters
    // for the appropriate device
    FBTftParam prm;
    prm.device_name = "adafruit22a"; //ili_9340
    prm.rotate = 90;
    prm.speed = 32;

    // create the GFX object
	RaspiGFX rgfx(&prm);

    // get screen geometry
    int16_t screenW = rgfx.getWidth();
    int16_t screenH = rgfx.getHeight();

    // fill the screen with RGB colors
	rgfx.fillScreen(BLACK); usleep(DELAY);
    rgfx.fillScreen(RED); usleep(DELAY);
    rgfx.fillScreen(BLUE); usleep(DELAY);
    rgfx.fillScreen(GREEN); usleep(DELAY);
    rgfx.fillScreen(BLACK);

    // draw some random color bars
    randomColorBarsRight(rgfx); usleep(DELAY);
    rgfx.fillScreen(BLACK); usleep(DELAY);

    randomColorBarsLeft(rgfx); usleep(DELAY);
    rgfx.fillScreen(BLACK); usleep(DELAY);

    // write some text at the middle of the screen
    rgfx.setFont(&FreeMono18pt7b);
    const char * text = "Hello World!";
    rgfx.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    typeWriting(rgfx, 
        150,
        (screenW - w) / 2, // x
        ((screenH - h) / 2) + h / 2, // y
        text, WHITE);
    usleep(DELAY);

    // drow some oblique lines
    RGB565 color(92, 254, 245);
    for(i = 10 ; i <= screenW; i += 10) {
        rgfx.drawLine(0, screenH, i, 0, color);
        usleep(20000);
    }
    for(i = 10; i <= screenH; i += 10) {
        rgfx.drawLine(0, screenH, screenW, i, color);
        usleep(20000);
    }
    rgfx.fillScreen(BLACK); usleep(DELAY);

    // switch to default font
    rgfx.setFont(NULL);
    rgfx.setTextSize(1);
    rgfx.print("Default font.", 10, 20, WHITE, BLACK); usleep(150000);
    rgfx.setTextSize(2);
    rgfx.print("Default font.", 10, 40, WHITE, BLACK); usleep(150000);
    rgfx.setTextSize(3);
    rgfx.print("Default font.", 10, 70, WHITE, BLACK); usleep(150000);
    rgfx.setTextSize(4);
    rgfx.print("Default font.", 10, 110, WHITE, BLACK); usleep(2000000);
    rgfx.setTextSize(1);
    rgfx.fillScreen(BLACK); usleep(DELAY);

    // test screen orientation
    countdownAndRotate(rgfx, 0);
    countdownAndRotate(rgfx, 270);
    countdownAndRotate(rgfx, 180);
    countdownAndRotate(rgfx, 90);

    // draw and fill some polygons
    point16 p[] = {{160, 140}, {240, 140}, {280, 200}, {120, 200}, {160, 140}};

    rgfx.drawRoundRect(20, 40, 100, 60, 8, RED); usleep(DELAY);
    rgfx.drawTriangle(140, 100, 240, 100, 190, 40, GREEN); usleep(DELAY);
    rgfx.drawCircle(60, 160, 40, BLUE); usleep(DELAY);
    rgfx.drawPolygon(p, 5, WHITE); usleep(DELAY);
    usleep(1000000);
    rgfx.fillRoundRect(20, 40, 100, 60, 8, RED);
    rgfx.fillTriangle(140, 100, 240, 100, 190, 40, GREEN);
    rgfx.fillCircle(60, 160, 40, BLUE);
    rgfx.fillPolygon(p, 5, WHITE);
    usleep(2000000);

    // fast drawing lines
    color = RGB565(92, 254, 245);
    for(i = 10; i < screenW; i += 10) {
        rgfx.drawFastVLine(i, 0, screenH, color);
        usleep(30000);
    }
    color = RGB565(255, 0, 0);
    for(i = 10; i < screenH; i += 10) {
        rgfx.drawFastHLine(0, i, screenW, color);
        usleep(50000);
    }
    usleep(DELAY);

    // write final text at the middle of the screen
    rgfx.fillScreen(BLACK); usleep(DELAY);
    color = RGB565(0, 255, 0);
    rgfx.setFont(&FreeMonoBold24pt7b);
    const char * text2 = "RaspiGFX";
    rgfx.getTextBounds(text2, 0, 0, &x1, &y1, &w, &h);
    int16_t x = (screenW - w) / 2;
    int16_t y = ((screenH - h) / 2) + h / 2;
    typeWriting(rgfx, 80, x, y, text2, GREEN);
    usleep(DELAY);

    // animate underline
    for(i = x; i <= w; i++) {
        rgfx.drawFastHLine(x, y + 16, i, RED);
        usleep(1000);
    }
    
    return 0;
    
}


