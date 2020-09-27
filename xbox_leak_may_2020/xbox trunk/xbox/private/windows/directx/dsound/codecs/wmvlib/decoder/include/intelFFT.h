

#ifndef __INTELFFT_H_
#define __INTELFFT_H_

//***************************************************************************
// Data Types
//***************************************************************************
typedef enum {INTELFFT_INIT_PROCESS, INTELFFT_INIT_THREAD,
    INTELFFT_FREE_THREAD, INTELFFT_FREE_PROCESS} INTELFFT_INITCODE;

//***************************************************************************
// Function Declarations
//***************************************************************************
int MyInitNSP(int fdwReason); // Actually returning a BOOL

void auSetMinMaxFrameSize(const int iMinFrameSizeLOG,
                          const int iMaxFrameSizeLOG);

#endif  // __INTELFFT_H_
