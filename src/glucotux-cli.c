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

    file        glucotux-cli.c

    date        08.03.2018

    author      Uwe Jantzen (Klabautermann@Klabautermann-Software.de)

    brief

    details

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "getargs.h"
#include "version.h"
#include "globals.h"
#include "contour.h"
#include "astm.h"
#include "files.h"


static const char * name = "GlucoTux CLI";


/*  function        int main( int argc, char *argv[] )

    brief           main function :
                        reads arguments
                        connect to a contour device
                        read out data

    param[in]       int argc, number of command line parameters
    param[in]       char *argv[], command line parameter list

    return          int, error code
*/
int main( int argc, char *argv[] )
    {
    int handle;
    int contour_type;
    char c;

    printf(title, name, version_cli, compiledate);
    getargs(argc, argv);

    if( strlen(get_oldfile_name()) != 0 )
        {
        if( is_reformat() )
            reformat(get_oldfile_name(), get_logfile_name());
        else
            mixfiles(get_oldfile_name(), get_logfile_name());
        return 0;
        }

    handle = wait_for_contour(&contour_type);
    if( handle < 0 )
        exit(handle);

    if( contour_type == CONTOUR_USB_CODE )
        {
        c = read_astm(handle);
        if( c != ENQ )
            goto finish;
        }
    else                                                                        // contour_type == CONTOUR_USB_NEXT_CODE
        usleep(5 * 1000 * 1000);

    data_transfer_mode(handle);

finish:
    close_contour(handle);

    printf("\n%s finished\n\n", name);

    return 0;
    }
