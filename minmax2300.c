/*  open2300 - minmax2300.c
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
	printf("minmax2300 - Reset minimum/maximum values in a WS-2300 weather station\n");
	printf("Version %s (C)2003-2004 Kenneth Lavrsen.\n", VERSION);
	printf("This program is released under the GNU General Public License (GPL)\n\n");
	printf("Usage:\n");
	printf("Reset Daily Maximum (Temp, Humid, WC, DP): minmax2300 dailymax config_filename\n");
	printf("Reset Daily Minimum (Temp, Humid, WC, DP): minmax2300 dailymin config_filename\n");
	printf("Reset Temperature Indoor Max|Min|Both: minmax2300 timax|timin|tiboth config_filename\n");
	printf("Reset Temperature Outdoor Max|Min|Both: minmax2300 tomax|tomin|toboth config_filename\n");
	printf("Reset Dewpoint Max|Min|Both: minmax2300 dpmax|dpmin|dpboth config_filename\n");
	printf("Reset Windchill Max|Min|Both: minmax2300 wcmax|wcmin|wcboth config_filename\n");
	printf("Reset Wind Max|Min|Both: minmax2300 wmax|wmin|wboth config_filename\n");
	printf("Reset Humidity Indoor Max|Min|Both: minmax2300 himax|himin|hiboth config_filename\n");
	printf("Reset Humidity Outdoor Max|Min|Both: minmax2300 homax|homin|hoboth config_filename\n");
	printf("Reset Pressure Max|Min|Both: minmax2300 pmax|pmin|pboth config_filename\n");
	printf("Reset Rain Maximum 1h|24h: minmax2300 r1max|r24max config_filename\n");
	printf("Reset Rain Counter 1h|24h|Total: minmax2300 r1|r24|rtotal config_filename\n");
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

	if (strcmp(argv[1],"timax") == 0 || strcmp(argv[1],"dailymax") == 0)
	{
		temperature_indoor_reset(ws2300, RESET_MAX);
	}
	if (strcmp(argv[1],"timin") == 0 || strcmp(argv[1],"dailymin") == 0)
	{
		temperature_indoor_reset(ws2300, RESET_MIN);
	}
	if (strcmp(argv[1],"tiboth") == 0)
	{
		temperature_indoor_reset(ws2300, RESET_MIN + RESET_MAX);
	}
	if (strcmp(argv[1],"tomax") == 0 || strcmp(argv[1],"dailymax") == 0)
	{
		temperature_outdoor_reset(ws2300, RESET_MAX);
	}
	if (strcmp(argv[1],"tomin") == 0 || strcmp(argv[1],"dailymin") == 0)
	{
		temperature_outdoor_reset(ws2300, RESET_MIN);
	}
	if (strcmp(argv[1],"toboth") == 0)
	{
		temperature_outdoor_reset(ws2300, RESET_MIN + RESET_MAX);
	}
	if (strcmp(argv[1],"dpmax") == 0 || strcmp(argv[1],"dailymax") == 0)
	{
		dewpoint_reset(ws2300, RESET_MAX);
	}
	if (strcmp(argv[1],"dpmin") == 0 || strcmp(argv[1],"dailymin") == 0)
	{
		dewpoint_reset(ws2300, RESET_MIN);
	}
	if (strcmp(argv[1],"dpboth") == 0)
	{
		dewpoint_reset(ws2300, RESET_MIN + RESET_MAX);
	}
	if (strcmp(argv[1],"wcmax") == 0 || strcmp(argv[1],"dailymax") == 0)
	{
		windchill_reset(ws2300, RESET_MAX);
	}
	if (strcmp(argv[1],"wcmin") == 0 || strcmp(argv[1],"dailymin") == 0)
	{
		windchill_reset(ws2300, RESET_MIN);
	}
	if (strcmp(argv[1],"wcboth") == 0)
	{
		windchill_reset(ws2300, RESET_MIN + RESET_MAX);
	}
	if (strcmp(argv[1],"wmax") == 0)
	{
		wind_reset(ws2300, RESET_MAX);
	}
	if (strcmp(argv[1],"wmin") == 0)
	{
		wind_reset(ws2300, RESET_MIN);
	}
	if (strcmp(argv[1],"wboth") == 0)
	{
		wind_reset(ws2300, RESET_MIN + RESET_MAX);
	}
	if (strcmp(argv[1],"himax") == 0 || strcmp(argv[1],"dailymax") == 0)
	{
		humidity_indoor_reset(ws2300, RESET_MAX);
	}
	if (strcmp(argv[1],"himin") == 0 || strcmp(argv[1],"dailymin") == 0)
	{
		humidity_indoor_reset(ws2300, RESET_MIN);
	}
	if (strcmp(argv[1],"hiboth") == 0)
	{
		humidity_indoor_reset(ws2300, RESET_MIN + RESET_MAX);
	}
	if (strcmp(argv[1],"homax") == 0 || strcmp(argv[1],"dailymax") == 0)
	{
		humidity_outdoor_reset(ws2300, RESET_MAX);
	}
	if (strcmp(argv[1],"homin") == 0 || strcmp(argv[1],"dailymin") == 0)
	{
		humidity_outdoor_reset(ws2300, RESET_MIN);
	}
	if (strcmp(argv[1],"hoboth") == 0)
	{
		humidity_outdoor_reset(ws2300, RESET_MIN + RESET_MAX);
	}
	if (strcmp(argv[1],"pmax") == 0)
	{
		pressure_reset(ws2300, RESET_MAX);
	}
	if (strcmp(argv[1],"pmin") == 0)
	{
		pressure_reset(ws2300, RESET_MIN);
	}
	if (strcmp(argv[1],"pboth") == 0)
	{
		pressure_reset(ws2300, RESET_MIN + RESET_MAX);
	}
	if (strcmp(argv[1],"r1max") == 0)
	{
		rain_1h_max_reset(ws2300);
	}
	if (strcmp(argv[1],"r24max") == 0)
	{
		rain_24h_max_reset(ws2300);
	}
	if (strcmp(argv[1],"r1") == 0)
	{
		rain_1h_reset(ws2300);
	}
	if (strcmp(argv[1],"r24") == 0)
	{
		rain_24h_reset(ws2300);
	}
	if (strcmp(argv[1],"rtotal") == 0)
	{
		rain_total_reset(ws2300);
	}

	close_weatherstation(ws2300);
	
	return (0);
}
