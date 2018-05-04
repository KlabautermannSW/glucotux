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

    date        01.04.2018

    author      Uwe Jantzen (jantzen@klabautermann-software.de)

    brief       Merge data files (current and older format)

    details     All data files are sorted chronological order, oldest data set
                first, newest dataset last.

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "errors.h"
#include "debug.h"
#include "astm.h"
#include "utils.h"
#include "files.h"


#define LINE_LEN                            48                                  // fits for older and current line format
#define NUM_OF_ELEMENTS                      7                                  // maximum number of data elements stored in one line
#define ELEMENT_LEN                         14                                  // element's maximum string length


/*  function        static int get_num_of_records( FILE * f )

    brief           Determines the number of data records (lines) in the data
                    file.

    param[in]       FILE * f, data files handle

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
    fseek(f, 0, SEEK_SET);
    debug("File length %ld\n", l);
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

debug("before : %s\n", line);
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
debug("after  : %s\n", line);
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
        case 0:
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
        case 1:
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

    while( (n = getline(&line, &len, f)) != -1 )
        {
        error = read_line(data + i, line, line_format);
        showerr(error);
        ++i;
        }

    free(line);

    return data;
    }


/*  function        void printline( FILE * f, dataset * data )

    brief           Prints one record to the file

    param[in]       FILE * f, output file's handle
    param[in]       dataset * data, record to print into the file
*/
void printline( FILE * f, dataset * data )
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

    return          int
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


/*  function        void reformat( const char * oldfile_name, const char * newfile_name )

    brief           Reads the data from <oldfile_name> using the data order that was
                    implemented before 30.3.2018. Then it writes back the data
                    to <newfile_name> using the current data order.

    param[in]       const char * oldfile_name, name of the file to read from
    param[in]       const char * newfile_name, name of the file to write to

    return          int
*/
void reformat( const char * oldfile_name, const char * newfile_name )
    {
    FILE * oldfile;
    FILE * newfile;
    FILE * tmpfile;
    int i;
    int o;
    int oldfile_records;
    int newfile_records;
    dataset * indata;
    dataset * outdata;

    printf("Reformatting %s and %s to %s\n", oldfile_name, newfile_name, "glucotux.tmp");

    debug("Opening %s\n", oldfile_name);
    oldfile = fopen(oldfile_name, "r");
    if( oldfile == 0 )
        showerr(errno);
    debug("Opening %s\n", newfile_name);
    newfile = fopen(newfile_name, "r");
    if( newfile == 0 )
        showerr(errno);
    debug("Opening %s\n", "glucotux.tmp");
    tmpfile = fopen("glucotux.tmp", "w");
    if( tmpfile == 0 )
        showerr(errno);

    indata = getfile(oldfile, &oldfile_records, 0);
    outdata = getfile(newfile, &newfile_records, 1);

    i = 0;
    o = 0;
// ************************************************************************************************************************
// der Fehler liegt in der while-Bedingung !!!!!!!!
// ************************************************************************************************************************
    while( ( i < oldfile_records ) || ( o < newfile_records ) )
        {
        int res = strcmp((indata + i)->timestamp, (outdata + o)->timestamp);
        if( res == 0 )
            {
debug("equal %s == %s at in=%d, out=%d\n", (indata + i)->timestamp, (outdata + o)->timestamp, i, o);
            printline(tmpfile, indata + i);
            ++i;
            ++o;
            }
        else if( res < 0 )
            {
debug("in smaller %s < %s at in=%d, out=%d\n", (indata + i)->timestamp, (outdata + o)->timestamp, i, o);
            printline(tmpfile, indata + i);
            ++i;
            }
        else
            {
debug("out smaller %s >= %s at in=%d, out=%d\n", (indata + i)->timestamp, (outdata + o)->timestamp, i, o);
            printline(tmpfile, outdata + o);
            ++o;
            }
        }

    free(indata);
    free(outdata);
    free(tmpfile);
    }
