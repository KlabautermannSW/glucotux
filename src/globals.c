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


#include <string.h>
#include "errors.h"
#include "globals.h"


#define FILENAME_LEN                        1024


static int verbose_flag = 0;
static int debug_flag = 0;
static int cvs_out_flag = 0;
static int reformat_flag = 0;
static char outfile_name[FILENAME_LEN] = {0, };
static char infile_name[2][FILENAME_LEN] = {0, };
static int infile_number = 0;


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


/*  function        void set_cvs_out( int flag )

    brief           Sets the cvs_out flag's state

    param[in]       int flag, cvs_out flag
*/
void set_cvs_out( int flag )
    {
    cvs_out_flag = flag;
    }


/*  function        int is_cvs_out( void )

    brief           Returns cvs_out flag's state

    return          int, cvs_out flag's state
*/
int is_cvs_out( void )
    {
    return cvs_out_flag;
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


/*  function        int set_outfile_name( char * filename )

    brief           Sets the the log file's name from filename.

    param[in]       char * filename, log file's name
*/
int set_outfile_name( char * filename )
    {
    int len = strlen(filename);

    if( len > FILENAME_LEN - 1 )
        return ERR_FILE_NAME_LENGTH;

    memcpy(outfile_name, filename, len+1);

    return NOERR;
    }


/*  function        char const *  get_outfile_name( void )

    brief           Return the pointer to the log file's name.

    return          char const *, pointer to the log file's name
*/
char const *  get_outfile_name( void )
    {
    return outfile_name;
    }


/*  function        int set_infile_name( char * filename, int i )

    brief           Sets the input file's name from filename.

    param[in]       char * filename, input file's name
    param[in]       int i, index of input file
*/
int set_infile_name( char * filename, int i )
    {
    int len = strlen(filename);

    if( i > 1 )
        return ERR_NUM_OF_INFILES;

    if( len > FILENAME_LEN - 1 )
        return ERR_FILE_NAME_LENGTH;

    memcpy(infile_name[i], filename, len+1);

    return NOERR;
    }


/*  function        char const *  set_infile_name( int i )

    brief           Return the pointer to the input file's name.

    return          char const *, pointer to the input file's name
*/
char const *  get_infile_name( int i )
    {
    if( i > 1 )
        showerr(ERR_NUM_OF_INFILES);

    return infile_name[i];
    }


/*  function        int set_infile_number( int i )

    brief           Sets the number of input file

    param[in]       int i, number of input files
*/
int set_infile_number( int i )
    {
    infile_number = i;

    return NOERR;
    }


/*  function        int const get_infile_number( void )

    brief           Return the number of input files.

    return          int const, number of input files.
*/
int const get_infile_number( void )
    {
    return infile_number;
    }
