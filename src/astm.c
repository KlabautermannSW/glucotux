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

    date        10.11.2019

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief       Handle communication with countour device using ASTM specifications.

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
#include <assert.h>
#include "errors.h"
#include "globals.h"
#include "debug.h"
#include "utils.h"
#include "contour.h"
#include "astm.h"


#define NUM_OF_FIELDS                       15
#define LEN_OF_FIELDS                       256                                 // there's no need to save space
#define NUM_OF_COMPONENTS                   11
#define LEN_OF_COMPONENTS                   20
#define FRAME_LEN                           (NUM_OF_FIELDS * LEN_OF_FIELDS)


// fielddelimiter: '|', repeat delimiter, component delimiter, escape delimiter
static char delimiters[4] = { '|', 0, 0, 0 };


/*  function        static int _send_astm( int handle, const char *buffer, size_t size )

    brief           Send a message to the contour device

    param[in]       int handle, handle to the contour device
    param[in]       const char *buffer, buffer to write the contour device
    param[in]       size_t size, number of bytes to send

    return          int, error code
*/
int _send_astm( int handle, const char *buffer, size_t size )
    {
    int result;
    char in_buffer[TRANSFER_BUFFER_LEN];
    assert(size);
    assert(size <= TRANSFER_BUFFER_LEN-5);
    assert(buffer);

    if( size > TRANSFER_BUFFER_LEN-5 )
        return ERR_BUFFER_LEN;

    usleep(30 * 1000);

    showbuffer(buffer, size);

    memset(in_buffer, 0, sizeof(in_buffer));
    in_buffer[4] = (char)(size);
    memcpy(in_buffer+5, buffer, size);
    result = write_contour(handle, in_buffer, size+5);

    return result;                                                              // error handling to be done outside this function
    }


/*  function        static int _read( int handle, char * buffer, size_t * len )

    brief           Reads TRANSFER_BUFFER_LEN bytes from the contour device

    param[in]       int handle, handle to the contour device
    param[out]      char * buffer, buffer to fill in the bytes read
    param[out]      size_t * len, number of bytes read

    return          int, error code
*/
static int _read( int handle, char * buffer, size_t * len )
    {
    size_t i;
    char * p;
    int result;
    assert(buffer);

    memset(buffer, 0, TRANSFER_BUFFER_LEN);
    result = read_contour(handle, buffer, TRANSFER_BUFFER_LEN, len);
    if( result )
        return result;
    for( i = 4, p = buffer + 4; i < TRANSFER_BUFFER_LEN; ++i )
        {
        if( *p++ == 0 )
            break;
        }
    *len = i;
    showbuffer(buffer, *len);

    return result;
    }


/*  function        static int _read_astm_part( int handle, char * buffer, size_t * len )

    brief           Read TRANSFER_BUFFER_LEN bytes from the contour device.
                    Every 16th call there has to be a wait time of 20ms to not
                    overrun the communications.

    param[in]       int handle, handle to the contour device
    param[out]      char * buffer, buffer to fill in the bytes read
    param[out]      size_t * len, number of bytes read

    return          int, if negative, error
*/
static int _read_astm_part( int handle, char * buffer, size_t * len )
    {
    static int num_of_calls_left = 16;
    char c;
    int result;
    assert(buffer);

    if( --num_of_calls_left == 0 )
        {
        usleep(20 * 1000);
        num_of_calls_left = 16;
        }

    c = ACK;
    if( (result = _send_astm(handle, &c, 1)) )
        return result;

    result = _read(handle, buffer, len);

    return result;
    }


/*  function        static int _verify_checksum( const char * buffer, size_t length )

    brief           Calculate the checksum over the frame and check it against
                    the frame's internal checksum.

    param[in]       const char * buffer, the frame
    param[in]       size_t length, number of bytes in buffer

    return          int, error code
*/
static int _verify_checksum( const char * buffer, size_t length )
    {
    const char * p = buffer;
    int checksum = 0;
    int cs_frame;
    int idx = 0;
    assert(buffer);

    while( *p++ != STX )
        {
        if( ++idx > 4 )                                                         // frame error
            return ERR_NO_START_OF_FRAME;
        }

    while( ( *p != ETB ) && ( *p != ETX ) )
        {
        if( ++idx > FRAME_LEN )                                                 // frame error
            return ERR_NO_FRAME_TERMINATION;
        checksum += (int)*p++;
        }
    checksum += (int)*p++;                                                      // include frame type character

    checksum &= 0xff;
    sscanf(p, "%x", &cs_frame);

    return (checksum == cs_frame ) ? NOERR : ERR_MESSAGE_CHECKSUM;
    }


/*  function        int _read_astm_frame( int handle, char * buffer, size_t size, size_t * len )

    brief           Reads an ASTM frame from the contour device.
                    A frame ends with a CR LF combination and has a correct
                    checksum.

    param[in]       int handle, handle to the contour device
    param[out]      char * buffer, buffer to fill in the bytes read
    param[in]       int size, buffer length
    param[out]      size_t * len, number of bytes read

    return          int, error code
*/
int _read_astm_frame( int handle, char * buffer, size_t size, size_t * len )
    {
    char in_buffer[TRANSFER_BUFFER_LEN];
    int result;
    size_t l;

    *len = 0;
    memset(buffer, 0, size);

    do
        {
        result = _read_astm_part(handle, in_buffer, &l);
        if( result )
            return result;
        if( (*len + l) > size )
            {
            *in_buffer = NAK;
            result = _send_astm(handle, in_buffer, 1);
            if( result )
                return result;
            return ERR_BUFFER_LEN;
            }
        if( l < 4 )
            {
            *in_buffer = NAK;
            result = _send_astm(handle, in_buffer, 1);
            if( result )
                return result;
            return ERR_UNKNOWN_LINE_FORMAT;
            }
        l -= 4;
        memcpy(buffer + *len, in_buffer + 4, l);
        *len += l;
        }
    while( buffer[*len-1] != LF );

    result = _verify_checksum(buffer, *len - 1);
    if( result )
        return result;

    return NOERR;
    }


/*  function        char read_astm( int handle )

    brief           Reads at least 36 bytes from the contour device

    param[in]       int handle, handle to the contour device

    return          char, last byte read
*/
char read_astm( int handle )
    {
    int result;
    size_t len;
    char buffer[TRANSFER_BUFFER_LEN];

    memset(buffer, 0, sizeof(buffer));
    while( 1 )
        {
        result = _read(handle, buffer, &len);
        if( result )
            {
            showerr(result);
            break;
            }
        debug("Number of bytes read : %d, last byte 0x%02x\n", len, buffer[len - 1]);
        if( len <= 36 )
            return buffer[len - 1];
        }

    return '\0';
    }


/*  function        static int _interpret_astm_frame( FILE * file, int handle, char * buffer, size_t length, int contour_type )

    brief           Interprets a frame read from a countour device. The frame
                    given in buffer was verified for a correct transfer.
                    If "file" is given stores the data to "file".

    param[in]       FILE * file, file to log data into
    param[in]       int handle, handle to the contour device
    param[in]       char * buffer, buffer to interpret as an ASTM E-1394 record
    param[in]       size_t length, the buffer's number of bytes
    param[in]       int contour_type, type of the currntly connected contour device

    return          int, error code
*/
static int _interpret_astm_frame( FILE * file, int handle, char * buffer, size_t length, int contour_type )
    {
    int result;
    char elements[NUM_OF_FIELDS * LEN_OF_FIELDS];
    char components[NUM_OF_COMPONENTS * LEN_OF_COMPONENTS];
    dataset data;
    static int frame_number = 1;
    char * p;
    size_t i;
    int j;
    char temp_buffer[16];

    for( i=0, p = buffer; ( (*p != STX ) && ( i < length ) ); ++p, ++i )
        ;
    ++p;

    if( frame_number++ != (*p++ & 0x0f) )
        {                                                                       // send NAK to get a repeated frame
        *temp_buffer = NAK;
        result = _send_astm(handle, temp_buffer, 1);
        if( result )
            return result;
        return ERR_FRAME_NUMBER;
        }
    frame_number &= 7;

    memset(&data, 0, sizeof(data));                                             // now every string ends with '\0'
    explode(elements, buffer, delimiters[0], NUM_OF_FIELDS, LEN_OF_FIELDS);
    data.record_type = *p++;                                                    // this is the record type
    switch( data.record_type )
        {
        case 'H':                                                               // Header Record
            delimiters[0] = *p++;
            delimiters[1] = *p++;
            delimiters[2] = *p++;
            delimiters[3] = *p;
            explode(components, elements + (4 * LEN_OF_FIELDS), delimiters[2], NUM_OF_COMPONENTS, LEN_OF_COMPONENTS);
            if( is_verbose() )
                {
                printf("%s ", components);                                      // meter product code
                printf("%s ", components + LEN_OF_COMPONENTS);                  // meter software version etc.
                }
            time2ger(components, elements + (13 * LEN_OF_FIELDS));
            if( is_verbose() )
                printf("%s\n", components);                                     // time stamp
            break;
        case 'R':                                                               // Result Record
            data.record_number = atoi(elements + LEN_OF_FIELDS);
            strncpy(data.UTID, elements + (2 * LEN_OF_FIELDS + 3), sizeof(data.UTID) - 1);    // remove leading ^^^
            data.result = atoi(elements + 3 * LEN_OF_FIELDS);
            explode(components, elements + (4 * LEN_OF_FIELDS), delimiters[2], NUM_OF_COMPONENTS, LEN_OF_COMPONENTS);
            memcpy(data.unit, components, sizeof(data.unit) - 1);

            if( strlen(elements + (6 * LEN_OF_FIELDS)) )
                {
                i = explode(components, elements + (6 * LEN_OF_FIELDS), '/', NUM_OF_COMPONENTS, LEN_OF_COMPONENTS);
                if( i > 9 )
                    i = 9;
                for( j = 0; j < i; ++j )
                    {
                    switch( *(components + (j * LEN_OF_COMPONENTS)) )
                        {
                        case 'M':
                            *(data.flags + j) = 'N';                            //   "out of regular intervals"
                            break;
                        case 0x00:
                            *(data.flags + j) = 'O';                            //   "out of regular intervals"
                            break;
                        default:
                            *(data.flags + j) = *components;                    // 'B' : before meal, 'A' : after meal, 'F' : fasting
                            break;
                        }
                    }
                *(data.flags + j) = 0;
                }
            if( contour_type == CONTOUR_NEXT_ONE )
                strncpy(data.timestamp, elements + (8 * LEN_OF_FIELDS), 14);
            else
                strncpy(data.timestamp, elements + (8 * LEN_OF_FIELDS), 12);
            if( memcmp(data.UTID, "Insu", 4) == 0 )
                sprintf(temp_buffer, "%5.1f", (double)data.result / 10.0);
            else
                sprintf(temp_buffer, "%5d", data.result);
            if( file )
                fprintf(file, "%s %s %-5s  %-9s  %-8s  %c  %4d\n",
                    data.timestamp, temp_buffer, data.unit, data.flags, data.UTID, data.record_type, data.record_number);
            if( is_verbose() )
                printf(" %s %s %-5s  %-9s  %-8s  %c  %4d\n",
                    data.timestamp, temp_buffer, data.unit, data.flags, data.UTID, data.record_type, data.record_number);
            else
                printf("%c%4d", CR, data.record_number);                        // show progress
            fflush(stdout);
            break;
        case 'L':                                                               // Message Terminator Record
            if( *(elements + (3 * LEN_OF_FIELDS)) != 'N' )
                return ERR_MESSAGE_TERMINATOR;
            break;
        case 'P':                                                               // Patient Information Record
        case 'O':                                                               // Test Order Record
            break;                                                              // empty / not used
        default:
            debug("Unknown frame type\n");
        }

    return NOERR;
    }


/*  function        int data_transfer_mode( int handle, int contour_type )

    brief           Reads in data using ASTM Data Transfer Mode
                    Reads until no more data available (ETX in last telegram).

    param[in]       int handle, handle to the contour device
    param[in]       int contour_type, type of the currntly connected contour device

    return          int, error code
*/
int data_transfer_mode( int handle, int contour_type )
    {
    const char * filename;
    FILE * file = 0;
    size_t length;
    char buffer[FRAME_LEN];
    int result = NOERR;

    filename = get_outfile_name();
    if( *filename != 0 )
        {
        file = fopen(filename, "w+");
        if( file == 0 )
            {
            result = errno;
            showerr(result);
            return ERR_OPEN_LOG_FILE;
            }
        }

    do
        {
        result = _read_astm_frame(handle, buffer, FRAME_LEN, &length);
        if( result )
            return result;
        showbuffer(buffer, length);
        buffer[length] = 0;
        result = _interpret_astm_frame(file, handle, buffer, length, contour_type);
        if( result )
            return result;
        }
    while( buffer[length - 5] != ETX );

    printf("\n");

    *buffer = NAK;
    result = _send_astm(handle, buffer, 1);
    if( result )
        return result;

    if( file != 0 )
        fclose(file);
    return result;
    }
