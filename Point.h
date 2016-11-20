/*
*  RaspGFX V0.1  
*  file: Point.h
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

#ifndef POINT_H
#define POINT_H

template<typename T> class Point {
public:
	Point(T x,T y): _x(x), _y(y) { }

	T getX() const { return _x; }
	T getY() const { return _y; }

	void set(T x, T y) { _x = x; _y = y; }
private:
	T _x;
	T _y;
};

#endif