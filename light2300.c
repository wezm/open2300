/*  open2300 - light2300.c
 *  
 *  Version 1.10
 *  
 *  Control WS2300 weather station
 *  
 *  Copyright 2003-2005, Kenneth Lavrsen
 *  This program is published under the GNU General Public license
 */

#include "rw2300.h"

/********************************************************************
 * print_usage prints a short user guide
 *
 * Input:   none
 * 
 * Output:  prints to stdout
 * 
 * Returns: exits program
 *
 ********************************************************************/
void print_usage(void)
{
	printf("\n");
	printf("light2300 - Control background light of a WS-2300 weather station\n");
	printf("Version %s (C)2003-2004 Kenneth Lavrsen.\n", VERSION);
	printf("This program is released under the GNU General Public License (GPL)\n\n");
	printf("Usage:\n");
	printf("Turn light off:    light2300 off config_filename\n");
	printf("Turn light on:     light2300 on config_filename\n");
	exit(0);
}
 
/********** MAIN PROGRAM ************************************************
 *
 * Control background light of a WS-2300 weather station
 * and writes the data to a log file.
 *
 * Just run the program without parameters for usage.
 *
 * It takes two parameters. The first is the log filename with path
 * The second is the config file name with path
 * If this parameter is omitted the program will look at the default paths
 * See the open2300.conf-dist file for info
 *
 ***********************************************************************/
int main(int argc, char *argv[])
{
	WEATHERSTATION ws2300;
	struct config_type config;

	if (argc < 2 || argc > 3)
	{
		print_usage();
	}			

	get_configuration(&config, argv[2]);

	ws2300 = open_weatherstation(config.serial_device_name);

   /* Get on or off */

	if (strcmp(argv[1],"on") == 0)
	{
		light(ws2300,1);
	}
	else if (strcmp(argv[1],"off") == 0)
	{
		light(ws2300,0);
	}
	else
	{
		print_usage();
	}

	close_weatherstation(ws2300);
	
	return (0);
}

