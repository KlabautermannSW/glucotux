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

    date        01.04.2018

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief       Show error message to stderr.

    details     Numbering starts at 256 (0x100) to prevent mangling with
                Linux's internal errors.

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
    "Buffer to small",
    "Can not open device",
    "Vendor code and product code do not match",
    "File name too long",
    "Can not open output file",
    "Can not open input file",
    "Illegal frame number",
    "Message terminator missing",
    "No output file name given",
    "Unknown data file line format",
    "Data line does not contain the expected number of elements",
    "Not enough memory to store data",
    "Error when writing to a file",
    0
    };


/*  function        void showerr( int error )

    brief           Shows error description if available.
                    Exits if fatal error.

    param[in]       int error, error code
*/
void showerr( int error )
    {
    int count;

    if( error == NOERR )
        return;

    if( error < ERR_BUFFER_LEN )
        {
        fprintf(stderr, "\nError %3d : %s\n", error, strerror(error));
        }
    else
        {
        for( count = 0; errors[count]; ++count )
            {
            if(count == error - ERR_BUFFER_LEN )
                break;
            }

            if( errors[count] )
                fprintf(stderr, "\nError %3d : %s\n", error, errors[count]);
            else
                fprintf(stderr, "\nError %3d : unknown error code !\n", error);
        }

    exit(error);
    }
