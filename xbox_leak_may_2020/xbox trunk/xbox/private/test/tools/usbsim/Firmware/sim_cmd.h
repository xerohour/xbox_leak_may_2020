//*****************************************************************************
//	    Filename: sim_cmd.h
//       Project: Microsoft USB Simulator
//	   Copyright: Microsoft, 2000
//
//   Description: This is the header file for the sim_cmd.c file
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/sim_cmd.h $
// $Revision: 1.6 $
// $Date: 2001/05/08 14:59:08Z $ 
// $Author: klpayne $
// $Log: sim_cmd.h $
// Revision 1.6  2001/05/08 14:59:08Z  klpayne
// Add stall subcommand
// Revision 1.5  2001/01/11 14:32:48Z  klpayne
// Add USB Reset Command
// Revision 1.4  2000/12/20 18:06:39Z  klpayne
// Add prototype for sim_to_cmd()
// Revision 1.3  2000/12/19 13:23:32Z  klpayne
// Initial support of buffers
// Revision 1.2  2000/12/14 21:30:47Z  klpayne
// Added handshake command support
// Revision 1.1  2000/12/06 15:33:02Z  klpayne
// Initial revision
//
//*****************************************************************************
#ifndef SIM_CMD_H
#define SIM_CMD_H

//-----------------------PUBLIC CONSTANTS AND MACROS---------------------------

enum CMDS	// commands
{
	CMDUSBDATA = 1,
	CMDUSBHS,
	CMDIPQUERY,
	CMDSIMSTAT,
	CMDSIMSETUP
};

enum SUBCMDSUSBDATA	// subcommands for USB data command
{
	SUBUSBDATA = 0
};

enum SUBCMDSUSBHA	// subcommands for USB HS command
{
	SUBUSBHSACK = 1,
	SUBUSBHSOTHER,
    SUBUSBHSRESET
};

enum SUBCMDIPQUERY // subcommands for IP query command
{
	SUBIPQUERY = 0
};

enum SUBCMDSIMSTAT // subcommands for sim status command
{
	SUBVERSION = 1,
	SUBMACADDR,
	SUBSTATUS
};

enum SUBCMDSIMSETUP // subcommands for sim setup command
{
	SUBATTRIB = 1,
	SUBADDR,
	SUBCONNECT,
    SUBSTALL = 7
};

//-------------------------EXTERN PUBLIC VARIABLES-----------------------------

//-----------------------PUBLIC STRUCTURE DEFINITIONS--------------------------

//------------------------PUBLIC FUNCTION PROTOTYPES---------------------------
extern void Sim_To_Cont_USBData(unsigned char brd, unsigned char pid, unsigned char ep, unsigned int size, unsigned char *ptr);
extern void Sim_To_Cont_USBHS(unsigned char brd, unsigned char pid, unsigned char ep, unsigned int size, unsigned char *ptr);
extern void Cont_To_Sim_Parser(unsigned char brd);

#endif