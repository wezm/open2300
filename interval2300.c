/*  open2300 - interval2300.c
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
	printf("interval2300 - Sets history saving interval of a WS-2300 weather station\n");
	printf("Version %s (C)2003-2004 Kenneth Lavrsen.\n", VERSION);
	printf("This program is released under the GNU General Public License (GPL)\n\n");
	printf("Usage:\n");
	printf("Set Interval: light2300 interval_minutes next_dataset_minutes config_filename\n");
	printf("Read current Interval: light2300 0 0 config_filename\n");
	printf("If interval is set to 0 the current setting is displayed but nothing changed\n");
	exit(0);
}
 
/********** MAIN PROGRAM ************************************************
 *
 * Set history interval of a WS-2300 weather station
 * and writes the data to a log file.
 *
 * Just run the program without parameters for usage.
 *
 * It takes 3 parameters. The first is the interval in minutes
 * The 2nd is the number of minutes till next dataset is saved
 * The third is the config file name with path.
 * If this parameter is omitted the program will look at the default paths
 * See the open2300.conf-dist file for info
 *
 ***********************************************************************/
int main(int argc, char *argv[])
{
	WEATHERSTATION ws2300;
	struct config_type config;
	unsigned char data[20];
	unsigned char command[25];  //Data returned is just ignored
	int address;                //Address if first nibble to write
	int number;                 //Number of nibbles to write
	int current_interval;       //Current history interval
	int countdown;              //Countdown timer till next record
	int interval;               //New history interval
	int time_next;              //Set time until next record
	int no_records;             //Number of valid records - reset when interval changes
	struct timestamp time_last; //Timestamp of last record - ignored in this program

	if (argc < 2 || argc > 4)
	{
		print_usage();
	}			

	get_configuration(&config, argv[3]);
	ws2300 = open_weatherstation(config.serial_device_name);

	interval = (int)strtol(argv[1],NULL,10);
	if (argc >= 3)
	{
		time_next = (int)strtol(argv[2],NULL,10);
	}
	else
	{
		time_next = 0;
	}
	
	if (interval >= 1 && interval <= 0xFFF && time_next >= 0 && time_next <= 0xFFF)
	{
		printf("Changing interval to %d minutes\n", interval);
		printf("Next record taken in %d minutes\n", time_next);		
		address=0x6B2;
		number=6;
		
		interval--;
		
		data[0] = interval & 0x00F;
		data[1] = (interval >> 4) & 0x00F;
		data[2] = (interval >> 8) & 0x00F;
		
		if (time_next >= 1)
			time_next--;		
		
		data[3] = time_next & 0x00F;
		data[4] = (time_next >> 4) & 0x00F;
		data[5] = (time_next >> 8) & 0x00F;
		
		write_safe(ws2300, address, number, WRITENIB, data, command);		

		// Set number of valid records to zero
		address=0x6C4;
		number=2;
		data[0] = 0;
		data[1] = 0;
		write_safe(ws2300, address, number, WRITENIB, data, command);
	}

	read_history_info(ws2300, &current_interval, &countdown, &time_last, &no_records);

	printf("History saving interval is %d minutes\n", current_interval);
	printf("Number of valid records is %d\n", no_records);

	close_weatherstation(ws2300);
	
	return (0);
}

