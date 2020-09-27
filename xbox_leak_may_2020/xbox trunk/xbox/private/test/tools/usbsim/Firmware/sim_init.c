//*****************************************************************************
//	    Filename: sim_init.c
//       Project: Microsoft USB Simulator
//	   Copyright: INDesign-LLC, 2000
//
//	 Description:
//
//
// MKS Source Control
// $Source: p:/Microsoft/621-USB_Simulator/Code/rcs/sim_init.c $
// $Revision: 1.6 $
// $Date: 2000/12/19 13:25:33Z $ 
// $Author: klpayne $
// $Log: sim_init.c $
// Revision 1.6  2000/12/19 13:25:33Z  klpayne
// Init buffers
// Revision 1.5  2000/12/18 22:33:01Z  rjgardner
// Remove Usb_Init from Sim_Init
// Revision 1.4  2000/12/14 21:31:33Z  klpayne
// Reorder inits
// Revision 1.3  2000/12/13 15:21:39Z  klpayne
// Add in init of GPIO
// Revision 1.2  2000/12/06 19:20:46Z  rjgardner
// add call to Ether_Init
// Revision 1.1  2000/12/05 22:04:53Z  klpayne
// Initial revision
//
//*****************************************************************************
#include <dos.h>
#include "sim.h"
#include "buffers.h"

//------------------------LOCAL CONSTANTS AND MACROS---------------------------

//-----------------------------GLOBAL VARIABLES--------------------------------

//----------------------STATIC LOCAL FUNCTION PROTOTYPES-----------------------
//*****************************************************************************
//
//	   FUNCTION: static void GPIO_Init(void)
//	DESCRIPTION: This function inits the GPIO.   
//
//	     INPUTS: none			
//	    RETURNS: none		
//        NOTES:
//
//*****************************************************************************
static void GPIO_Init(void) {

    outp(GPIO_CTRL,GPIO_CTRL_DATA);
    outp(GPIO_PORTA,0x55);  // init port A, all boards: reset, D+ off
}
 
//*****************************************************************************
//
//	   FUNCTION: void Sim_Init(void)
//	DESCRIPTION: This function inits the simulator.   
//
//	     INPUTS: none			
//	    RETURNS: none
//        NOTES:
//
//*****************************************************************************
void Sim_Init(void) {

    GPIO_Init();    // init the GPIO
    Init_Buffers(); // init the buffers
	Ether_Init();   // init network channel
}


