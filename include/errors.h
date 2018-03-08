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

    date        08.03.2018

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief       Defines application specific erros.

    details     Numbering starts at 256 (0x100) to prevent mangling with
                Linux's internal errors.

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#ifndef __ERRORS_H__
#define __ERRORS_H__


#define NOERR                                         0

#define ERR_BUFFER_LEN                              256
#define ERR_OPENING_DEVICE                          257
#define ERR_VENDOR_AND_PRODUCT_CODES_DO_NOT_MATCH   258
#define ERR_FILE_NAME_LENGTH                        259
#define ERR_OPEN_LOG_FILE                           260
#define ERR_FRAME_NUMBER                            261
#define ERR_MESSAGE_TERMINATOR                      262


#endif  // __ERRORS_H__
