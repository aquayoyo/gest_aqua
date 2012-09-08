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
INC_DIR=-I$(TOP)sqlite3/

#REP_LIB = $(TOP)mysql/lib_$(BOARD) 
#UTIL_LIB= -lmysqlcppconn 

ifeq ($(BOARD),PC)
ifeq ($(CROSS_COMPILE),yes)
REP_LIB = $(TOP)sqlite3/.libs/
UTIL_LIB=-lsqlite3
endif
#else
#REP_LIB = /usr/lib/
#UTIL_LIB=-lsqlite3
endif

OPT = -O3 -s -Wall  -fivopts $(LDARCH)
CFLAGS = $(GDB) $(OPT) $(INC_DIR) -D$(BOARD)
CPPFLAGS = $(GDB) $(OPT) $(INC_DIR) -D$(BOARD)

OBJECT = main.o
NOMEXE = GestAqua

appli: $(OBJECT) 
	$(CPP) -L$(REP_LIB) -lpthread $(UTIL_LIB) $(OBJECT) -o $(NOMEXE)

.SUFFIXES: .cpp .c .o .fl .h

.c.o:
	$(CC) $(CFLAGS) $< -c

.cpp.o:
	$(CPP) $(CPPFLAGS) $< -c

clean:
	/bin/rm -f $(NOMEXE) *.o
