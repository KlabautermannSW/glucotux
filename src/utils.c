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

    file        utils.c

    date        23.11.2019

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "globals.h"
#include "errors.h"
#include "debug.h"
#include "utils.h"


/*  function    int explode( char * elements, char * str, char delimiter,
                             size_t lines, size_t length )

    brief       Divides <str> into elements separted by <delimiter>.
                <elements> is used as char elements[lines][length].
                A new element is created every time a delimiter is found, so
                the first element is the string before the first delimiter
                and the last element is the string behind the last delimiter.
                If two delimiters follow each other without a string between
                an element containing an empty string (a '0' only) is
                created.

                To use <elements> to handle string #n do as
                    str = elements + (n * length)

    param[out]  char * elements, handled as an array with <lines> lines of a
                length of <length>
    param[in]   char * str, the string to explode (must be terminated with a '0')
    param[in]   char delimiter, separates the string's elements
    param[in]   size_t lines, number of separate strings that fits into elements
    param[in]   size_t length, maximum length of a sperate string - including
                the terminating '0'

    return      size_t, number of separated strings found
*/
unsigned int explode( char * elements, char * str, char delimiter, size_t lines, size_t length )
    {
    int idx;                                                                    // data index
    unsigned int dst_idx;                                                                // element's string index
    unsigned int line;
    size_t len;

    assert(elements);
    assert(str);
    assert(lines);
    assert(length);

    len = strlen(str);
    assert(len);

    memset(elements, 0, lines * length);                                        // now all elements are empty
    idx = 0;
    line = 0;
    dst_idx = 0;

    while( idx < len )
        {
        while( ( str[idx] != delimiter ) && ( idx < len ) && ( dst_idx < length ) )
            {
            *(elements + (line * length + dst_idx)) = str[idx];
            ++dst_idx;
            ++idx;
            }
        dst_idx = 0;                                                            // start of next element
        ++line;
        ++idx;                                                                  // remove delimiter
        }

    return line;
    }


/*  function        int printline( dataset * data, FILE * f )

    brief           Prints one record to the file
                    Prints it to screen if verbose output and/or debug is
                    enabled

    param[in]       dataset * data, record to print into the file
    param[in]       FILE * f, output file's handle
*/
int printline( dataset * data, FILE * f )
    {
    int error = NOERR;
    char value[16];
    char buffer[128];

    if( ( *(data->UTID) == 'I' ) || ( *(data->UTID) == 'W' ) )
        snprintf(value, 16, "%3d.%d", data->result/10, data->result%10);
    else
        snprintf(value, 16, "%5d", data->result);
    snprintf(buffer, 128, "%-14s  %5s  %-6s  %-9s  %-8s  %c  %4d\n",
            data->timestamp,
            value,
            data->unit,
            data->flags,
            data->UTID,
            data->record_type,
            data->record_number);

    debug("printline : ");
    if( is_verbose() || is_debug() )
        printf("%s", buffer);

    if( fprintf(f, "%s", buffer) < 0 )
        error = ERR_WRITE_TO_FILE;

    return error;
    }


/*  function        void time2ger( char * dst, char * src )

    brief           Converts a time from a record to a string holding german
                    time format.

    param[out]      char * dst, german formatted time and date
                    DD.MM.YYYY hh:mm:ss
                    Has to be at least 20 byte long!
                    NO length check is done here!!
    param[in]       char * src, the timestamp read from a record
*/
void time2ger( char * dst, char * src )
    {
    assert(dst);
    assert(src);

    memcpy(dst, src + 6, 2);                                                    // day
    *(dst + 2) = '.';
    memcpy(dst + 3, src + 4, 2);                                                // month
    *(dst + 5) = '.';
    memcpy(dst + 6, src, 4);                                                    // year
    *(dst + 10) = ' ';
    memcpy(dst + 11, src + 8, 2);                                               // hour
    *(dst + 13) = ':';
    memcpy(dst + 14, src + 10, 2);                                              // minute
    *(dst + 16) = ':';
    if( *(src + 12) )
        memcpy(dst + 17, src + 12, 2);                                          // second
    else
        {
        *(dst + 17) = '0';
        *(dst + 18) = '0';
        }
    *(dst + 19) = 0;
    }


/*  function        void rotating_bar( void )

    brief           Rotates a bar at cursor position showing the application is
                    still working.
*/
void rotating_bar( void )
    {
    char const bars[] = { '-', '\\', '|', '/' };
    static int idx = 0;

    putc(BS, stdout);
    putc(bars[idx++], stdout);
    fflush(stdout);
    idx &= 0x03;
    }


/*  function        void showhelp( char * name )

    brief           Print out help text

    param[in]       char * name, the application's name
*/
void showhelp( char * name )
    {
    printf("Usage:\n");
#ifdef NIX
    printf("        %s [options] [-o <outfile> [-i <infile> [-i|-r <infile>]]]\n", name);
#else
    printf("        %s [options] [-o <outfile> [-i <infile> [-i <infile>]]]\n", name);
#endif
    printf("Options:\n");
    printf("        -o <outfile>  File to put the data in,\n");
    printf("                      if not set, data is printed to screen\n");
    printf("        -i <infile>   File to get the data from,\n");
    printf("                      if set, data is read from <infile> and sorted into <outfile>\n");
    printf("                      removing duplicate lines.\n");
    printf("                      <outfile> and <infile> must NOT BE THE SAME!\n");
    printf("        -c            If this option is selected the application reads the data\n");
    printf("                      from <infile> and writes it to <outfile> using CVS format as\n");
    printf("                      follows:\n");
    printf("                      date|time|glucose|glucose unit|insulin|insulin type|carb|carb unit|user mark\n");
#ifdef NIX
    printf("        -r <infile>   If this option is selected the application reads the data\n");
    printf("                      from <infile> using the data order that was implemented\n");
    printf("                      before 30.3.2018. Then it writes back the data to <outfile>\n");
    printf("                      using the current data order.\n");
#endif
    printf("\n");
    printf("           If you give two <infile>s they are mixed and put out to <outfile> (not implemented yet!).\n");
    printf("\n");
    printf("        -v            Enable verbose mode\n");
#ifdef _DEBUG_
    printf("        -d            Enable debug mode\n");
#endif  //  _DEBUG_
    printf("\n");
    printf("        -h            Show this help then stop without doing anything more\n");
    printf("\n");
    printf("\n");
    exit(0);
    }


#ifdef _DEBUG_
/*  function        void Showbuffer( const char * buffer, size_t size )

    brief           Displays a frame with decoded special characters, unknown
                    characters as their ascii number in hex and all others as
                    printable characters.
                    This is done in debug mode only!

    param[in]       const char *buffer, message buffer
    param[in]       size_t size, number of bytes in message
*/
void Showbuffer( const char * buffer, size_t size )
    {
    size_t i;
    assert(buffer);

    if( !is_debug() )
        return;

    printf("Size : %lu\n", size);
    for( i = 0; i < size; ++i, ++buffer )
        {
        if( (*buffer >= ' ') && (*buffer <= 0x7e) )
            {
            printf("%c", *buffer);
            }
        else
            {
            switch( *buffer )
                {
                case ACK :
                    printf("<ACK>");
                    break;
                case CR :
                    printf("<CR>");
                    break;
                case ENQ :
                    printf("<ENQ>");
                    break;
                case EOT :
                    printf("<EOT>");
                    break;
                case ETB :
                    printf("<ETB>");
                    break;
                case ETX :
                    printf("<ETX>");
                    break;
                case LF :
                    printf("<LF>");
                    break;
                case NAK :
                    printf("<NAK>");
                    break;
                case STX :
                    printf("<STX>");
                    break;
                default :
                    printf("<0x%02x>", *buffer);
                    break;
                }
            }
        }
    printf("\n");
    }
#endif  // _DEBUG_
