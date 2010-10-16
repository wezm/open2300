/*		 sqlitelog2300.c
 *
 *		 Open2300 1.11 - sqlitelog2300 1.0
 *
 *		 Get current data from WS2300 weather station
 *		 and add store it in an SQLite database
 *
 *		 Copyright 2010, Wesley Moore
 *
 *		 This program is published under the GNU General Public license
 *
 */

#include <sqlite3.h>
#include "rw2300.h"

/********************************************************************
 * print_usage prints a short user guide
 *
 * Input:	none
 *
 * Output:	prints to stderr
 *
 * Returns: void
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

/********************************************************************
 * The state struct stores the state of this program. It exists to
 * allow clean shutdown in the event an error ocurrs.
 *
 ********************************************************************/
struct state {
	sqlite3 *db;
	WEATHERSTATION station;
	sqlite3_stmt *statement;
};

/* QUERY_BUF_SIZE specifies the size in bytes for the SQL INSERT statement
 * that is built */
#define QUERY_BUF_SIZE 4096


/********************************************************************
 * state_init initializes a state struct
 *
 * Input:	Pointer to state structure to initialize
 *			An open2300 configuration
 *			The path to the SQLite database
 *			A NULL terminated array of column names in the weather
 *			table.
 *
 * Output:	Initialized state structure
 *
 * Returns: void
 *
 ********************************************************************/
void state_init(struct state* state, struct config_type *config, const char *db_path, char * const *column_names)
{
	int i, rc;
	char query[QUERY_BUF_SIZE + 1] = ""; /* +1 for trailing NUL */

	/* Connect to the weather station */
	state->station = open_weatherstation(config->serial_device_name);

	/* Connect to the database */
	rc = sqlite3_open(db_path, &state->db);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "Unable to open database (%s): %s\n", db_path, sqlite3_errmsg(state->db));
		sqlite3_close(state->db);
		exit(EXIT_FAILURE);
	}

	/* Ensure there's always a NUL char at the end of the buffer.
	   strncat won't override this char as its beyond QUERY_BUF_SIZE */
	query[QUERY_BUF_SIZE] = '\0';

	/* Build the query */
	strncat(query, "INSERT INTO weather (", QUERY_BUF_SIZE);
	for(i = 0; column_names[i] != NULL; i++) {
		strncat(query, column_names[i], QUERY_BUF_SIZE);
		if(column_names[i + 1] != NULL) strncat(query, ", ", QUERY_BUF_SIZE);
	}
	strncat(query, ") VALUES (", QUERY_BUF_SIZE);
	for(i = 0; column_names[i] != NULL; i++) {
		if(strcmp(column_names[i], "datetime") == 0)
		{
			strncat(query, "datetime('now')", QUERY_BUF_SIZE);
		}
		else
		{
			strncat(query, ":", QUERY_BUF_SIZE);
			strncat(query, column_names[i], QUERY_BUF_SIZE);
		}
		if(column_names[i + 1] != NULL) strncat(query, ", ", QUERY_BUF_SIZE);
	}
	strncat(query, ")", QUERY_BUF_SIZE);

	/* Prepare the query */
	int nByte = -1;
	rc = sqlite3_prepare_v2(state->db, query, nByte, &state->statement, NULL);
	if(rc != SQLITE_OK) {
		fprintf(stderr, "Unable to prepare query (%s): %s\n", query, sqlite3_errmsg(state->db));
		sqlite3_close(state->db);
		exit(EXIT_FAILURE);
	}
}

/********************************************************************
 * state_finish finalizes a state struct. The struct is not usable
 * after this function has been called on it. This function should
 * be called to free resources when the state is no longer needed.
 *
 * Input:	Pointer to state structure to finalize
 *
 * Output:	Finalized state structure
 *
 * Returns: void
 *
 ********************************************************************/
void state_finish(struct state* state)
{
	sqlite3_finalize(state->statement);
	sqlite3_close(state->db);
	close_weatherstation(state->station);
}

/********************************************************************
 * check_rc ensures the passed return code (rc) from SQLite
 * indicates success. If not it prints the current SQLite
 * error message, finalizes the state and exits the program.
 *
 * Input:	Pointer to state structure to finalize
 *			Return code from SQLite function
 *
 * Output:	Program exit if the return code indicates an error
 *
 * Returns: void
 *
 ********************************************************************/
void check_rc(struct state* state, int rc)
{
	if(rc != SQLITE_OK) {
		fprintf(stderr, "Unable to bind value: %s\n", sqlite3_errmsg(state->db));
		state_finish(state);
		exit(EXIT_FAILURE);
	}
}

/********** MAIN PROGRAM ************************************************
 *
 * This program reads current weather data from a WS2300
 * and writes the data to a SQLite database.
 *
 * The open2300.conf config file must contain the following parameters
 *
 * Schema for table `weather` is shown in sqlitelog2300.sql file
 *
 * It takes two parameters. The path to an SQLite database that has had
 * schema file loaded and an optional path to the open2300 config file.
 * If this parameter is omitted the program will look at the default paths
 * See the open2300.conf-dist file for info
 *
 ***********************************************************************/

int main(int argc, char *argv[])
{
	double winddir[6];
	int winddir_index;
	char tendency[15];
	char forecast[15];
	struct config_type config;

	char *columns[] = {
		"datetime",
		"dewpoint",
		"forecast",
		"rain_1h",
		"rain_24h",
		"rain_total",
		"rel_humidity_in",
		"rel_humidity_out",
		"rel_pressure",
		"temperature_in",
		"temperature_out",
		"tendency",
		"wind_angle",
		"wind_chill",
		"wind_direction",
		"wind_speed",
		NULL
	};
	const char *directions[]= {"N","NNE","NE","ENE","E","ESE","SE","SSE",
							   "S","SSW","SW","WSW","W","WNW","NW","NNW"};
	int rc;

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

	struct state s;
	state_init(&s, &config, argv[1], columns);

	/* Read the weather station values and bind them to the prepared statement */

	/* INDOOR TEMPERATURE */
	/* TODO: add contraints to values and error out if invalid E.g. temp over 60 */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":temperature_in"), temperature_indoor(s.station, config.temperature_conv));
	check_rc(&s, rc);

	/* OUTDOOR TEMPERATURE */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":temperature_out"), temperature_outdoor(s.station, config.temperature_conv));
	check_rc(&s, rc);

	/* READ DEWPOINT */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":dewpoint"), dewpoint(s.station, config.temperature_conv));
	check_rc(&s, rc);

	/* READ RELATIVE HUMIDITY INDOOR */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":rel_humidity_in"), humidity_indoor(s.station));
	check_rc(&s, rc);

	/* READ RELATIVE HUMIDITY OUTDOOR */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":rel_humidity_out"), humidity_outdoor(s.station));
	check_rc(&s, rc);

	/* READ WIND SPEED AND DIRECTION */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":wind_speed"), wind_all(s.station, config.wind_speed_conv_factor, &winddir_index, winddir));
	check_rc(&s, rc);

	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":wind_angle"), winddir[0]);
	check_rc(&s, rc);

	rc = sqlite3_bind_text(s.statement, sqlite3_bind_parameter_index(s.statement, ":wind_direction"), directions[winddir_index], -1, SQLITE_STATIC);
	check_rc(&s, rc);

	/* READ WINDCHILL */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":wind_chill"), windchill(s.station, config.temperature_conv));
	check_rc(&s, rc);

	/* READ RAIN 1H */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":rain_1h"), rain_1h(s.station, config.rain_conv_factor));
	check_rc(&s, rc);

	/* READ RAIN 24H */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":rain_24h"), rain_24h(s.station, config.rain_conv_factor));
	check_rc(&s, rc);

	/* READ RAIN TOTAL */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":rain_total"), rain_total(s.station, config.rain_conv_factor));
	check_rc(&s, rc);

	/* READ RELATIVE PRESSURE */
	rc = sqlite3_bind_double(s.statement, sqlite3_bind_parameter_index(s.statement, ":rel_pressure"), rel_pressure(s.station, config.pressure_conv_factor));
	check_rc(&s, rc);

	/* READ TENDENCY AND FORECAST */
	tendency_forecast(s.station, tendency, forecast);
	rc = sqlite3_bind_text(s.statement, sqlite3_bind_parameter_index(s.statement, ":tendency"), tendency, -1, SQLITE_STATIC);
	check_rc(&s, rc);
	rc = sqlite3_bind_text(s.statement, sqlite3_bind_parameter_index(s.statement, ":forecast"), forecast, -1, SQLITE_STATIC);
	check_rc(&s, rc);

	/* Run the query */
	rc = sqlite3_step(s.statement);
	if(rc != SQLITE_DONE)
	{
		fprintf(stderr, "Error executing query: %s\n", sqlite3_errmsg(s.db));
		state_finish(&s);
		exit(EXIT_FAILURE);
	}

	state_finish(&s);
	return(EXIT_SUCCESS);
}
