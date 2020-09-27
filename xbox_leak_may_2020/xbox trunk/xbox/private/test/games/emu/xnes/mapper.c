/*
	Mapper routine
*/
#include <stdio.h>
#include <string.h>
#include "m6502.h"
#include "fce.h"
#include "mapper.h"
#include "memory.h"
#include "sound.h"
#include "nsf.h"
#include "fds.h"
#include "svga.h"
#include "gg.h"


byte vmask;
uint32 vmask1;
uint32 vmask2;
uint32 vmask4;
uint32 pmask8;
byte pmask16;
byte pmask32;

byte PRGBankList[4],PRGBankListLow[4];
word CHRBankList[8];    

int32 IRQLatch;
int32 IRQCount;
uint8 IRQa;

uint8 mapbyte[4][8];
uint16 mapword1[4];

byte MapperExRAM[0x8000];

#include "banksw.h"

int (*MapExpSound)(int32 *Wave);
void (*MapStateRestore)(int version);
void (*MapHBIRQHook)(void);

void DoFourscreen(void)
{
	int x;
	Mirroring=2;
	for(x=0;x<4;x++)
	{
		vnapage[x]=VRAM+0x2000+0x400*x;
		VPAL2[x]=0x8000|x;
	}
}

void onemir(byte V)
{
	int x;
	if(Mirroring==2) return;
	for(x=0;x<4;x++)
	{
		VPAL2[x]=0x8000|V;
		vnapage[x]=VRAM+0x2000+(V<<10);
	}
}

void MIRROR_SET2(byte V)
{
	if(Mirroring==2) return;
	Mirroring=V;
	switch(V)
	{
	case 0:         
        VPAL2[0]=VPAL2[1]=0x8000;VPAL2[2]=VPAL2[3]=0x8002;
        vnapage[0]=vnapage[1]=VRAM+0x2000;vnapage[2]=vnapage[3]=VRAM+0x2800;
        break;
	case 1:
        VPAL2[0]=VPAL2[2]=0x8000;VPAL2[1]=VPAL2[3]=0x8002;
        vnapage[0]=vnapage[2]=VRAM+0x2000;vnapage[1]=vnapage[3]=VRAM+0x2800;
        break;
	}
}

void MIRROR_SET(byte V)
{
	if(Mirroring==2) return;
	V^=1;
	Mirroring=V;
	switch(V)
	{
	case 0:         
        VPAL2[0]=VPAL2[1]=0x8000;VPAL2[2]=VPAL2[3]=0x8002;
        vnapage[0]=vnapage[1]=VRAM+0x2000;vnapage[2]=vnapage[3]=VRAM+0x2800;
        break;
	case 1:
        VPAL2[0]=VPAL2[2]=0x8000;VPAL2[1]=VPAL2[3]=0x8002;
        vnapage[0]=vnapage[2]=VRAM+0x2000;vnapage[1]=vnapage[3]=VRAM+0x2800;
		break;
	}
}

void (*PPU_hook)(unsigned long A);

static void NONE_init(void)
{
	int x;
	
	vmask = VROM_size-1;
	vmask1 = (VROM_size<<3)-1;
	vmask2 = (VROM_size<<2)-1;
	vmask4 = (VROM_size<<1)-1;
	pmask8=(ROM_size<<1)-1;
	pmask16=ROM_size-1;
	pmask32=(ROM_size>>1)-1;
	ROM_BANK16(0x8000,0);
	ROM_BANK16(0xC000,pmask16);
	if(VROM_size) 
	{
		VROM_BANK8(0);
		for(x=0;x<8;x++) VPAL[x]=0;	 
	}
	else
		for(x=0;x<8;x++)  
		{VPAL[x]=1;VPage[x]=VRAM;}
}

int FindVSUniPalette(void)
{
	
	if (!(memcmp(ROM+(131044&(ROM_size*0x4000-1)),"CASTLE VANIA",12)))
		return 1;
	if (!(memcmp(ROM+(9983&(ROM_size*0x4000-1)),"EIIIEGG",7))) /* VS SMB*/
		return 2;
	if (!(memcmp(ROM+(28483&(ROM_size*0x4000-1)),"iopnHHJqSLn",11))) /* VS ICE */
		return 2;
	if (!(memcmp(ROM+(29664&(ROM_size*0x4000-1)),"<8887776644444444333333332222222222222222",41))) /* VS DUCK*/
		return 0;        
	if (!(memcmp(ROM+(4003&(ROM_size*0x4000-1)),"#*L",3))) /* VS Hogan */
		return 0;
	if (!(memcmp(ROM+(32497&(ROM_size*0x4000-1)),"DDUDDDD3",8))) /* VS Dr Mario */
		return 3;
	if (!(memcmp(ROM+(32673&(ROM_size*0x4000-1)),"bBjBpBbjBq",10))) /* VS EB*/
		return 7;
	if (!(memcmp(ROM+(30379&(ROM_size*0x4000-1)),"SRRQQ",5))) /* VS Golf*/
		return 1;
	if (!(memcmp(ROM+(28848&(ROM_size*0x4000-1)),"YKKqqq",6))) /* VS Pinball*/
		return 4;
	if (!(memcmp(ROM+(42193&(ROM_size*0x4000-1)),"tsruvw",6))) /* VS Gradius*/
		return 4;
	if (!(memcmp(ROM+(2142&(ROM_size*0x4000-1)),"|||hi",5))) /* VS Platoon*/
		return 4;
	if (!(memcmp(ROM+(10&(ROM_size*0x4000-1)),"861128H",7))) /* VS Goonies*/
		return 5;
	if (!(memcmp(ROM+(0xA29&(ROM_size*0x4000-1)),"SLALOM",6))) /* VS Slalom */
		return 6;
	if (!(memcmp(ROM+(0x9E22&(ROM_size*0x4000-1)),"1986",4))) /* VS RBI Baseball */
		vsdip=joy2or=0x20;
	if (!(memcmp(ROM+(0x140e&(ROM_size*0x4000-1)),"oAmAoAm",7))) /* VS Sky Kid */
		vsdip=joy2or=0x20;
	
	return 0;
}
static int VSindex;

uint8 VSRead(word A)
{
	switch(A)
	{
	case 0x5e00: VSindex=0;return 0xFF;
	case 0x5e01: switch(VSindex++)
				 {
	case 9: return 0x6F;
	default: return 0xB4;
				 }
	}
	return 0xFF;
}



void (*MapInitTab[256])(void)=
{
	0,
		Mapper1_init,Mapper2_init,Mapper3_init,Mapper4_init,
		Mapper5_init,Mapper6_init,Mapper7_init,Mapper8_init,
		Mapper9_init,Mapper10_init,Mapper11_init,0,
		Mapper13_init,0,Mapper15_init,Mapper16_init,
		Mapper17_init,Mapper18_init,Mapper19_init,0,
		Mapper21_init,Mapper22_init,Mapper23_init,Mapper24_init,
		Mapper25_init,Mapper26_init,0,0,
		0,0,0,Mapper32_init,
		Mapper33_init,Mapper34_init,0,0,
		0,0,0,Mapper40_init,
		Mapper41_init,Mapper42_init,Mapper43_init,Mapper44_init,
		Mapper45_init,Mapper46_init,Mapper47_init,Mapper33_init,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,Mapper64_init,
		Mapper65_init,Mapper66_init,Mapper67_init,Mapper68_init,
		Mapper69_init,Mapper70_init,Mapper71_init,0,
		Mapper73_init,0,Mapper75_init,Mapper76_init,
		0,Mapper78_init,Mapper79_init,Mapper80_init,
		0,0,0,0,
		Mapper85_init,0,Mapper87_init,0,
		0,Mapper90_init,0,0,
		Mapper93_init,Mapper94_init,Mapper95_init,Mapper96_init,
		Mapper97_init,0,Mapper99_init,0,
		0,0,0,0,0,0,0,0,
		0,0,0,Mapper112_init,Mapper113_init,0,0,0,
		Mapper117_init,Mapper118_init,Mapper119_init,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,Mapper151_init,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,Mapper180_init,
		0,Mapper182_init,0,Mapper184_init,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,Mapper225_init,Mapper226_init,0,Mapper228_init,
		Mapper229_init,0,0,0,0,0,Mapper43_init,0,
		0,0,0,Mapper240_init,0,0,0,0,
		0,Mapper246_init,0,0,0,0,0,0,0,0,0
};

int MMC_init(int type)
{
	int x;
	
	MapStateRestore=0;
	PPU_hook=0;
	MapHBIRQHook=0;
	MapExpSound=0;
	
	IRQa=IRQLatch=IRQCount=0;
	
	for(x=0;x<4;x++)
		memset(mapbyte[x],0,8);
	memset(mapword1,0,8);
	
	if(type==5)
		memset(MapperExRAM,0,8192);
	else
		memset(MapperExRAM,0,0x4000);
	
	NONE_init();
	
	if(type<=255)
	{
		if(MapInitTab[type]) MapInitTab[type]();
	}
	if(GameAttributes&GA_VSUNI)
		SetReadHandler(0x5e00,0x5e01,(void *)VSRead);
	return 1;
}

