#ifndef __M6502H
/** M6502: portable 6502 emulator ****************************/
/**                                                         **/
/**                         M6502.h                         **/
/**                                                         **/
/** This file contains declarations relevant to emulation   **/
/** of 6502 CPU.                                            **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996                      **/
/**               Alex Krasivsky  1996                      **/
/** Modyfied      BERO            1998                      **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/   
/**     changes to this file.                               **/
/*************************************************************/
#include "types.h"

#pragma warning( disable : 4242 )
#pragma warning( disable : 4244 )

                               /* 6502 status flags:         */
#define	C_FLAG	  0x01         /* 1: Carry occured           */
#define	Z_FLAG	  0x02         /* 1: Result is zero          */
#define	I_FLAG	  0x04         /* 1: Interrupts disabled     */
#define	D_FLAG	  0x08         /* 1: Decimal mode            */
#define	B_FLAG	  0x10         /* Break [0 on stk after int] */
#define	R_FLAG	  0x20         /* Always 1                   */
#define	V_FLAG	  0x40         /* 1: Overflow occured        */
#define	N_FLAG	  0x80         /* 1: Result is negative      */

typedef struct
{
  unsigned int  IPeriod;
  int  ICount; /* Set IPeriod to number of CPU cycles */
                      /* between calls to Loop6502()         */
  pair PC;
  byte A,P,X,Y,S;     /* CPU registers and program counter   */
  int ICounta;
  byte HPeriod;
} M6502;

void Reset6502(void);
void Run6502(void);
void Loop6502(void);
extern unsigned char hblank;
extern unsigned long timestamp;
void TriggerIRQSync(void);
void TriggerIRQ(void);
void TriggerNMI(void);
void TriggerNMINSF(void);

#define NTSC_CPU 1789772.7272727272727272
#define PAL_CPU  1773447.2
//#define PAL_CPU (long)((long double)((long double)4433618.75*6)/(long double)15)
//#define PAL_CPU 1232400
extern byte IRQlow;
extern M6502 M;


#define __M6502H
#endif
