/*  open2300 - log2300.c
 *  
 *  Version 1.11
 *  
 *  Control WS2300 weather station
 *  
 *  Copyright 2003-2006, Kenneth Lavrsen
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
	printf("log2300 - Read and interpret data from WS-2300 weather station\n");
	printf("and write it to a log file. Perfect for a cron driven task.\n");
	printf("Version %s (C)2003-2006 Kenneth Lavrsen.\n", VERSION);
	printf("This program is released under the GNU General Public License (GPL)\n\n");
	printf("Usage:\n");
	printf("Save current data to logfile:    log2300 filename config_filename\n");
	exit(0);
}
 
/********** MAIN PROGRAM ************************************************
 *
 * This program reads current weather data from a WS2300
 * and writes the data to a log file.
 *
 * Log file format:
 * Timestamp Date Time Ti To DP RHi RHo Wind Dir-degree Dir-text WC
 *              Rain1h Rain24h Rain-tot Rel-Press Tendency Forecast
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
	FILE *fileptr;
	char logline[3000] = "";
	char tempstring[1000] = "";
	char datestring[50];        //used to hold the date stamp for the log file
	const char *directions[]= {"N","NNE","NE","ENE","E","ESE","SE","SSE",
	                           "S","SSW","SW","WSW","W","WNW","NW","NNW"};
	double winddir[6];
	int tempint;
	char tendency[15];
	char forecast[15];
	struct config_type config;
	time_t basictime;

	get_configuration(&config, argv[2]);

	ws2300 = open_weatherstation(config.serial_device_name);

	/* Get log filename. */

	if (argc < 2 || argc > 3)
	{
		print_usage();
	}

	fileptr = fopen(argv[1], "a+");
	if (fileptr == NULL)
	{
		printf("Cannot open file %s\n",argv[1]);
		exit(-1);
	}


	/* READ TEMPERATURE INDOOR */

	sprintf(logline,"%.1f ", temperature_indoor(ws2300, config.temperature_conv));


	/* READ TEMPERATURE OUTDOOR */

	sprintf(tempstring,"%.1f ", temperature_outdoor(ws2300, config.temperature_conv));
	strcat(logline, tempstring);


	/* READ DEWPOINT */

	sprintf(tempstring,"%.1f ", dewpoint(ws2300, config.temperature_conv));
	strcat(logline, tempstring);


	/* READ RELATIVE HUMIDITY INDOOR */

	sprintf(tempstring,"%d ", humidity_indoor(ws2300));	
	strcat(logline, tempstring);


	/* READ RELATIVE HUMIDITY OUTDOOR */

	sprintf(tempstring,"%d ", humidity_outdoor(ws2300));	 
	strcat(logline, tempstring);


	/* READ WIND SPEED AND DIRECTION */

	sprintf(tempstring,"%.1f ",
	       wind_all(ws2300, config.wind_speed_conv_factor, &tempint, winddir));
	strcat(logline, tempstring);
	sprintf(tempstring,"%.1f %s ", winddir[0], directions[tempint]);
	strcat(logline, tempstring);


	/* READ WINDCHILL */

	sprintf(tempstring,"%.1f ", windchill(ws2300, config.temperature_conv));
	strcat(logline, tempstring);


	/* READ RAIN 1H */

	sprintf(tempstring,"%.2f ", rain_1h(ws2300, config.rain_conv_factor));
	strcat(logline, tempstring);


	/* READ RAIN 24H */

	sprintf(tempstring,"%.2f ", rain_24h(ws2300, config.rain_conv_factor));
	strcat(logline, tempstring);


	/* READ RAIN TOTAL */

	sprintf(tempstring,"%.2f ", rain_total(ws2300, config.rain_conv_factor));
	strcat(logline, tempstring);


	/* READ RELATIVE PRESSURE */

	sprintf(tempstring,"%.3f ", rel_pressure(ws2300, config.pressure_conv_factor));
	strcat(logline, tempstring);


	/* READ TENDENCY AND FORECAST */

	tendency_forecast(ws2300, tendency, forecast);
	sprintf(tempstring,"%s %s ", tendency, forecast);
	strcat(logline, tempstring);


	/* GET DATE AND TIME FOR LOG FILE, PLACE BEFORE ALL DATA IN LOG LINE */

	time(&basictime);
	strftime(datestring, sizeof(datestring), "%Y%m%d%H%M%S %Y-%b-%d %H:%M:%S",
	         localtime(&basictime));


	// Print out and leave

	// printf("%s %s\n",datestring, logline); //disabled to be used in cron job
	fprintf(fileptr, "%s %s\n", datestring, logline);

	close_weatherstation(ws2300);
	
	fclose(fileptr);

	return(0);
}

