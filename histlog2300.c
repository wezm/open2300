/*  open2300 - histlog2300.c
 *  
 *  Version 1.11
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
	printf("histlog2300 - Log history data from WS-2300 to file.\n");
	printf("Version %s (C)2003-2006 Kenneth Lavrsen.\n", VERSION);
	printf("This program is released under the GNU General Public License (GPL)\n\n");
	printf("Usage:\n");
	printf("histlog2300 log_filename config_filename\n");
	exit(0);
}

 
/********** MAIN PROGRAM ************************************************
 *
 * This program reads the history records from a WS2300
 * weather station at a given record range
 * and prints the data to stdout and to a file.
 * Just run the program without parameters for usage.
 *
 * It uses the config file for device name.
 * Config file locations - see open2300.conf-dist
 *
 ***********************************************************************/
int main(int argc, char *argv[])
{
	WEATHERSTATION ws2300;
	FILE *fileptr;
	char logline[3000] = "";
	char tempstring[1000] = "";
	int interval, countdown, no_records;
	struct config_type config;
	long counter;
	char ch;
	char datestring[50];        //used to hold the date stamp for the log file
	struct timestamp time_last;
	time_t time_lastlog, time_lastrecord;
	struct tm time_lastlog_tm, time_lastrecord_tm;
	int current_record, next_record, lastlog_record, new_records;
	double temperature_in;
	double temperature_out;
	double dewpoint;
	double windchill;
	double pressure;
	double pressure_term;
	int humidity_in;
	int humidity_out;
	double rain;
	double windspeed;
	double winddir_degrees;
	const char *directions[]= {"N","NNE","NE","ENE","E","ESE","SE","SSE",
	                           "S","SSW","SW","WSW","W","WNW","NW","NNW"};

	int temp_int1, temp_int2, i;

	if (argc < 2 || argc > 3)
	{
		print_usage();
	}

	// Get serial port from config file. Use 2nd command line parameter

	get_configuration(&config, argv[2]);

	// Setup serial port

	ws2300 = open_weatherstation(config.serial_device_name);

	// Get in-data and select mode.

	fileptr = fopen(argv[1], "ab+");
	if (fileptr == NULL)
	{
		fprintf(stderr,"Cannot open file %s\n",argv[1]);
		exit(EXIT_FAILURE);
	}

	fseek(fileptr, 1L, SEEK_END);
	counter = 60;

	do
	{
		counter++;
		if (fseek(fileptr, -counter, SEEK_END) < 0 )
			break;
		ch = getc(fileptr);
	} while (ch != '\n' && ch != '\r');
	
	if (fscanf(fileptr,"%4d%2d%2d%2d%2d", &temp_int1, &temp_int2,
	           &time_lastlog_tm.tm_mday, &time_lastlog_tm.tm_hour,
	           &time_lastlog_tm.tm_min) == 5)
	{
		time_lastlog_tm.tm_year = temp_int1 - 1900;
		time_lastlog_tm.tm_mon = temp_int2 - 1;	
		time_lastlog_tm.tm_sec = 0;
		time_lastlog_tm.tm_isdst = -1;
	}
	else
	{	//if no valid log we set the date to 1 Jan 1990 0:00
		time_lastlog_tm.tm_year = 90;
		time_lastlog_tm.tm_mon = 0;
		time_lastlog_tm.tm_mday = 1;
		time_lastlog_tm.tm_hour = 0;
		time_lastlog_tm.tm_min = 0;
		time_lastlog_tm.tm_sec = 0;
		time_lastlog_tm.tm_isdst = -1;
	}
	
	time_lastlog = mktime(&time_lastlog_tm);
		
	current_record = read_history_info(ws2300, &interval, &countdown, &time_last,
	                           &no_records);
	                           
	time_lastrecord_tm.tm_year = time_last.year - 1900;
	time_lastrecord_tm.tm_mon  = time_last.month - 1;
	time_lastrecord_tm.tm_mday = time_last.day;
	time_lastrecord_tm.tm_hour = time_last.hour;
	time_lastrecord_tm.tm_min  = time_last.minute;
	time_lastrecord_tm.tm_sec  = 0;
	time_lastrecord_tm.tm_isdst = -1;

	time_lastrecord = mktime(&time_lastrecord_tm);
	
	pressure_term = pressure_correction(ws2300, config.pressure_conv_factor);

	new_records = (int)difftime(time_lastrecord,time_lastlog) / (60 * interval);
	
	if (new_records > 0xAF)
		new_records = 0xAF;
		
	if (new_records > no_records)
		new_records = no_records;

	lastlog_record = current_record - new_records;
	
	if (lastlog_record < 0)
		lastlog_record = 0xAE + lastlog_record + 1;

	time_lastrecord_tm.tm_min -= new_records * interval;
	
	for (i = 1; i <= new_records; i++)
	{ 
		next_record = (i + lastlog_record) % 0xAF;
		
		read_history_record(ws2300, next_record, &config,
		                &temperature_in,
		                &temperature_out,
		                &pressure,
		                &humidity_in,
		                &humidity_out,
		                &rain,
		                &windspeed,
		                &winddir_degrees,
		                &dewpoint,
		                &windchill);


		/* READ TEMPERATURE INDOOR */

		sprintf(logline,"%.1f ", temperature_in);


		/* READ TEMPERATURE OUTDOOR */

		sprintf(tempstring,"%.1f ", temperature_out);
		strcat(logline, tempstring);


		/* CALCULATE DEWPOINT */

		sprintf(tempstring,"%.1f ", dewpoint);
		strcat(logline, tempstring);


		/* READ RELATIVE HUMIDITY INDOOR */

		sprintf(tempstring,"%d ", humidity_in);
		strcat(logline, tempstring);


		/* READ RELATIVE HUMIDITY OUTDOOR */

		sprintf(tempstring,"%d ", humidity_out);
		strcat(logline, tempstring);


		/* READ WIND SPEED AND DIRECTION aND WINDCHILL */

		sprintf(tempstring,"%.1f %.1f %s ", windspeed, winddir_degrees,
		        directions[(int)(winddir_degrees/22.5)]);
		strcat(logline, tempstring);


		/* READ WINDCHILL */

		sprintf(tempstring,"%.1f ", windchill);
		strcat(logline, tempstring);


		/* READ RAIN 1H */

		/*	sprintf(logline,"%s%.2f ", logline,
		        rain_1h(ws2300, config.rain_conv_factor));
		*/	
		
		/* READ RAIN 24H */

		/*
		sprintf(logline,"%s%.2f ", logline,
		        rain_24h(ws2300, config.rain_conv_factor));
		*/

		/* READ RAIN TOTAL */

		sprintf(tempstring,"%.2f ", rain);
		strcat(logline, tempstring);

		/* READ RELATIVE PRESSURE */

		sprintf(tempstring,"%.3f ", pressure + pressure_term);
		strcat(logline, tempstring);


		/* GET DATE AND TIME FOR LOG FILE, PLACE BEFORE ALL DATA IN LOG LINE */
		
		//	printf("time now: %d\n",time(&basictime));
		//	time_lastrecord_tm.tm_hour=time_last.hour;

		time_lastrecord_tm.tm_min += interval;
		mktime(&time_lastrecord_tm);                 //normalize time_lastlog_tm
		
		strftime(datestring, sizeof(datestring), "%Y%m%d%H%M%S %Y-%b-%d %H:%M:%S",
		         &time_lastrecord_tm);

		// Print out
		fseek(fileptr, 0L, SEEK_END);
		fprintf(fileptr, "%s %s\n", datestring, logline);
		fflush(NULL);
	}

	// Goodbye and Goodnight
	close_weatherstation(ws2300);
	fclose(fileptr);

	return(0);
}

