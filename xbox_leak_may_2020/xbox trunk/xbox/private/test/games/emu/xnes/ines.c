#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m6502.h"
#include "fce.h"
#include "mapper.h"
#include "version.h"
#include "memory.h"
#include "svga.h"
#include "gg.h"

static char savename[1024];
static int SaveGame = 0;

#define ROM_MIRROR      1
#define ROM_SAVERAM     2
#define ROM_TRAINER     4
#define ROM_FOUR        8

static iNES_HEADER head;

static int zpow(int x, int y)
{
	int zap;
	int ret;
	
	ret=1;
	
	for(zap=0;zap<y;zap++)
		ret*=x;
	return ret;
}

static void iNESGI(int h)
{
	char szErrorMsg[256];

	switch(h)
	{
	case GI_POWER:
		if(genie & 2) 
		{
			genie = 3;
			SetGenieROMImage();
		}
		if(head.ROM_type & ROM_TRAINER)
		{
			memset(WRAM,0x00,4096);
			memset(WRAM+4096+512,0x00,4096-512);
		}
		else if(!(head.ROM_type & ROM_SAVERAM)) 
			memset(WRAM,0x00,8192);
		break;
	case GI_RESET:
		if(!(genie & 1))
			MMC_init(MapperNo);
		else GenieInit();
		break;
	case GI_CLOSE:
		{
			FILE *sp;
			if(SaveGame)
			{
				sp=fopen(savename,"wb");
				if (sp==NULL)
				{
					sprintf(szErrorMsg, "%s cannot be written to/created.\n",savename);
					PrintNonFatal(szErrorMsg);
				}
				else
				{
					fwrite(WRAM,8192,1,sp);
					if(MapperNo==5)
					{
						extern byte MMC5WRAMsize;
						if(MMC5WRAMsize==4)
							fwrite(MapperExRAM+8192,32768-8192,1,sp);
					}
					fclose(sp);
				}
			}
		}
		break;
	}
}

int iNESLoad(char *name, FILE *fp)
{
	FILE *sp;
	int x;
	
	char *loc=NULL;
	
	if(fread(&head,1,16,fp)!=16)
		return 1;
	
	if(memcmp(head.ID,"NES\x1a",4))
		return 1;
	
	ROM_type = head.ROM_type;
	ROM_size = head.ROM_size;
	VROM_size = head.VROM_size;
	for(x=0;x<=8;x++)
	{
		if((ROM_size-(zpow(2,x)))<=0)
		{
			ROM_size=zpow(2,x);
			break;
		}
	}
	if(VROM_size)
		for(x=0;x<8;x++)
		{
			if((VROM_size-(zpow(2,x)))<=0)
			{
				VROM_size=zpow(2,x);
				break;
			}
		}
        MapperNo = (head.ROM_type>>4);
        MapperNo|=(head.ROM_type2&0xF0);
        Mirroring = (head.ROM_type&ROM_MIRROR);
        MIRROR_SET2(Mirroring);
		
        if(ROM_type&8)
			DoFourscreen();
		
        ROM=(byte *)malloc(ROM_size<<14);
        if (VROM_size) VROM=(byte *)malloc(VROM_size<<13);
		
        memset(ROM,0xFF,ROM_size<<14);
        if(VROM_size) 
			memset(VROM,0xFF,VROM_size<<13);
        if(ROM_type&4)
			fread(WRAM+0x1000,512,1,fp);
		
        fread(ROM,0x4000,head.ROM_size,fp);
        fread(VROM,0x2000,head.VROM_size,fp);
		
        pale=0;
        if(head.ROM_type2&1)
        {
			GameAttributes|=GA_VSUNI;
			pale=FindVSUniPalette();
        }
		
        //printf("File %s loaded.\n",name);
        if(MapperNo==5) DetectMMC5WRAMSize();
		
		if(ROM_type&2)
		{
			SaveGame=1;
			loc=strstr(name,".nes");
			if(loc!=NULL)
			{
				strncpy(savename,name,loc-name);
				strncpy(savename+(loc-name),".sav\x00",5);
			}
			else
			{
				strncpy(savename,name,strlen(name));
				strncpy(savename+(strlen(name)),".sav\x00",5);
			}
			
			sp=fopen(savename,"rb");
			if(sp!=NULL)
			{
				if(fread(WRAM,1,8192,sp)==8192)
				{
					if(MapperNo==5)
					{
						extern byte MMC5WRAMsize;
						if(MMC5WRAMsize==4)
						{
							if(fread(MapperExRAM+8192,1,32768-8192,sp)==32768-8192)
								goto loaded;
						}
						else
							goto loaded;
					}
					else
						goto loaded;
				}
				goto notloaded;
loaded:
				//printf("  WRAM Save File %s loaded...\n",savename);
notloaded:
				fclose(sp);
			}
			
		}
		//printf("\nROM:  %dx16k VROM:  %dx8k Mapper:  %d Mirroring: %s", head.ROM_size,head.VROM_size,MapperNo,Mirroring == 2 ? "None(Four-screen)" : Mirroring ? "Vertical" : "Horizontal");
        if(genie & 2)
			genie = 3;
		GameInterface = (void *)iNESGI;
        return 0;
}

