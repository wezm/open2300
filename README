open2300

version 1.11

Copyright 2003-2005 by Kenneth Lavrsen
This program is published under the GNU General Public License version 2.0
or later. Please read the file 'COPYING' for more info.


With open2300 you can read and write to and from a WS-2300 weather station.
You do anything - maybe even harm the station or bring it into a mode it
never comes out of again.
It is your choice if you want to take the risk.
The author takes no responsibility for any damage the use of this program
may cause.


dump2300 is a special tool which will dump a range of memory into a file
AND to the screen. This makes is possible to easily see what has changed in
an area larger than 15 bytes and even use a file comparison tool to spot the
changes. Note that the WS-2300 operates with addresses of 4-bit nibbles.
When you specify an odd number of addresses the program will always add one
as it always fetches bytes from the station and displays these bytes.
If you only want a file and no display output just run it as:
dump2300 filename start end > /dev/null
If you only want to display and not create a file run:
dump2300 /dev/null start end


bin2300 does the same as dump2300 except the data is written in binary
format to the file. It is not byte for byte. Each nibble from the station
is saved as one byte in the file (always value from 00 to 0F).
The advantage of this is that when you look with a binary file viewer and
you started from address 0 the addresses fit 1:1.


history2300 read out a selected range of the history records as raw data to
both screen and file. Output is human readable.


log2300 is a new useful tool. It reads all the current data from the station
and append the data to a log file with a time stamp. The file format is
human readable text but without any labels - only with single space between
each value and a newline for each record. This makes it easy to pick up the
data using e.g. Perl or PHP for presentation on the web.


fetch2300 was added in 0.6. This software reads all current values from the  
station incl min/max data and dates and output them in a simple format to
standard out. The idea is that you can call this program from a Perl or PHP
page and load the data in an array ready for display.
To load the data into php place this example code in the page.

<?php
exec("/usr/local/bin/fetch2300",$fetcharray);
foreach ($fetcharray as $value)
{
	list($parameter,$parvalue)=explode(" ", $value);
	$ws["$parameter"]=$parvalue;
}
?>

You can let the rest be pure HTML and just put in for example
<?=$ws["To"]?> to show outdoor temperature.


wu2300 was added in 1.0. This is a development of fetch2300 which only fetches
the data used by Weather Underground (http://www.wunderground.com) and it
uploads your data to your personal weather station page at Weather Underground.
For this to work you need to register your station at Weather Underground.
You will then receive an ID and a password. This ID and password must be added
to the config file (open2300.conf) before the program is used.


cw2300 was added in 1.2. This again is a version of fetch2300 which fetched
current data from the weather station and sends it to the Citizen Weather
Observation Program via the APRSWXNET (Automatic Positioning Reporting System
Weather Network) but using one of the Internet APRS servers.
I received the original version from Randy Miller (radio ham N6ABM).
It was fully working and a great piece of work.
I had to completely rewrite it to fit my new library rw2300 and to make
it compatible with Windows. Thanks to Randy for a significant contribution.
Here are some URLs if you want to know more
http://www.wxqa.com/
http://pond1.gladstonefamily.net:8080/aprswxnet.html
http://www.findu.com/
ftp://ftp.tapr.org/aprssig/aprsspec/spec/aprs101/APRS101.pdf


xml2300 was added in 1.2. It is another example of work derived from
fetch2300. Again it collects data from the weather station and stores
the data in an XML file.
I originally received this program from Matt Woodward. I have re-written
it completely to work with the new rw2300 function library. I also changed
the XML coding for the fun of it. It is very easy to modify the source to
any XML coding you like.


mysql2300 was added in 1.2 and based on a contribution from Thomas Grieder.
It works like log2300 but instead of writing to a flat file it stores the
weather data in a MySQL database.
The program requires that MySQL client library and header files are installed.
At the release of 1.2 I had not yet built a version on Windows. But with the
right MySQL things installed and some tweaks in the Makefile it should
compile also on Windows.

pgsql2300 was added in 1.9 and based on a contribution from Przemyslaw Sztoch.
It works like log2300 but instead of writing to a flat file it stores the
weather data in a PostgresSQL database.
The program requires that PostgresSQL client library and header files are installed.
This is only available for Linux for the moment.

light2300.c was added in 1.3
It is a small tool that can turn the LCD backlight on and off


histlog2300.c was added in 1.3
It does a similar job to log2300.c but instead of reading the current data
it read the history data stored in the weather station. Windchill and dewpoint
are calculated values based on the other measurements.
It checks the log file for the last record written and read all the new
records and add them to the log file.


interval2300.c was added in 1.3
This is a small tool set can set and read the interval at which the weather
station saves the history datasets.


minmax2300.c was added in 1.4
It is a new tool using the new rw2300 reset functions to reset min/max for
all measurements and resetting rain counters.


rw2300.c / rw2300.h
This is the common function library. This has been extended in 1.2 so that
now you can read actual weather data using these functions without having
to think about decoding the data from the weather station.
Thanks again to Randy Miller for giving inspiration to creating these new
functions.


linux2300.c / linux2300.h
This is part of the common function library and contains all the platform
unique functions. These files contains the functions that are special for
Linux. It contains re-written code from the 1.1 version of rw2300. The function
that connects to the APRS server is built on code partly writen by Randy
Miller.


win2300.c / win2300.h
This is part of the common function library and contains all the platform
unique functions. These files contains the functions that are special for
Windows. The most important new part is the code that handles the serial port.
This function is written by Matt Woodward.
As long as a windows C compiler defines a variable called WIN32 the compiler
will choose to the win2300.c and .h files when compiling.
Kenneth has used the MingW32 compiler on Windows. This is a free open source
compiler that uses the gcc compiler and makefiles.
The Windows programs have been tested on Windows XP.


memory_map_2300.txt is a very useful information file that tells all the
currently known positions of data inside the weather station.
The information in this file may not be accurate. It is gathered by
people mainly from the German language Weather Station Forum
http://www.wetterstationsforum.de/phpBB/viewforum.php?forum=28&403
by hours of experiments. None of the information has come from the
manufacturer of the station. If you find something new please send email
to the author (email address below).
Using the memory map and the rw2300 library is it pretty easy to create
your own Linux driven interface for your weather station.


Installing:

Read the file INSTALL.



How to use the programs:
All the programs now uses a config file to get information like
COM port (serial device name), preferred units of dimentions,
etc etc.
See the INSTALL file and the open2300.conf-dist file for more info.
Note that you should copy the open2300.conf-dist to your preferred location
and that it should be renamed to open2300.conf.

open2300
Read bytes:    open2300 address_hex r number_of_bytes
Write nibbles: open2300 address_hex w text_hex_string
Set bits:      open2300 address_hex s bit_number
Unset bits:    open2300 address_hex c bit_number

dump2300
Write address to file:	dump2300 filename start_address end_address
The addresses are simply written in hex. E.g. 21C 3A1

bin2300
Write address to file:	bin2300 filename start_address end_address
The addresses are simply written in hex. E.g. 21C 3A1

history2300
Write records to file:	history2300 filename start_record end_record
The addresses are simply written in hex. E.g. 1B 3A

log2300
Write current data to log interpreted: log2300 filename config_filename
This is very suitable for a cron job since it makes no output to screen.
If no config_filename is given the program will search for it using the
default search sequence - see the open2300.conf-dist file.

fetch2300
Write current data to standard out: fetch2300 config_filename
It takes one parameter which is the config file name with path.
If this parameter is omitted the program will look at the default paths.
See the open2300.conf-dist file for info.

wu2300
Send current data to Weather Underground: wu2300 config_filename
It takes one parameter which is the config file name with path.
If this parameter is omitted the program will look at the default paths.
See the open2300.conf-dist file for info.
Remember to add your Weather Underground ID and password to the config file.
To get an account at Weather Underground - go here
http://www.wunderground.com/weatherstation/index.asp

cw2300
Send current data to CWOP: cw2300 config_filename
It takes one parameter which is the config file name with path.
If this parameter is omitted the program will look at the default paths.
See the open2300.conf-dist file for info.
To join the Citizen Weather Observer Program (CWOP) - go here
http://www.wxqa.com/
At the release of version 1.4 the 4 APRS servers given should be valid.
If they later change simply update the config file.
Without a config file two default servers are hard coded in the program.

xml2300
Write current data to XML file: xml2300 xml-filename config_filename
It takes two parameters. xml_file_path and config_filename.
If the config_filename parameter is omitted the program will look
at the default paths.  See the open2300.conf-dist file for info

mysql2300
Write current data to MySQL database: mysql2300 config_filename
It takes one parameter which is the config file name with path.
If this parameter is omitted the program will look at the default paths.
See the open2300.conf-dist file for info.

pgsql2300
Write current data to PostgresSQL database: pgsql2300 config_filename
It takes one parameter which is the config file name with path.
If this parameter is omitted the program will look at the default paths.
See the open2300.conf-dist file for info.

light2300
Turn light off:    light2300 off config_filename
Turn light on:     light2300 on config_filename
If the config_filename parameter is omitted the program will look
at the default paths.  See the open2300.conf-dist file for info

histlog2300
Write history data to log file interpreted.
histlog2300 log_filename config_filename
If the config_filename parameter is omitted the program will look
at the default paths.  See the open2300.conf-dist file for info

interval2300
Read or set the time interval at which the weatherstation saves the
history data.
Set Interval: light2300 interval_minutes next_dataset_minutes config_filename.
Read current Interval: light2300 0 0 config_filename\n".
If interval is set to 0 the current setting is displayed but nothing changed.
If the config_filename parameter is omitted the program will look
at the default paths.  See the open2300.conf-dist file for info

minmax2300
Reset minimum/maximum values in a WS-2300 weather station.
Reset Daily Maximum (Temp, Humid, WC, DP): minmax2300 dailymax config_filename
Reset Daily Minimum (Temp, Humid, WC, DP): minmax2300 dailymin config_filename
Reset Temperature Indoor Max|Min|Both: minmax2300 timax|timin|tiboth config_filename
Reset Temperature Outdoor Max|Min|Both: minmax2300 tomax|tomin|toboth config_filename
Reset Dewpoint Max|Min|Both: minmax2300 dpmax|dpmin|dpboth config_filename
Reset Windchill Max|Min|Both: minmax2300 wcmax|wcmin|wcboth config_filename
Reset Wind Max|Min|Both: minmax2300 wmax|wmin|wboth config_filename
Reset Humidity Indoor Max|Min|Both: minmax2300 himax|himin|hiboth config_filename
Reset Humidity Outdoor Max|Min|Both: minmax2300 homax|homin|hoboth config_filename
Reset Pressure Max|Min|Both: minmax2300 pmax|pmin|pboth config_filename
Reset Rain Maximum 1h|24h: minmax2300 r1max|r24max config_filename
Reset Rain Counter 1h|24h|Total: minmax2300 r1|r24|rtotal config_filename
If the config_filename parameter is omitted the program will look
at the default paths.  See the open2300.conf-dist file for info


In version 0.7 I added a directory htdocs. It contains a simple PHP
webpage that will fetch the current weather data directly from your
station and show it on a nice webpage.
It will run on any webserver running PHP. Just copy the files to any
directory in the web tree. The png and jpg files are small graphics
used on the webpage to show forecast and tendency.

Kenneth Lavrsen
kenneth@lavrsen.dk