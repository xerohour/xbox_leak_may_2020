/***********************************************************************
* Microsoft BBT
*
* Microsoft Confidential.  Copyright (c) Microsoft Corporation.  All rights reserved.
*
* File Comments:
*
*
***********************************************************************/

#pragma once

#pragma pack(push, 4)

struct IDFHDR                          // Instrumentation Data File Header
{
   DWORD    dwSignature;               // ( 0) File signature
   DWORD    dwVersion;                 // ( 4)
   DWORD    dwId;                      // ( 8) Id of original image (.exe, .dll, .sys, ...)
   DWORD    ibszIdfKeyPath;            // ( C) Name of .IDFKEY file
   DWORD    dwGeneration;              // (10) Database generation
   DWORDLONG    qwStartTime;               // (14) Creation time of this data

   DWORD    tov;                       // (1C) Variety of time order instrumentation

   DWORD    cb;                        // (20) Size of valid data
   DWORD    cqwCount;                  // (24) Count of static edge counts
   DWORD    ibrgqwCount;               // (28) Offset to array of static edge count
   DWORD    ciblh;                     // (2C) Count of IBLHs
   DWORD    ibrgiblh;                  // (30) Offset to array of IBLHs
   DWORD    cible;                     // (34) Number of IBLE structs allocated
   DWORD    ctlhCode;                  // (38) Count of TLHs for code
   DWORD    ibrgtlhCode;               // (3C) Offset to array of TLHs for code
   DWORD    ctlhResource;              // (40) Count of TLHs for resources
   DWORD    ibrgtlhResource;           // (44) Offset to array of TLHs for resources
   DWORD    cthd;                      // (48) Count of THDs
   DWORD    ibthdHead;                 // (4C) Offset to first THD
   DWORD    cmsStartTime;              // (50) Tick count at initialization

   DWORD    ifunEndBoot;               // (54) Index of EndBoot function (from IRTP)
   DWORD    iSequenceEndBoot;          // (58) Seq number of the last boot time code or resource
   DWORD    iIntervalEndBoot;          // (5C) Last internal of boot sequence

   // The following data is used at runtime and appears here
   // so that it can be shared among multiple processes.

   DWORD    dwLock;                    // (60) Used to lock shared structures

   DWORD    cbAllocated;               // (64) Size of allocated (committed) memory
   DWORD    cbMax;                     // (68) Maximum size of memory region

   DWORD    cpsActive;                 // (6C) Count of processes using this data

   DWORD    fSweepPending;             // (70)
   DWORD    cthdActive;                // (74) Number of threads within runtime

   DWORD    cmsTimer;                  // (78) Timer interval in milliseconds
   DWORD    pidTimer;                  // (7C) Process ID of timer process or 0?
   DWORD    iIntervalCur;              // (80) Current time interval
   DWORD    cmsNextAttempt;            // (84)

   DWORD    iSequence;                 // (88) Sequence number of initial references
   DWORD    iIntervalStart;            // (8C) First valid time interval
   DWORD    iSequenceStart;            // (90) First valid sequence number

                                       // Runtime-only state for storage allocators

   DWORD    cbPermanentFree;           // (94) Base of permanent free page
   DWORD    cbPermanentLimit;          // (98) End of permanent free page
   DWORD    cbReclaimableFree;         // (9C) Base of reclaimable free page
   DWORD    cbReclaimableLimit;        // (A0) End of reclaimable free page
   DWORD    cbReclaimablePageHead;     // (A4) Head of linked list of reclaimable pages
   DWORD    cbReclaimablePageTail;     // (A8) Tail of linked list of reclaimable pages

                                       // Bitfields are implemented from LSB to MSB.
                                       // Since the position of LSB is different for little endian
                                       // and big endian processors, bit fields cannot be used for
                                       // IDF purposes. Instead, a series of chars are used instead.
   char     fBigEndian;                // (AC) Non-zero if collected data is big-endian
   char     fSweepable;                //      Non-zero if IDF is sweepable.
   char     fWinNTPerf;
   char     fFastRT;                   // Are we using the faster runtime?
   

   DWORDLONG hIdfFile;                 // (B0) HANDLE of open .IDF file

   DWORD    dwDynProbeCount;           // (B8) Number of TOB Probes to execute to increment time.
   DWORDLONG   pWinNTBufInterval;      // (BC) pinter to NT buffer timer interval.
   DWORD    ibineHead;                 
                                       
};

#pragma pack(pop)

#define szIrtMutexPrefix   "BBT30.Mtx."
#define szIrtFMutexPrefix  "BBT30.Ftx."
#define szIrtMemoryPrefix  "BBT30.Mem."
#define szIrtTimerApp      "bbtimr30"

#define wszIrtMutexPrefix  L"BBT30.Mtx."
#define wszIrtMemoryPrefix L"BBT30.Mem."
#define wszIrtTimerApp     L"bbtimr30"

#define szIdfIdTemplate    "%BBT_Idf_Id%"
#define szIdfKeyIdTemplate "%BBT_Key_Id%"
#define szIdfSuffix        ".idf"
#define szIdfKeySuffix     ".key"

#define dwIdfKeySignature       0xa97df1eb
#define dwIdfSignature          0x09040600
#define dwDIdfSignature         0x09040601
#define dwIdfVerCurrent         0x00001010  // This also much be changed in bbtirt.h

// Flag to indicate IDF originated with pogo runtime

#define dwIdfVerPogo            0x90000000


struct IBLH                            // Indirect Branch List Header
{
   DWORD    ibibleFirst;               // Offset to first IBLE
};


struct IBLE                            // Indirect Branch List Element
{
   DWORD    ibibleNext;                // Offset to next IBLE or 0
   DWORD    blkidSrc;
   DWORDLONG    qwCount;
};


#define cdwTob          4              // Keep 4 DWORDs for each TOB
#define cbitDw          32             // There are 32 bits in a DWORD

struct TOB                             // Time Order Bits
{
   DWORD    iInterval;                 // Time interval represented
   DWORD    rgdwBit[cdwTob];           // One bit for each time interval
};


struct TLH                             // Time List Header
{
   DWORD    ibtleFirst;                // Offset to first TLE
   DWORD    iSequenceInit;             // Initial sequence number
   DWORDLONG    qwTOBFanin;                // Directly measured fanin for this block
};


struct TLE                             // Time List Element
{
   DWORD    ibtleNext;                 // Offset to next TLE or 0
   TOB      tob;
};


struct THD
{

   DWORD    iBrc;                      // Last recorded branch
   DWORD    blkidSrc;                  // Last recorded indirect branch source
   DWORD    iDst;                      // Last recorded indirect branch target
   DWORD    ibthdNext;                 // Offset of next THD or 0
};


struct RPH
{
   DWORD    cbNext;
};


#ifndef TOV_DEFINED
#define TOV_DEFINED

enum TOV                               // Time Order Variety
{
   tovNone,
   tovFunction,
   tovBlock,
   tovEveryBlock,
   
   tov4Mask,
   tov4Fun,
   tov4Block,
   tov4EveryBlock,

   tovFunctionAndPostCall,
   tovThreadEveryBlock,

   tovCTO,                            // IRT to sweep counts for tobs
};

struct INE                             // Interval name entry
{
   DWORD ibineNext;
   DWORD iInterval;
   char  szName[16];
};

#endif
