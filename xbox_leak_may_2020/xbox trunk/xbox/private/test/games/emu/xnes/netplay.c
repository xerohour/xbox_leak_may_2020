#ifdef NETWORK

#include <stdio.h>
#include <stdlib.h>

#include "m6502.h"
#include "svga.h"
#include "netplay.h"

uint8 netplay=0;        /*      Bit 0 is set if we are a server */
/*      Bit 1 is set if we need a host  */

char netplayhost[256];
uint8 netskip=3;


static void SendCommand(byte c)
{
	byte x;
	
	x=0xFF;
	
	NetworkSendByte(&x);
	NetworkSendByte(&c);
}



static void NetError(void)
{
	sprintf(errmsg,"Network error/connection lost!");
	howlong=255;
	netplay=0;
}

int InitNetplay(void)
{
	if(netplay==1)
	{
		//printf("Now listening to connections on port 4046...\n");
		if(NetworkConnect(0)==-1)
		{
		//	printf("Error listening on socket!\n");
			return 0;
		}
		NetworkSendByte(&netskip);
        //printf("Sent virtual joystick refresh interval: %d...\n",netskip);
	}
	else if(netplay==2)
	{
		//printf("Now attempting to connect to %s...\n",netplayhost);
		if(NetworkConnect(netplayhost)==-1)
		{
		//	printf("Error connecting to %s!\n",netplayhost);
			return 0;
		}

		NetworkRecvByte(&netskip);
		//printf("Got virtual joystick refresh interval: %d...\n",netskip);
	}
	return 1;
}

static uint8 netcount=0;
static uint8 netlast=0;
static uint8 netlastremote=0;

void NetplayUpdate(uint32 *JS)
{
	if(netplay==1)         // We're a server
	{
		if(!netcount)
		{
			netlast=(*JS&0xFF)|((*JS>>8)&0xFF);    // Combine joysticks on this end
			
			if(CommandQueue)
			{
				SendCommand(CommandQueue);
			}
			if(netlast==0xFF) netlast&=0x7F;
			if(!NetworkSendByte(&netlast)) NetError();
			if(!NetworkRecvByte(&netlastremote)) NetError();
			
			if(CommandQueue)
			{
				DoCommand(CommandQueue);     
				CommandQueue=0;
			}
			
		}
		*JS=netlast;
		*JS|=netlastremote<<8;
	}
	else if(netplay==2)    // We're connected to a host (we're second player)
	{
		byte temp;
		if(!netcount)
		{
			netlast=((*JS>>8)&0xFF)|(*JS&0xFF);    // Combine joysticks on this end
			
			if(netlast==0xFF) netlast&=0x7F;
			if(!NetworkSendByte(&netlast)) NetError();
			
refetch:
			if(!NetworkRecvByte(&temp)) NetError();
			
			if(temp==0xFF)
			{
				if(!NetworkRecvByte(&temp)) NetError();
				DoCommand(temp);
				goto refetch;
			}
			
			netlastremote=temp;
		}
		*JS=netlast<<8;
		*JS|=netlastremote;
	}
	if(netskip) netcount=(netcount+1)%netskip;
}
#endif
