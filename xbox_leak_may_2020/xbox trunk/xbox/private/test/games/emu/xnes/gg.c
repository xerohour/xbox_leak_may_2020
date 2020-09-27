#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "m6502.h"
#include "fce.h"
#include "mapper.h"
#include "gg.h"
#include "ggrom.h"
static byte *GENIEROM;

uint8 modcon=0xFF;
uint8 genieval[3];
uint8 geniech[3];
uint16 genieaddr[3];

void SetGenieROMImage(void)
{
	int x;	
	if(!(GENIEROM=malloc(9216))) {genie=0;return;}
	memcpy(GENIEROM,GenieData,4352);
	for(x=0;x<4;x++)
		memcpy(GENIEROM+8192+(x<<8),GENIEROM+4096,256);
	memcpy(GENIEROM+4096,GENIEROM,4096);
}


static void GenieWrite(word A, byte V)
{
	switch(A)
	{
	case 0x800c:genieval[2]=V;break;
	case 0x8008:genieval[1]=V;break;
	case 0x8004:genieval[0]=V;break;
		
	case 0x800b:geniech[2]=V;break;
	case 0x8007:geniech[1]=V;break;
	case 0x8003:geniech[0]=V;break;
		
	case 0x800a:genieaddr[2]&=0xFF00;genieaddr[2]|=V;break;
	case 0x8006:genieaddr[1]&=0xFF00;genieaddr[1]|=V;break;
	case 0x8002:genieaddr[0]&=0xFF00;genieaddr[0]|=V;break;
		
	case 0x8009:genieaddr[2]&=0xFF;genieaddr[2]|=(V|0x80)<<8;break;
	case 0x8005:genieaddr[1]&=0xFF;genieaddr[1]|=(V|0x80)<<8;break;
	case 0x8001:genieaddr[0]&=0xFF;genieaddr[0]|=(V|0x80)<<8;break;
	case 0x8000:if(!V)
				{
					genie&=0xFE;
					ResetMapper();GameInterface(GI_RESET);Reset6502();
				}
		else
		{
			modcon=V;
			if(V==0x71) modcon=0xFF;
		}
		break;
	}
}

void GenieInit(void)
{
	int x;
	memset(genieval,0xFF,3);
	memset(geniech,0xFF,3);
	memset(genieaddr,0xFF,6);
	modcon=0xFF;
	SetWriteHandler(0x8000,0xFFFF,(void *)GenieWrite);
	for(x=0;x<4;x++)
		Page[x+4]=GENIEROM-0x8000-(8192*x);
	for(x=0;x<8;x++)
		VPage[x]=MMC5SPRVPage[x]=MMC5BGVPage[x]=GENIEROM+8192-0x400*x;
	
}

