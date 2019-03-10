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

    date        09.03.2019

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
#include "globals.h"


/*  function    int explode( char * elements, char * str, char delimiter,
                             int lines, int length )

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
    param[in]   char * str, the string to explode
    param[in]   char delimiter, separates the string's elements
    param[in]   int lines, number of separate strings that fits into elements
    param[in]   int length, maximum length of a sperate string - including
                the terminating '0'

    return      int, number of separated strings found
*/
int explode( char * elements, char * str, char delimiter, int lines, int length )
    {
    int idx;                                                                    // data index
    int dst_idx;                                                                // element's string index
    int line;
    int len;

    len = strlen(str);
    memset(elements, 0, lines * length);                                        // now all elements are empty
    idx = 0;
    line = 0;
    dst_idx = 0;

    while( idx < len )
        {
        while( ( str[idx] != delimiter ) && ( idx < len ) )
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


/*  function        void time2ger( char * dst, char * src )

    brief           Converts a time from a record to a string holding german
                    time format.

    param[out]      char * dst, german formatted time and date
                    DD.MM.YYYY hh:mm
                    Has to be at least 17 byte long!
                    NO lenmgth check done here!!
    param[in]       char * src, the timestamp read from a record
*/
void time2ger( char * dst, char * src )
    {
    memcpy(dst, src + 6, 2);                                                    // day
    *(dst + 2) = '.';
    memcpy(dst + 3, src + 4, 2);                                                // month
    *(dst + 5) = '.';
    memcpy(dst + 6, src, 4);                                                    // year
    *(dst + 10) = ' ';
    memcpy(dst + 11, src + 8, 2);                                               // hour
    *(dst + 13) = ':';
    memcpy(dst + 14, src + 10, 2);                                              // minute
    *(dst + 16) = 0;
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
    printf("        %s [options] [-o <outfile> [-i <infile> [-i|-r <infile>]]]\n", name);
    printf("Options:\n");
    printf("        -o <outfile>  file to put the data in,\n");
    printf("                      if not set data is printed to screen\n");
    printf("        -i <infile>   file to get the data from,\n");
    printf("                      if set data is read from <infile> and sorted into <outfile>\n");
    printf("                      removing duplicate lines.\n");
    printf("                      <outfile> and <infile> must NOT BE THE SAME!\n");
    printf("        -c <infile>   If this option is selected the application reads the data\n");
    printf("                      from <infile> and writes it to <outfile> using CVS format as\n");
    printf("                      follows:\n");
    printf("                      date|time|glucose|glucose unit|insulin|insulin type|carb|carb unit|user mark\n");
    printf("        -r <infile>   If this option is selected the application reads the data\n");
    printf("                      from <infile> using the data order that was implemented\n");
    printf("                      before 30.3.2018. Then it writes back the data to <outfile>\n");
    printf("                      using the current data order.\n");
    printf("           If you give two <infile>s they are mixed and put out to <outfile> (not implemented yet!).\n");
    printf("\n");
    printf("        -v            enable verbose mode\n");
#ifdef _DEBUG_
    printf("        -d            enable debug mode\n");
#endif  //  _DEBUG_
    printf("\n");
    printf("        -h            show this help then stop without doing anything more\n");
    printf("\n");
    printf("\n");
    exit(0);
    }


/*  function        void showbuffer( char const * buffer, int size )

    brief           Displays a frame with decoded special characters, unknown
                    characters as their ascii number in hex and all others as
                    printable characters.
                    This is done in debug mode only!

    param[in]       char const * buffer, message buffer
    param[in]       size_t size, number of bytes in message
*/
void showbuffer( char const * buffer, int size )
    {
    size_t i;

    if( !is_debug() )
        return;

    printf("Size : %d\n", size);
    for( i = 0; i < size; ++i, ++buffer )
        {
        if( (*buffer >= ' ') && (*buffer <= 0xfe) )
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
