/*	pgsql2300.c
 *
 *	Version 1.1 - with open2300 v 1.10
 *
 *	Get data from WS2300 weather station
 *	and add them to PostgreSQL database
 *
 *	Copyright 2004-2005, Kenneth Lavrsen/Thomas Grieder/Przemyslaw Sztoch
 *
 *	This program is published under the GNU General Public license
 *
 *	1.1  2004 Nov 25  Przemyslaw Sztoch
 *	Creates pgsql2300. A Rewrite of mysql2300.
 */

#include <libpq-fe.h>
#include "rw2300.h"

 
/********** MAIN PROGRAM ************************************************
 *
 * This program reads current weather data from a WS2300
 * and writes the data to a PgSQL database.
 *
 * The open2300.conf config file must contain the following parameters
 * 
 * It takes one parameters. The config file name with path
 * If this parameter is omitted the program will look at the default paths
 * See the open2300.conf-dist file for info
 *
 ***********************************************************************/
int main(int argc, char *argv[])
{
	WEATHERSTATION ws2300;
	PGconn *conn;
	PGresult *res;
	int retval = 1;
	unsigned char logline[200] = "";
	const char *directions[]= {"N","NNE","NE","ENE","E","ESE","SE","SSE",
	                           "S","SSW","SW","WSW","W","WNW","NW","NNW"};
	double winddir[6];
	int tempint;
	char tendency[15];
	char forecast[15];
	struct config_type config;
	char query[4096];

	get_configuration(&config, argv[1]);

	ws2300 = open_weatherstation(config.serial_device_name);

	/* READ TEMPERATURE INDOOR */

	sprintf(logline,"%s\'%.1f\',", logline,
	        temperature_indoor(ws2300, config.temperature_conv) );
	

	/* READ TEMPERATURE OUTDOOR */

	sprintf(logline,"%s\'%.1f\',", logline,
	        temperature_outdoor(ws2300, config.temperature_conv) );
	

	/* READ DEWPOINT */

	sprintf(logline,"%s\'%.1f\',", logline,
	        dewpoint(ws2300, config.temperature_conv) );
	

	/* READ RELATIVE HUMIDITY INDOOR */

	sprintf(logline,"%s\'%d\',", logline, humidity_indoor(ws2300) );
	
	
	/* READ RELATIVE HUMIDITY OUTDOOR */

	sprintf(logline,"%s\'%d\',", logline, humidity_outdoor(ws2300) );


	/* READ WIND SPEED AND DIRECTION aND WINDCHILL */

	sprintf(logline,"%s\'%.1f\',", logline,
	        wind_all(ws2300, config.wind_speed_conv_factor,
	                 &tempint, winddir) );

	sprintf(logline,"%s\'%.1f\',\'%s\',", logline,
	        winddir[0], directions[tempint]);
	

	/* READ WINDCHILL */

	sprintf(logline,"%s\'%.1f\',", logline,
	        windchill(ws2300, config.temperature_conv) );

	
	/* READ RAIN 1H */

	sprintf(logline,"%s\'%.1f\',", logline,
	        rain_1h(ws2300, config.rain_conv_factor) );
	
	
	/* READ RAIN 24H */

	sprintf(logline,"%s\'%.1f\',", logline,
	        rain_24h(ws2300, config.rain_conv_factor) );

	
	/* READ RAIN TOTAL */

	sprintf(logline,"%s\'%.1f\',", logline,
	        rain_total(ws2300, config.rain_conv_factor) );

	
	/* READ RELATIVE PRESSURE */

	sprintf(logline,"%s\'%.1f\',", logline,	
	        rel_pressure(ws2300, config.pressure_conv_factor) );


	/* READ TENDENCY AND FORECAST */
	
	tendency_forecast(ws2300, tendency, forecast);
	sprintf(logline,"%s\'%s\',\'%s\'", logline, tendency, forecast);


	/* CLOSE THE WEATHER STATION TO ENABLE OTHER PROGRAMS TO ACCESS */
	close_weatherstation(ws2300);

	sprintf(query, "INSERT INTO %s VALUES ('%s', current_timestamp, %s)", config.pgsql_table, config.pgsql_station, logline);

	//printf("%s\n",query);  //disabled to be used in cron job
	//printf("%s\n",config.pgsql_connect); //debug

	/* INIT PQ AND EXECUTE QUERY */
	conn = PQconnectdb(config.pgsql_connect);

	if (PQstatus(conn) == CONNECTION_BAD)
	{
		fprintf(stderr, "Connection to PgSQL failed:\n%s\n", config.pgsql_connect);
		fprintf(stderr, "%s", PQerrorMessage(conn));
	}
	else
	{
		res = PQexec(conn, query);
	
		if (PQresultStatus(res) == PGRES_COMMAND_OK)
			retval = 0;
		else
			fprintf(stderr, "Could not insert row. %s:\n%s\n", PQresultErrorMessage(res), query);
				
		PQclear(res);
	}
		
	PQfinish(conn);

	return retval;
}
