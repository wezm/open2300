/*  open2300 - dump2300.c
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
	printf("dump2300 - Dump all data from WS-2300 to file.\n");
	printf("Data is stored with address in human readable format\n");
	printf("Version %s (C)2003 Kenneth Lavrsen.\n", VERSION);
	printf("This program is released under the GNU General Public License (GPL)\n\n");
	printf("Usage:\n");
	printf("dump2300 filename start_address end_address\n");
	printf("Addresses in hex, range 0-1FFF\n");
	exit(0);
}

 
/********** MAIN PROGRAM ************************************************
 *
 * This program reads from a WS2300 weather station at a given address
 * range and write the data in a text file in human readable format.
 *
 * Just run the program without parameters
 * for usage.
 *
 * It uses the config file for device name.
 * Config file locations - see open2300.conf-dist
 *
 ***********************************************************************/
int main(int argc, char *argv[])
{
	WEATHERSTATION ws2300;
	FILE *fileptr;
	unsigned char data[20];
	unsigned char command[25]; //room for write data also
	int i, j;
	int address, start_adr, end_adr;
	int bytes = 15;
	struct config_type config;

	// Get serial port from connfig file.
	// Note: There is no command line config file path feature!
	// history2300 will only search the default locations for the config file
	
	get_configuration(&config, "");


	// Setup serial port

	ws2300 = open_weatherstation(config.serial_device_name);


	// Get in-data and select mode.

	if (argc!=4)
	{
		print_usage();
		exit(EXIT_FAILURE);
	}
	
	fileptr = fopen(argv[1], "w");
	if (fileptr == NULL)
	{
		printf("Cannot open file %s\n",argv[1]);
		exit(EXIT_FAILURE);
	}
	
	start_adr = strtol(argv[2],NULL,16);
	end_adr = strtol(argv[3],NULL,16);
	
	if (start_adr < 0 || start_adr > 0x1FFF || end_adr < 0 ||
	                  end_adr > 0x1FFF || start_adr>=end_adr)
	{
		printf("Address range invalid\n");
		exit(EXIT_FAILURE);
	}
	
	for (address=start_adr ; address<=end_adr ; address+=15*2)
	{

		// Start the process of reading data
		// Again we retry until success or MAXRETRIES
		for(j=0;j<MAXRETRIES;j++)
		{
			reset_06(ws2300);
			
			if ( (end_adr - address < 2*15) && (end_adr - address >= 0) )
			{
				bytes = (end_adr - address + 1)/2 + (end_adr - address + 1)%2;
			}
			
			// Read the data. If expected number of bytes read break out of loop.
			if (read_data(ws2300, address, bytes, data, command)==bytes)
			{
				break;
			}
		}
		
		// If we have tried MAXRETRIES times to read we expect not to
		// have valid data
		if (j==MAXRETRIES)
		{
			printf("\nError reading data\n");
			fclose(fileptr);
			exit(EXIT_FAILURE);
		}
	
		// Write out the data
		for (i=0; i<bytes;i++)
		{		
			printf("Address: %04X|%04X - Data: %02X\n",address+2*i+1,
			       address+2*i,data[i]);
			fprintf(fileptr,"A: %04X|%04X - D: %02X\n",address+2*i+1,
			        address+2*i,data[i]);
		}
	}

	// Goodbye and Goodnight
	close_weatherstation(ws2300);

	return(0);
}

