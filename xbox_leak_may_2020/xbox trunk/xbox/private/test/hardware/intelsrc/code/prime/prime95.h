// (c) Copyright 1999-2000 Intel Corp. All rights reserved.
// Intel Confidential.
//
// Prime95.h : main header file for the PRIME95 application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//#include "resource.h"       // main symbols


/////////////////////////////////////////////////////////////////////////////
// My non-MFC stuff went here
/////////////////////////////////////////////////////////////////////////////

// Constants

#define MIN_PRIME	5L		// Smallest testable prime
#define MAX_PRIME	LIMIT256	// Largest testable prime
#define MAX_FACTOR	LIMIT256	// Largest factorable number

// Limits based on 1000 iterations with an error no greater than 0.30.

#define LIMIT256	5260000L	// Crossover points
#define LIMIT224	4605000L
#define LIMIT192	3960000L
#define LIMIT160	3310000L
#define LIMIT128	2655000L
#define LIMIT112	2330000L
#define LIMIT96		2000000L
#define LIMIT80		1675000L
#define LIMIT64		1345000L

#define FAC61	LIMIT224		// How far to factor
#define FAC60	3540000L
#define FAC59	2750000L
#define FAC58	LIMIT112
#define FAC57	1750000L
#define FAC56	LIMIT64
#define FAC55	1050000L
#define FAC54	750000L

#define DATA_ADDR	0x20010000	// Addr of the input doubles
#define THE_END_64	0x200D0780	// Max addr for 64K FFT len
#define THE_END_80	0x200FBF00	// Max addr for 80K FFT len
#define THE_END_96	0x20127700	// Max addr for 96K FFT len
#define THE_END_112	0x201531C0	// Max addr for 112K FFT len
#define THE_END_128	0x2017E9C0	// Max addr for 128K FFT len
#define THE_END_160	0x201D61A0	// Max addr for 160K FFT len
#define THE_END_192	0x2022D520	// Max addr for 192K FFT len
#define THE_END_224	0x202848E0	// Max addr for 224K FFT len
#define THE_END_256	0x202DBBC0	// Max addr for 256K FFT len
extern long THE_END;			// End of the used address space

#define REL_486_SPEED	7.0	// 486 is 7 times slower than Pentium
#define REL_PRO_SPEED	0.87	// Pro is little faster than Pentium

#ifndef SEC1
#define SEC1(p)		0
#define SEC2(p,hi,lo)	0
#define SEC3(p)		0
#define SEC4(p)		0
#endif

// Global variables

extern char *INI_SECTION;		// Section name in INI file
extern int THREAD_ACTIVE;		// True if a thread is active
extern int volatile THREAD_STOP;	// TRUE if thread should stop
extern int AUTO_CONTINUE;		// 1 if auto continue is on
extern int ADVANCED_ENABLED;		// 1 if advanced menu is enabled
extern int TRAY_ICON;			// Display tiny tray icon
extern int HIDE_ICON;			// Display no icon
//extern "C" int volatile ERRCHK;		// 1 to turn on error checking
extern int JUST_FIND_FACTORS;		// Factor the range
extern int DOUBLE_CHECK;		// Double-check the range
extern "C" unsigned int volatile CPU_TYPE;// 3=Cyrix, 4=486, 5=Pentium, 6=Pro
extern unsigned long volatile CPU_SPEED;// For 90MHz = 90
extern unsigned long RANGE_START;	// Start of range to test
extern unsigned long RANGE_END;		// End of range to test
extern unsigned long NEXT_RANGE_START;	// Start of range to test next
extern unsigned long NEXT_RANGE_END;	// End of range to test next
extern unsigned int volatile ITER_OUTPUT;// Iterations between outputs
extern int TXT_EXTENSION;		// True if output file is results.txt

extern int PRIORITY;			// Desired priority level
extern int TIMED_RUN;			// TRUE if this is a timed run
extern CTime END_TIME;			// End time for a timed run
extern int SILENT_VICTORY;		// Quiet find of new Mersenne prime

extern HMODULE HLIB;			// Handle of networking DLL
extern int (__stdcall *HPROC_GET_NEXT_EXPONENT)(unsigned long *, int *);
extern int (__stdcall *HPROC_WRITE_RESULTS)(char *);

const int NumLines = 32;
extern char *lines[NumLines];
extern int charHeight;

// Internal routines

UINT threadDispatch (LPVOID);
void findSmallestP (unsigned long *);
int readFileHeader (char *, int *, short *, unsigned long *);
int readFileData (int, unsigned long, unsigned long);
int writeToFile (unsigned long, char *, unsigned long, unsigned long);
void clearPrime (unsigned long, int);
void makestr64 (char *);
void writeResults (char *);
void primeClear (unsigned long, unsigned long);
void generateRandomData (unsigned long, unsigned long);
void checkResultsFile (unsigned long, int *, int *);

extern "C" void setupf ();
extern "C" int factor64 ();
extern "C" void setup64 ();
extern "C" int lucas64 ();
extern "C" void setup80 ();
extern "C" int lucas80 ();
extern "C" void setup96 ();
extern "C" int lucas96 ();
extern "C" void setup112 ();
extern "C" int lucas112 ();
extern "C" void setup128 ();
extern "C" int lucas128 ();
extern "C" void setup160 ();
extern "C" int lucas160 ();
extern "C" void setup192 ();
extern "C" int lucas192 ();
extern "C" void setup224 ();
extern "C" int lucas224 ();
extern "C" void setup256 ();
extern "C" int lucas256 ();

long  addr (unsigned long, unsigned long, unsigned long);
int is_big_word (unsigned long, unsigned long, unsigned long);
void get_fft_value (unsigned long, unsigned long, unsigned long, long *);
void set_fft_value (unsigned long, unsigned long, unsigned long, long);

