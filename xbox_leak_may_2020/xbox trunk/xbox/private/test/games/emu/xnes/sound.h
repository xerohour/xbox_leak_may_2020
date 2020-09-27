#define OVERSAMPLESHIFT 4
#define OVERSAMPLE *16
#define SND_BUFSIZE 256

void FillSoundBuffer(unsigned char *Buf);
extern double nesincsize;
extern uint32 PSG_base;
extern unsigned short lfsr;
extern long PCMIRQCount;

extern void Write_PSG(uint16 A, uint8 V);
extern uint8 Read_PSG(uint16 A);
void SetSoundVariables(int Rate);
void ResetSound(void);
extern unsigned char decvolume[];

extern int vdis;
extern long count[]; 
extern long vcount[];
extern unsigned char sqnon;
extern ullong RawPCMCount;

typedef struct squ {
	void (*func)(byte A, byte V);
	byte A;
	byte V;
	word __junk;
	uint32 ts;
} squ;

typedef struct __rpd {
        unsigned long ts;
       		 char d; 
	unsigned char new;
} rpd;
extern rpd RawPCMData[];
extern unsigned char RawC;
extern long RawWrite[32];
extern unsigned char RawPCMOn;
extern uint8 triangleor;
extern uint8 tricoop;
extern uint8 PCMBitIndex;
extern uint32 PCMAddressIndex;
extern int32 PCMSizeIndex;
extern uint8 PCMBuffer;

extern uint8 SweepCount[2];
extern uint8 DecCountTo1[3];

#define AddQueue(f,a,v) {uint32 dokp; SoundQueue[SQWrPtr].func=(void *)f;SoundQueue[SQWrPtr].A=a;SoundQueue[SQWrPtr].V=v;dokp=(SQWrPtr+1)&32767;SoundQueue[SQWrPtr].ts=timestamp;SQWrPtr=dokp;}

extern squ SoundQueue[32768];
extern uint32 SQWrPtr;
extern uint32 SQRdPtr;

void HaltSound(int how);
void UnHaltSound(void);

extern int SndRate;

extern uint8 fcnt;
extern int32 fhcnt;
void FrameSoundUpdate(void);


int GetSoundBuffer(int32 **W);
