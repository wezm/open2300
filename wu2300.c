/*  open2300 - wu2300.c
 *  
 *  Version 1.11
 *  
 *  Control WS2300 weather station
 *  
 *  Copyright 2004-2006, Kenneth Lavrsen
 *  This program is published under the GNU General Public license
 */

#define DEBUG 0  // wu2300 stops writing to standard out if setting this to 0
#define GUST  1  // report wind gust information (resets wind min/max)

#include "rw2300.h"

/********** MAIN PROGRAM ************************************************
 *
 * This program reads all current weather data from a WS2300
 * and sends it to Weather Underground.
 *
 * It takes one parameter which is the config file name with path
 * If this parameter is omitted the program will look at the default paths
 * See the open2300.conf-dist file for info
 *
 ***********************************************************************/
int main(int argc, char *argv[])
{
	WEATHERSTATION ws2300;
	struct config_type config;
	char urlline[3000] = "";
	char tempstring[1000] = "";
	char datestring[50];        //used to hold the date stamp for the log file
	double tempfloat;
	time_t basictime;

	get_configuration(&config, argv[1]);

	ws2300 = open_weatherstation(config.serial_device_name);


	/* START WITH URL, ID AND PASSWORD */

	sprintf(urlline, "GET %s?ID=%s&PASSWORD=%s", WEATHER_UNDERGROUND_PATH,
	        config.weather_underground_id,config.weather_underground_password);

	/* GET DATE AND TIME FOR URL */
	
	time(&basictime);
	basictime = basictime - atof(config.timezone) * 60 * 60;
	strftime(datestring,sizeof(datestring),"&dateutc=%Y-%m-%d+%H%%3A%M%%3A%S",
	         localtime(&basictime));
	strcat(urlline, datestring);


	/* READ TEMPERATURE OUTDOOR - deg F for Weather Underground */

	sprintf(tempstring, "&tempf=%.2f", temperature_outdoor(ws2300, FAHRENHEIT) );
	strcat(urlline, tempstring);


	/* READ DEWPOINT - deg F for Weather Underground*/
	
	sprintf(tempstring, "&dewptf=%.2f", dewpoint(ws2300, FAHRENHEIT) );
	strcat(urlline, tempstring);


	/* READ RELATIVE HUMIDITY OUTDOOR */

	sprintf(tempstring, "&humidity=%d", humidity_outdoor(ws2300) );
	strcat(urlline, tempstring);


	/* READ WIND SPEED AND DIRECTION - miles/hour for Weather Underground */

	sprintf(tempstring, "&windspeedmph=%.2f", wind_current(ws2300, MILES_PER_HOUR, &tempfloat) );
	strcat(urlline, tempstring);
	sprintf(tempstring,"&winddir=%.1f", tempfloat);
	strcat(urlline, tempstring);


	/* READ WIND GUST - miles/hour for Weather Underground */

	if (GUST)
	{
		sprintf(tempstring, "&windgustmph=%.2f",
		        wind_minmax(ws2300, MILES_PER_HOUR, NULL, NULL, NULL, NULL));
		strcat(urlline, tempstring);
	}


	/* READ RAIN 1H - inches for Weather Underground */
	
	sprintf(tempstring, "&rainin=%.2f", rain_1h(ws2300, INCHES) );
	strcat(urlline, tempstring);


	/* READ RAIN 24H - inches for Weather Underground */

	sprintf(tempstring, "&dailyrainin=%.2f", rain_24h(ws2300, INCHES) );
	strcat(urlline, tempstring);


	/* READ RELATIVE PRESSURE - Inches of Hg for Weather Underground */

	sprintf(tempstring, "&baromin=%.3f", rel_pressure(ws2300, INCHES_HG) );
	strcat(urlline, tempstring);


	/* ADD SOFTWARE TYPE AND ACTION */
	sprintf(tempstring, "&softwaretype=open2300-%s&action=updateraw", VERSION);
	strcat(urlline, tempstring);
	
	sprintf(tempstring, " HTTP/1.0\r\nUser-Agent: open2300/%s\r\nAccept: */*\r\n"
	                   "Host: %s\r\nConnection: Keep-Alive\r\n\r\n",
	        VERSION, WEATHER_UNDERGROUND_BASEURL);
	strcat(urlline, tempstring);


	/* Reset minimum and maximum wind readings if reporting gusts */
	if (GUST)
	{
		wind_reset(ws2300, RESET_MIN + RESET_MAX);
	}


	/* SEND DATA TO WEATHER UNDERGROUND AS HTTP REQUEST */
	/* or print the URL if DEBUG is enabled in the top of this file */

	close_weatherstation(ws2300);

	if (DEBUG)
	{
		printf("%s\n",urlline);
	}
	else
	{
		http_request_url(urlline);
	}
	
	return(0);
}

