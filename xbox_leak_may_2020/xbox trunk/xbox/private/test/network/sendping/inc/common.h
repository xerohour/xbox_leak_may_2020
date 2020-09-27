/*
 * File : Controller.cpp
 * Author : Sandeep Prabhu
 * Revision History
 *      28/4/2000       sandeep           Created
 *
 * Contains stuff shared between AutoSrv and Controller
 */
 
#ifndef __COMMON__H
#define __COMMON__H

 
/*********************************************************************************
 * Constants
 *********************************************************************************/ 
//
// Max size of a request/response packet
//
#define MAX_PACKET_LENGTH           65535
//
// Maximum length (in bytes) of IP address
//
#define MAX_IPADDR_LEN              12
//
// Maximum length (in bytes) of MAC addres
//
#define MAX_MACADDR_LEN             6
//
// Maximum lenght of friendly name for an interface
//
#define MAX_INTERFACE_NAME_LENGTH   256
//
// Maximum size of an option set on a socket
//
#define MAX_OPTION_SIZE             4
//
// Maximum size of the subkey
//
#define MAX_REG_SUBKEY_LENGTH       256
//
// Maximum size of value name
//
#define MAX_REG_VALUENAME_LENGTH    256
//
// Maximum size of value
//
#define MAX_REG_VALUE_LENGTH        256
//
// The port on which AutoSrv is listening
//
#define AUTOSRV_PORT                9000

  

                                                                                  
/*********************************************************************************
 * Error codes
 *********************************************************************************/ 
//
// Malloc failure
//
#define ERROR_MALLOC                ERROR_OUTOFMEMORY       
//
// An invalid (TID/Command code don't match with request) response is received from AutoSrv
//
#define ERROR_INVALID_RESPONSE      ERROR_INVALID_MESSAGE
//
// An invalid argument was passed to a command
//                                     
#define ERROR_INVALID_ARGUMENT      ERROR_INVALID_PARAMETER
//
// No error code has been specified
//
#define NO_ERROR_CODE               4
//
// Invalid command code
//
#define ERROR_INVALID_COMMAND       ERROR_BAD_COMMAND
//
// Session has been terminated
//
#define ERROR_SESSION_CLOSED        6

/**********************************************************************************
 * Debug levels and macros
 **********************************************************************************/

#ifndef DEBUGMSG1
 
#define DEBUGMSG1(level, format, arg) (printf(format, arg))

#endif

// JP - added temporarily, change as needed
#ifndef DEBUGMSG

#define DEBUGMSG(level, format) (printf(format))

#endif

//
// Debug levels
//
//#define DBG_AUTOSRV             1
//#define DBG_LIST                1
#define DBG_CONTROLLER_VERBOSE  3
#define DBG_VERBOSE             3
#define DBG_ERROR               2
#define DBG_WARNING             1


//
// Service specific stuff
//
#define DBG_SERVICE_ERROR        1         // Will report the errors in service
#define DBG_SERVICE_VERBOSE      2         // Will report the service info
#define SERVICE_NAME             "AutoSrv" // Name of the service
      
#define MAX_DEBUG_STRING_LENGTH  255       // Max length of a string to be dumped on debugger

      
#endif

