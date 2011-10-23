####### Makefile for open2300 - manually generated
#
# Starting from v 1.2 all user parameters are stored in a config file
# Default locations are
# 1. Path to config file including filename given as parameter
# 2. ./open2300.conf
# 3. /usr/local/etc/open2300.conf
# 4. /etc/open2300.conf
#
# This makefile is made for Linux.
# For Windows version modify the CC_LDFLAG by adding a -lwsock32
#
# You may want to adjust the 3 directories below

prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
libdir = ${prefix}/lib

#########################################

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
LFLAGS = -shared -Wl,-dylib_install_name
LSUFFIX = dylib
endif
ifeq ($(UNAME), Linux)
LFLAGS = -shared -Wl,-soname
LSUFFIX = so
endif

CC  = gcc
LIB = lib2300
LIB_C = rw2300.c linux2300.c
LIBOBJ = rw2300.o linux2300.o

VERSION = 1.11

CFLAGS = -Wall -O3 -DVERSION=\"$(VERSION)\"
CC_LDFLAGS = -L. -lm -l2300
INSTALL = install
MAKE_EXEC = $(CC) $(CFLAGS) $@.c -o $@ $(CC_LDFLAGS)

####### Build rules

all: open2300 dump2300 dumpconfig2300 log2300 fetch2300 wu2300 cw2300 history2300 histlog2300 bin2300 xml2300 light2300 interval2300 minmax2300 sqlitelog2300

lib2300 :
	$(CC) -c -fPIC $(CFLAGS) $(LIB_C)
	$(CC) $(LFLAGS),$@.$(LSUFFIX) -o $@.$(LSUFFIX).$(VERSION) $(LIBOBJ)
	ln -sf $@.$(LSUFFIX).$(VERSION) $@.$(LSUFFIX)

open2300 : $(LIB)
	$(MAKE_EXEC)

dump2300 : $(LIB)
	$(MAKE_EXEC)

dumpconfig2300 : $(LIB)
	$(MAKE_EXEC)

log2300 : $(LIB)
	$(MAKE_EXEC)

fetch2300 : $(LIB)
	$(MAKE_EXEC)

srv2300 : $(LIB)
	$(MAKE_EXEC)

wu2300 : $(LIB)
	$(MAKE_EXEC)

cw2300 : $(LIB)
	$(MAKE_EXEC)

history2300 : $(LIB)
	$(MAKE_EXEC)

histlog2300 : $(LIB)
	$(MAKE_EXEC)

bin2300 : $(LIB)
	$(MAKE_EXEC)

xml2300 : $(LIB)
	$(MAKE_EXEC)

mysql2300: $(LIB)
	$(CC) $(CFLAGS) $@.c -o $@ -I/usr/include/mysql -L/usr/lib/mysql $(CC_LDFLAGS) -lmysqlclient

pgsql2300: $(LIB)
	$(CC) $(CFLAGS) $@.c -o $@ -I/usr/include/pgsql -L/usr/lib/pgsql $(CC_LDFLAGS) -lpq

sqlitelog2300: $(LIB)
	$(CC) $(CFLAGS) $@.c -o $@ $(CC_LDFLAGS) -lsqlite3

light2300: $(LIB)
	$(MAKE_EXEC)

interval2300: $(LIB)
	$(MAKE_EXEC)

minmax2300: $(LIB)
	$(MAKE_EXEC)

mysqlhistlog2300 : $(LIB)
	$(CC) $(CFLAGS) $@.c -o $@ -I/usr/include/mysql -L/usr/lib/mysql $(CC_LDFLAGS) -lmysqlclient


install:
	mkdir -p $(bindir)
	mkdir -p $(libdir)
	$(INSTALL) $(LIB).$(LSUFFIX).$(VERSION) $(libdir)
	ln -sf $(libdir)/$(LIB).$(LSUFFIX).$(VERSION) $(libdir)/$(LIB).$(LSUFFIX)
	$(INSTALL) srv2300 $(bindir)
	$(INSTALL) open2300 $(bindir)
	$(INSTALL) dump2300 $(bindir)
	$(INSTALL) log2300 $(bindir)
	$(INSTALL) fetch2300 $(bindir)
	$(INSTALL) wu2300 $(bindir)
	$(INSTALL) cw2300 $(bindir)
	$(INSTALL) histlog2300 $(bindir)
	$(INSTALL) xml2300 $(bindir)
	$(INSTALL) light2300 $(bindir)
	$(INSTALL) interval2300 $(bindir)
	$(INSTALL) minmax2300 $(bindir)
#	$(INSTALL) mysql2300 $(bindir)
#	$(INSTALL) mysqlhistlog2300 $(bindir)

uninstall:
	rm -f $(libdir)/$(LIB).* $(bindir)/open2300 $(bindir)/dump2300 $(bindir)/log2300  $(bindir)/fetch2300 $(bindir)/srv2300 $(bindir)/wu2300 $(bindir)/cw2300 $(bindir)/xml2300 $(bindir)/light2300 $(bindir)/interval2300 $(bindir)/minmax2300 $(bindir)/histlog2300 $(bindir)/mysql2300 $(bindir)/mysqlhistlog2300

clean:
	rm -f *~ *.o *.$(LSUFFIX)* open2300 dump2300 log2300 fetch2300 wu2300 cw2300 history2300 histlog2300 bin2300 xml2300 mysql2300 pgsql2300 light2300 interval2300 minmax2300 mysql2300 mysqlhistlog2300
