#ifndef _FCEH
void InitPAL(void);
void TrashFCE(void);
int InitFCE(char *name);
void hcheck(void);
void BlankMem(void);
void BNull(word A, byte V);
byte ANull(word A);
byte AROM(word A);
void BRAM(word A, byte V);
byte ARAM(word A);
void BWRAM(word A, byte V);
byte AWRAM(word A);

byte A2000( word A);
byte A2001( word A);
byte A2002( word A);
byte A2007( word A);
byte A4016( word A);
byte A4017( word A);

void B2000(word A, byte V);
void B2001(word A, byte V);
void B2003(word A, byte V);
void B2004(word A, byte V);
void B2005(word A, byte V);
void B2006(word A, byte V);
void B2007(word A, byte V);

void B4014(word A, byte V);
void B4016(word A, byte V);
void Loop6502(void);
void ResetMapper(void);
void CartClose(void);
void ResetNES(void);
void PowerNES(void);
void RunFCE(void);

extern byte *Page[],*VPage[],*MMC5SPRVPage[],*MMC5BGVPage[];
extern byte RAM[],SPRAM[],WRAM[],VRAM[];
extern byte *ROM,*VROM;
extern pair __RefreshAddr,__TempAddr;
#define RefreshAddr __RefreshAddr.W
#define TempAddr __TempAddr.W
extern byte vtoggle,XOffset,VRAMBuffer;
extern byte PPU[4];
extern byte Mirroring;
extern byte joy_readbit,joy2_readbit,joy1or,joy2or;
extern word joy_1,joy_2;
extern byte PSG[];

extern int scanline;
extern int MapperNo;
extern byte *vnapage[4];
extern word VPAL2[4];
extern char nothing[8192];
extern byte vsdip;
extern byte genie;
extern byte BaseDirectory[2048];

extern byte VPAL[8];
extern word ROM_size;
extern word VROM_size;
extern byte ROM_type;
extern byte Mirroring;
extern byte PAL;

extern byte (*ARead[0x10000])( word A);
extern void (*BWrite[0x10000])( word A, byte V);
void SetReadHandler(int32 start, int32 end, byte (*func)(uint16 A));
void SetWriteHandler(int32 start, int32 end, void (*func)(uint16 A, uint8 V));
void *GetWriteHandler(int32 a);

#define	VBlankON	(PPU[0]&0x80)	/* Generate VBlank NMI */
#define	SpHitON		(PPU[0]&0x40)	
#define	Sprite16	(PPU[0]&0x20)	/* Sprites 8x16/8x8        */
#define	BGAdrHI		(PPU[0]&0x10)	/* BG pattern adr $0000/$1000 */
#define	SpAdrHI		(PPU[0]&0x08)	/* Sprite pattern adr $0000/$1000 */
#define	INC32		(PPU[0]&0x04)	/* auto increment 1/32  */
#define	NameTable	(PPU[0]&0x3)	/* name table $2000/$2400/$2800/$2C00 */

#define SpriteON        (PPU[1]&0x10)   /* Show Sprite             */
#define ScreenON        (PPU[1]&0x08)   /* Show screen             */
#define PPU_status      (PPU[2])

#define	GA_VSUNI	1

// kevin
#define NOSIGNALS
#define ALTERNATE_MAIN


void (*GameInterface)(int h);

#define GI_RESET	1
#define GI_POWER	2
#define GI_CLOSE	3

extern int GameAttributes;
	typedef struct {
		char ID[4]; /*NES^Z*/
		byte ROM_size;
		byte VROM_size;
		byte ROM_type;
		byte ROM_type2;
                byte reserve[8];
	} iNES_HEADER;

#endif
#define _FCEH
