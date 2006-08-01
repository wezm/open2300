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

#########################################

CC  = gcc
OBJ = open2300.o rw2300.o linux2300.o win2300.o
LOGOBJ = log2300.o rw2300.o linux2300.o win2300.o
FETCHOBJ = fetch2300.o rw2300.o linux2300.o win2300.o
WUOBJ = wu2300.o rw2300.o linux2300.o win2300.o
CWOBJ = cw2300.o rw2300.o linux2300.o win2300.o
DUMPOBJ = dump2300.o rw2300.o linux2300.o win2300.o
HISTOBJ = history2300.o rw2300.o linux2300.o win2300.o
HISTLOGOBJ = histlog2300.o rw2300.o linux2300.o win2300.o
DUMPBINOBJ = bin2300.o rw2300.o linux2300.o win2300.o
XMLOBJ = xml2300.o rw2300.o linux2300.o win2300.o
PGSQLOBJ = pgsql2300.o rw2300.o linux2300.o win2300.o
LIGHTOBJ = light2300.o rw2300.o linux2300.o win2300.o
INTERVALOBJ = interval2300.o rw2300.o linux2300.o win2300.o
MINMAXOBJ = minmax2300.o rw2300.o linux2300.o win2300.o
MYSQLHISTLOGOBJ = mysqlhistlog2300.o rw2300.o linux2300.o win2300.o

VERSION = 1.11

CFLAGS = -Wall -O3 -DVERSION=\"$(VERSION)\"
CC_LDFLAGS = -lm
CC_WINFLAG = 
# For Windows - comment the two line above and un-comment the two lines below.
#CC_LDFLAGS = -lm -lwsock32
#CC_WINFLAG = -mwindows
INSTALL = install

####### Build rules

all: open2300 dump2300 log2300 fetch2300 wu2300 cw2300 history2300 histlog2300 bin2300 xml2300 light2300 interval2300 minmax2300 mysql2300 mysqlhistlog2300

open2300 : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(CC_LDFLAGS)
	
dump2300 : $(DUMPOBJ)
	$(CC) $(CFLAGS) -o $@ $(DUMPOBJ) $(CC_LDFLAGS)
	
log2300 : $(LOGOBJ)
	$(CC) $(CFLAGS) -o $@ $(LOGOBJ) $(CC_LDFLAGS) $(CC_WINFLAG)
	
fetch2300 : $(FETCHOBJ)
	$(CC) $(CFLAGS) -o $@ $(FETCHOBJ) $(CC_LDFLAGS)
	
wu2300 : $(WUOBJ)
	$(CC) $(CFLAGS) -o $@ $(WUOBJ) $(CC_LDFLAGS) $(CC_WINFLAG)
	
cw2300 : $(CWOBJ)
	$(CC) $(CFLAGS) -o $@ $(CWOBJ) $(CC_LDFLAGS) $(CC_WINFLAG)

history2300 : $(HISTOBJ)
	$(CC) $(CFLAGS) -o $@ $(HISTOBJ) $(CC_LDFLAGS)
	
histlog2300 : $(HISTLOGOBJ)
	$(CC) $(CFLAGS) -o $@ $(HISTLOGOBJ) $(CC_LDFLAGS) $(CC_WINFLAG)
	
bin2300 : $(DUMPBINOBJ)
	$(CC) $(CFLAGS) -o $@ $(DUMPBINOBJ) $(CC_LDFLAGS)

xml2300 : $(XMLOBJ)
	$(CC) $(CFLAGS) -o $@ $(XMLOBJ) $(CC_LDFLAGS) $(CC_WINFLAG)

mysql2300:
	$(CC) $(CFLAGS) -o mysql2300 mysql2300.c rw2300.c linux2300.c $(CC_LDFLAGS) $(CC_WINFLAG) -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient

pgsql2300: $(PGSQLOBJ)
	$(CC) $(CFLAGS) -o $@ $(PGSQLOBJ) $(CC_LDFLAGS) $(CC_WINFLAG) -I/usr/include/pgsql -L/usr/lib/pgsql -lpq

light2300: $(LIGHTOBJ)
	$(CC) $(CFLAGS) -o $@ $(LIGHTOBJ) $(CC_LDFLAGS)
	
interval2300: $(INTERVALOBJ)
	$(CC) $(CFLAGS) -o $@ $(INTERVALOBJ) $(CC_LDFLAGS)
	
minmax2300: $(MINMAXOBJ)
	$(CC) $(CFLAGS) -o $@ $(MINMAXOBJ) $(CC_LDFLAGS) $(CC_WINFLAG)
	
mysqlhistlog2300 :
	$(CC) $(CFLAGS) -o mysqlhistlog2300 mysqlhistlog2300.c rw2300.c linux2300.c $(CC_LDFLAGS) $(CC_WINFLAG) -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient


install:
	mkdir -p $(bindir)
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
	$(INSTALL) mysql2300 $(bindir)
	$(INSTALL) mysqlhistlog2300 $(bindir)
	
uninstall:
	rm -f $(bindir)/open2300 $(bindir)/dump2300 $(bindir)/log2300 $(bindir)/fetch2300 $(bindir)/wu2300 $(bindir)/cw2300 $(bindir)/xml2300 $(bindir)/light2300 $(bindir)/interval2300 $(bindir)/minmax2300 $(bindir)/mysql2300 $(bindir)/mysqlhistlog2300

clean:
	rm -f *~ *.o open2300 dump2300 log2300 fetch2300 wu2300 cw2300 history2300 histlog2300 bin2300 xml2300 mysql2300 pgsql2300 light2300 interval2300 minmax2300 mysql2300 mysqlhistlog2300

cleanexe:
	rm -f *~ *.o open2300.exe dump2300.exe log2300.exe fetch2300.exe wu2300.exe cw2300.exe history2300.exe histlog2300.exe bin2300.exe xml2300.exe mysql2300.exe pgsql2300.exe light2300.exe interval2300.exe minmax2300.exe