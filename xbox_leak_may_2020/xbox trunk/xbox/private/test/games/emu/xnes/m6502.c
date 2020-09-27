/** M6502: portable 6502 emulator ****************************/
/**                                                         **/
/**                         M6502.c                         **/
/**                                                         **/
/** This file contains implementation for 6502 CPU.         **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996                      **/
/**               Alex Krasivsky  1996                      **/
/** Modified      BERO            1998                      **/
/** Modified	  Xodnizel	  2000			    **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

#include <string.h>
#include <stdio.h>
#include "m6502.h"
#include "tables.h"
#include "fce.h"
#include "svga.h"
#include "sound.h"

M6502 M;

extern int toevent;
extern int PPUEvent;
/** INLINE ***************************************************/
/** Different compilers inline C functions differently.     **/
/*************************************************************/
#define INLINE static __inline

static byte BadOPList[8192]; 
unsigned long timestamp=0;

/** System-Dependent Stuff ***********************************/
/** This is system-dependent code put here to speed things  **/
/** up. It has to stay inlined to be fast.                  **/
/*************************************************************/

static __inline byte Op6502(register unsigned A) {
	return(Page[A>>13][A]);
	//return ARead[A](A);
}

//#define Op6502 ARead[A
int toga;
byte hblank=1;

#define Iish     (Page[(_PC_)>>13][(_PC_)])
#define	RdRAM(A)	RAM[A]
#define WrRAM(A,V)      {RAM[A]=V;}
#define	AdrRAM(A)	&RAM[A]


/** FAST_RDOP ************************************************/
/** With this #define not present, Rd6502() should perform  **/
/** the functions of Rd6502().                              **/
/*************************************************************/

#define Rd6502(A) ARead[A](A)
#define Wr6502(A,V) BWrite[A](A,V)

#define	C_SFT	0
#define	Z_SFT	1
#define	I_SFT	2
#define	D_SFT	3
#define	B_SFT	4
#define	R_SFT	5
#define	V_SFT	6
#define	N_SFT	7


#define _A      M.A
#define _P      M.P
#define _X      M.X
#define _Y      M.Y
#define _S      M.S


#define _PC     	M.PC
#define _PC_    	M.PC.W
#define _IPeriod        M.IPeriod
#define _ICounta	M.ICounta
#define _ICount         M.ICount
#define _IPeriod        M.IPeriod
#define _IBackup        M.IBackup
#define _User   	M.User

#define	ZP	0
#define	SP	0x100

/** Addressing Methods ***************************************/
/** These macros calculate and return effective addresses.  **/
/*************************************************************/
#define MCZp()	(Op6502(_PC_++))
#define MCZx()	(byte)(Op6502(_PC_++)+_X)
#define MCZy()	(byte)(Op6502(_PC_++)+_Y)

#define MC_Ab(Rg)	M_LDWORD(Rg)
#define MC_Zp(Rg)	Rg.B.l=Op6502(_PC_);_PC_++;Rg.B.h=0
#define MC_Zx(Rg)       Rg.B.l=Op6502(_PC_)+_X;_PC_++;Rg.B.h=0
#define MC_Zy(Rg)       Rg.B.l=Op6502(_PC_)+_Y;_PC_++;Rg.B.h=0
#define MC_Ax(Rg)	M_LDWORD(Rg);Rg.W+=_X
#define MC_Ay(Rg)	M_LDWORD(Rg);Rg.W+=_Y
#define MC_Ix(Rg)       K.B.l=Op6502(_PC_)+_X;_PC_++;K.B.h=0;Rg.B.l=RdRAM(K.W);Rg.B.h=RdRAM(++K.B.l);
#define MC_Iy(Rg)       K.B.l=Op6502(_PC_);_PC_++;K.B.h=0;Rg.B.l=RdRAM(K.W);Rg.B.h=RdRAM(++K.B.l);Rg.W+=_Y;

#define MC_IyA(Rg)      K.B.l=Op6502(_PC_);_PC_++;K.B.h=0;Rg.B.l=RdRAM(K.W);Rg.B.h=RdRAM(K.W+1);if((Rg.B.l+_Y)>0xFF) _ICounta+=1;Rg.W+=_Y

#define MC_AxA(Rg) M_LDWORD(Rg);K.B.h=Rg.B.h;Rg.W+=_X;_ICounta+=((K.B.h&1)^(Rg.B.h&1))
#define MC_AyA(Rg) M_LDWORD(Rg);K.B.h=Rg.B.h;Rg.W+=_Y;_ICounta+=((K.B.h&1)^(Rg.B.h&1))

/** Reading From Memory **************************************/
/** These macros calculate address and read from it.        **/
/*************************************************************/
#define MR_Ab(Rg)	MC_Ab(J);Rg=Rd6502(J.W)
#define MR_Im(Rg)	Rg=Op6502(_PC_++)
#define	MR_Zp(Rg)	Rg=RdRAM(MCZp())
#define MR_Zx(Rg)	Rg=RdRAM(MCZx())
#define MR_Zy(Rg)	Rg=RdRAM(MCZy())

#define	MR_Ax(Rg)	MC_AxA(J);Rg=Rd6502(J.W)	// Modded to add 1
#define MR_Ay(Rg)	MC_AyA(J);Rg=Rd6502(J.W)	// if p bound crossed

#define MR_Ix(Rg)	MC_Ix(J);Rg=Rd6502(J.W)
#define MR_Iy(Rg)	MC_IyA(J);Rg=Rd6502(J.W)

/** Writing To Memory ****************************************/
/** These macros calculate address and write to it.         **/
/*************************************************************/
#define MW_Ab(Rg)	MC_Ab(J);Wr6502(J.W,Rg)
#define MW_Zp(Rg)	WrRAM(MCZp(),Rg)
#define MW_Zx(Rg)	WrRAM(MCZx(),Rg)
#define MW_Zy(Rg)	WrRAM(MCZy(),Rg)
#define MW_Ax(Rg)	MC_Ax(J);Wr6502(J.W,Rg)
#define MW_Ay(Rg)	MC_Ay(J);Wr6502(J.W,Rg)
#define MW_Ix(Rg)	MC_Ix(J);Wr6502(J.W,Rg)
#define MW_Iy(Rg)	MC_Iy(J);Wr6502(J.W,Rg)

/** Modifying Memory *****************************************/
/** These macros calculate address and modify it.           **/
/**						            **/
/** These macros have been modified to emulate RMW ops      **/
/** a little better(ly).				    **/
/**  -Xodnizel						    **/
/*************************************************************/
#define MM_Ab(Cmd)	MC_Ab(J);I=Rd6502(J.W);Wr6502(J.W,I);Cmd(I);Wr6502(J.W,I)
#define MM_Zp(Cmd)	{unsigned A=MCZp();I=RdRAM(A);Cmd(I);WrRAM(A,I); }
#define MM_Zx(Cmd)	{unsigned A=MCZx();I=RdRAM(A);Cmd(I);WrRAM(A,I); }
#define MM_Ax(Cmd)	MC_Ax(J);I=Rd6502(J.W);Wr6502(J.W,I);Cmd(I);Wr6502(J.W,I)

/** Other Macros *********************************************/
/** Calculating flags, stack, jumps, arithmetics, etc.      **/
/*************************************************************/
#define M_FL(Rg)        _P=(_P&~(Z_FLAG|N_FLAG))|ZNTable[Rg]
#define M_LDWORD(Rg)    Rg.B.l=Op6502(_PC_);_PC_++;Rg.B.h=Op6502(_PC_);_PC_++
#define M_PUSH(Rg)	WrRAM(SP+_S,Rg);_S--
#define M_POP(Rg)	_S++;Rg=RdRAM(SP+_S)

#define M_JR            _ICounta+=1;K.W=M.PC.W;	K.W+=1;	_PC_+=(offset)Op6502(_PC_)+1;_ICounta+=(K.B.h&1)^(M.PC.B.h&1);


#define M_ADC(Rg) K.W=_A+Rg+(_P&C_FLAG);_P&=~(N_FLAG|V_FLAG|Z_FLAG|C_FLAG);_P|=((~(_A^Rg)&(_A^K.B.l)&0x80)?V_FLAG:0)|(K.B.h? C_FLAG:0)|ZNTable[K.B.l];_A=K.B.l;


/* Warning! C_FLAG is inverted before SBC and after it */
#define M_SBC(Rg) K.W=_A-Rg-(~_P&C_FLAG); _P&=~(N_FLAG|V_FLAG|Z_FLAG|C_FLAG); _P|=(((_A^Rg)&(_A^K.B.l)&0x80)?V_FLAG:0)|(K.B.h? 0:C_FLAG)|ZNTable[K.B.l]; _A=K.B.l; 


#define M_CMP(Rg1,Rg2) K.W=Rg1-Rg2;_P&=~(N_FLAG|Z_FLAG|C_FLAG);_P|=ZNTable[K.B.l]|(K.B.h? 0:C_FLAG)
#define M_BIT(Rg) _P&=~(N_FLAG|V_FLAG|Z_FLAG);_P|=(Rg&(N_FLAG|V_FLAG))|(Rg&_A? 0:Z_FLAG)

#define M_AND(Rg)	_A&=Rg;M_FL(_A)
#define M_ORA(Rg)	_A|=Rg;M_FL(_A)
#define M_EOR(Rg)	_A^=Rg;M_FL(_A)
#define M_INC(Rg)	Rg++;M_FL(Rg)
#define M_DEC(Rg)	Rg--;M_FL(Rg)

#define M_ASL(Rg)	_P&=~C_FLAG;_P|=Rg>>7;Rg<<=1;M_FL(Rg)
#define M_LSR(Rg)	_P&=~C_FLAG;_P|=Rg&C_FLAG;Rg>>=1;M_FL(Rg)
#define M_ROL(Rg)	K.B.l=(Rg<<1)|(_P&C_FLAG);_P&=~C_FLAG;_P|=Rg>>7;Rg=K.B.l; M_FL(Rg)
#define M_ROR(Rg)       K.B.l=(Rg>>1)|(_P<<7);_P&=~C_FLAG;_P|=Rg&C_FLAG;Rg=K.B.l; M_FL(Rg)

/** Reset6502() **********************************************/
/** This function can be used to reset the registers before **/
/** starting execution with Run6502(). It sets registers to **/
/** their initial values.                                   **/
/*************************************************************/
void Reset6502(void)
{
	_A=_X=_Y=0x00;
	_P=Z_FLAG|R_FLAG|I_FLAG;
	_S=0xFF;
	_PC.B.l=Op6502(0xFFFC);
	_PC.B.h=Op6502(0xFFFD);   

	if(MapperNo==666) _PC_=0x2012;
	_ICount=_IPeriod;
	_ICounta=0;
	memset(BadOPList,0x00,8192);
}

/*	This is a hack to reduce screen jumpiness with certain games */
void TriggerIRQSync(void)
{
	
	if(!(_P&I_FLAG))
	{
		M.ICount=0;
		_ICounta+=7;
		M_PUSH(_PC.B.h);
		M_PUSH(_PC.B.l);
		M_PUSH(_P&~B_FLAG);
		_P&=~D_FLAG;
		_P|=I_FLAG;
#ifdef LSB_FIRST
		_PC_=*(word *)(Page[0xFFFE>>13]+0xFFFE);
#else
		_PC.B.l=Op6502(0xFFFE);_PC.B.h=Op6502(0xFFFF);
#endif
	}
}

void TriggerIRQ(void)
{
	if(!(_P&I_FLAG))
	{
		_ICounta+=7;M_PUSH(_PC.B.h);M_PUSH(_PC.B.l);M_PUSH(_P&~B_FLAG);
		_P&=~D_FLAG;
		_P|=I_FLAG;
#ifdef LSB_FIRST
		_PC_=*(word *)(Page[0xFFFE>>13]+0xFFFE);
#else
		_PC.B.l=Op6502(0xFFFE);_PC.B.h=Op6502(0xFFFF);
#endif
	}
}

void TriggerNMINSF(void)
{
	_ICounta+=7;
	M_PUSH(_PC.B.h);
	M_PUSH(_PC.B.l);
	M_PUSH(_P&~B_FLAG);
	_P&=~D_FLAG;
	_PC_=0x2000;
}

void TriggerNMI(void)
{
	_ICounta+=7;
	M_PUSH(_PC.B.h);
	M_PUSH(_PC.B.l);
	M_PUSH(_P&~B_FLAG);
	_P&=~D_FLAG;
#ifdef LSB_FIRST
	_PC_=*(word *)(Page[0xFFFA>>13]+0xFFFA);
#else
	_PC.B.l=Op6502(0xFFFA);
	_PC.B.h=Op6502(0xFFFB);
#endif
}


/** Run6502() ************************************************/
/** This function will run 6502 code.                       **/
/*************************************************************/
void (*MapIRQHook)(int a)=0;
byte IRQlow=0;
void Run6502(void)
{
	register pair J,K;
	register byte I;
	int32 temp;

M6502Loop:
    I=Op6502(_PC_);
    _PC_++;
	
    switch(I)
    {
#include "codes.h"
    }
    temp=(_ICounta<<1)+_ICounta;
    _ICount-=temp;
    fhcnt-=temp;
    if(fhcnt<=0)
	{
		FrameSoundUpdate();
		fhcnt+=7457*3;
	}
    if(MapIRQHook) MapIRQHook(_ICounta);
	
    if(toevent>-1) 
    {
		toevent-=temp; 
		if(toevent<=0) 
		{
			switch(PPUEvent)
			{
			case 0x40: PPU[2]|=PPUEvent;toevent=-1;break;
			case 0: PPU[2]|=0x80;toevent+=25;PPUEvent=1;break;
			case 1: if(VBlankON) TriggerNMI();toevent=-1;break;
			}
		}
    }
    timestamp+=_ICounta;
    _ICounta=0;
    if(_ICount<=0)
    {
doagain:
	if(hblank&1)
	{
		hcheck();
		_ICount+=M.HPeriod;
	}
	else
	{
		Loop6502();
		_ICount+=_IPeriod;
	}
	hblank^=1;
	if(_ICount<=0) goto doagain;
    }
	if(!IRQlow)
		goto M6502Loop;
	TriggerIRQ();
	goto M6502Loop;
}

