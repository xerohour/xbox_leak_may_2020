/*++
Copyright (c) 2001 Microsoft Corporation

Module Name:	vshgen.cpp
Abstract:		generates random vertex shaders
Author:			Jason Gould (jgould) 07-Aug-2001
Environment:	xbox or possibly win2k static library, for testing vertex shaders
Notes:			used by satest, and various other vertex shader tests
Revision History:

	07-Aug-2001 jgould
		Initial Version

--*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#include "d3d8-xbox.h"
#endif

#include "xgraphics.h"

#include "vshgen.h"

#define countof(a) (sizeof(a) / sizeof((a)[0]))

#define NUMOUTREGS 13
#define NUMINREGS  16


struct SHADERGENERATORSTUFF {
	bool bWriteConsts;
	bool bIsXvs;
	bool bAllowIndex;
	bool bAllowPair;
	bool bAllowNop;
	bool bHasWrittenReg[13][4];
	bool bHasWrittenArl;
	bool bHasWrittenConst;
	bool bMustWriteConst;
	bool bMustWriteOpos;
	bool bStateShader;

	char temp[100]; //temporary input buffer: has enough room for "mad c[-96].xyzw, -c[a0.x+-96].xyzw, -c[a0.x+-96].xyzw, -c[a0.x+-96].xyzw"
};


	typedef struct _D3DVsInstruction {
		unsigned long eos:1;        /* 0:00 last instruction */
		unsigned long cin:1;        /* 0:01 ctx indexed address */
		unsigned long  om:1;        /* 0:02 output mux */
		unsigned long  oc:9;        /* 0:03 output write control */
		unsigned long owm:4;        /* 0:12 output write mask */
		unsigned long swm:4;        /* 0:16 secondary register write mask */
		unsigned long  rw:4;        /* 0:20 register write */
		unsigned long rwm:4;        /* 0:24 primary register write mask */
		unsigned long cmx:2;        /* 0:28 c mux (NA,r1,v,c) */
		unsigned long crr:4;        /* 0:30 c register read */
		unsigned long cws:2;        /* 1:02 c w swizzle */
		unsigned long czs:2;        /* 1:04 c z swizzle */
		unsigned long cys:2;        /* 1:06 c y swizzle */
		unsigned long cxs:2;        /* 1:08 c x swizzle */
		unsigned long cne:1;        /* 1:10 c negate */
		unsigned long bmx:2;        /* 1:11 b mux (NA,r1,v,c) */
		unsigned long brr:4;        /* 1:13 b register read */
		unsigned long bws:2;        /* 1:17 b w swizzle */
		unsigned long bzs:2;        /* 1:19 b z swizzle */
		unsigned long bys:2;        /* 1:21 b y swizzle */
		unsigned long bxs:2;        /* 1:23 b x swizzle */
		unsigned long bne:1;        /* 1:25 b negate */
		unsigned long amx:2;        /* 1:26 a mux (NA,r0,v,c) */
		unsigned long arr:4;        /* 1:28 a register read */
		unsigned long aws:2;        /* 2:00 a w swizzle */
		unsigned long azs:2;        /* 2:02 a z swizzle */
		unsigned long ays:2;        /* 2:04 a y swizzle */
		unsigned long axs:2;        /* 2:06 a x swizzle */
		unsigned long ane:1;        /* 2:08 a negate */
		unsigned long  va:4;        /* 2:09 ibuffer address */
		unsigned long  ca:8;        /* 2:13 ctx address */
		unsigned long mac:4;        /* 2:21 MLU/ALU op */
		unsigned long ilu:3;        /* 2:25 ILU op */
	} D3DVsInstruction;

	namespace XGRAPHICS 
{
	//internal instruction format. 
	//It isn't too pretty to work with, since things are not byte aligned,
	//but there's already a disassembler that uses this format. No need to duplicate that...
	//and it's easy to convert, since this instruction generator uses this hardware format for
	//generating instructions anyway.


	//xgraphics' built-in instruction-disassembler. Used in xsdasm and the .lst generator
	HRESULT  DisassembleInstruction( const D3DVsInstruction * pI, XBuffer &buffer );

};


//returns true if the particular mac instruction produces standard output
//(false for nop and arl)
bool MacOutputs(int mac) 
{
	//return false if mac is nop or arl (mov to a0.x).
	if((mac == 0) || (mac == 13)) {
		return false;
	} else {
		return true;
	}
}

//initialize variables that are used all over the place
HRESULT InitShaderStuff(
	SHADERGENERATORSTUFF* pStuff,
	bool bWriteConsts,
	bool bIsXvs,
	bool bAllowIndex,
	bool bAllowPair,
	bool bAllowNop
)
{
	HRESULT hr = S_OK;
	if(!pStuff) {
		return E_INVALIDARG;
	}

	pStuff->bWriteConsts     = bWriteConsts;
	pStuff->bIsXvs           = bIsXvs;
	pStuff->bAllowIndex      = bAllowIndex;
	pStuff->bAllowPair       = bAllowPair;
	pStuff->bAllowNop        = bAllowNop;

	pStuff->bMustWriteOpos   = true;
	pStuff->bMustWriteConst  = false;
	pStuff->bHasWrittenConst = false;
	pStuff->bHasWrittenArl   = false;

	memset(pStuff->bHasWrittenReg,0,sizeof(pStuff->bHasWrittenReg));
	pStuff->temp[0] = 0;
	return hr;
}


//Generate header info for the new shader
//Used only by GenerateRandomVertexShader
HRESULT DoHeader (XBuffer* pbuf, DWORD dwType, bool bScreenspace, SHADERGENERATORSTUFF* pStuff) 
{
	HRESULT hr = S_OK;

	switch (dwType) {
	case 0x7378:
	case SASMT_VERTEXSTATESHADER:
		hr = pbuf->Printf("xvss.1.1\n");
		pStuff->bStateShader = true;
		pStuff->bMustWriteConst = true;
		pStuff->bMustWriteOpos = false;
		pStuff->bWriteConsts = true;
		break;

	case 0x7778:
	case SASMT_READWRITE_VERTEXSHADER:
		hr = pbuf->Printf("xvsw.1.1\n");
		pStuff->bStateShader = false;
		pStuff->bMustWriteConst = false;
		pStuff->bMustWriteOpos = true;
		//only write consts if we are told to earlier
		pStuff->bWriteConsts &= true; 
		break;

	case 0x2078:
	case SASMT_VERTEXSHADER:
		hr = pbuf->Printf("xvs.1.1\n");
		pStuff->bStateShader = false;
		pStuff->bMustWriteConst = false;
		pStuff->bMustWriteOpos = true;
		pStuff->bWriteConsts = false;
		break;
	default:
		return E_INVALIDARG;
	}

	if(FAILED(hr)) {
		return hr;
	}

	if(bScreenspace) {
		hr = pbuf->Printf("#pragma screenspace\n");
	}

	return hr;
}

//choose mac and ilu instructions to work with
void ChooseInstructions(SHADERGENERATORSTUFF* pStuff, int* pMac, int* pIlu, bool bIsLastInstruction) 
{
chooseinstructions:
	*pIlu = (rand() % 8);
	*pMac = (rand() % 14);
	if((!pStuff->bAllowIndex) && (*pMac == 13)) goto chooseinstructions; //13 == write to a0.x
	if((!pStuff->bIsXvs) && (*pMac == 12)) goto chooseinstructions; //12 == dph, == xbox extension
	if((!pStuff->bIsXvs) && (*pIlu == 2)) goto chooseinstructions; //2 == rcc, == xbox extension
	if((!pStuff->bAllowNop) && (*pMac == 0) && (*pIlu == 0)) goto chooseinstructions; //0 == nop
	//if we're about to do a nop, but requirements haven't been met, try again.
	if(bIsLastInstruction && ((pStuff->bMustWriteConst && !pStuff->bHasWrittenConst) || (pStuff->bMustWriteOpos && (!pStuff->bHasWrittenReg[12][0] || !pStuff->bHasWrittenReg[12][1])))) {
		if(!MacOutputs(*pMac) && (*pIlu == 0)) {
			goto chooseinstructions;
		}
	}
	if(!pStuff->bAllowPair && *pIlu && *pMac) goto chooseinstructions; //can't pair == something has to nop!
}

//select "indexed" or "non-indexed" mode for this instruction
void ChooseArl(SHADERGENERATORSTUFF* pStuff, bool* pArl) 
{
	//bias: use a0.x 1/6 of the time.
	if(pStuff->bAllowIndex && pStuff->bHasWrittenArl && (!(rand() % 6))) {
		*pArl = true;
	} else {
		*pArl = false;
	} 
}

//pick a constant within range. Make a few constants show up more often than others
void ChooseConstant(SHADERGENERATORSTUFF* pStuff, int * pCa)
{
	int ca;
	//if we can access all consts
	if(pStuff->bIsXvs) { 
		//only use 18 of the available constants, so we have some chance of using the same constant
		ca = rand() % 18;
		if(ca < 6) ca -= 96;               //[-96, -91]
		else if (ca < 12) ca -= 7;         //[-1,    4] 
		else if (ca < 17) ca += (95 - 16); //[91,   95]
		else ca = (rand() % 192) - 96;     //[-96,  95]
	} else {
		//only use 12 of the 96 available constants
		ca = (rand() % 12);                
		if(ca < 6) ca += 0;                //[0,   5]
		else if (ca < 11) ca += (95 - 10); //[91, 95]
		else ca = (rand() % 96);           //[0,  95]
	}
	*pCa = ca + 96; //offset by 96, since that's what the hardware does
}

//choose input register (v0, v1, etc)
void ChooseVa(SHADERGENERATORSTUFF* pStuff, int * pVa)
{
	if(pStuff->bStateShader) {
		*pVa = 0;
	} else {
		*pVa = RNDE(0,NUMINREGS);
	}
}

//choose swizzling and negation of the three input pipes
void ChooseSwizzleNe(
	SHADERGENERATORSTUFF* pStuff,
	bool* pAne, bool* pBne, bool* pCne,
	int * pAs , int * pBs , int * pCs 
	)
{
	//create an array of the pointers, so we don't repeat code
	bool* ne[] = {pAne, pBne, pCne}; 
	int * sw[] = {pAs,  pBs,  pCs };

	int i, j;

	//repeat for a, b, c
	for(j = 0; j < 3; j++) { 
		//half the time, use standard swizzles ( ex: r1.xyzw )
		if(rand() & 1) {
			for (i = 0; i < 4; i++) {
				sw[j][i] = i;
			}
		//the other half, use random swizzles ( ex: r1.yyzx )
		} else {
			for (i = 0; i < 4; i++) {
				sw[j][i] = rand() & 3;
			}
		}

		//2/3 of the time, use non-negated source ( ex: r1)
		if(rand() & 3) {
			*ne[j] = false; 
		//1/3 of the time, negate the source ( ex: -r1 )
		} else { 
			*ne[j] = true;
		}
	}
}

//choose r*, c*, or v* for each input pipe. 
//figure out which r* registers can be used here (which ones have been written already)
void ChooseInput(
	SHADERGENERATORSTUFF* pStuff,
	int * pAmx, int * pBmx, int * pCmx,
	int * pArr, int * pBrr, int * pCrr,
	const int* pAs, const int* pBs, const int* pCs
	)
{
	int * mx[] = {pAmx, pBmx, pCmx};
	int * rr[] = {pArr, pBrr, pCrr};
	const int* sw[] = {pAs , pBs , pCs };

	int j, i;

	//repeat for a, b, c
	for(j = 0; j < 3; j++) { 
ChooseMux:
		//weight the Mux to read from registers (1) more often than Input (2) or Const (3)
		*mx[j] = RND(1, 8);
		if(*mx[j] >= 4) {
			*mx[j] = 1;
		}

		if(*mx[j] == 1) {
			//choose random input register. If it's an xbox shader, we can use r12.
			*rr[j] = RND(0, (11 + pStuff->bIsXvs));
			for(i = 0; i < 4; i++) {
				//if the components of the register we want to read haven't been written, try again.
				if(!pStuff->bHasWrittenReg[ *rr[j] ][ sw[j][i] ]) {
					goto ChooseMux;
				}
			}
		}
	}
}

//generate an output mask. Select the whole register most of the time.
int GenerateRandomOutputMask() 
{
	//bias: write to the whole register 1/2 of the time, never write to none of the register
	int r = RND(-15,15);
	if (r < 1) {
		return 15;
	} else {
		return r;
	}
}

//choose whether to write to oPos, oD0, etc. (used by ChooseOutput, if output regs are to be written)
void ChooseOutputRegister(SHADERGENERATORSTUFF* pStuff, int* pOr)
{
chooseoutputregister:

	*pOr = RND(2, NUMOUTREGS - 1);
	if(*pOr == 2) {
		*pOr = 0;
	}

	//7 and 8 are oB0 and oB1, which are xbox-specific
	if((*pOr == 7 || *pOr == 8) && (pStuff->bIsXvs)) {
		goto chooseoutputregister;
	}
}

//do everything with output: which pipe writes to which registers? 
//also deal with pairing.
void ChooseOutput(
	SHADERGENERATORSTUFF* pStuff,
	bool* pOm,
	int * pRwm, int * pSwm, int * pOwm,
	int * pRw,  int * pOc,  int * pOr,
	int mac, int ilu,
	bool bIsLastInstruction
	)
{
	if(bIsLastInstruction) {
		if(pStuff->bMustWriteConst && !pStuff->bHasWrittenConst) {
			//we must write a constant. Write to defaults.
			*pRwm = 0; //don't write to r*
			*pSwm = 0; //don't write to r1
			*pOwm = 0xf; //write to output
			if(MacOutputs(mac)) { //we won't choose nop or arl on last instruction.
				*pOm = 0; //if we are using a mac instruction, read from mac
				if (ilu) {
					*pSwm = 0x1; //if ilu is also running, write to r1.x
				}
			} else {
				*pOm = 1; //if we aren't using a mac instruction, read from ilu
			}
			*pRw = 0; //dummy
			*pOc = 0; //write to constant, rather than output
			*pOr = 0; //write to c0.
			return;
		}

		if(pStuff->bMustWriteOpos && (!pStuff->bHasWrittenReg[12][0] || !pStuff->bHasWrittenReg[12][1])) {
			//we must write to oPos. don't bother pairing.
			*pRwm = 0; //don't write to r*
			*pSwm = 0; //don't write to r1
			*pOwm = 0xf; //write to output
			if(MacOutputs(mac)) {
				*pOm = 0; //if we are using a mac instruction, read from mac
				if (ilu) {
					*pSwm = 0x1; //if ilu is also running, write to r1.x
				}
			} else {
				*pOm = 1; //if we aren't using a mac instruction, read from ilu
			}
			*pRw = 0; //dummy
			*pOc = 1; //write to output, rather than constant
			*pOr = 0; //write to oPos (o0).
			return;
		}

		//if we're here, it's the last instruction, but all requriements have been met already. 
		//proceed as normal
	}

	//bAllowPair has already previously decided whether mac & ilu will run simultaneously.
	//so, the two things we do here are:
	//make sure that if the mac is doing something, the result goes one or two places
	//make sure that if the ilu is doing something, the result goes one or two places
	//make sure that they only go one place if pairs aren't allowed.
	//make sure that we only write to the output registers we're allowed to use
	//make sure that we set swm if both ilu and mac are going, and ilu isn't using output.
	//make sure that if ilu is going, rw isn't 1

	if(mac && ilu) {
		//pairing has to be enabled for us to be here...
		//five options: 
		//0 mac to reg, ilu to out
		//1 mac to reg, ilu to both
		//2 mac to reg, ilu to reg
		//3 mac to both, ilu to reg
		//4 mac to out, ilu to reg
		int choice = RND(0,4);
		if(mac == 13) choice = RND(0, 2);

		if(choice > 2) {
			*pOm = 0; //use mac for output
		} else {
			*pOm = 1;
		}

		if(choice < 4 && mac != 13) { //mac writes to reg
			*pRwm = GenerateRandomOutputMask();
		} else {
			*pRwm = 0;
		}

		if(choice > 0) { //ilu writes to reg
			*pSwm = GenerateRandomOutputMask();
		} else {
			*pSwm = 0;
		}

		if(choice != 2) { //something writes to output
			*pOwm = GenerateRandomOutputMask();
		} else {
			*pOwm = 0;
		}

		*pRw = RND(1, 11);
		if(*pRw == 1) { //mac can't write to 1 if Ilu is running
			*pRw = 0;
		}
	} else if (!mac && !ilu) { //nop

		*pRwm = 0; //don't write to r*
		*pSwm = 0; //don't write to r1
		*pOwm = 0; //write to output

		//dummy stuff
		*pOr  = 0; 
		*pRw  = 0; 
		*pOc  = 0;
		*pOm  = 0;

	} else {
		//Only mac, or only ilu.

		//The only difference: if it's mac, output mux goes to 0, otherwise, 1.
		//also, ilu always uses swm, never rwm.
		//Other stuff has already been taken care of earlier

		//two/three options: 
		//0 mac/ilu to reg
		//1 mac/ilu to output 
		//2 mac/ilu to both (only if pairing is allowed)
		int choice = RND(0,(1 + pStuff->bAllowPair)); //[0,1], or [0,2]

		if(mac) {
			*pOm = 0;
		} else {
			*pOm = 1;
		}


		if((choice != 1) && MacOutputs(mac)) {
			*pRwm = GenerateRandomOutputMask();
		} else {
			*pRwm = 0;
		}

		if((choice != 1) && ilu) {
			*pSwm = GenerateRandomOutputMask();
		} else {
			*pSwm = 0;
		}

		if(choice != 0 && mac != 13) {
			*pOwm = GenerateRandomOutputMask();
		} else {
			*pOwm = 0;
		}

		*pRw = RND(0, 11);
	} 


	if(pStuff->bStateShader) {
		*pOc = 0; //constant register always
	} else {
		if(pStuff->bWriteConsts) {
			*pOc = RND(0,1);
		} else {
			*pOc = 1; //it's a normal shader. Never write to c*
		}
	}

	if(*pOc == 0) { //constant
		ChooseConstant(pStuff, pOr);
	} else { //output register
		ChooseOutputRegister(pStuff, pOr);
	}
}

//now that output has been decided, update the array that says which
//registers have been written, so we can read from them later
// (and also update whether we've written to Arl, or a Const, or oPos)
void UpdateWrittenMasks (
	SHADERGENERATORSTUFF* pStuff,
	bool Om,
	int  Rwm, int  Swm, int  Owm,
	int  Rw,  int  Oc,  int  Or,
	int  mac,  int  ilu
	)
{
	int i;
	//update rw
	for(i = 0; i < 4; i++) {
		pStuff->bHasWrittenReg[Rw][3-i] |= !!(Rwm & (1 << i));
	}

	//if mac is active, ilu writes to r1, not rw... update as appropriate
	for(i = 0; i < 4; i++) {
		pStuff->bHasWrittenReg[mac ? 1 : Rw][3-i] |= !!(Swm & (1 << i));
	}

	//if we are writing to oPos... it maps to r12.
	if(Oc == 1 && Or == 0) {
		for(i = 0; i < 4; i++) {
			pStuff->bHasWrittenReg[12][3-i] |= !!(Owm & (1 << i));
		}
	}

	if(Oc == false && Owm != 0) {
		pStuff->bHasWrittenConst = true;
	}

	if(mac == 13) { //arl
		pStuff->bHasWrittenArl = true;
	}
}

//convert the 50 variables into the xgraphics vertex-shader instruction format,
//and have xgraphics print out the results
void PrintInstruction(
  	XBuffer* pbuf,
	int Ca, int Va, int Arr, int Brr, int Crr,
	bool Arl,
	int Amx, int Bmx, int Cmx,
	bool Ane, bool Bne, bool Cne,
	int* pAs, int* pBs, int* pCs,
	
  	bool Om,
	int  Rwm, int  Swm, int  Owm,
	int  Rw,  int  Oc,  int  Or,
	int  Mac,  int  Ilu
	)
{
	D3DVsInstruction ins;
	ins.amx = Amx;
	ins.ane = Ane;
	ins.arr = Arr;
	ins.aws = pAs[3];
	ins.axs = pAs[0];
	ins.ays = pAs[1];
	ins.azs = pAs[2];
	ins.bmx = Bmx;
	ins.bne = Bne;
	ins.brr = Brr;
	ins.bws = pBs[3];
	ins.bxs = pBs[0];
	ins.bys = pBs[1];
	ins.bzs = pBs[2];
	ins.ca  = Ca;
	ins.cin = Arl;
	ins.cmx = Cmx;
	ins.cne = Cne;
	ins.crr = Crr;
	ins.cws = pCs[3];
	ins.cxs = pCs[0];
	ins.cys = pCs[1];
	ins.czs = pCs[2];
	ins.eos = 0; 
	ins.ilu = Ilu;
	ins.mac = Mac;
	ins.oc  = (Oc << 8) + Or;
	ins.om  = Om;
	ins.owm = Owm;
	ins.rw  = Rw;
	ins.rwm = Rwm;
	ins.swm = Swm;
	ins.va  = Va;

	XGRAPHICS::DisassembleInstruction(&ins, *pbuf);
	pbuf->Append("\n");
}

//produce and print one instruction
void DoInstruction (
	SHADERGENERATORSTUFF* pStuff,
	XBuffer* pbuf,
	bool bIsLastInstruction
)
{
	int ca, va, arr, brr, crr; //c#, v#, (a)r#, (b)r#, (c)r#
	bool arl;                  //true to use indexed input
	int amx, bmx, cmx;         //input muxes (1 for r#, 2 for v#, 3 for c#)
	bool ane, bne, cne;		   //negation
	int as[4], bs[4], cs[4];   //swizzle
	bool om;                   //output mux: 0 to read from mac, 1 from ilu
	int rwm, swm, owm;         //register mask, secondary mask, output mask
	int rw, oc;                //output r#, constant / output register
	int or;                    //constant / output register # 
	int mac, ilu;              //mac & ilu instruction

	ChooseInstructions(pStuff, &mac, &ilu, bIsLastInstruction);

	ChooseArl(pStuff, &arl);
	ChooseConstant(pStuff, &ca);
	ChooseVa(pStuff, &va);
	ChooseSwizzleNe(pStuff, &ane, &bne, &cne, as, bs, cs);
	ChooseInput(pStuff, &amx, &bmx, &cmx, &arr, &brr, &crr, as, bs, cs);

	//do all output stuff (including pairing, masks, registers)
	ChooseOutput(pStuff, &om, &rwm, &swm, &owm, &rw, &oc, &or, mac, ilu, bIsLastInstruction);

	UpdateWrittenMasks(pStuff, om, rwm, swm, owm, rw, oc, or, mac, ilu);

	PrintInstruction(pbuf, ca,va,arr,brr,crr,arl,amx,bmx,cmx,ane,bne,cne,as,bs,cs,om,rwm,swm,owm,rw,oc,or,mac,ilu);

}



HRESULT GenerateRandomVertexShader(
	int minlength, 
	int maxlength, 
	DWORD dwNopsAtBeginning,
	DWORD dwType, 
	bool bWriteConsts,
	bool bIsXvs,
	bool bAllowIndex,
	bool bAllowPair,
	bool bScreenspace,
	bool bAllowNop,
	XBuffer** ppOutput
)	
{
	HRESULT hr = S_OK;
	DWORD i;
	
	if(!ppOutput)
		return E_INVALIDARG;
	if(minlength > maxlength)
		return E_INVALIDARG;
	if(maxlength <= 0)
		return E_INVALIDARG;

	*ppOutput = new XBuffer;
	XBuffer* pbuf = *ppOutput;

	if(!pbuf) {
		return E_OUTOFMEMORY;
	}

	hr = pbuf->Initialize(1000);
	if(FAILED(hr)) {
		return hr;
	}

	SHADERGENERATORSTUFF Stuff;
	hr = InitShaderStuff(&Stuff, bWriteConsts, bIsXvs, bAllowIndex, bAllowPair, bAllowNop);
	if(FAILED(hr)) {
		return hr;
	}

	hr = DoHeader(pbuf, dwType, bScreenspace, &Stuff);
	if(FAILED(hr)) {
		return hr;
	}

	for(i = 0; i < dwNopsAtBeginning; i++) {
		hr = pbuf->Printf("nop\n");
		if(FAILED(hr)) {
			return hr;
		}
	}

	DWORD NumInstructions = RND(minlength, maxlength);
	
	//note: go to  < NumInstructions-1. If needed, the 
	//last instruction can be used to force a write to oPos or c0.
	for( ; i < NumInstructions - 1; i++) {
		DoInstruction(&Stuff, pbuf, false);
	}
	DoInstruction(&Stuff, pbuf, true);

	return hr;
}




/*
The smallest shader that still repro's (the test returns ltrTestFailed) will be returned in 
  a new buffer, which will be pointed to by *ppOutput.

Return: 
	if a parameter is NULL (other than pData), return E_POINTER
	if it runs out of memory, it returns E_OUTOFMEMORY
	if the original shader isn't valid, return E_INVALIDARG
	if the original shader doesn't repro (if the test returns ltrTestPassed), it returns S_FALSE.
	if it successfully shrinks the shader, it returns S_OK
	if something internally bad happens, it returns S_UNEXPECTED


Algorithm: 
0	start with the last line
1   if we're at the beginning of the shader, end
2   go to the beginning of the current line
3   try commenting it out.
4   run the test. 
5	If it fails, then
6        move to the next line up
7        goto 1
8	if it didn't fail
9	    try commenting out all the other lines above it, one at a time (only if bNSquared is true)
10		if those cause the test to fail (reproduce the problem), then
11		    leave both lines commented
12			go to next line up
13			goto 1
14		else 
15		    uncomment everything that has been commented since our last visit to (1)
16			move to next line up
17			goto 1
*/
HRESULT LipoVsh(
	XBuffer*  pInput,    //input buffer (constant)
	XBuffer** ppOutput,  //pointer to receive smallest output
	LIPOTEST pfnTest,   //pointer to function to test the lipo'd shader
	void*    pData,     //pointer to data to pass to the test function
	bool     bNSquared, //true to try removing 2 lines at a time
	bool     bTryAgain  //true to keep trying to repeat if it succeeds the first time
	)
{
	HRESULT hr = S_OK;
	XBuffer* pbuf;
	LIPOTESTRETURN ret;

	if(!ppOutput || !pInput || !pfnTest) {
		return E_POINTER;
	}

	*ppOutput = new XBuffer;
	pbuf = *ppOutput;
	if(!pbuf) {
		return E_OUTOFMEMORY;
	}

	hr = pbuf->Append(*pInput);
	if(FAILED(hr)) {
		return hr;
	}

	ret = pfnTest(pbuf, pData);
	switch(ret) {
	case ltrTestFailed:
		//this is good. We shrink the input now...
		break;

	case ltrTestPassed:
		//the original didn't repro whatever bug there was, so there's nothing we can do.
		return S_FALSE;

	case ltrInvalid:
		//the original isn't a valid shader, or whatever, so just return with the error.
		return E_INVALIDARG;
	}

	//initial test failed, commence liposuction!
	char* pTest = pbuf->GetText();
	char b, b2;
	bool bHasShrunk = false;

	do {
		for(int i = pbuf->GetUsed() - 1; i >= 0; i--) {
			//look for new-line
			if(pTest[i] == '\n') {
				//save the char after the newline
				b = pTest[i + 1];
				//if it's the last char in the buf, or it has already been commented out, ignore it
				if(b == '\0' || b == ';') {
					continue;
				}

				pTest[i + 1] = ';'; //comment out this line

				//test it...
				ret = pfnTest(pbuf, pData);

				switch (ret) {
				case ltrTestFailed:
					//The code still repros. Leave this line commented out, and continue shrinking
					bHasShrunk = true;
					continue;

				case ltrTestPassed:
				case ltrInvalid:
					if (!bNSquared) {
						//we broke something, or it no longer reproduces, and we're not 
						//going to try to comment out 2 lines at a time, so put the line 
						// back the way it was, and try the next one. 
						pTest[i + 1] = b;
						continue;

					} else {
						//try to do 2 lines at once...
						bool bSecondTimeWorked = false;

						for(int j = i-1; j >= 0; j--) {
							//look for new-line
							if(pTest[j] == '\n') {
								//save the char after the newline
								b2 = pTest[j + 1];
								//if it's the last char in the buf, or it has already been commented out, ignore it
								if(b2 == '\0' || b2 == ';') {
									continue;
								}

								pTest[j + 1] = ';'; //comment out this line

								//test it...
								ret = pfnTest(pbuf, pData);

								switch (ret) {
								case ltrTestFailed:
									//The code still repros. Leave this line commented out, and continue shrinking
									bHasShrunk = true;
									bSecondTimeWorked = true;
									continue;

								case ltrTestPassed:
								case ltrInvalid:
									//we broke something, or it no longer reproduces, so put the line 
									// back the way it was, and try the next one. 
									pTest[j + 1] = b2;
									continue;
								}
							}
						}
						if(!bSecondTimeWorked) {
							//commenting out the first line causes the thing to either
							//totally break or not repro again. Put it back!
							pTest[i + 1] = b;
							continue;
						} else {
							//commenting out the first line plus something else causes it
							//to reproduce... leave them both commented.
						}
					}
				}
			}
		}
	} while (bHasShrunk && bTryAgain);

	return hr;
}
	

