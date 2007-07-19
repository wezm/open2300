/*       mysql2300.c
 *
 *       Open2300 1.11 - mysql2300 1.6
 *
 *       Get data from WS2300 weather station
 *       and add them to MySQL database
 *
 *       Copyright 2003,2004, Kenneth Lavrsen/Thomas Grieder
 *
 *       This program is published under the GNU General Public license
 *
 *  0.1  2004 Feb 21  Thomas Grieder
 *       Creates mysql2300. A Rewrite of log2300.
 *       Logline is now comma delimited for added support to write
 *       to MySQL
 *       (see also http://www.unixreview.com/documents/s=8989/ur0401a/)
 *
 *  1.2  2004 Mar 07  Kenneth Lavrsen
 *       Complete rewrite of the code to support the new rw2300 library.
 *       Logfile feature removed to make it a clean MySQL program.
 *       Added support for config file and program should be able to
 *       compile under Windows with the right MySQL client headers
 *       and libraries installed.
 *
 *  1.3  As 1.2
 *
 *  1.4  2006 Feb 24  Emiliano Parasassi
 *       Enhancement of mysql2300 program. It removes 2 redundant fields
 *       and let you to perform custom query more easily.
 *
 *  1.5  2006 Jul 19  Kenneth Lavrsen
 *       Replaced a risky use of sprintf with safer strcat
 *
 *  1.6  2007 Jul 19  Emiliano Parasassi
 *       http://www.lavrsen.dk/twiki/bin/view/Open2300/MysqlPatch2
 *       Plus updates in ALTER TABLE to match patch from Rolan Yang
 */

#include <mysql.h>
#include "rw2300.h"

 
/********** MAIN PROGRAM ************************************************
 *
 * This program reads current weather data from a WS2300
 * and writes the data to a MySQL database.
 *
 * The open2300.conf config file must contain the following parameters
 * 
 * Table structure for table `weather` is shown in mysql2300.sql file
 * 
 * If you already have the 'weather' table of the previous
 * release(<=1.3), launch these SQL commands to modify it correctly:

ALTER TABLE `open2300`.`weather` MODIFY COLUMN `timestamp` DATETIME DEFAULT '0000-00-00 00:00:00' NOT NULL;
ALTER TABLE `open2300`.`weather` DROP COLUMN `rec_date`;
ALTER TABLE `open2300`.`weather` DROP COLUMN `rec_time`;
ALTER TABLE `open2300`.`weather` CHANGE COLUMN `timestamp`    `datetime`     DATETIME NOT NULL DEFAULT 0;
ALTER TABLE `open2300`.`weather` CHANGE COLUMN `windspeed`    `wind_speed`   DECIMAL(3,1) NOT NULL DEFAULT 0;
ALTER TABLE `open2300`.`weather` CHANGE COLUMN `rel_pressure` `rel_pressure` DECIMAL(5,1) NOT NULL;
ALTER TABLE `open2300`.`weather` CHANGE COLUMN `rain_total`   `rain_total`   DECIMAL(5,1) NOT NULL;
ALTER TABLE `open2300`.`weather` CHANGE COLUMN `wind_angle`   `wind_angle`   DECIMAL(4,1) NOT NULL;
ALTER TABLE `open2300`.`weather` CHANGE COLUMN `temp_in`      `temp_in`      DECIMAL(4,1) NOT NULL DEFAULT 0;
ALTER TABLE `open2300`.`weather` CHANGE COLUMN `temp_out`     `temp_out`     DECIMAL(4,1) NOT NULL DEFAULT 0;
ALTER TABLE `open2300`.`weather` CHANGE COLUMN `dewpoint`     `dewpoint`     DECIMAL(4,1) NOT NULL DEFAULT 0;
ALTER TABLE `open2300`.`weather` CHANGE COLUMN `wind_chill`   `wind_chill`   DECIMAL(4,1) NOT NULL DEFAULT 0;

 * It takes one parameters. The config file name with path
 * If this parameter is omitted the program will look at the default paths
 * See the open2300.conf-dist file for info
 *
 ***********************************************************************/
int main(int argc, char *argv[])
{
	WEATHERSTATION ws2300;
	MYSQL mysql;
	char logline[3000] = "";
	char tempstring[1000] = "";
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
	sprintf(logline,"\'%.1f\',", temperature_indoor(ws2300, config.temperature_conv) );
	
	/* READ TEMPERATURE OUTDOOR */
	sprintf(tempstring,"\'%.1f\',", temperature_outdoor(ws2300, config.temperature_conv) );
	strcat(logline, tempstring);
	
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

	mysql_close(&mysql);

	return(0);
}
