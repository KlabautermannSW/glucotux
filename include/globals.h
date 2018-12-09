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

    date        09.11.2018

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


//#include "errors.h"
//#include "debug.h"


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
extern void set_verbose( int flag );
extern void set_debug( int flag );
extern int is_debug( void );
extern void set_cvs_out( int flag );
extern int is_cvs_out( void );
extern void set_reformat( int flag );
extern int is_reformat( void );
extern int set_outfile_name( char * filename );
extern char const *  get_outfile_name( void );
extern int set_infile_name( char * filename, int i );
extern char const *  get_infile_name( int i );
extern int set_infile_number( int i );
extern int const get_infile_number( void );


#endif  // __GLOBALS_H__
