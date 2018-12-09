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

    file        version.c

    date        09.11.2018

    author      Uwe Jantzen (jantzen@klabautermann-software.de)

    brief       Version data

    details     The file version.o will be deleted after every compiling, so
                date is always equal to the compile date.

    project     glucotux
    target      Linux
    begin       03.03.2012

    note

    todo

*/


#include "version.h"


char const version_cli[] = VERSION_CLI;
char const version[] = VERSION;
char const compiledate[] = __DATE__;
char const title[] = "\n\"%s version V%s\", (c) Uwe Jantzen (Klabautermann-Software) %s\n\n";
char const name[] = "GlucoTux";
