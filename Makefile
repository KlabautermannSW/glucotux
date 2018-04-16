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
#   date        30.03.2018
#
#   author      Uwe Jantzen (jantzen@klabautermann-software.de)
#
#   brief       Makefile for glucotux and glucotux-cli
#
#   details     Rebuilds all necessary files to create glucotux and glucotux-cli
#				An object file depends on every include file's change to prevent
#				missing a change in a nested include file.
#
#   project     glucotux
#   target      Linux
#   begin       03.03.2012
#
#   note
#
#   todo
#

# debug code
# .PHONY: printvars
# printvars:
#	@$(foreach V,$(sort $(.VARIABLES)), \
	$(if $(filter-out environ% default automatic, \
	$(origin $V)),$(info $V=$($V) ($(value $V)))))

vpath %.h include
vpath %.c src
vpath %.o obj

CC  := gcc

DSRC := src
DINC := include
DOBJ := obj
DBIN := bin

# create the list of object files from the ist of source files
OBJ = $(shell ls $(DSRC) | sed 's/\(\.c\)/.o/' )

VERSION := 1.00
VERSION_CLI := 0.03

CC_LDFLAGS = -lm
# use this for the release version
#CFLAGS = `pkg-config --cflags gtk+-3.0` -I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\" -DVERSION_CLI=\"$(VERSION_CLI)\"
# use this for the debug version
CFLAGS = `pkg-config --cflags gtk+-3.0` -I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\" -DVERSION_CLI=\"$(VERSION_CLI)\" -D_DEBUG_

.c.o: $(DOBJ)
	$(CC) $(CFLAGS) -c $< -o $(DOBJ)/$@

####### Build rules

all: install glucotux-cli


glucotux-cli : $(OBJ) $(DBIN)
	$(CC) $(CC_LDFLAGS) -o $(DBIN)/$@ \
		$(DOBJ)/glucotux-cli.o \
		$(DOBJ)/astm.o \
		$(DOBJ)/contour.o \
		$(DOBJ)/files.o \
		$(DOBJ)/debug.o \
		$(DOBJ)/utils.o \
		$(DOBJ)/errors.o \
		$(DOBJ)/getargs.o \
		$(DOBJ)/globals.o \
		$(DOBJ)/version.o


glucotux-cli.o : glucotux-cli.c  getargs.h version.h globals.h contour.h astm.h files.h

astm.o : astm.c errors.h globals.h debug.h utils.h contour.h astm.h

contour.o : contour.c errors.h globals.h debug.h utils.h contour.h

files.o : files.c errors.h debug.h astm.h utils.h files.h

debug.o : debug.c globals.h

utils.o : utils.c globals.h

errors.o : errors.c errors.h

getargs.o : getargs.c errors.h globals.h debug.h utils.h getargs.h

globals.o : globals.c globals.h

version.o : FORCE

FORCE:

####### create object and executable directory if missing
install:
	@if [ ! -d  $(DBIN) ]; then mkdir $(DBIN); fi
	@if [ ! -e  $(DOBJ) ]; then mkdir $(DOBJ); fi

####### cleanup all objects and executables
clean:
	rm -v -f $(DOBJ)/* $(DBIN)/*
	rmdir $(DOBJ) $(DBIN)
