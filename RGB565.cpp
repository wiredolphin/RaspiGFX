/*
*  RaspGFX V0.1  
*  file: RGB565.h
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

#include "RGB565.h"

RGB565::RGB565(uint8_t red, uint8_t green, uint8_t blue) : rgb{0} {
	setRGB(red, green, blue);
}

RGB565::RGB565(uint16_t _rgb) : rgb{_rgb} {
}

uint8_t RGB565:: getRed() const {
   auto r5 = (rgb >> 11) & 0x1F;
   return (r5 << 3) | (r5 >> 2);
}

uint8_t RGB565::getGreen() const {
	auto g6 = (rgb >> 5) & 0x3F;
   return (g6 << 2) | (g6 >> 4);
}

uint8_t RGB565::getBlue() const {
	auto b5 = rgb & 0x1F;
	return (b5 << 3) | (b5 >> 2);
}

void RGB565::setRGB(uint8_t red, uint8_t green, uint8_t blue) {
	rgb = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
}

void RGB565::set565(uint16_t _rgb) { 
	rgb = _rgb; 
}

uint16_t RGB565::get565() const { 
	return rgb; 
}

