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

    file        getargs.c

    date        09.03.2019

    author      Uwe Jantzen (jantzen@klabautermann-software.de)

    brief       Read commnd line arguments

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"
#include "globals.h"
#include "debug.h"
#include "utils.h"
#include "getargs.h"


/*  function        void getargs( int argc, char *argv[] )

    brief           Handles command line parameters.
                    Exits program on error.

    param[in]       int argc, number of command line parameters
    param[in]       char *argv[], command line parameter list
*/
void getargs( int argc, char *argv[] )
    {
    int i = 0;
    int option = 0;

    debug("Options:\n");
    while( ( option = getopt(argc, argv, "vdcri:o:h") ) != -1 )
        {
        switch( option )
            {
            case 'o':
                showerr(set_outfile_name(optarg));
                debug(" -o %s\n", get_outfile_name());
                break;
            case 'i':
                showerr(set_infile_name(optarg, i++));
                set_infile_number(i);
                break;
            case 'c':
                set_cvs_out(TRUE);
                debug(" -c\n");
                break;
            case 'r' :
                set_reformat(1);
                showerr(set_infile_name(optarg, i++));
                set_infile_number(i);
                break;
            case 'v':
                set_verbose(TRUE);
                debug(" -v\n");
                break;
#ifdef _DEBUG_
            case 'd':
                set_debug(TRUE);
                debug(" -d\n");
                break;
#endif  // _DEBUG_
            case 'h':
            default:
                showhelp(argv[0]);
                break;
            }
        }

    for( int j=0; j<i; ++j )
        debug(" -i %s\n", get_infile_name(j));
    if( is_reformat() )
        debug(" -r\n");
    debug("\n");
    }
