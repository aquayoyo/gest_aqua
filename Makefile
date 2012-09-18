#######################################################################
#
# (C) Copyright 2003
# Stim
# All rights reserved.
#
#
#  captimage 
#
# $Id: Makefile,v 1.1 2003/11/19 16:28:49 wd Exp $
#
#######################################################################
TOP = $(shell pwd)/
CONFIG = $(TOP)/config
include $(CONFIG)

#INC_DIR=-I./mysql/ -I./mysql/cppconn -I/usr/include
INC_DIR=-I./ -I$(TOP)commun/ -I$(TOP)sqlite3/

#REP_LIB = $(TOP)mysql/lib_$(BOARD) 
#UTIL_LIB= -lmysqlcppconn 

ifeq ($(BOARD),PC)
	ifeq ($(CROSS_COMPILE),yes)
		REP_LIB = $(TOP)sqlite3/.libs/
	endif
endif

UTIL_LIB=-lsqlite3 -lpthread

OBJECT = $(TOP)commun/Thread.o MainTache.o Timer.o AquaGest.o
NOMEXE = GestAqua
DESTDIR= debug/
ifeq ($(GDB),)
OPT = -O2 -s -Wall  -fivopts $(LDARCH)
MAP=
else
OPT =
MAP=-Wl,-Map,$(NOMEXE).map
endif

CFLAGS = $(GDB) $(OPT) $(INC_DIR) -D$(BOARD)
CPPFLAGS = $(GDB) $(OPT) $(INC_DIR) -D$(BOARD) 

appli: $(OBJECT) 
	$(CPP) $(MAP) -L$(REP_LIB) $(UTIL_LIB) $(OBJECT) -o $(NOMEXE)

.SUFFIXES: .cpp .c .o .fl .h

.c.o:
	$(CC) -c $(CFLAGS) -o "$@" "$<"
	
.cpp.o:
	$(CPP) -c $(CPPFLAGS) -o "$@" "$<"

clean:
	/bin/rm -f $(NOMEXE) $(NOMEXE).map $(OBJECT)
