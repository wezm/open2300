/*  open2300.c
 *  
 *  Version 1.11
 *  
 *  Control WS2300 weather station
 *  
 *  Copyright 2003-2006, Kenneth Lavrsen
 *  This program is published under the GNU General Public license
 */

#include "rw2300.h"

#define READMODE 0

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
	printf("open2300 - Read and write data to your WS-2300 weather station.\n");
	printf("Version %s (C)2003-2006 Kenneth Lavrsen.\n", VERSION);
	printf("This program is released under the GNU General Public License (GPL)\n\n");
	printf("Usage:\n");
	printf("Read bytes:    open2300 address_hex r number_of_bytes\n");
	printf("Write nibbles: open2300 address_hex w text_hex_string\n");
	printf("Set bits:      open2300 address_hex s bit_number\n");
	printf("Unset bits:    open2300 address_hex u bit_number\n");
	exit(0);
}

 
/********** MAIN PROGRAM ************************************************
 *
 * This program reads and writes from/to a WS2300
 * weather station at a given address
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
	unsigned char data[20];
	unsigned char command[25]; //room for write data also
	char tempchar[] = "0";
	int i,j;
	int address;
	int bytes = 0;
	int nibbles = 0;
	int writemode = 0;
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

	if (!strcmp( argv[2],"r")) //Read bytes mode
	{
		writemode = READMODE;
		bytes =  atoi(argv[3]);
	}
	else if (!strcmp( argv[2],"w")) //Write nibble mode
	{
		nibbles = strlen(argv[3]);
		
		if (nibbles>15)
		{
			printf("Too many digits\n");
			exit(EXIT_FAILURE);
		}
	
		for (i=0;i<nibbles;i++)
		{
			// Convert each byte in string from ASCII to char
			tempchar[0]=argv[3][i];
			data[i]=(char)strtol(tempchar,NULL,16);
		}
		
		writemode = WRITENIB;
		bytes = nibbles/2 + nibbles%2;
	}
	else if (!strcmp( argv[2],"s") || !strcmp( argv[2],"u")) //Bit mode
	{
		nibbles = strlen(argv[3]);
		
		if (nibbles>1)
		{
			printf("Only one digit allowed for set/unset bit\n");
			exit(EXIT_FAILURE);
		}
			
		// Convert bit from ASCII to char. Check range first
		tempchar[0]=argv[3][0];
		if (tempchar[0]>'3' || tempchar[0]<'0')
		{
			printf("Bit must be values 0, 1, 2 or 3\n");
			exit(EXIT_FAILURE);
		}
		
		data[0]=(char)strtol(tempchar,NULL,16);
				
		bytes = nibbles/2 + nibbles%2;
		writemode = !strcmp( argv[2],"s") ? SETBIT : UNSETBIT;
	}
	else
	{
		print_usage();
	}
	
	// Covert address from string array to integer
	address = strtol(argv[1],NULL,16);

	//Range checks
	if (address < 0 || address > 0x1FFF || bytes < 0 || bytes > 15)
	{
		printf("Invalid input range\n");
		exit(EXIT_FAILURE);
	}

	//If writemode start the process of writing data.
	if (writemode)  
	{
		// Retrying as ws2300 often fails communication when its cpu is busy
		// We retry until success or MAXRETRIES
		for(j=0;j<MAXRETRIES;j++)
		{
			reset_06(ws2300);
			
			// Write data sends the address and data to WS2300 and
			// returns number of successfully written data nibbles
			// We have success if we have sent all the ones we wanted to send
			
			
			if (write_data(ws2300, address, nibbles, writemode, data, command)
			                                                        ==nibbles)
			{
				break;
				
			}
		}
		
		// If we have tried MAXRETRIES times to read we expect not to
		// have valid data
		if (j==MAXRETRIES)
		{
			printf("\nError writing data\n");
			exit(EXIT_FAILURE);
		}

		// Write the sent commands
		printf("Write Commands sent:");
		for (i=0; i<nibbles+4; i++)
		{
			printf(" %02X", command[i]);
		}
		printf("\n");
	}

	// Start the process of reading data - we always read data
	// Again we retry until success or MAXRETRIES
	for(j=0;j<MAXRETRIES;j++)
	{
		reset_06(ws2300);
		
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
		exit(EXIT_FAILURE);
	}

	// Write the sent commands
	printf("Read Commands sent:");
	for (i=0; i<5; i++)
	{
		printf(" %02X", command[i]);
	}
	printf("\n");


	// Write out the data
	for (i=0; i<bytes;i++)
	{
		printf("Address: %04X|%04X - Data: %02X\n",
		       address+2*i+1, address+2*i, data[i]);
	}

	// Goodbye and Goodnight
	close_weatherstation(ws2300);

	return(0);
}

