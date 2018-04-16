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

    file        globals.c

    date        08.03.2018

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief       Handle com munication with countour device using ASTM specifications.

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#include <string.h>
#include "errors.h"
#include "globals.h"


#define FILENAME_LEN                        1024


static int verbose_flag = 0;
static int debug_flag = 0;
static int reformat_flag = 0;
static char logfile_name[FILENAME_LEN] = {0, };
static char oldfile_name[FILENAME_LEN] = {0, };


/*  function        void set_verbose( int flag )

    brief           Sets the verbose flag's state

    param[in]       int flag, verbose flag
*/
void set_verbose( int flag )
    {
    verbose_flag = flag;
    }


/*  function        int is_verbose( void )

    brief           Returns verbose flag's state

    return          int, verbose flag's state
*/
int is_verbose( void )
    {
    return verbose_flag;
    }


/*  function        void set_debug( int flag )

    brief           Sets the debug flag's state

    param[in]       int flag, debug flag
*/
void set_debug( int flag )
    {
    debug_flag = flag;
    }


/*  function        int is_debug( void )

    brief           Returns debug flag's state

    return          int, debug flag's state
*/
int is_debug( void )
    {
    return debug_flag;
    }


/*  function        void set_reformat( int flag )

    brief           Sets the reformat flag's state

    param[in]       int flag, reformat flag
*/
void set_reformat( int flag )
    {
    reformat_flag = flag;
    }


/*  function        int is_reformat( void )

    brief           Returns reformat flag's state

    return          int, reformat flag's state
*/
int is_reformat( void )
    {
    return reformat_flag;
    }


/*  function        int set_logfile_name( char * filename )

    brief           Sets the the log file's name from filename.

    param[in]       char * filename, log file's name
*/
int set_logfile_name( char * filename )
    {
    int len = strlen(filename);

    if( len > FILENAME_LEN - 1 )
        return ERR_FILE_NAME_LENGTH;

    memcpy(logfile_name, filename, len+1);

    return NOERR;
    }


/*  function        const char *  get_logfile_name( void )

    brief           Return the pointer to the log file's name.

    return          const char *, pointer to the log file's name
*/
const char *  get_logfile_name( void )
    {
    return logfile_name;
    }


/*  function        int set_oldfile_name( char * filename )

    brief           Sets the the log file's name from filename.

    param[in]       char * filename, log file's name
*/
int set_oldfile_name( char * filename )
    {
    int len = strlen(filename);

    if( len > FILENAME_LEN - 1 )
        return ERR_FILE_NAME_LENGTH;

    memcpy(oldfile_name, filename, len+1);

    return NOERR;
    }


/*  function        const char *  set_oldfile_name( void )

    brief           Return the pointer to the log file's name.

    return          const char *, pointer to the log file's name
*/
const char *  get_oldfile_name( void )
    {
    return oldfile_name;
    }
