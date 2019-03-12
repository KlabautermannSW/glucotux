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
#   date        14.10.2018
#
#   author      Uwe Jantzen (jantzen@klabautermann-software.de)
#
#   brief       Makefile for glucotux-cli and glucotux
#
#   details     Rebuilds all necessary files to create glucotux and glucotux
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
#.PHONY: printvars
#printvars:
#	@$(foreach V,$(sort $(.VARIABLES)), \
#	$(if $(filter-out environ% default automatic, \
#	$(origin $V)),$(info $V=$($V) ($(value $V)))))

vpath %.h include
vpath %.c src
vpath %.o obj

CC  := gcc

DSRC := src
DINC := include
DOBJ := obj
DBIN := bin

OBJ := glucotux.o mainwindow.o graphs.o astm.o contour.o files.o debug.o utils.o errors.o getargs.o globals.o version.o
OBJ_CLI := glucotux-cli.o astm.o contour.o files.o debug.o utils.o errors.o getargs.o globals.o version.o

VERSION := 1.00
VERSION_CLI := 0.03
COMMITDATE := $(shell git show --pretty=format:"%cd" --date=format:"%d.%m.%Y" -s)

CC_LDFLAGS = -lm
CFLAGS = -I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\" -DVERSION_CLI=\"$(VERSION_CLI)\" -DCOMMITDATE=\"$(COMMITDATE)\" -D_DEBUG_
# CFLAGS = `-I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\" -DVERSION_CLI=\"$(VERSION_CLI)\"
CFLAGS_GTK = `pkg-config --cflags gtk+-3.0` -I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\" -DVERSION_CLI=\"$(VERSION_CLI)\" -DCOMMITDATE=\"$(COMMITDATE)\" -D_DEBUG_
# CFLAGS_GTK = `pkg-config --cflags gtk+-3.0` -I $(DINC) -Wall -O3 -DVERSION=\"$(VERSION)\" -DVERSION_CLI=\"$(VERSION_CLI)\"

####### Build rules

all: glucotux-cli

glucotux-cli : install $(OBJ_CLI) $(DBIN)
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

glucotux : install $(OBJ) $(DBIN)
	$(CC) $(CC_LDFLAGS) -o $(DBIN)/$@ \
		$(DOBJ)/glucotux.o \
		$(DOBJ)/mainwindow.o \
		$(DOBJ)/astm.o \
		$(DOBJ)/graphs.o \
		$(DOBJ)/contour.o \
		$(DOBJ)/files.o \
		$(DOBJ)/debug.o \
		$(DOBJ)/utils.o \
		$(DOBJ)/errors.o \
		$(DOBJ)/getargs.o \
		$(DOBJ)/globals.o \
		$(DOBJ)/version.o \
		`pkg-config --libs gtk+-3.0`

glucotux-cli.o : glucotux-cli.c  getargs.h version.h globals.h contour.h astm.h files.h
	$(CC) $(CFLAGS) -c $(DSRC)/glucotux-cli.c -o $(DOBJ)/glucotux-cli.o

glucotux.o : glucotux.c getargs.h version.h globals.h graphs.h contour.h astm.h files.h
	$(CC) $(CFLAGS_GTK) -c $(DSRC)/glucotux.c -o $(DOBJ)/glucotux.o

mainwindow.o : mainwindow.c mainwindow.h graphs.h
	$(CC) $(CFLAGS_GTK) -c $(DSRC)/mainwindow.c -o $(DOBJ)/mainwindow.o

graphs.o : graphs.c graphs.h
	$(CC) $(CFLAGS_GTK) -c $(DSRC)/graphs.c -o $(DOBJ)/graphs.o

astm.o : astm.c errors.h globals.h debug.h utils.h contour.h astm.h
	$(CC) $(CFLAGS) -c $(DSRC)/astm.c -o $(DOBJ)/astm.o

contour.o : contour.c errors.h globals.h debug.h utils.h contour.h
	$(CC) $(CFLAGS) -c $(DSRC)/contour.c -o $(DOBJ)/contour.o

files.o : files.c errors.h debug.h astm.h utils.h files.h
	$(CC) $(CFLAGS) -c $(DSRC)/files.c -o $(DOBJ)/files.o

debug.o : debug.c globals.h
	$(CC) $(CFLAGS) -c $(DSRC)/debug.c -o $(DOBJ)/debug.o

utils.o : utils.c globals.h
	$(CC) $(CFLAGS) -c $(DSRC)/utils.c -o $(DOBJ)/utils.o

errors.o : errors.c errors.h
	$(CC) $(CFLAGS) -c $(DSRC)/errors.c -o $(DOBJ)/errors.o

getargs.o : getargs.c errors.h globals.h debug.h utils.h getargs.h
	$(CC) $(CFLAGS) -c $(DSRC)/getargs.c -o $(DOBJ)/getargs.o

globals.o : globals.c globals.h debug.h
	$(CC) $(CFLAGS) -c $(DSRC)/globals.c -o $(DOBJ)/globals.o

version.o : FORCE
	$(CC) $(CFLAGS) -c $(DSRC)/version.c -o $(DOBJ)/version.o

FORCE:

####### create object and executable directory if missing
install: FORCE
	@if [ ! -d  $(DBIN) ]; then mkdir $(DBIN); fi
	@if [ ! -d  $(DOBJ) ]; then mkdir $(DOBJ); fi

####### cleanup all objects and executables
.PHONY clean:
clean:
	-rm -v -f $(DOBJ)/* $(DBIN)/*
	-rmdir $(DOBJ) $(DBIN)
