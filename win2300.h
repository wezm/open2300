/* Include file for the open2300 Windows specific functions
 */

#ifndef _INCLUDE_WIN2300_H_
#define _INCLUDE_WIN2300_H_ 

#include <windows.h>
#include <winsock.h>

#define STRINGIZE(x) #x

typedef HANDLE WEATHERSTATION;

#define BAUDRATE CBR_2400
#define DEFAULT_SERIAL_DEVICE "COM1"

#endif /* _INCLUDE_WIN2300_H_ */

