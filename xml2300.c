/*  open xml2300.c
 *  
 *  Version 1.10
 *  
 *  Control WS2300 weather station
 *  
 *  Copyright 2003-2005, Kenneth Lavrsen
 *  This program is published under the GNU General Public license
 *  
 *  Update History:
 *  1.0 2004 Feb 27 Matt Woodward    Inital version sent to Kenneth
 *                                   Lavrsen 2004 Feb 27
 *                                   based on fetch2300 v 1.0.
 *                                   Introduced also the serial port
 *                                   interface for Windows
 *
 *  1.2 2004 Mar 07 Kenneth Lavrsen  Completely re-written to match
 *                  (OZ1IDD)         rw2300 v 1.2
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
	printf("xml2300 - Read and write data to your WS-2300 weather station.\n");
	printf("Version %s (C)2004 Kenneth Lavrsen.\n", VERSION);
	printf("This program is released under the GNU General Public License (GPL)\n\n");
	printf("Usage:\n");
	printf("With default config file: xml2300 xml-file\n");
	printf("With given config file:   xml2300 xml-file config-file\n");
	exit(0);
}

/********** MAIN PROGRAM ************************************************
 *
 * This program reads all current and min/max data from a WS2300
 * weather station and write it to an XML file.
 *
 * It takes two parameters. xml_file_path and config_file_path
 *
 * If the config_file_path parameter is omitted the program will look
 * at the default paths.  See the open2300.conf-dist file for info
 *
 ***********************************************************************/
int main(int argc, char *argv[])
{
	WEATHERSTATION ws2300;
	char datestring[50];        //used to hold the date stamp for the log file
	const char *directions[]= {"N","NNE","NE","ENE","E","ESE","SE","SSE",
	                           "S","SSW","SW","WSW","W","WNW","NW","NNW"};
	double winddir[6];
	char tendency[15];
	char forecast[15];
	struct config_type config;
	double tempfloat_min, tempfloat_max;
	int tempint, tempint_min, tempint_max;
	struct timestamp time_min, time_max;
	time_t basictime;
	//char datestring[100];		//used to hold the date stamp for the log file
	FILE *fileptr;

	if (argc < 2 || argc > 3)
	{
		print_usage();
	}
	
	get_configuration(&config, argv[2]);
	
	fileptr = fopen(argv[1], "w");
	if (fileptr == NULL)
	{
		printf("Cannot open file %s\n",argv[1]);
		exit(EXIT_FAILURE);
	}
	

	ws2300 = open_weatherstation(config.serial_device_name);

	/* XML header */

	fprintf(fileptr, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			"<ws2300 version=\"1.0\">\n");

	/* GET DATE AND TIME FOR LOG FILE, PLACE BEFORE ALL DATA IN LOG LINE */

	/* <date>, <time> */
	
	time(&basictime);
	strftime(datestring, sizeof(datestring), "\t<Date>%Y-%m-%d</Date>\n"
			 "\t<Time>%H:%M:%S</Time>\n", localtime(&basictime));
			 
	fprintf(fileptr, datestring);


	/* <temperature> <indoor> */
	
	fprintf(fileptr, "\t<Temperature>\n" "\t\t<Indoor>\n");
	fprintf(fileptr, "\t\t\t<Value>%.1f</Value>\n",
		temperature_indoor(ws2300, config.temperature_conv));

	temperature_indoor_minmax(ws2300, config.temperature_conv, &tempfloat_min,
		                      &tempfloat_max, &time_min, &time_max);

	fprintf(fileptr, "\t\t\t<Min>%.1f</Min>\n", tempfloat_min);
	fprintf(fileptr, "\t\t\t<Max>%.1f</Max>\n", tempfloat_max);

	fprintf(fileptr, "\t\t\t<MinTime>%02d:%02d</MinTime>\n"
			"\t\t\t<MinDate>%04d-%02d-%02d</MinDate>\n",
			time_min.hour, time_min.minute, time_min.year,
			time_min.month, time_min.day);

	fprintf(fileptr, "\t\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t\t</Indoor>\n" "\t\t<Outdoor>\n");


	/* <temperature> <outdoor> */

	fprintf(fileptr, "\t\t\t<Value>%.1f</Value>\n",
			temperature_outdoor(ws2300, config.temperature_conv));
	
	temperature_outdoor_minmax(ws2300, config.temperature_conv, &tempfloat_min,
	                          &tempfloat_max, &time_min, &time_max);

	fprintf(fileptr, "\t\t\t<Min>%.1f</Min>\n", tempfloat_min);
	fprintf(fileptr, "\t\t\t<Max>%.1f</Max>\n", tempfloat_max);


	fprintf(fileptr, "\t\t\t<MinTime>%02d:%02d</MinTime>\n"
			"\t\t\t<MinDate>%04d-%02d-%02d</MinDate>\n",
			time_min.hour, time_min.minute, time_min.year,
			time_min.month, time_min.day);

	fprintf(fileptr, "\t\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t\t</Outdoor>\n" "\t</Temperature>\n");


	/* <indoor> <humidity> */

	fprintf(fileptr, "\t<Humidity>\n" "\t\t<Indoor>\n");

	fprintf(fileptr, "\t\t\t<Value>%d</Value>\n",
			humidity_indoor_all(ws2300, &tempint_min, &tempint_max,
			                    &time_min, &time_max));

	fprintf(fileptr, "\t\t\t<Min>%d</Min>\n", tempint_min);
	fprintf(fileptr, "\t\t\t<Max>%d</Max>\n", tempint_max);

	fprintf(fileptr, "\t\t\t<MinTime>%02d:%02d</MinTime>\n"
			"\t\t\t<MinDate>%04d-%02d-%02d</MinDate>\n",
			time_min.hour, time_min.minute, time_min.year,
			time_min.month, time_min.day);

	fprintf(fileptr, "\t\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t\t</Indoor>\n" "\t\t<Outdoor>\n");
	

	/* <outdoor> <humidity> */

	fprintf(fileptr, "\t\t\t<Value>%d</Value>\n",
			humidity_outdoor_all(ws2300, &tempint_min, &tempint_max,
			                    &time_min, &time_max));

	fprintf(fileptr, "\t\t\t<Min>%d</Min>\n", tempint_min);
	fprintf(fileptr, "\t\t\t<Max>%d</Max>\n", tempint_max);

	fprintf(fileptr, "\t\t\t<MinTime>%02d:%02d</MinTime>\n"
			"\t\t\t<MinDate>%04d-%02d-%02d</MinDate>\n",
			time_min.hour, time_min.minute, time_min.year,
			time_min.month, time_min.day);

	fprintf(fileptr, "\t\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t\t</Outdoor>\n" "\t</Humidity>\n" "\t<Dewpoint>\n");

	/* <Dewpoint> */

	fprintf(fileptr, "\t\t<Value>%.1f</Value>\n",
	        dewpoint(ws2300, config.temperature_conv));

	dewpoint_minmax(ws2300, config.temperature_conv, &tempfloat_min,
	               &tempfloat_max, &time_min, &time_max);
	                           
	fprintf(fileptr, "\t\t<Min>%.1f</Min>\n", tempfloat_min);
	fprintf(fileptr, "\t\t<Max>%.1f</Max>\n", tempfloat_max);

	fprintf(fileptr, "\t\t<MinTime>%02d:%02d</MinTime>\n"
			"\t\t<MinDate>%04d-%02d-%02d</MinDate>\n",
			time_min.hour, time_min.minute, time_min.year,
			time_min.month, time_min.day);

	fprintf(fileptr, "\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t</Dewpoint>\n" "\t<Wind>\n");
	

	/* <Wind> */

	fprintf(fileptr, "\t\t<Value>%.1f</Value>\n",
		   wind_all(ws2300, config.wind_speed_conv_factor, &tempint, winddir));

	fprintf(fileptr, "\t\t<Direction>\n");
	fprintf(fileptr, "\t\t\t<Text>%s</Text>\n"
			"\t\t\t<Dir0>%0.1f</Dir0>\n"
			"\t\t\t<Dir1>%0.1f</Dir1>\n"
			"\t\t\t<Dir2>%0.1f</Dir2>\n"
			"\t\t\t<Dir3>%0.1f</Dir3>\n"
			"\t\t\t<Dir4>%0.1f</Dir4>\n"
			"\t\t\t<Dir5>%0.1f</Dir5>\n"
			"\t\t</Direction>\n",
			directions[tempint],
			winddir[0], winddir[1], winddir[2],
			winddir[3], winddir[4], winddir[5]);

	//Get Windspeed min/max
	wind_minmax(ws2300, config.wind_speed_conv_factor, &tempfloat_min,
	            &tempfloat_max, &time_min, &time_max);
	
	fprintf(fileptr, "\t\t<Min>%.1f</Min>\n", tempfloat_min);
	fprintf(fileptr, "\t\t<Max>%.1f</Max>\n", tempfloat_max);

	fprintf(fileptr, "\t\t<MinTime>%02d:%02d</MinTime>\n"
			"\t\t<MinDate>%04d-%02d-%02d</MinDate>\n",
			time_min.hour, time_min.minute, time_min.year,
			time_min.month, time_min.day);

	fprintf(fileptr, "\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t</Wind>\n" "\t<Windchill>\n");
	
	
	/* <Windchill> */

	fprintf(fileptr, "\t\t<Value>%.1f</Value>\n",
	        windchill(ws2300, config.temperature_conv));
	
	windchill_minmax(ws2300, config.temperature_conv, &tempfloat_min,
	                 &tempfloat_max, &time_min, &time_max);

	fprintf(fileptr, "\t\t<Min>%.1f</Min>\n",tempfloat_min);
	fprintf(fileptr, "\t\t<Max>%.1f</Max>\n",tempfloat_max);

	fprintf(fileptr, "\t\t<MinTime>%02d:%02d</MinTime>\n"
			"\t\t<MinDate>%04d-%02d-%02d</MinDate>\n",
			time_min.hour, time_min.minute, time_min.year,
			time_min.month, time_min.day);

	fprintf(fileptr, "\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t</Windchill>\n" "\t<Rain>\n" "\t\t<OneHour>\n");


	/* <Rain> <OneHour> */

	fprintf(fileptr, "\t\t\t<Value>%.2f</Value>\n",
	        rain_1h_all(ws2300, config.rain_conv_factor,
	                    &tempfloat_max, &time_max));

	fprintf(fileptr, "\t\t\t<Max>%.2f</Max>\n", tempfloat_max);

	fprintf(fileptr, "\t\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t\t</OneHour>\n" "\t\t<TwentyFourHour>\n");
	
	
	/* <Rain> <TwentyFourHour> */

	fprintf(fileptr, "\t\t\t<Value>%.2f</Value>\n",
			rain_24h_all(ws2300, config.rain_conv_factor,
			             &tempfloat_max, &time_max));

	fprintf(fileptr, "\t\t\t<Max>%.2f</Max>\n", tempfloat_max);

	fprintf(fileptr, "\t\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t\t</TwentyFourHour>\n" "\t\t<Total>\n");
	
	
	/* <Rain> <Total> */

	fprintf(fileptr, "\t\t\t<Value>%.2f</Value>\n",
	        rain_total_all(ws2300, config.rain_conv_factor, &time_max));

	fprintf(fileptr, "\t\t\t<Time>%02d:%02d</Time>\n"
			"\t\t\t<Date>%04d-%02d-%02d</Date>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);

	fprintf(fileptr, "\t\t</Total>\n" "\t</Rain>\n" "\t<Pressure>\n");
	

	/* <Pressure> */

	fprintf(fileptr, "\t\t<Value>%.3f</Value>\n",
	        rel_pressure(ws2300, config.pressure_conv_factor));

	rel_pressure_minmax(ws2300, config.pressure_conv_factor, &tempfloat_min,
	                    &tempfloat_max, &time_min, &time_max);

	fprintf(fileptr, "\t\t<Min>%.3f</Min>\n", tempfloat_min);
	fprintf(fileptr, "\t\t<Max>%.3f</Max>\n", tempfloat_max);

	fprintf(fileptr, "\t\t<MinTime>%02d:%02d</MinTime>\n"
			"\t\t<MinDate>%04d-%02d-%02d</MinDate>\n",
			time_min.hour, time_min.minute, time_min.year,
			time_min.month, time_min.day);

	fprintf(fileptr, "\t\t<MaxTime>%02d:%02d</MaxTime>\n"
			"\t\t<MaxDate>%04d-%02d-%02d</MaxDate>\n",
			time_max.hour, time_max.minute, time_max.year,
	        time_max.month, time_max.day);
			

	/* <Tendency> <Forecast> */
	
	tendency_forecast(ws2300, tendency, forecast);

	fprintf(fileptr, "\t\t<Tendency>%s</Tendency>\n"
			"\t</Pressure>\n"
			"\t<Forecast>%s</Forecast>\n", tendency, forecast);


	fprintf(fileptr, "</ws2300>\n");

	fflush(fileptr);
	fclose(fileptr);

	close_weatherstation(ws2300);

	return (0);
}
