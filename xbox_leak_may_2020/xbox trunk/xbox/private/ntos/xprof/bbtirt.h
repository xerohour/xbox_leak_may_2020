/***********************************************************************
* Microsoft BBT
*
* Microsoft Confidential.  Copyright 1994-2000 Microsoft Corporation.
*
* Component:
*
* File: bbtirt.h
*
* File Comments:
*
*
***********************************************************************/


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                                                                         !
//                                                                         !
//                                                                         !
// Please note that this file is being shared with the NT group for kernel !
// instrumentation since they are generating their own IDF files for BBT.  !
// So please notify one of us JohnLef/HoiV/BobFitz before changing any     !
// data structures that has to do with the IDFHDR or IRTP.                 !
//                                                                         !
//                                                                         !
//                                                                         !
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


struct IRTP                            // Instrumentation Run Time Parameters
{
   DWORD       dwVersion;              // Version of this structure
   DWORD       dwId;                   // Id of database
   DWORD       dwGeneration;           // Database generation
   void        *pvImageBase;           // Base address of this module
   DWORD       cmsTimer;               // Timer interval in milliseconds
   DWORD       fMultipleInstance;      // Instrument multiple instances together
   DWORD       fSweepable;             // Sweeping is supported
   DWORD       fNtVdm;                 // This is NTVDM.EXE or WOW32.DLL
   DWORD       tov;                    // Variety of time order instrumentation
   DWORD       *rgpfnCallback;         // Pointer to array of callback pointers
   void        *pfnNopCallback;        // Pointer to nop thunk
#ifdef  POGO
   DWORD       pfnNopCallback2;        // Pointer to nop thunk for switch probe
#endif
   void        **rgpvResource;         // Pointer to array of resource addresses
   DWORD       cbIdfMax;               // User requested maximum IDF size
   DWORD       cbrc;                   // Count of static branches
   DWORD       cibs;                   // Count of indirect branch sources
   DWORD       cibd;                   // Count of indirect branch targets
   DWORD       cfun;                   // Count of functions
   DWORD       crsc;                   // Count of resources
#ifdef  POGO
   DWORD       csbr;                   // Count of switches
#endif
   DWORD       ifunEndBoot;            // Last function of boot; 0xFFFFFFFF if none

   char        *szIdfPath;             // UNDONE: Remove when new IRT complete
   char        *szIdfKeyPath;          // UNDONE: Remove when new IRT complete
   DWORD       dwDynProbeCount;        // Number of TOB Probes to execute to increment time.
   DWORD       fFourBitTobs;           // Are we using 4-bit tobs?
};

// This is a copy of IRTP with all the pointer fields
// replaced by DWORDLONG so that we can get 64 bit values
// to pass over to IRT.
//
struct IRTPIA64                        // Instrumentation Run Time Parameters
{
   DWORD       dwVersion;              // Version of this structure
   DWORD       dwId;                   // Id of database
   DWORD       dwGeneration;           // Database generation
   DWORDLONG   pvImageBase;            // Base address of this module
   DWORD       cmsTimer;               // Timer interval in milliseconds
   DWORD       fMultipleInstance;      // Instrument multiple instances together
   DWORD       fSweepable;             // Sweeping is supported
   DWORD       fNtVdm;                 // This is NTVDM.EXE or WOW32.DLL
   DWORD       tov;                    // Variety of time order instrumentation
   DWORDLONG   rgpfnCallback;          // Pointer to array of callback pointers
   DWORDLONG   pfnNopCallback;         // Pointer to nop thunk
#ifdef  POGO
   DWORD       pfnNopCallback2;        // Pointer to nop thunk for switch probe
#endif
   DWORDLONG   rgpvResource;           // Pointer to array of resource addresses
   DWORD       cbIdfMax;               // User requested maximum IDF size
   DWORD       cbrc;                   // Count of static branches
   DWORD       cibs;                   // Count of indirect branch sources
   DWORD       cibd;                   // Count of indirect branch targets
   DWORD       cfun;                   // Count of functions
   DWORD       crsc;                   // Count of resources
#ifdef  POGO
   DWORD       csbr;                   // Count of switches
#endif
   DWORD       ifunEndBoot;            // Last function of boot; 0xFFFFFFFF if none

   DWORDLONG   szIdfPath;             // UNDONE: Remove when new IRT complete
   DWORDLONG   szIdfKeyPath;          // UNDONE: Remove when new IRT complete
   DWORD       dwDynProbeCount;        // Number of TOB Probes to execute to increment time.
   DWORD       fFourBitTobs;           // Are we using 4-bit tobs?
};


#ifdef POGO

const DWORD dwIrtVerCurrent = 0x8000100b;

#else   // !POGO

const DWORD dwIrtVerCurrent = 0x00001010; // This also much be changed in idf.h

#endif  // !POGO


enum IIFN                              // Index into IRTP.rpfnCallback array
{
#ifdef POGO
   iifnInit,                           // UNDONE: remove when complier updated
#endif
   iifnBranch,                         // Direct branch
   iifnIndirectSource,                 // Indirect branch source
   iifnIndirectTarget,                 // Indirect branch target
   iifnFunction,                       // Function entry
#ifdef POGO
   iifnSwitch,                         // Switch statement
#endif
   iifnMax
};

struct FASTRTPARAM
{
   char  szModulePath[ _MAX_PATH ];
};

struct FASTINDDATA
{
   DWORDLONG pidfhdr;
   DWORDLONG rgiblh;
   DWORDLONG negaddrbase;
   DWORDLONG pfnInd;
};

struct FASTINITINDIRECT
{
    DWORDLONG    pirtp; //const IRTP  *
    DWORDLONG    rgCounts; //DWORD *
    DWORDLONG    rgTobBytes; //BYTE *
    DWORDLONG    pfrtp;     //FASTRTPARAM *
    DWORDLONG    rgSeqNums; //DWORD *
    DWORDLONG    pFastInd;  //FASTINDDATA *
};


struct REGAXP
{
   DWORDLONG v0;                  // 0x00                                   $0
   DWORDLONG t0;                  // 0x08                                   $1
   DWORDLONG t1;                  // 0x10                                   $2
   DWORDLONG t2;                  // 0x18                                   $3
   DWORDLONG t3;                  // 0x20                                   $4
   DWORDLONG t4;                  // 0x28                                   $5
   DWORDLONG t5;                  // 0x30                                   $6
   DWORDLONG t6;                  // 0x38                                   $7
   DWORDLONG t7;                  // 0x40                                   $8
   DWORDLONG s0;                  // 0x48        Currently not used         $9
   DWORDLONG s1;                  // 0x50        Currently not used         $10
   DWORDLONG s2;                  // 0x58                                   $11
   DWORDLONG s3;                  // 0x60                                   $12
   DWORDLONG s4;                  // 0x68                                   $13
   DWORDLONG s5;                  // 0x70                                   $14
   DWORDLONG fp;                  // 0x78        Currently not used         $15
   DWORDLONG a0;                  // 0x80                                   $16
   DWORDLONG a1;                  // 0x88                                   $17
   DWORDLONG a2;                  // 0x90                                   $18
   DWORDLONG a3;                  // 0x98                                   $19
   DWORDLONG a4;                  // 0xA0                                   $20
   DWORDLONG a5;                  // 0xA8                                   $21
   DWORDLONG t8;                  // 0xB0                                   $22
   DWORDLONG t9;                  // 0xB8                                   $23
   DWORDLONG t10;                 // 0xC0                                   $24
   DWORDLONG t11;                 // 0xC8                                   $25
   DWORDLONG ra;                  // 0xD0                                   $26
   DWORDLONG t12;                 // 0xD8                                   $27
   DWORDLONG at;                  // 0xE0                                   $28

   DWORDLONG reserved;            // 0xE8
   DWORDLONG lr1;                 // 0xF0
   DWORDLONG arg;                 // 0xF8

   DWORDLONG f0;                  // 0x100
   DWORDLONG f1;                  // 0x108
   DWORDLONG f10;                 // 0x110
   DWORDLONG f11;                 // 0x118
   DWORDLONG f12;                 // 0x120
   DWORDLONG f13;                 // 0x128
   DWORDLONG f14;                 // 0x130
   DWORDLONG f15;                 // 0x138
   DWORDLONG f16;                 // 0x140
   DWORDLONG f17;                 // 0x148
   DWORDLONG f18;                 // 0x150
   DWORDLONG f19;                 // 0x158
   DWORDLONG f20;                 // 0x160
   DWORDLONG f21;                 // 0x168
   DWORDLONG f22;                 // 0x170
   DWORDLONG f23;                 // 0x178
   DWORDLONG f24;                 // 0x180
   DWORDLONG f25;                 // 0x188
   DWORDLONG f26;                 // 0x190
   DWORDLONG f27;                 // 0x198
   DWORDLONG f28;                 // 0x1a0
   DWORDLONG f29;                 // 0x1a8
   DWORDLONG f30;                 // 0x1b0

   DWORDLONG gp;                  //             Currently not saved        $29
   DWORDLONG sp;                  //             Currently not saved        $30
   DWORDLONG zr;                  //             Currently not saved        $31
};


struct REGMIPS
{
   DWORD reserved[4];         // 0x00, 16 byte preallocated argument build area

   DWORDLONG at;                  // 0x10,                                  $1
   DWORDLONG v0;                  // 0x18,                                  $2
   DWORDLONG v1;                  // 0x20,                                  $3
   DWORDLONG a0;                  // 0x28,                                  $4
   DWORDLONG a1;                  // 0x30,                                  $5
   DWORDLONG a2;                  // 0x38,                                  $6
   DWORDLONG a3;                  // 0x40,                                  $7
   DWORDLONG t0;                  // 0x48,                                  $8
   DWORDLONG t1;                  // 0x50,                                  $9
   DWORDLONG t2;                  // 0x58,                                  $10
   DWORDLONG t3;                  // 0x60,                                  $11
   DWORDLONG t4;                  // 0x68,                                  $12
   DWORDLONG t5;                  // 0x70,                                  $13
   DWORDLONG t6;                  // 0x78,                                  $14
   DWORDLONG t7;                  // 0x80,                                  $15
   DWORDLONG s0;                  // 0x88,                                  $16
   DWORDLONG s1;                  // 0x90,                                  $17
   DWORDLONG s2;                  // 0x98,                                  $18
   DWORDLONG s3;                  // 0xA0,                                  $19
   DWORDLONG s4;                  // 0xA8,                                  $20
   DWORDLONG s5;                  // 0xB0,                                  $21
   DWORDLONG t8;                  // 0xB8,                                  $24
   DWORDLONG t9;                  // 0xC0,                                  $25
   DWORDLONG ra;                  // 0xC8,                                  $31

   DWORDLONG rs1;                 // 0xD0     reserved1
   DWORDLONG rs2;                 // 0xD8     reserved2

                              //          k0 Not needed to save         $26
                              //          k1 Not needed to save         $27
                              //          gp Not needed to save         $29
                              //          sp Not needed to save         $30

};


struct REGMPPC
{
   DWORD reserved1[9];        // 0x00, Filler
   DWORD ibPatch;             // 0x24,          ib to instr code to be patched
   DWORD reserved2[22];       // 0x28, Filler

   DWORD ctr;                 // 0x80,          Offset to original CTR
   DWORD cr;                  // 0x84,          Offset to CR
   DWORD lr;                  // 0x88,          Offset to instrumentation site
   DWORD lr1;                 // 0x8C,          Offset to instr thunk site
   DWORD r0;                  // 0x90,
   DWORD r2;                  // 0x94,
   DWORD r3;                  // 0x98,
   DWORD r4;                  // 0x9C,
   DWORD r5;                  // 0xA0,
   DWORD r6;                  // 0xA4,
   DWORD r7;                  // 0xA8,
   DWORD r8;                  // 0xAC,
   DWORD r9;                  // 0xB0,
   DWORD r10;                 // 0xB4,
   DWORD r11;                 // 0xB8,
   DWORD r12;                 // 0xBC,

   DWORD arg;                 // 0xC0,
   DWORD pidfhdr;             // 0xC4,

   DWORD reserved3[14];       // 0xC8,
   DWORD r0Alias;             // 0x100,
};


struct REGPPC
{
   DWORD reserved[32];        // 0x00, Filler

   DWORD ctr;                 // 0x80,          Offset to original CTR
   DWORD cr;                  // 0x84,          Offset to CR
   DWORD lr;                  // 0x88,          Offset to instrumentation site
   DWORD lr1;                 // 0x8C,          Offset to instr thunk site
   DWORD r0;                  // 0x90,
   DWORD r2;                  // 0x94,
   DWORD r3;                  // 0x98,
   DWORD r4;                  // 0x9C,
   DWORD r5;                  // 0xA0,
   DWORD r6;                  // 0xA4,
   DWORD r7;                  // 0xA8,
   DWORD r8;                  // 0xAC,
   DWORD r9;                  // 0xB0,
   DWORD r10;                 // 0xB4,
   DWORD r11;                 // 0xB8,
   DWORD r12;                 // 0xBC,

   DWORD arg;                 // 0xC0,
   DWORD pidfhdr;             // 0xC4,
   DWORD pthd;                // 0xC8,
};
