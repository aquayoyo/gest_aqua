#######################################################################
#
# (C) Copyright 2012
# lionel
# All rights reserved.
#
#
#  olinuxino_gest_aqua 
#
# $Id: Makefile
#
#######################################################################
TOP = $(shell pwd)/
CONFIG = $(TOP)/config
include $(CONFIG)

#INC_DIR=-I./mysql/ -I./mysql/cppconn -I/usr/include
INC_DIR=-I./ -I$(TOP)commun/ -I$(TOP)sqlite3/

#REP_LIB = $(TOP)mysql/lib_$(BOARD) 
#UTIL_LIB= -lmysqlcppconn 

UTIL_LIB=-lsqlite3 -lpthread

OBJECT = Profil.o Lune.o Soleil.o \
$(TOP)commun/Thread.o $(TOP)commun/General.o \
BaseSqlite.o MainTache.o Timer.o gpio-mmap.o PwmGpio.o AquaGest.o
 
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

ifeq ($(BOARD),PC)
	ifeq ($(CROSS_COMPILE),yes)
		REP_LIB = $(TOP)sqlite3/.libs/
		$(CFLAGS)+= -DOLINUXINO
		$(CPPFLAGS)+= -DOLINUXINO
	endif
endif
appli: $(OBJECT) 
	$(CPP) $(MAP) -L$(REP_LIB) $(UTIL_LIB) $(OBJECT) -o $(NOMEXE)

.SUFFIXES: .cpp .c .o .fl .h

.c.o:
	$(CC) -c $(CFLAGS) -o "$@" "$<"
	
.cpp.o:
	$(CPP) -c $(CPPFLAGS) -o "$@" "$<"

clean:
	/bin/rm -f $(NOMEXE) $(NOMEXE).map $(OBJECT)
