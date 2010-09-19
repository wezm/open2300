/*  openconfig2300 - dumpconfig2300.c
 *  
 *  Version 1.10
 *  
 *  Control WS2300 weather station
 *  
 *  Copyright 2010, Wesley Moore
 *  This program is published under the GNU General Public license
 */

#include <stdlib.h>
#include <stdio.h>

#include "rw2300.h"

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
	struct config_type config;
	char *config_path = NULL;
	int i;
	if(argc >= 2) config_path = argv[1];

	// Get serial port from config file.
	get_configuration(&config, config_path);

	// struct config_type
	// {
	// 	char   serial_device_name[50];
	// 	char   citizen_weather_id[30];
	// 	char   citizen_weather_latitude[20];
	// 	char   citizen_weather_longitude[20];
	// 	hostdata aprs_host[MAX_APRS_HOSTS]; // max 6 possible aprs hosts 1 primary and 5 alternate
	// 	int    num_hosts;					// total defined hosts
	// 	char   weather_underground_id[30];
	// 	char   weather_underground_password[50];
	// 	char   timezone[6];                //not integer because of half hour time zones
	// 	double wind_speed_conv_factor;     //from m/s to km/h or miles/hour
	// 	int    temperature_conv;           //0=Celcius, 1=Fahrenheit
	// 	double rain_conv_factor;           //from mm to inch
	// 	double pressure_conv_factor;       //from hPa (=millibar) to mmHg
	// 	char   mysql_host[50];             //Either localhost, IP address or hostname
	// 	char   mysql_user[25];
	// 	char   mysql_passwd[25];
	// 	char   mysql_database[30];
	// 	int    mysql_port;                 //0 works for local connection
	// 	char   pgsql_connect[128];
	// 	char   pgsql_table[25];
	// 	char   pgsql_station[25];
	// };

	printf("serial_device_name\t%s\n",           config.serial_device_name);
	printf("citizen_weather_id\t%s\n",           config.citizen_weather_id);
	printf("citizen_weather_latitude\t%s\n",     config.citizen_weather_latitude);
	printf("citizen_weather_longitude\t%s\n",    config.citizen_weather_longitude);
	for(i = 0; i < config.num_hosts; i++)
	{
		printf("aprs_host %d\t%s:%d\n", i, config.aprs_host[i].name, config.aprs_host[i].port);
	}
	printf("weather_underground_id\t%s\n",       config.weather_underground_id);
	printf("weather_underground_password\t%s\n", config.weather_underground_password);
	printf("timezone\t%s\n",                     config.timezone);
	printf("wind_speed_conv_factor\t%lf\n",      config.wind_speed_conv_factor);
	printf("temperature_conv\t%d\n",             config.temperature_conv);
	printf("rain_conv_factor\t%lf\n",            config.rain_conv_factor);
	printf("pressure_conv_factor\t%lf\n",        config.pressure_conv_factor);
	printf("mysql_host\t%s\n",                   config.mysql_host);
	printf("mysql_user\t%s\n",                   config.mysql_user);
	printf("mysql_passwd\t%s\n",                 config.mysql_passwd);
	printf("mysql_database\t%s\n",               config.mysql_database);
	printf("mysql_port\t%d\n",                   config.mysql_port);
	printf("pgsql_connect\t%s\n",                config.pgsql_connect);
	printf("pgsql_table\t%s\n",                  config.pgsql_table);
	printf("pgsql_station\t%s\n",                config.pgsql_station);

	return(EXIT_SUCCESS);
}

