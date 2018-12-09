/*
    Copyright (C)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.
    If not, see <http://www.gnu.org/licenses/>.

    Klabautermann Software
    Uwe Jantzen
    Weingartener Straße 33
    76297 Stutensee
    Germany

    file        contour.h

    date        09.11.2018

    author      Uwe Jantzen (jantzen@klabautermann-software.de)

    brief       Handle the contour device

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#ifndef __CONTOUR_H__
#define __CONTOUR_H__


#define CONTOUR_USB_CODE                    0x6002
#define CONTOUR_USB_NEXT_CODE               0x7410


extern int open_contour( int * contour_type );
extern void close_contour( int handle );
extern int wait_for_contour( int * contour_type );
extern int read_contour( int handle, char * buffer, int size );
extern int write_contour( int handle, char const * buffer, int size );


#endif  // __CONTOUR_H__
