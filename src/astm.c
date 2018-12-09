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

    file        astm.c

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


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"
#include "globals.h"
#include "debug.h"
#include "utils.h"
#include "contour.h"
#include "astm.h"


#define NUM_OF_ELEMENTS                     14
#define LEN_OF_ELEMENTS                     160
#define NUM_OF_COMPONENTS                   5
#define LEN_OF_COMPONENTS                   20
#define FRAME_LEN                           1024
#define BUFFER_LEN                          64


static char field_delimiter = '|';
static char repeat_delimiter = 0;
static char component_delimiter = 0;
static char escape_delimiter = 0;


/*  function        int send_astm( int handle, char const * buffer, int size )

    brief           Send a message to the contour device

    param[in]       int handle, handle to the contour device
    param[in]       char const * buffer, buffer to write the contour device
    param[in]       int size, number of bytes to send

    return          int, 0
*/
int send_astm( int handle, char const * buffer, int size )
    {
    int result;
    char in_buffer[BUFFER_LEN] = {0,};

    if( size > BUFFER_LEN-5 )
        return ERR_BUFFER_LEN;

    usleep(30 * 1000);

    showbuffer(buffer, size);

    in_buffer[4] = (char)(size);
    memcpy(in_buffer+5, buffer, size);
    result = write_contour(handle, in_buffer, size+5);
    if( result )
        exit(result);

    return 0;
    }


/*  function        static int _read( int handle, char * buffer )

    brief           Reads BUFFER_LEN bytes from the contour device

    param[in]       int handle, handle to the contour device
    param[out]      char * buffer, buffer to fill in the bytes read

    return          int, if negative, error
                         if positive, number of bytes read
*/
static int _read( int handle, char * buffer )
    {
    int j = 0;
    int result;

    while( j < BUFFER_LEN  )
        {
        result = read_contour(handle, buffer+j, BUFFER_LEN-j);

        if( result < 0 )
            break;

        j += result;
        }

    result = strlen((char const *)buffer);
    showbuffer(buffer, result);

    return result;
    }


/*  function        static int _read_astm_part( int handle, char * buffer )

    brief           Read BUFFER_LEN bytes from the contour device
                    Every 16th call there has to be a wait time of 20ms to not
                    overrun the communications.

    param[in]       int handle, handle to the contour device
    param[out]      char * buffer, buffer to fill in the bytes read

    return          int, if negative, error
                         if positive, number of bytes read
*/
static int _read_astm_part( int handle, char * buffer )
    {
    static int num_of_calls = 1;
    int result;

    if( num_of_calls++ == 15 )
        usleep(20 * 1000);
    num_of_calls &= 15;

    *buffer = ACK;
    send_astm(handle, buffer, 1);

    result = _read(handle, buffer);
    return result;
    }


/*  function        static int _verify_checksum( char * buffer )

    brief           Calculate the checksum over the frame and check it against
                    the frame's internal checksum.

    param[in]       char * buffer, the frame

    return          int, 0 : checksum ok
                         1 : checksum error
*/
static int _verify_checksum( char * buffer )
    {
    char * p = buffer;
    int checksum = 0;
    int cs_frame;

    while( *p++ != STX )
        ;
    while( ( *p != ETB ) && ( *p != ETX ) )
        checksum += (int)*p++;
    checksum += (int)*p++;                                                      // include frame type character

    checksum &= 0xff;
    sscanf(p, "%x", &cs_frame);

    return (checksum == cs_frame ) ? 0 : 1;
    }


/*  function        int _read_astm_frame( int handle, char * buffer, int size )

    brief           Reads an ASTM frame from the contour device.
                    A frame ends with a CR LF combination and has a correct
                    checksum.

    param[in]       int handle, handle to the contour device
    param[out]      char * buffer, buffer to fill in the bytes read
    param[in]       int size, buffer length

    return          int, if negative, error
                         if positive, number of bytes read
*/
int _read_astm_frame( int handle, char * buffer, int size )
    {
    char in_buffer[BUFFER_LEN];
    int result;
    int length = 0;

    memset(buffer, 0, size);

    do
        {
        result = _read_astm_part(handle, in_buffer);
        if( (length + result) > size )
            {
            *in_buffer = NAK;
            send_astm(handle, buffer, 1);
            return ERR_BUFFER_LEN;
            }
        result -= 4;
        memcpy(buffer+length, in_buffer+4, result);
        length += result;
        }
    while( buffer[length-1] != LF );

    if( _verify_checksum(buffer) == 1 )
        length = 0;

    return length;
    }


/*  function        char read_astm( int handle )

    brief           Reads at last 36 bytes from the contour device

    param[in]       int handle, handle to the contour device

    return          char, last byte read
*/
char read_astm( int handle )
    {
    int result;
    char buffer[BUFFER_LEN] = {0, };

    while( 1 )
        {
        result = _read(handle, buffer);
        debug("Number of bytes read : %d, last byte 0x%02x\n", result, buffer[result-1]);
        if( result <= 36 )
            return buffer[result-1];
        }

    return 0;
    }


/*  function        static int _interpret_astm_frame( FILE * file, int handle, char * buffer )

    brief           Reads an ASTM frame from the contour device.
                    A frame ends with a CR LF combination and has a correct
                    checsum.
                    If "file" is given stores the data to "file".

    param[in]       FILE * file, file to log data into
    param[in]       int handle, handle to the contour device
    param[out]      char * buffer, buffer to fill in the bytes read

    return          int, error code
*/
static int _interpret_astm_frame( FILE * file, int handle, char * buffer )
    {
    char elements[NUM_OF_ELEMENTS * LEN_OF_ELEMENTS];
    char components[NUM_OF_COMPONENTS * LEN_OF_COMPONENTS];
    dataset data;
    static int frame_number = 1;
    char * p;

    memset(&data, 0, sizeof(data));
    explode(elements, buffer, field_delimiter, NUM_OF_ELEMENTS, LEN_OF_ELEMENTS);

    p = buffer;
    if( frame_number++ != (*(p + 1) & 0x0f) )
        return ERR_FRAME_NUMBER;
    frame_number &= 7;

    data.record_type = *(p + 2);                                                // this is the record type
    switch( data.record_type )
        {
        case 'H':                                                               // Header Record
            field_delimiter = *(p + 3);
            repeat_delimiter = *(p + 4);
            component_delimiter = *(p + 5);
            escape_delimiter = *(p + 6);
            explode(components, elements + (4 * LEN_OF_ELEMENTS), component_delimiter, NUM_OF_COMPONENTS, LEN_OF_COMPONENTS);
            if( is_verbose() )
                {
                printf("%s ", components);                                      // meter product code
                printf("%s ", components + LEN_OF_COMPONENTS);                  // meter software version etc.
                }
            time2ger(components, elements + (13 * LEN_OF_ELEMENTS));
            if( is_verbose() )
                printf("%s\n", components);                                     // meter product code
            break;
        case 'R':                                                               // Result Record
            data.record_number = atoi(elements + LEN_OF_ELEMENTS);
            strncpy(data.UTID, elements + (2 * LEN_OF_ELEMENTS + 3), sizeof(data.UTID) - 1);    // remove leading ^^^
            data.result = atoi(elements + 3 * LEN_OF_ELEMENTS);
            explode(components, elements + (4 * LEN_OF_ELEMENTS), component_delimiter, NUM_OF_COMPONENTS, LEN_OF_COMPONENTS);
            strncpy(data.unit, components, sizeof(data.unit) - 1);
            *components = *(elements + (6 * LEN_OF_ELEMENTS));
            switch( *components )
                {
                case 'M':
                    *(data.flags) = 'N';                                            //   "out of regular intervals"
                    break;
                case 0x00:
                    *(data.flags) = 'O';                                            //   "out of regular intervals"
                    break;
                default:
                    *(data.flags) = *components;                                    // 'B' : before meal, 'A' : after meal, 'F' : fasting
                    break;
                }
            *(data.flags + 1) = 0;
            strncpy(data.timestamp, elements + (8 * LEN_OF_ELEMENTS), 12);

            if( file )
                fprintf(file, "%s  %3d %-5s  %s  %-8s  %c  %4d\n", data.timestamp, data.result, data.unit, data.flags, data.UTID, data.record_type, data.record_number);
            if( is_verbose() )
                printf(" %s  %3d %-5s  %s  %-8s  %c  %4d\n", data.timestamp, data.result, data.unit, data.flags, data.UTID, data.record_type, data.record_number);
            printf("%c%4d", CR, data.record_number);                            // show progress
            fflush(stdout);
            break;
        case 'L':                                                               // Message Terminator Record
            if( *(elements + (2 * LEN_OF_ELEMENTS)) != 'N' )
                return ERR_MESSAGE_TERMINATOR;
            break;
        case 'P':                                                               // Patient Information Record
        case 'O':                                                               // Test Order Record
            break;                                                              // empty / not used
        default:
            debug("Unknown frame type\n");
        }

    return 0;
    }


/*  function        int data_transfer_mode( void )

    brief           Reads in data using ASTM Data Transfer Mode
                    Reads until no more data available (ETX in last telegram).

    param[in]       int handle, handle to the contour device

    return          int, error code
*/
int data_transfer_mode( int handle )
    {
    char const * filename;
    FILE * file = 0;
    int length;
    char buffer[FRAME_LEN];
    int result = NOERR;

    filename = get_outfile_name();
    if( *filename != 0 )
        {
        file = fopen(filename, "w+");
        if( file == 0 )
            return ERR_OPEN_LOG_FILE;
        }

    do
        {
        length = _read_astm_frame(handle, buffer, FRAME_LEN);
        showbuffer(buffer, length);
        buffer[length] = 0;
        result = _interpret_astm_frame(file, handle, buffer);
        if( result != 0 )
            return result;
        }
    while( buffer[length - 5] != ETX );

    printf("\n");

    *buffer = NAK;
    send_astm(handle, buffer, 1);

    fclose(file);
    return result;
    }
