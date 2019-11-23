/*
    Copyright (C)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MEresultHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.
    If not, see <http://www.gnu.org/licenses/>.

    Klabautermann Software
    Uwe Jantzen
    Weingartener Straße 33
    76297 Stutensee
    Germany

    file        files.c

    date        23.11.2019

    author      Uwe Jantzen (jantzen@klabautermann-software.de)

    brief       Merge data files (current and older format)

    details     All data records are sorted in chronological order, oldest
                record first, newest record last.

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "errors.h"
#include "debug.h"
#include "astm.h"
#include "utils.h"
#include "globals.h"
#include "files.h"


#define LINE_LEN                            48                                  // fits for older and current line format
#define NUM_OF_ELEMENTS                      7                                  // maximum number of data elements stored in one line
#define ELEMENT_LEN                         14                                  // element's maximum string length


/*  function        static int _get_num_of_records( FILE * f )

    brief           Calculates the number of data records (lines) in the data
                    file.

    param[in]       FILE * f, data file's handle

    return          long, number of data records
*/
static long _get_num_of_records( FILE * f )
    {
    int i;
    long l;
    int c;

    for( i = 0, c = 0xff; c != 0x0a; ++i )
        c = fgetc(f);
    debug("Line length %d\n", i);
    fseek(f, 0, SEEK_END);
    l = ftell(f);
    debug("File length %ld\n", l);
    fseek(f, 0, SEEK_SET);
    l = l / i;
    debug("Records found %ld\n", l);

    return l;
    }


/*  function        static void _shrink( char * line, char c )

    brief           shrinks multiple occurrences of "c" to one and removes the
                    trailing newlines

    param[in/out]   char * line, the line to shrink
    param[in]       char c, the character to shrink from multiple to one
*/
static void _shrink( char * line, char c )
    {
    int no_char = 0;
    char * p_line = line;
    char dst[LINE_LEN];
    char * p_dst;

    memset(dst, 0, LINE_LEN);
    p_dst = dst;
    while( *p_line != 0 )
        {
        if( *p_line != c )
            {
            if( *p_line != 0x0a )                                               // remove liefeeds
                {
                *(p_dst++) = *p_line;
                no_char = 0;
                }
            }
        else
            {
            if( !no_char )
                {
                *(p_dst++) = *p_line;
                no_char = 1;
                }
            }
        ++p_line;
        }

    strcpy(line, dst);
    }


/*  function        static int _read_line( dataset * data, char * line )

    brief           Reads a line of CSV formatted data and stores the data to an
                    instance of type dataset.

    param[out]      dataset * data, store the data read in here
    param[in]       char * line, string containing one line of CVS formatted data

    return          int, error code
*/
static int _read_line( dataset * data, char * line )
    {
    int error = NOERR;
    char elements[ELEMENT_LEN * NUM_OF_ELEMENTS] = {0, };
    unsigned int num_of_elements;
    char result_buffer[ELEMENT_LEN];
    char const str_mg[] = "mg";
    char const str_mml[] = "mm";
    int i;
    char * unit;

    _shrink(line, ' ');
    debug("Shrunk \"%s\"\n", line);
    memset(data, 0, sizeof(dataset));

    memset(result_buffer, 0, ELEMENT_LEN);
    num_of_elements = explode(elements, line, ' ', NUM_OF_ELEMENTS, ELEMENT_LEN);

    if( *elements == 'R' )
        {
        debug("Number of elements in old record : %d\n", num_of_elements);
        if( num_of_elements == 6 )
            {
            data->record_type = elements[0];
            sscanf(elements + ELEMENT_LEN, "%d", &(data->record_number));
            strncpy(data->UTID, elements + 2 * ELEMENT_LEN, sizeof(data->UTID));
            strncpy(data->timestamp, elements + 3 * ELEMENT_LEN, sizeof(data->timestamp));
            sscanf(elements + 4 * ELEMENT_LEN, "%d", &(data->result));
            unit = strstr(elements + 4 * ELEMENT_LEN, str_mg);
            if( unit == 0 )
                unit = strstr(elements + 4 * ELEMENT_LEN, str_mml);
            strncpy(data->unit, unit, sizeof(data->unit));
            strncpy(data->flags, elements + 5 * ELEMENT_LEN, 1);
            }
        else
            error = ERR_NUM_OF_DATA_IN_LINE;
        }
    else
        {
        memset(result_buffer, 0, ELEMENT_LEN);
        num_of_elements = explode(elements, line, ' ', NUM_OF_ELEMENTS, ELEMENT_LEN);
        debug("Number of elements in new record : %d\n", num_of_elements);
        if( ( num_of_elements == 6 ) || ( num_of_elements == 7 ) )
            {
            memcpy(data->timestamp, elements, sizeof(data->timestamp) - 1);
            sscanf(elements + ELEMENT_LEN, "%s", result_buffer);
            strncpy(data->unit, elements + 2 * ELEMENT_LEN, sizeof(data->unit));
            i = 3;
            if( num_of_elements == 7 )
                {
                strncpy(data->flags, elements + i * ELEMENT_LEN, sizeof(data->flags));
                ++i;
                }
            strncpy(data->UTID, elements + i * ELEMENT_LEN, sizeof(data->UTID));
            ++i;
            data->record_type = elements[i * ELEMENT_LEN];
            ++i;
            sscanf(elements + i * ELEMENT_LEN, "%d", &(data->record_number));
            if( ( data->UTID[0] == 'I' ) || ( data->UTID[0] == 'W' ) )
                {
                int i1, i2, num;
                num = sscanf(result_buffer, "%d.%d", &i1, &i2);
                if( num == 2 )
                    data->result = i1 * 10 + i2;
                else
                    data->result = i1;
                }
            else
                sscanf(result_buffer, "%d", &(data->result));
            }
        else
            error = ERR_NUM_OF_DATA_IN_LINE;
        }

    return error;
    }


/*  function        static void _compare_timestamp( void const * data1, void const * data2 )

    brief           Compares to timestamps.
                    Returns an integer less than, equal to, or greater than zero
                    corresponding to whether the first timestamp is less than,
                    equal to, or greater than the second timestamp.

    param[in]       void const * data1, record to get the first timestamp from
    param[in]       void const * data2, record to get the second timestamp from

    return          int, result of comparison (s.o.)
*/
static int _compare_timestamp( void const * data1, void const * data2 )
    {
    char * timestamp1 = ((dataset *)data1)->timestamp;
    char * timestamp2 = ((dataset *)data2)->timestamp;

    return strcmp(timestamp1, timestamp2);
    }


/*  function        static int _getfile( FILE * f, dataset * p_data, size_t * records )

    brief           Get a file, allocate enough memory and read the data
                    into an array of type dataset.
                    On error p_data and/or the array contents are undefined.
                    --- You have to free the memory elsewhere ---

    param[in]       FILE * f, file handle
    param[out]      dataset ** p_data, pointer to dataset array
    param[out]      size_t * records, number of records in the dataset array

    return          int, error code
*/
static int _getfile( FILE * f, dataset ** p_data, size_t * records )
    {
    int error = NOERR;
    int i = 0;
    char * line = 0;
    size_t len = 0;
    ssize_t n;
    dataset * data;
    assert(f);

    *records = (size_t)_get_num_of_records(f);
    data = (dataset *)malloc(*records * sizeof(dataset));
    if( data == 0 )
        return ERR_NOT_ENOUGH_MEMORY;
    *p_data = data;

    while( (n = getline(&line, &len, f)) != -1 )    // getline allocates memory for "line"
        {
        error = _read_line(data + i, line);
        if( error )
            goto _getfile_err;
        ++i;
        }

    qsort(data, *records, sizeof(dataset), _compare_timestamp);

_getfile_err:
    free(line);

showerr(error);
    return error;
    }


/*  function        int mixfiles( const char *infile_name1, const char *infile_name2, const char *outfile_name )

    brief           Reads the data from <infile_name> and sorts them into <outfile_name>
                    removing duplicate lines.
                    No functionality yet!!!

    param[in]       const char *infile_name1, name of the file to read from
    param[in]       const char *infile_name2, name of the file to read from
    param[in]       const char *outfile_name, name of the file to write to

    return          int, error code
*/
int mixfiles( const char *infile_name1, const char *infile_name2, const char *outfile_name )
    {
    int result = NOERR;
    FILE * infile[2];
    FILE * outfile;
    size_t infile_records[2];
    dataset * indata[2];
    int idx[2];

    infile[0] = fopen(infile_name1, "r");
    if( infile[0] == 0 )
        {
        result = errno;
        debug("%s: \n", infile_name1);
        showerr(result);
        return result;
        }
    infile[1] = fopen(infile_name2, "r");
    if( infile[1] == 0 )
        {
        result = errno;
        debug("%s: \n", infile_name2);
        showerr(result);
        fclose(infile[0]);
        return result;
        }
    outfile = fopen(outfile_name, "w");
    if( outfile == 0 )
        {
        result = errno;
        debug("%s: \n", outfile_name);
        showerr(result);
        fclose(infile[0]);
        fclose(infile[1]);
        return result;
        }

    result = _getfile(infile[0], &(indata[0]), &(infile_records[0]));
    if( result )
        goto mixfiles_err;
    result = _getfile(infile[1], &(indata[1]), &(infile_records[1]));

    fclose(infile[0]);
    fclose(infile[1]);

    if( result )
        goto mixfiles_err;

    idx[0] = 0;
    idx[1] = 0;
    while( ( idx[0] < infile_records[0] ) || ( idx[1] < infile_records[1] ) )
        {
        int res = memcmp(indata[0] + idx[0], indata[1] + idx[1], sizeof(dataset)-sizeof(int));
        if( res == 0 )
            {
            printline(indata[0] + idx[0], outfile);
            ++idx[0];
            ++idx[1];
            }
        else if( res < 0 && ( idx[0] < infile_records[0] ) )
            {
            printline(indata[0] + idx[0], outfile);
            ++idx[0];
            }
        else if( idx[1] < infile_records[1] )
            {
            printline(indata[1] + idx[1], outfile);
            ++idx[1];
            }
        }

mixfiles_err:
    free(indata[0]);
    free(indata[1]);
    fclose(outfile);
    return result;
    }


/*  function        int csvformat( const char *infile_name, const char *outfile_name )

    brief           Reads the data from <infile_name>,
                    reformats them to a new csv file named <outfile_name>.
                    The new format is as follows:
                    date|time|glucose|glucose unit|insulin|insulin type|carb|carb unit|user mark

    param[in]       const char *infile_name, name of the file to read from
    param[in]       const char *outfile_name, name of the file to write to

    result          int, error code
*/
int csvformat( const char *infile_name, const char *outfile_name )
    {
    int result = NOERR;
    FILE * infile;
    FILE * outfile;
    size_t infile_records;
    int i;
    char line[256];
    int chars_written;
    char * p_line;
    dataset * indata;
    dataset * p_indata;

    infile = fopen(infile_name, "r");
    if( infile == 0 )
        {
        result = errno;
        showerr(result);
        return result;
        }
    outfile = fopen(outfile_name, "w");
    if( outfile == 0 )
        {
        result = errno;
        showerr(result);
        fclose(infile);
        return result;
        }

    result = _getfile(infile, &indata, &infile_records);
    if( result )
        goto csvformat_err;
    p_indata = indata;

    for( i = 0; i < infile_records; ++i )
        {
        p_line = line;
        memcpy(p_line, p_indata->timestamp+6, 2);
        p_line += 2;
        *p_line++ = '.';
        memcpy(p_line, p_indata->timestamp+4, 2);
        p_line += 2;
        *p_line++ = '.';
        memcpy(p_line, p_indata->timestamp, 4);
        p_line += 4;
        *p_line++ = '|';
        memcpy(p_line, p_indata->timestamp+8, 2);
        p_line += 2;
        *p_line++ = ':';
        memcpy(p_line, p_indata->timestamp+10, 2);
        p_line += 2;
        *p_line++ = '|';
        if( *p_indata->UTID == 'G' )
            {
            chars_written = sprintf(p_line, "%d|", (int)(p_indata->result));
            p_line += chars_written;
            chars_written = sprintf(p_line, "%s|", p_indata->unit);
            }
        else
            chars_written = sprintf(p_line, "||");
        p_line += chars_written;
        if( *p_indata->UTID == 'I' )
            {
            chars_written = sprintf(p_line, "%d,%d|",
                p_indata->result/10, p_indata->result%10);
            p_line += chars_written;
            chars_written = sprintf(p_line, "%s|", p_indata->unit);
            }
        else
            chars_written = sprintf(p_line, "||");
        p_line += chars_written;
        if( *p_indata->UTID == 'C' )
            {
            chars_written = sprintf(p_line, "%d|", (int)(p_indata->result));
            p_line += chars_written;
            switch( *p_indata->unit )
                {
                case '1':
                    chars_written = sprintf(p_line, "Gramm|");
                    break;
                case '2':
                    chars_written = sprintf(p_line, "BE|");
                    break;
                case '3':
                    chars_written = sprintf(p_line, "KE|");
                    break;
                default:
                    chars_written = sprintf(p_line, "|");
                    break;
                }
            }
        else
            chars_written = sprintf(p_line, "||");
        p_line += chars_written;
        chars_written = sprintf(p_line, "%s", p_indata->flags);
        p_line += chars_written;
        *p_line++ = 0x0a;
        *p_line++ = 0;                              // close string
        debug("%s", line);
        fprintf(outfile, "%s", line);
        p_indata++;
        }

csvformat_err:
    fclose(outfile);
    fclose(infile);
    free(indata);
    return result;
    }
