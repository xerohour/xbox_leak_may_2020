
#ifdef STARTUPANIMATION

#pragma data_seg("INIT_RW")
#pragma code_seg("INIT")
#pragma bss_seg("INIT_RW")
#pragma const_seg("INIT_RD")

// Tell linker to put bootsound code and data into INIT section
#pragma comment(linker, "/merge:DSOUND=INIT")

#pragma comment(linker, "/merge:INIT_RD=INIT")
#pragma comment(linker, "/merge:INIT_RW=INIT")

#endif //STARTUPANIMATION


unsigned const short pitch_table_dsp[] = {
		0x80
};



unsigned short	FM32768[32768];

unsigned short	Sin128[128];


unsigned short	Saw128[128];



unsigned short Noise8192[8192];

unsigned short	ThunEl16[0x5540];	// size from thunel16k.equ
unsigned short	ReverseThunEl16[0x5540]; // reversed version

unsigned const char	ThunEl16Data[] = {
#include "thunel16.x00"
};


unsigned short	Glock[3768];	// size from glock.equ

unsigned const char	GlockData[] = {
#include "glock.x00"
};


unsigned short	Bubble[6719];	// size from bubble.equ

unsigned const char	BubbleData[] = {
#include "Bubble.x00"
};


