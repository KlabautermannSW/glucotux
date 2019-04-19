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

    file        astm.h

    date        09.11.2018

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief       Handle com munication with countour device using ASTM specifications.

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#ifndef __ASTM_H__
#define __ASTM_H__


typedef struct dataset_t
    {
    char timestamp[13];
    int result;
    char unit[10];
    char flags[10];
    char UTID[12];                                                              // Universal Test ID
    char record_type;
    int record_number;
    } dataset;


extern int send_astm( int handle, const char *buffer, int size );
extern char read_astm( int handle );
extern int data_transfer_mode( int handle );


#endif  // __ASTM_H__
