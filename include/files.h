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

    file        files.h

    date        09.11.2018

    author      Uwe Jantzen (jantzen@klabautermann-software.de)

    brief       Merge data files (current and older format)

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#ifndef __FILES_H__
#define __FILES_H__


extern void mixfiles( char const * infile_name, char const * outfile_name );
extern void csvformat( char const * infile_name, char const * outfile_name );
extern void reformat( char const * infile_name, char const * outfile_name );


#endif  // __FILES_H__
