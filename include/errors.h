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

    ERRORS      errors.h

    date        10.11.2019

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief       Defines application specific erros.

    details     All application specific error codes are negative integers.

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#ifndef __ERRORS_H__
#define __ERRORS_H__


#include <errno.h>


#define NOERR                                         0

#define ERR_BUFFER_LEN                               -1
#define ERR_OPENING_DEVICE                           -2
#define ERR_VENDOR_AND_PRODUCT_CODES_DO_NOT_MATCH    -3
#define ERR_READING_FROM_DEVICE                      -4
#define ERR_WRITING_TO_DEVICE                        -5
#define ERR_FILE_NAME_LENGTH                         -6
#define ERR_OPEN_LOG_FILE                            -7
#define ERR_OPEN_IN_FILE                             -8
#define ERR_NO_START_OF_FRAME                        -9
#define ERR_NO_FRAME_TERMINATION                    -10
#define ERR_FRAME_NUMBER                            -11
#define ERR_MESSAGE_TERMINATOR                      -12
#define ERR_MESSAGE_CHECKSUM                        -13
#define ERR_NO_LOGFILE                              -14
#define ERR_UNKNOWN_LINE_FORMAT                     -15
#define ERR_NUM_OF_DATA_IN_LINE                     -16
#define ERR_NOT_ENOUGH_MEMORY                       -17
#define ERR_WRITE_TO_FILE                           -18
#define ERR_NUM_OF_INFILES                          -19
#define ERR_UNIT_STRING_TOO_LONG                    -20


extern void showerr( int error );


#endif  // __ERRORS_H__
