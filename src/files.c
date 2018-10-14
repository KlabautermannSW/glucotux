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

    date        14.10.2018

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
#include "errors.h"
#include "debug.h"
#include "astm.h"
#include "utils.h"
#include "files.h"


#define LINE_LEN                            48                                  // fits for older and current line format
#define NUM_OF_ELEMENTS                      7                                  // maximum number of data elements stored in one line
#define ELEMENT_LEN                         14                                  // element's maximum string length


/*  function        static int get_num_of_records( FILE * f )

    brief           Calculates the number of data records (lines) in the data
                    file.

    param[in]       FILE * f, data file's handle

    return          int, number of data records
*/
static int get_num_of_records( FILE * f )
    {
    int i;
    long l;
    char c;

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


/*  function        static void shrink( char * line, char c )

    brief           Shrinks multiple occurrences of "c" to one and removes the
                    trailing newlines

    param[in/out]   char * line, the line to shrink
    param[in]       char c, the character to shrink from multiple to one
*/
static void shrink( char * line, char c )
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
        else if( *p_line == c )
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


/*  function        static int read_line( dataset * data, char * line, int line_format )

    brief           Reads a line of CSV formatted data and stores the data to an
                    instance of type dataset.

    param[out]      dataset * data, store the data read in here
    param[in]       char * line, string containing one line of CVS formatted data
    param[in]       int line_format,  0 : used up to 29.03.2018
                                      1 : used starting on 30.03.2018

    return          int, error code
*/
static int read_line( dataset * data, char * line, int line_format )
    {
    int error = NOERR;
    char elements[ELEMENT_LEN * NUM_OF_ELEMENTS] = {0, };
    int num_of_elements;
    const char str_mg[] = "mg";
    const char str_mml[] = "mm";
    char * unit;

    shrink(line, ' ');
    debug("Shrunken \"%s\"\n", line);
    memset(data, 0, sizeof(dataset));
    switch( line_format )
        {
        case 0:     // the older format
            num_of_elements = explode(elements, line, ' ', NUM_OF_ELEMENTS, ELEMENT_LEN);
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
            break;
        case 1:     // the newer format
            num_of_elements = explode(elements, line, ' ', NUM_OF_ELEMENTS, ELEMENT_LEN);
            debug("Number of elements in new record : %d\n", num_of_elements);
            if( num_of_elements == 7 )
                {
                strncpy(data->timestamp, elements, sizeof(data->timestamp));
                sscanf(elements + ELEMENT_LEN, "%d", &(data->result));
                strncpy(data->unit, elements + 2 * ELEMENT_LEN, sizeof(data->unit));
                strncpy(data->flags, elements + 3 * ELEMENT_LEN, 1);
                strncpy(data->UTID, elements + 4 * ELEMENT_LEN, sizeof(data->UTID));
                data->record_type = elements[5 * ELEMENT_LEN];
                sscanf( elements + 6 * ELEMENT_LEN, "%d", &(data->record_number));
                }
            else
                error = ERR_NUM_OF_DATA_IN_LINE;
            break;
        default:
            error = ERR_UNKNOWN_LINE_FORMAT;
            break;
        }

    return error;
    }


/*  function        static void compare_timestamp( const void * data1, const void * data2 )

    brief           Compares to timestamps.
                    Returns an integer less than, equal to, or greater than zero
                    corresponding to whether the first timestamp is less than,
                    equal to, or greater than the second timestamp.

    param[in]       const void * data1, record to get the first timestamp from
    param[in]       const void * data2, record to get the second timestamp from

    return          int, result of comparison (s.o.)
*/
static int compare_timestamp( const void * data1, const void * data2 )
    {
    char * timestamp1 = ((dataset *)data1)->timestamp;
    char * timestamp2 = ((dataset *)data2)->timestamp;

    return strcmp( timestamp1, timestamp2);
    }


/*  function        static dataset * getfile( FILE * f, int * records, int line_format )

    brief           Get a file, allocate enough memory and read the data
                    into an array of type dataset.

    param[in]       FILE * f, file handle
    param[out]      int * records, number of records in the dataset arry
    param[in]       int line_format,  0 : used up to 29.03.2018
                                      1 : used starting on 30.03.2018

    return          pointer to dataset array
*/
static dataset * getfile( FILE * f, int * records, int line_format )
    {
    int error = NOERR;
    int i = 0;
    char * line = 0;
    size_t len = 0;
    ssize_t n;
    dataset * data;

    *records = get_num_of_records(f);
    data = (dataset *)malloc(*records * sizeof(dataset));
    if( data == 0 )
        showerr(ERR_NOT_ENOUGH_MEMORY);

    while( (n = getline(&line, &len, f)) != -1 )    // getline allocates memory for "line"
        {
        error = read_line(data + i, line, line_format);
        showerr(error);
        ++i;
        }

    qsort(data, *records, sizeof(dataset), compare_timestamp);

    free(line);

    return data;
    }


/*  function        static void printline( FILE * f, dataset * data )

    brief           Prints one record to the file

    param[in]       FILE * f, output file's handle
    param[in]       dataset * data, record to print into the file
*/
static void printline( FILE * f, dataset * data )
    {
    debug("printline : %s  %3d %-5s  %s  %-8s  %c  %4d\n",
            data->timestamp,
            data->result,
            data->unit,
            data->flags,
            data->UTID,
            data->record_type,
            data->record_number);
    if( fprintf(f, "%s  %3d %-5s  %s  %-8s  %c  %4d\n",
            data->timestamp,
            data->result,
            data->unit,
            data->flags,
            data->UTID,
            data->record_type,
            data->record_number) < 0 )
        showerr(ERR_WRITE_TO_FILE);
    }


/*  function        void mixfiles( const char * infile_name, const char * outfile_name )

    brief           Reads the data from <infile_name> and sorts them into <outfile_name>
                    removing duplicate lines.

    param[in]       const char * infile_name, name of the file to read from
    param[in]       const char * outfile_name, name of the file to write to
*/
void mixfiles( const char * infile_name, const char * outfile_name )
    {
    FILE * infile;
    FILE * outfile;
    int infile_records;
    int outfile_records;
    dataset * indata;
    dataset * outdata;

    infile = fopen(infile_name, "r");
    if( infile == 0 )
        showerr(errno);
    outfile = fopen(outfile_name, "r");
    if( outfile == 0 )
        showerr(errno);

    indata = getfile(infile, &infile_records, 1);
    outdata = getfile(outfile, &outfile_records, 1);

    free(indata);
    free(outdata);
    }


/*  function        void csvformat( const char * infile_name, const char * outfile_name )

    brief           Reads the data from <infile_name>,
                    reformats them to a new csv file named <outfile_name>.
                    The new format is as follows:
                    date|time|glucose|glucose unit|insulin|insulin type|carb|carb unit|user mark

    param[in]       const char * infile_name, name of the file to read from
    param[in]       const char * outfile_name, name of the file to write to
*/
void csvformat( const char * infile_name, const char * outfile_name )
    {
    FILE * infile;
    FILE * outfile;
    int infile_records;
    int i;
    char line[256];
    int chars_written;
    char * p_line;
    dataset * indata;
    dataset * p_indata;

    infile = fopen(infile_name, "r");
    if( infile == 0 )
        showerr(errno);
    outfile = fopen(outfile_name, "w");
    if( outfile == 0 )
        showerr(errno);

    indata = getfile(infile, &infile_records, 1);
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
        if( strcmp(p_indata->UTID, "Glucose") == 0 )
            {
            chars_written = sprintf(p_line, "%d|", p_indata->result);
            p_line += chars_written;
            chars_written = sprintf(p_line, "%s|", p_indata->unit);
            }
        else
            chars_written = sprintf(p_line, "||");
        p_line += chars_written;
        if( strcmp(p_indata->UTID, "Insulin") == 0 )
            {
            chars_written = sprintf(p_line, "%d,%d|", p_indata->result/10, p_indata->result%10);
            p_line += chars_written;
            chars_written = sprintf(p_line, "%s|", p_indata->unit);
            }
        else
            chars_written = sprintf(p_line, "||");
        p_line += chars_written;
        if( strcmp(p_indata->UTID, "Carb") == 0 )
            {
            chars_written = sprintf(p_line, "%d|", p_indata->result);
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
        *p_line++ = p_indata->flags[0];
        *p_line++ = 0x0a;
        *p_line++ = 0;                              // close string
        debug("%s", line);
        fprintf(outfile, "%s", line);
        p_indata++;
        }

    free(indata);
    }


/*  function        void reformat( const char * infile_name, const char * newfile_name )

    brief           Reads the data from <infile_name> using the data line's
                    format that was implemented before 30.3.2018. Then it writes
                    back the data to <newfile_name> using the current data
                    line's format.
                    4.5.2018 : output to file "glucotux.tmp".

    param[in]       const char * infile_name, name of the file to read from
    param[in]       const char * newfile_name, name of the file to write to
*/
void reformat( const char * infile_name, const char * newfile_name )
    {
    char tmpfile_name[] = "glucotux.tmp";
    FILE * oldfile;
    FILE * newfile;
    FILE * tmpfile;
    int idx_old;
    int idx_new;
    int oldfile_records;
    int newfile_records;
    dataset * olddata;
    dataset * newdata;

    printf("Reformatting %s and %s to %s\n", infile_name, newfile_name, tmpfile_name);

    debug("Opening %s\n", infile_name);
    oldfile = fopen(infile_name, "r");
    if( oldfile == 0 )
        showerr(errno);
    debug("Opening %s\n", newfile_name);
    newfile = fopen(newfile_name, "r");
    if( newfile == 0 )
        showerr(errno);
    debug("Opening %s\n", tmpfile_name);
    tmpfile = fopen(tmpfile_name, "w");
    if( tmpfile == 0 )
        showerr(errno);

    olddata = getfile(oldfile, &oldfile_records, 0);
    newdata = getfile(newfile, &newfile_records, 1);

    fclose(newfile);
    fclose(oldfile);

    idx_old = 0;
    idx_new = 0;
    while( ( idx_old < oldfile_records ) || ( idx_new < newfile_records ) )
        {
        int res = memcmp(olddata + idx_old, newdata + idx_new, sizeof(dataset)-sizeof(int));
        if( res == 0 )
            {
            printline(tmpfile, olddata + idx_old);
            ++idx_old;
            ++idx_new;
            }
        else if( res < 0 && ( idx_old < oldfile_records ) )
            {
            printline(tmpfile, olddata + idx_old);
            ++idx_old;
            }
        else if( idx_new < newfile_records )
            {
            printline(tmpfile, newdata + idx_new);
            ++idx_new;
            }
        }

    fclose(tmpfile);

    free(olddata);
    free(newdata);
    free(tmpfile);
    }
