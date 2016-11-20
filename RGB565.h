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

#ifndef RGB565_H
#define RGB565_H

#include <cstdint>

class RGB565 {
private:
   uint16_t rgb;
public:
	RGB565(uint8_t red, uint8_t green, uint8_t blue);
	explicit RGB565(uint16_t rgb);
   uint8_t getRed() const;
   uint8_t getGreen() const;
   uint8_t getBlue() const;
	uint16_t get565() const;
   void setRGB(uint8_t red, uint8_t green, uint8_t blue);
	void set565(uint16_t _rgb);
	operator uint16_t() const { return rgb; }
};



inline bool operator != (const RGB565& lhs, const RGB565& rhs) {
	return lhs.get565() != rhs.get565();
}

inline bool operator == (const RGB565& lhs, const RGB565& rhs) {
	return !(lhs != rhs);
}

#endif