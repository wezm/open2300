/*  openconfig2300 - dumpconfig2300.c
 *
 *  Version 1.10
 *
 *  Dump the open2300 configuration
 *
 *  Copyright 2010, Wesley Moore
 *  This program is published under the GNU General Public license
 */

#include <stdlib.h>
#include <stdio.h>

#include "rw2300.h"

/********** MAIN PROGRAM ************************************************
 *
 * This program prints out the content of the open2300 configuration
 * file passed as the first argument or the first configuration file
 * found in the standard search paths.
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

