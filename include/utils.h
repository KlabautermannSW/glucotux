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

    file        utils.h

    date        23.11.2019

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#ifndef __UTILS_H__
#define __UTILS_H__


#include "astm.h"


#ifdef _DEBUG_
 #define showbuffer(b,s)                    Showbuffer((b),(s))
#else
 #define showbuffer(b,s)
#endif


extern unsigned int explode( char * elements, char * str, char delimiter, size_t lines, size_t length );
extern int printline( dataset * data, FILE * f );
extern void time2ger( char * dst, char * src );
extern void rotating_bar( void );
extern void showhelp( char * name );
extern void Showbuffer( const char * buffer, size_t size );


#endif  // __UTILS_H__
