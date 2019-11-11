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

    ERRORS      errors.c

    date        10.11.2019

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief       Defines application specific erros.

    details     All application specific error codes are negative integers.

    project     glucotux
    target      Linux
    begin       27.03.2012

    note

    todo

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"


static char * errors[] =
    {
    "Buffer too small",
    "No contour device found",
    "Vendor code and product code do not match",
    "Error when reading from contour device",
    "Error when writing to contour device",
    "File name too long",
    "Can not open output file",
    "Can not open input file",
    "No STX found in frame",
    "No frame ternimation byte found",
    "Illegal frame number",
    "Message terminator missing",
    "Message checksum error",
    "No output file name given",
    "Unknown data file line format",
    "Data line does not contain the expected number of elements",
    "Not enough memory to store data",
    "Error when writing to a file",
    "Number of input files out of range [0 .. 2]",
    "Unit string read from meter device is longer than expected"
    };


/*  function        void showerr( int error )

    brief           Shows error description if available.

    param[in]       int error, error code
*/
void showerr( int error )
    {
    if( error == NOERR )
        return;

    if( error > 0 )
        {
        fprintf(stderr, "\nError %3d : %s\n", error, strerror(error));
        }
    else
        {
        int idx = -1 * error;
        if( idx > sizeof(errors)/sizeof(errors[0]) )
            fprintf(stderr, "\nError %3d : unknown error code !\n", error);
        else
            fprintf(stderr, "\nError %3d : %s\n", error, errors[--idx]);
        }
    }
