/*  open2300  - win2300 library functions
 *  This file contains the common functions that are unique to
 *  windows. The entire file is ignored in case of Linux
 *  
 *  Version 1.11
 *  
 *  Control WS2300 weather station
 *  
 *  Copyright 2003-2006, Kenneth Lavrsen
 *  This program is published under the GNU General Public license
 */

#ifdef WIN32
#define DEBUG 0

#include "rw2300.h"

/********************************************************************
 * open_weatherstation, Windows version
 *
 * Input:   devicename (COM1, COM2 etc)
 * 
 * Returns: Handle to the weatherstation (type WEATHERSTATION)
 *
 ********************************************************************/
WEATHERSTATION open_weatherstation (char *device)
{
	WEATHERSTATION ws;
	DCB dcb;
	COMMTIMEOUTS commtimeouts;

	//Setup serial port

	ws = CreateFile( device,
	               GENERIC_READ | GENERIC_WRITE,
	               0,    // must be opened with exclusive-access
	               NULL, // no security attributes
	               OPEN_EXISTING, // must use OPEN_EXISTING
	               0,    // not overlapped I/O
	               NULL  // hTemplate must be NULL for comm devices
	               );
	                     
	if (ws == INVALID_HANDLE_VALUE)
	{
		printf ("\nUnable to open serial device");
		exit (0);
	}

	if (!GetCommState (ws, &dcb))
	{
		printf ("\nUnable to GetCommState");
		exit (0);
	}

	dcb.DCBlength = sizeof (DCB);
	dcb.BaudRate = BAUDRATE;
	dcb.fParity = FALSE;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fRtsControl = RTS_CONTROL_ENABLE;
	dcb.fAbortOnError = FALSE;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fNull = false;

	if (!SetCommState (ws, &dcb))
	{
		printf ("\nUnable to SetCommState");
		exit (0);
	}

	commtimeouts.ReadIntervalTimeout = MAXDWORD;
	commtimeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
	commtimeouts.ReadTotalTimeoutConstant = 175;
	commtimeouts.WriteTotalTimeoutConstant = 0;
	commtimeouts.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts (ws, &commtimeouts))
	{
		printf ("\nUnable to SetCommTimeouts");
		exit (0);
	}

	return ws;
}


/********************************************************************
 * close_weatherstation, windows version
 *
 * Input: Handle to the weatherstation (type WEATHERSTATION)
 *
 * Returns nothing
 *
 ********************************************************************/
void close_weatherstation (WEATHERSTATION ws)
{
	CloseHandle (ws);
	return;
}

/********************************************************************
 * reset_06 WS2300 by sending command 06 (windows version) 
 * 
 * Input:   device number of the already open serial port
 *           
 * Returns: nothing, exits progrsm if failing to reset
 *
 ********************************************************************/
void reset_06(WEATHERSTATION serdevice)
{
	unsigned char command = 0x06;
	unsigned char answer;
	int i;

	for (i = 0; i < 100; i++)
	{

		PurgeComm(serdevice, PURGE_RXCLEAR);

		write_device(serdevice, &command, 1);

		// Occasionally 0, then 2 is returned.  If zero comes back, continue
		// reading as this is more efficient than sending an out-of sync
		// reset and letting the data reads restore synchronization.
		// Occasionally, multiple 2's are returned.  Read with a fast timeout
		// until all data is exhausted, if we got a two back at all, we
		// consider it a success
		
		while (1 == read_device(serdevice, &answer, 1))
		{
			if (answer == 2)
			{

				// clear anything that might come after the response
				PurgeComm(serdevice, PURGE_RXCLEAR);

				return;
			}
		}

		Sleep(5 * i);
	}
	printf("\nCould not reset\n");
	exit(EXIT_FAILURE);
}

/********************************************************************
 * read_device WIN32 emulation of Linux read() 
 * Reads data from the handle
 *
 * Inputs:  serdevice - opened file handle
 *          buffer - pointer to the buffer to read into
 *          size - number of bytes to read
 *
 * Output:  *buffer - modified on success
 * 
 * Returns: number of bytes read
 *
 ********************************************************************/
int read_device(WEATHERSTATION serdevice, unsigned char *buffer, int size)
{
	DWORD dwRead = 0;

	if (!ReadFile(serdevice, buffer, size, &dwRead, NULL))
	{
		return -1;
	}

	return (int) dwRead;
}

/********************************************************************
 * write_device WIN32 emulation of Linux write() 
 * Writes data to the handle
 *
 * Inputs:  serdevice - opened file handle
 *          buffer - pointer to the buffer to write from
 *          size - number of bytes to write
 *
 * Returns: number of bytes written
 *
 ********************************************************************/
int write_device(WEATHERSTATION serdevice, unsigned char *buffer, int size)
{
	DWORD dwWritten;

	if (!WriteFile(serdevice, buffer, size, &dwWritten, NULL))
	{
		return -1;
	}

	return (int) dwWritten;
}

/********************************************************************
 * sleep_short - Windows version
 * 
 * Inputs: Time in milliseconds (integer)
 *
 * Returns: nothing
 *
 ********************************************************************/
void sleep_short(int milliseconds)
{
	Sleep(milliseconds);
}

/********************************************************************
 * sleep_long - Windows version
 * 
 * Inputs: Time in seconds (integer)
 *
 * Returns: nothing
 *
 ********************************************************************/
void sleep_long(int seconds)
{
	Sleep(seconds*1000);
}

/********************************************************************
 * http_request_url - Windows version
 * 
 * Inputs: urlline - URL to Weather Underground with path and data
 *                   as a pointer to char array (string)
 *
 * Returns: 0 on success and -1 if fail.
 *
 * Action: Send a http request to Weather Underground
 *
 ********************************************************************/
int http_request_url(char *urlline)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	SOCKET sockfd;
	struct hostent *hostinfo;
	struct sockaddr_in urladdress;
	char buffer[1024];
	int bytes_read;
	
	wVersionRequested = MAKEWORD( 1, 1 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		perror("Couldn't find a useable winsock.dll");
	    return(-1);
	}
	
	/* Confirm that the Windows Sockets DLL supports 1.1.*/
	
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
	         HIBYTE( wsaData.wVersion ) != 1 )
	{
	    WSACleanup();
		perror("Couldn't find a useable winsock.dll");
	    return(-1);   
	}

	if ( (hostinfo = gethostbyname(WEATHER_UNDERGROUND_BASEURL)) == NULL )
	{
		perror("Host not known by DNS server or DNS server not working");
		return(-1);
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd == INVALID_SOCKET)
	{
		perror("Cannot open socket");
		return(-1);
	}

	memset(&urladdress, 0, sizeof(urladdress));
	urladdress.sin_family = AF_INET;
	urladdress.sin_port = htons(80); /*default HTTP Server port */

	urladdress.sin_addr = *(struct in_addr *)*hostinfo->h_addr_list;

	if (connect(sockfd,(struct sockaddr*)&urladdress,sizeof(urladdress)) != 0)
	{
		perror("Cannot connect to host");
		return(-1);
	}
	
	sprintf(buffer, "%s", urlline);
	send(sockfd, buffer, strlen(buffer), 0);

	/* While there's data, read and print it */
	do
	{
		memset(buffer, 0, sizeof(buffer));
		bytes_read = recv(sockfd, buffer, sizeof(buffer), 0);
		if ( bytes_read > 0 )
			if (DEBUG) printf("%s", buffer);
	}
	while ( bytes_read > 0 );

	/* Close socket and clean up winsock */
	closesocket(sockfd);
	
	WSACleanup();
	
	return(0);
}

/********************************************************************
 * citizen_weather_send - Linux version
 * 
 * Inputs: config structure (pointer to) - containing CW ID
 *         datastring (pointer to) - containing all the data
 *
 * Returns: 0 on success and -1 if fail.
 *
 * Action: Send a http request to Weather Underground
 *
 ********************************************************************/
int citizen_weather_send(struct config_type *config, char *aprsline)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	SOCKET sockfd = INVALID_SOCKET; // Prevent warning
	struct hostent *hostinfo;
	struct sockaddr_in urladdress;
	char buffer[1024];
	int bytes_read;
	int hostnum;
	
	wVersionRequested = MAKEWORD( 1, 1 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		perror("Couldn't find a useable winsock.dll");
	    return(-1);
	}
	
	/* Confirm that the Windows Sockets DLL supports 1.1.*/
	
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
	         HIBYTE( wsaData.wVersion ) != 1 )
	{
	    WSACleanup();
		perror("Couldn't find a useable winsock.dll");
	    return(-1);   
	}


	for (hostnum = 0; hostnum <= config->num_hosts; hostnum++)
	{
		if ( hostnum == config->num_hosts )
			return(-1);          // tried 'em all, fail exit		

		if ( (hostinfo = gethostbyname(config->aprs_host[hostnum].name) ) == NULL )		
		{
			sprintf(buffer,"Host, %s, not known ", config->aprs_host[hostnum].name);
			perror(buffer);
			continue;
		}
				
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
		if (sockfd == INVALID_SOCKET)
		{
			sprintf(buffer,"Cannot open socket on %s ", config->aprs_host[hostnum].name);
			perror(buffer);
			continue;
		}
		
		memset(&urladdress, 0, sizeof(urladdress));// clear the structure
		urladdress.sin_family = AF_INET;
		urladdress.sin_port = htons(config->aprs_host[hostnum].port);
		urladdress.sin_addr = *(struct in_addr *)*hostinfo->h_addr_list;

		if ( connect(sockfd, (struct sockaddr*)&urladdress, sizeof(urladdress)) != 0 )
		{
			sprintf(buffer,"Cannot connect to host: %s ", config->aprs_host[hostnum].name);
			perror(buffer);
			continue;
		}
		else
		{
			break;   // success
		}
	}

	if (DEBUG) printf("%d: %s: ",hostnum, config->aprs_host[hostnum].name);

	memset(buffer, 0, sizeof(buffer));

	// read login prompt
	if ( (recv(sockfd, buffer, sizeof(buffer), 0) > 0) && (DEBUG != 0) )
	{
		printf("%s", buffer);	// display prompt - if debug
	}

	// The login/header line
	sprintf(buffer,"user %s pass -1 vers open2300 %s\n",
	        config->citizen_weather_id, VERSION);
	send(sockfd, buffer, strlen(buffer), 0);
	if (DEBUG)
		printf("%s\n", buffer);

	// now the data
	sprintf(buffer,"%s\n", aprsline);
	send(sockfd, buffer, strlen(buffer), 0);
	if (DEBUG)
		printf("%s\n", buffer);

	/* While there's data, read and print it - Not sure it is needed */
	do
	{
		memset(buffer, 0, sizeof(buffer));
		bytes_read = recv(sockfd, buffer, sizeof(buffer), 0);
		if ( bytes_read > 0 )
		{
			if (DEBUG)
				printf("Data returned from server\n%s\n", buffer);
			break;
		}
	}
	while ( bytes_read > 0 );

	/* Close socket*/
	close(sockfd);
	
	WSACleanup();

	return(0);
}

#endif
