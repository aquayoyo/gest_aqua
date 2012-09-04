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

INC_DIR=-I./mysql/ -I./mysql/cppconn

REP_LIB = $(TOP)mysql/lib 
UTIL_LIB= -lmysqlcppconn 

OPT = -O3 -s -Wall  -fivopts $(LDARCH)
CFLAGS = $(GDB) $(OPT) $(INC_DIR) -D$(BOARD)
CPPFLAGS = $(GDB) $(OPT) $(INC_DIR) -D$(BOARD)

OBJECT = main.o
NOMEXE = GestAqua

appli: $(OBJECT) 
	$(CPP) -Wall -L$(REP_LIB) -lpthread $(OBJECT) $(UTIL_LIB) -o $(NOMEXE)

.SUFFIXES: .cpp .c .o .fl .h

.c.o:
	$(CC) $(CFLAGS) $< -c

.cpp.o:
	$(CPP) $(CPPFLAGS) $< -c

clean:
	/bin/rm -f $(NOMEXE) *.o