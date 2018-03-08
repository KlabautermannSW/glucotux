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

    file        globals.h

    date        08.03.2018

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief       Variables and functions that are needed globally in the
                project.

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#ifndef __GLOBALS_H__
#define __GLOBALS_H__


#include "errors.h"
#include "debug.h"


#define STX                                 0x02
#define ETX                                 0x03
#define EOT                                 0x04
#define ENQ                                 0x05
#define ACK                                 0x06
#define BS                                  0x08
#define LF                                  0x0a
#define CR                                  0x0d
#define NAK                                 0x15
#define ETB                                 0x17


extern int is_verbose( void );
extern int is_debug( void );
extern void set_verbose( int flag );
extern void set_debug( int flag );
extern int set_logfile_name( char * filename );
extern const char *  get_logfile_name( void );


#endif  // __GLOBALS_H__
