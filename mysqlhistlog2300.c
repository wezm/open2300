/*  open2300 - histlog2300.c
 *  
 *  Version 1.15 (open200 1.11) Lars Hinrichsen
 *  
 *  Control WS2300 weather station
 *  
 *  Copyright 2003-2005, Kenneth Lavrsen
 *  This program is published under the GNU General Public license
 * 
 *  History:
 *  Version 1.12 
 *  - Code refactored
 * 
 *  1,14 2006  July 19 (included in open2300 1.11)
 *  1.15 2007  July 19  EmilianoParasassi
 *             http://www.lavrsen.dk/twiki/bin/view/Open2300/MysqlPatch2 
 */
#include <mysql.h>
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
	printf("mysqlhistlog2300 - Log history data from WS-2300 to MySQL.\n");
	printf("Version %s (C)2007 Kenneth Lavrsen, Lars Hinrichsen.\n", VERSION);
	printf("This program is released under the GNU General Public License (GPL)\n\n");
	printf("Usage:\n");
	printf("mysqlhistlog2300 config_filename\n");
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
	MYSQL mysql, *mysql_connection;
	int mysql_state;
	char mysql_insert_stmt[512] = 
		"INSERT INTO weather(datetime, temp_in, temp_out, dewpoint, rel_hum_in, "
		"rel_hum_out, wind_speed, wind_angle, wind_direction,wind_chill, rain_total, rel_pressure)";
	char mysql_values_stmt[512], mysql_stmt[1024];

	int interval, countdown, no_records;
	struct config_type config;
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

	int i;

	
	// Get serial port from config file. Use first command line parameter

	get_configuration(&config, argv[1]);

	// Setup serial port

	ws2300 = open_weatherstation(config.serial_device_name);

	
	// Open MySQL Database and read timestamp of the last record written

	if(!mysql_init(&mysql))
	{
		fprintf(stderr, "Cannot initialize MySQL");
		exit(EXIT_FAILURE);
	}
	
	// connect to database
	mysql_connection = mysql_real_connect(&mysql, config.mysql_host, config.mysql_user,
	                                      config.mysql_passwd, config.mysql_database,
	                                      config.mysql_port, NULL, 0);

	if(mysql_connection == NULL)
	{
		fprintf(stderr, "%d: %s \n",
		mysql_errno(&mysql), mysql_error(&mysql));
		exit(EXIT_FAILURE);
	}

	// By default read all records
	// We set the date to 1 Jan 1990 0:00
	time_lastlog_tm.tm_year = 90;
	time_lastlog_tm.tm_mon = 0;
	time_lastlog_tm.tm_mday = 1;
	time_lastlog_tm.tm_hour = 0;
	time_lastlog_tm.tm_min = 0;
	time_lastlog_tm.tm_sec = 0;
	time_lastlog_tm.tm_isdst = -1;

	time_lastlog = mktime(&time_lastlog_tm);


	// Start reading the history from the WS2300
		
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

	// Run through the records read
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

		// Build the three first DB columns
		time_lastrecord_tm.tm_min += interval;
		mktime(&time_lastrecord_tm);                 //normalize time_lastlog_tm
		
		// If humidity is > 100 the record is skipped
		if(humidity_out < 100)
		{
			strftime(datestring,sizeof(datestring),"\"%Y-%m-%d %H:%M:%S\"",
			         &time_lastrecord_tm);
			// Line up all value in order of appearance in the database
			sprintf(mysql_values_stmt," VALUES(%s", datestring);
			sprintf(mysql_values_stmt,"%s,%.1f",mysql_values_stmt, temperature_in);
			sprintf(mysql_values_stmt,"%s,%.1f",mysql_values_stmt, temperature_out);
			sprintf(mysql_values_stmt,"%s,%.1f",mysql_values_stmt, dewpoint);
			sprintf(mysql_values_stmt,"%s,%d",mysql_values_stmt, humidity_in);
			sprintf(mysql_values_stmt,"%s,%d",mysql_values_stmt, humidity_out);
			sprintf(mysql_values_stmt,"%s,%.1f ",mysql_values_stmt, windspeed);
			sprintf(mysql_values_stmt,"%s,%.1f,\"%s\"",mysql_values_stmt,
			        winddir_degrees, directions[(int)(winddir_degrees/22.5)]);
			sprintf(mysql_values_stmt,"%s,%.1f",mysql_values_stmt, windchill);
			sprintf(mysql_values_stmt,"%s,%.2f",mysql_values_stmt, rain);
			sprintf(mysql_values_stmt,"%s,%.3f)",mysql_values_stmt, pressure + pressure_term);

			// Build SQL string
			sprintf(mysql_stmt,"%s %s",mysql_insert_stmt, mysql_values_stmt);

			// Push to database	
			mysql_state = mysql_query(mysql_connection, mysql_stmt);

			// Error no 1062 means record is already stored
			// We only report other errors
			if((mysql_state != 0) && (mysql_errno(&mysql) != 1062))
			{
				// Something went wrong
				// Just print error message and move ahead
				fprintf(stderr, "Could not insert row. %d: \%s \nStatement was : %s\n",
				        mysql_errno(&mysql), mysql_error(&mysql),mysql_stmt);
			}
		}
		else
		{
			strftime(datestring,sizeof(datestring),"%Y-%m-%d %H:%M:%S",
			         &time_lastrecord_tm);
			fprintf(stderr, "Humidity is %d. Dataset for %s skipped.\n",humidity_out, datestring);
		}

	
	}

	// Goodbye and Goodnight
	close_weatherstation(ws2300);
	mysql_close(&mysql);

	return(0);
}
