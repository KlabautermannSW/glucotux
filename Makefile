#
#   Copyright (C)
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.
#   If not, see <http://www.gnu.org/licenses/>.
#
#   Klabautermann Software
#   Uwe Jantzen
#   Weingartener Straße 33
#   76297 Stutensee
#   Germany
#
#   file        Makefile
#
#   date        08.03.2018
#
#   author      Uwe Jantzen (jantzen@klabautermann-software.de)
#
#   brief       Makefile for glucotux-cli
#
#   details     Check usb lines for a Bayer Contuor USB device
#
#   project     glucotux
#   target      Linux
#   begin       03.03.2012
#
#   note
#
#   todo
#

vpath %.h include
vpath %.c src
vpath %.o obj

CC  = gcc
OBJ = glucotux-cli.o astm.o contour.o debug.o utils.o getargs.o globals.o version.o

DSRC = src
DINC = include
DOBJ = obj
DBIN = bin

VERSION = 0.03

CFLAGS = -I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\" -D_DEBUG_
#CFLAGS = -I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\"
CC_LDFLAGS = -lm

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(DOBJ)/$@

####### Build rules

all: install glucotux-cli no-version

glucotux-cli : $(OBJ)
	$(CC) $(CC_LDFLAGS) -o $(DBIN)/$@ \
		$(DOBJ)/glucotux-cli.o \
		$(DOBJ)/astm.o \
		$(DOBJ)/contour.o \
		$(DOBJ)/debug.o \
		$(DOBJ)/utils.o \
		$(DOBJ)/getargs.o \
		$(DOBJ)/globals.o \
		$(DOBJ)/version.o

no-version :
	@rm -v -f $(DOBJ)/version.o > /dev/null

glucotux-cli.o : glucotux-cli.c astm.h contour.h utils.h getargs.h version.h

astm.o : astm.c astm.h globals.h

contour.o : contour.c contour.h globals.h

debug.o : debug.c globals.h

utils.o : utils.c

getargs.o : getargs.c getargs.h globals.h utils.h

globals.o : globals.c globals.h

version.o : version.c version.h

####### create object and executable directory if missing
install:
	@if [ ! -d  $(DBIN) ]; then mkdir $(DBIN); fi
	@if [ ! -e  $(DOBJ) ]; then mkdir $(DOBJ); fi

####### cleanup all objects and executables
clean:
	rm -v -f $(DOBJ)/* $(DBIN)/*
	rmdir $(DOBJ) $(DBIN)
