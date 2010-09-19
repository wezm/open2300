/*       sqlitelog2300.c
 *
 *       Open2300 1.11 - sqlitelog2300 1.0
 *
 *       Get current data from WS2300 weather station
 *       and add store it in an SQLite database
 *
 *       Copyright 2010, Wesley Moore
 *
 *       This program is published under the GNU General Public license
 *
 */

#include <sqlite3.h>
#include "rw2300.h"

/********************************************************************
 * print_usage prints a short user guide
 *
 * Input:   none
 * 
 * Output:  prints to stderr
 * 
 * Returns: exits program
 *
 ********************************************************************/
void print_usage(void)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "sqlitelog2300 - Log history data from WS-2300 to SQLite.\n");
	fprintf(stderr, "Version %s (C)2010 Wesley Moore.\n", VERSION);
	fprintf(stderr, "This program is released under the GNU General Public License (GPL)\n\n");
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "sqlitelog2300 sqlite_db_filename [config_filename]\n");
}

/********** MAIN PROGRAM ************************************************
 *
 * This program reads current weather data from a WS2300
 * and writes the data to a MySQL database.
 *
 * The open2300.conf config file must contain the following parameters
 *
 * Table structure for table `weather` is shown in mysql2300.sql file
 *
 * It takes one parameters. The config file name with path
 * If this parameter is omitted the program will look at the default paths
 * See the open2300.conf-dist file for info
 *
 ***********************************************************************/

#define QUERY_BUF_SIZE 4096
#define VALUE_BUF_SIZE 256

int main(int argc, char *argv[])
{
	sqlite3 *db;
	WEATHERSTATION ws2300;
	char *columns[] = {
		"datetime",
		"temperature_in",
		"temperature_out",
		"dewpoint",
		"rel_humidity_in",
		"rel_humidity_out",
		"wind_speed",
		"wind_angle",
		"wind_direction",
		"wind_chill",
		"rain_1h",
		"rain_24h",
		"rain_total",
		"rel_pressure",
		"tendency",
		"forecast",
		NULL
	};
	const char *directions[]= {"N","NNE","NE","ENE","E","ESE","SE","SSE",
	                           "S","SSW","SW","WSW","W","WNW","NW","NNW"};
	double winddir[6];
	int tempint;
	char tendency[15];
	char forecast[15];
	struct config_type config;
	
	char query[QUERY_BUF_SIZE + 1] = "";
	sqlite3_stmt *statement;

	char *value[VALUE_BUF_SIZE];
	int rc;
	int i;

	/* Read the configuration */
	if(argc >= 3) {
		get_configuration(&config, argv[2]);
	}
	else if(argc == 2) {
		get_configuration(&config, NULL);
	}
	else {
		print_usage();
		exit(2);
	}

	/* Connect to the weather station */
	ws2300 = open_weatherstation(config.serial_device_name);

	/* Connect to the database */
	rc = sqlite3_open(argv[1], &db);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "Unable to open database (%s): %s\n", argv[1], sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}
  
	/* Ensure there's always a NUL char at the end of the buffer.
	   strncat won't override this char as its beyond QUERY_BUF_SIZE */
	query[QUERY_BUF_SIZE] = '\0';

	/* Build the query */
	strncat(query, "INSERT INTO weather (", QUERY_BUF_SIZE);
	for(i = 0; columns[i] != NULL; i++) {
		strncat(query, columns[i], QUERY_BUF_SIZE);
		if(columns[i + 1] != NULL) strncat(query, ", ", QUERY_BUF_SIZE);
	}
	strncat(query, ") VALUES (", QUERY_BUF_SIZE);
	for(i = 0; columns[i] != NULL; i++) {
		strncat(query, "?", QUERY_BUF_SIZE);
		if(columns[i + 1] != NULL) strncat(query, ", ", QUERY_BUF_SIZE);
	}
	strncat(query, ")", QUERY_BUF_SIZE);

	/* Prepare the query */
	int nByte = -1;
	rc = sqlite3_prepare_v2(db, query, nByte, &statement, NULL);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare query (%s): %s\n", query, sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(EXIT_FAILURE);
	}

#if 0
	/* READ TEMPERATURE INDOOR */
	temperature_indoor(ws2300, config.temperature_conv)
	//sprintf(logline,"\'%.1f\',", temperature_indoor(ws2300, config.temperature_conv) );

	/* READ TEMPERATURE OUTDOOR */
	temperature_outdoor(ws2300, config.temperature_conv);
	// sprintf(tempstring,"\'%.1f\',",  );
	// strcat(logline, tempstring);

	/* READ DEWPOINT */
	sprintf(tempstring,"\'%.1f\',", dewpoint(ws2300, config.temperature_conv) );
	strcat(logline, tempstring);

	/* READ RELATIVE HUMIDITY INDOOR */
	sprintf(tempstring,"\'%d\',", humidity_indoor(ws2300) );
	strcat(logline, tempstring);

	/* READ RELATIVE HUMIDITY OUTDOOR */
	sprintf(tempstring,"\'%d\',", humidity_outdoor(ws2300) );
	strcat(logline, tempstring);

	/* READ WIND SPEED AND DIRECTION aND WINDCHILL */
	sprintf(tempstring,"\'%.1f\',",
	        wind_all(ws2300, config.wind_speed_conv_factor, &tempint, winddir) );
	strcat(logline, tempstring);
	sprintf(tempstring,"\'%.1f\',\'%s\',", winddir[0], directions[tempint]);
	strcat(logline, tempstring);

	/* READ WINDCHILL */
	sprintf(tempstring,"\'%.1f\',", windchill(ws2300, config.temperature_conv) );
	strcat(logline, tempstring);

	/* READ RAIN 1H */
	sprintf(tempstring,"\'%.1f\',", rain_1h(ws2300, config.rain_conv_factor) );
	strcat(logline, tempstring);

	/* READ RAIN 24H */
	sprintf(tempstring,"\'%.1f\',", rain_24h(ws2300, config.rain_conv_factor) );
	strcat(logline, tempstring);

	/* READ RAIN TOTAL */
	sprintf(tempstring,"\'%.1f\',", rain_total(ws2300, config.rain_conv_factor) );
	strcat(logline, tempstring);

	/* READ RELATIVE PRESSURE */
	sprintf(tempstring,"\'%.1f\',", rel_pressure(ws2300, config.pressure_conv_factor) );
	strcat(logline, tempstring);

	/* READ TENDENCY AND FORECAST */
	tendency_forecast(ws2300, tendency, forecast);
	sprintf(tempstring,"\'%s\',\'%s\'", tendency, forecast);
	strcat(logline, tempstring);

	/* CLOSE THE WEATHER STATION TO ENABLE OTHER PROGRAMS TO ACCESS */
	close_weatherstation(ws2300);

	/* INIT MYSQL AND CONNECT */
	if (!mysql_init(&mysql))
	{
		fprintf(stderr, "Cannot initialize MySQL");
		exit(EXIT_FAILURE);
	}

	if(!mysql_real_connect(&mysql, config.mysql_host, config.mysql_user,
	                       config.mysql_passwd, config.mysql_database,
	                       config.mysql_port, NULL, 0))
	{
		fprintf(stderr, "%d: %s \n",
		mysql_errno(&mysql), mysql_error(&mysql));
		exit(EXIT_FAILURE);
	}

	sprintf(query, "INSERT INTO weather VALUES ( NOW(), %s )", logline);

	if (mysql_query(&mysql, query))
	{
		fprintf(stderr, "Could not insert row. %s %d: \%s \n",
		        query, mysql_errno(&mysql), mysql_error(&mysql));
		mysql_close(&mysql);
		exit(EXIT_FAILURE);
	}

#endif

	sqlite3_finalize(statement);
	sqlite3_close(db);

	return(EXIT_SUCCESS);
}
