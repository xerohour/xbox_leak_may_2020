///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) Microsoft Corporation.  All Rights Reserved.
//
//  File:       api.cpp
//  Content:    The public api functions for the shadeasm library
//					Includes most vertex shader code
//
///////////////////////////////////////////////////////////////////////////
#include "pchshadeasm.h"

namespace XGRAPHICS
{

// Local Debug flags
#ifdef DBG

//#define DBG_REORDERER
//#define DBG_STRIPPER
//#define DBG_PRINT_PRE_OPTIMIZED
//#define DBG_PRINT_OPTIMIZED
//#define DBG_PAIRER
//#define DBG_RENAMER
//#define DBG_VERIFIER
//#define DBG_SPLICER

//#define VERIFY_EACH_STEP // Slow, but helps debug optimizer bugs

//#define DISABLE_STRIPPER
//#define DISABLE_RENAMER
//#define DISABLE_REORDERER
//#define DISABLE_PEEPHOLE
//#define DISABLE_PAIRER1
//#define DISABLE_PAIRER2
//#define DISABLE_VERIFIER

// Fine grain control of Reorderer
//#define DISABLE_MOVE_AND_PAIR
//#define DISABLE_MOVES_TOWARDS_BEGINNING
//#define DISABLE_MOVES_TOWARDS_BEGINNING_AFTER_MOVE_NUMBER 7
//#define DISABLE_PAIRS_AFTER_PAIR_NUMBER 7

#endif

#define DISABLE_TREE_PRINT 

// To keep the Xbox non-debug runtime small, we don't include the verifier or the validator.

#if defined(XBOX) && !defined(DBG)
#define DISABLE_VERIFIER
#define DISABLE_VALIDATOR
#endif



HRESULT D3DTokensToUCode (const DWORD* pTokens, D3DVertexShaderProgram* pProg, XD3DXErrorLog* pErrorLog);


#ifndef DISABLE_VERIFIER
bool Verify(class VerTable* pa, class VerTable* pb, XD3DXErrorLog* pErrorLog);
bool Verify(class VerTable* pa, D3DVertexShaderProgram * pb, const char* ptitle);
bool Verify(D3DVertexShaderProgram * pa, D3DVertexShaderProgram * pb, XD3DXErrorLog* pErrorLog);
#else
#define Verify(pa,pb,pc) (true)
#endif

#define E_OPTIMIZER_FAILED -4123
#define E_ASSEMBLER_FAILED -4124 

#ifdef VERIFY_EACH_STEP
#define VerifyStep(pa,pb,pc) (pa ? (Verify(pa,pb,pc) ? S_OK : E_OPTIMIZER_FAILED) : S_OK)
#else
#define VerifyStep(pa,pb,pc) (S_OK)
#endif


// Forward reference to some debug helpers
#ifdef DBG
void DPF2(const char* message,...);

void Print(D3DVsInstruction& instruction);
void Print(DWORD shaderType, D3DVertexShaderProgram& program);
void PrintInstruction(int address, const D3DVsInstruction* pI);
void PrintInstruction(int address, const D3DVertexShaderProgram* pUcode);
#endif

bool Pairable(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b);
bool ForcedPair(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b,
                DWORD* pReason);

// ignoring swizzle, which compontents of the input arguments are used
// by this opcode?
// Results is a 3 character array, where results[0] = arg a's mask,
// Mask bit order is the same as the output mask
void ComputePostSwizzleUseMasks(const D3DVsInstruction* pI, UCHAR* results);
void ComputeEffectiveSwizzles(const D3DVsInstruction* a, char* pSwizzles);
bool MergeSwizzles(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b);
bool SwapAC(D3DVsInstruction* pOut, const D3DVsInstruction* pIn);
void StripMacInstruction(D3DVsInstruction* pI);
UCHAR ComputeEffectiveReadMask(const D3DVsInstruction* a, int channel);


struct PaddedUCode {
    DWORD padding;  // Should be zero.
    D3DVsInstruction ucode;
};

bool Epsilon(float a, float b, float e) {
	if(((a - b) < e) && ((a - b) > -e)) 
		return true;
	else
		return false;
}
// Thunks

extern "C" 
ULONG   WINAPI XGBuffer_AddRef(XGBuffer *pThis){
    pThis->refCount += 1;
    return pThis->refCount;
}

extern "C" 
ULONG   WINAPI XGBuffer_Release(XGBuffer *pThis){
    ULONG cRef = pThis->refCount;
    pThis->refCount -= 1;

    if (cRef == 1){
        delete pThis->pData;
        delete pThis;
    }

    return cRef-1;
}

extern "C" 
LPVOID WINAPI XGBuffer_GetBufferPointer(XGBuffer *pThis){
    return pThis->pData;
}

extern "C" 
DWORD WINAPI XGBuffer_GetBufferSize(XGBuffer *pThis){
    return pThis->size;
}

extern "C" 
HRESULT WINAPI XGBufferCreate(DWORD numBytes, LPXGBUFFER* ppBuffer){
    LPXGBUFFER pBuffer = new XGBuffer();
    if(!pBuffer){
        return E_OUTOFMEMORY;
    }
    pBuffer->refCount = 1;
    pBuffer->pData = new char[numBytes];
    pBuffer->size = numBytes;
    if(! pBuffer->pData){
        delete pBuffer;
        return E_OUTOFMEMORY;
    }
    *ppBuffer = pBuffer;
    return S_OK;
}

// class Buffer

Buffer::Buffer(){
    m_size = 0;
    m_buf = 0;
    m_used = 0;
}

Buffer::~Buffer(){
    delete [] m_buf;
}

HRESULT Buffer::Initialize(DWORD size){
    m_used = 0;
    return GrowTo(size);
}

HRESULT Buffer::GrowTo(DWORD size){
    HRESULT hr = S_OK;
    if(size + 1 > m_size){
        size += 4*1024;
        char* newBuf = new char[size];
        if(!newBuf){
            SETERROR(hr, E_OUTOFMEMORY);
        }
        else{
            memcpy(newBuf,m_buf, m_used);
            delete [] m_buf;
            m_buf = newBuf;
            m_size = size;
            m_buf[m_used] = 0;
        }
    }
    return hr;
}

HRESULT Buffer::Printf(const char* msg,...){
    HRESULT hr = S_OK;
    if(m_size - m_used < 1000){
        hr = GrowTo(m_size + 2000);
    }
    if(SUCCEEDED(hr)){
        va_list list;
        va_start(list, msg );
        m_used += _vsnprintf(m_buf + m_used, m_size - m_used, msg, list);
        va_end(list);
    }
    return hr;
}

HRESULT Buffer::Append(LPCVOID data, DWORD length){
    HRESULT hr = GrowTo(m_used + length);
    if(SUCCEEDED(hr)){
        memcpy(m_buf + m_used, data, length);
        m_used += length;
        m_buf[m_used] = '\0';
    }
    return hr;
}

HRESULT Buffer::Append(char c){
    return Append(&c, sizeof(char));
}

HRESULT Buffer::Append(const char* c){
    return Append(c, strlen(c));
}

HRESULT Buffer::Append(Buffer& buffer){
    return Append(buffer.GetText(), buffer.GetUsed());
}


char* Buffer::GetText(){
    return m_buf;
}

char* Buffer::GetTextEnd(){
    return m_buf + m_used;
}

DWORD Buffer::GetUsed(){
    return m_used;
}

void Buffer::Clear(){
    m_used = 0;
    if(m_buf){
        m_buf[0] = 0;
    }
}

static const bool kMacUsesA[] = {
    false, // MAC_NOP        0x00
    true, // MAC_MOV        0x01
    true, // MAC_MUL        0x02
    true, // MAC_ADD        0x03
    true, // MAC_MAD        0x04
    true, // MAC_DP3        0x05
    true, // MAC_DPH        0x06
    true, // MAC_DP4        0x07
    true, // MAC_DST        0x08
    true, // MAC_MIN        0x09
    true, // MAC_MAX        0x0a
    true, // MAC_SLT        0x0b
    true, // MAC_SGE        0x0c
    true, // MAC_ARL        0x0d
    false, // dummy 0xe
    false, // dummy 0xf
};

static const bool kMacUsesB[] = {
    false, // MAC_NOP        0x00
    false, // MAC_MOV        0x01
    true,  // MAC_MUL        0x02
    false, // MAC_ADD        0x03
    true,  // MAC_MAD        0x04
    true,  // MAC_DP3        0x05
    true,  // MAC_DPH        0x06
    true,  // MAC_DP4        0x07
    true,  // MAC_DST        0x08
    true,  // MAC_MIN        0x09
    true,  // MAC_MAX        0x0a
    true,  // MAC_SLT        0x0b
    true,  // MAC_SGE        0x0c
    false, // MAC_ARL        0x0d
    false, // dummy 0xe
    false, // dummy 0xf
};

static const bool kMacUsesC[] = {
    false, // MAC_NOP        0x00
    false, // MAC_MOV        0x01
    false, // MAC_MUL        0x02
    true,  // MAC_ADD        0x03
    true,  // MAC_MAD        0x04
    false, // MAC_DP3        0x05
    false, // MAC_DPH        0x06
    false, // MAC_DP4        0x07
    false, // MAC_DST        0x08
    false, // MAC_MIN        0x09
    false, // MAC_MAX        0x0a
    false, // MAC_SLT        0x0b
    false, // MAC_SGE        0x0c
    false, // MAC_ARL        0x0d
    false, // dummy 0xe
    false, // dummy 0xf
};

// Note that this is at the microcode level, not the DX8 level.
// The microcode uses component x for scalar operations, not component w
// as you might expect. (DX8's assembler forces the operation to be .xxxx, .yyyy. .zzzz, or .wwww,
// which masks this difference.)

const UCHAR kMACInputRegFixedComponentUse[16][3] = {
                   //       a---- b---- c----
    {0x0,0x0,0x0}, // 0 NOP
    {0x0,0x0,0x0}, // 1 MOV  
    {0x0,0x0,0x0}, // 3 MUL  
    {0x0,0x0,0x0}, // 4 ADD  
    {0x0,0x0,0x0}, // 5 MAD  
    {0xe,0xe,0x0}, // 6 DP3  xyz   xyz 
    {0xe,0xf,0x0}, // 7 DPH  xyz   xyzw
    {0xf,0xf,0x0}, // 8 DP4  xyzw  xyzw
    {0x6,0x5,0x0}, // 9 DST   yz    y w
    {0x0,0x0,0x0}, // a MIN  
    {0x0,0x0,0x0}, // b MAX  
    {0x0,0x0,0x0}, // c SLT  
    {0x0,0x0,0x0}, // d SGE  
    {0x8,0x0,0x0}, // d ARL  x     <--- yes, x and not w
    {0x0,0x0,0x0}, // e unknown
    {0x0,0x0,0x0}, // f unknown
};

const UCHAR kMACInputRegColumnatedComponentUse[16][3] = {
                   //       a---- b---- c----
    {0x0,0x0,0x0}, // 0 NOP
    {0xf,0x0,0x0}, // 1 MOV  xyzw
    {0xf,0xf,0x0}, // 3 MUL  xyzw  xyzw
    {0xf,0x0,0xf}, // 4 ADD  xyzw        xyzw
    {0xf,0xf,0xf}, // 5 MAD  xyzw  xyzw  xyzw
    {0x0,0x0,0x0}, // 6 DP3   
    {0x0,0x0,0x0}, // 7 DPH  
    {0x0,0x0,0x0}, // 8 DP4  
    {0x0,0x0,0x0}, // 9 DST  
    {0xf,0xf,0x0}, // a MIN  xyzw  xyzw
    {0xf,0xf,0x0}, // b MAX  xyzw  xyzw
    {0xf,0xf,0x0}, // c SLT  xyzw  xyzw
    {0xf,0xf,0x0}, // d SGE  xyzw  xyzw
    {0x0,0x0,0x0}, // d ARL  
    {0x0,0x0,0x0}, // e unknown
    {0x0,0x0,0x0}, // f unknown
};

// Note that this is at the microcode level, not the DX8 level.
// The microcode uses component x for scalar operations, not component w
// as you might expect. (DX8's assembler forces the operation to be .xxxx, .yyyy. .zzzz, or .wwww,
// which masks this difference.)

const UCHAR kILUInputRegFixedComponentUse[8][3] = {
                   //       a---- b---- c----
    {0x0,0x0,0x0}, // 0 NOP
    {0x0,0x0,0x0}, // 0 MOV              
    {0x0,0x0,0x8}, // 0 RCP              x       <--- yes, x and not w
    {0x0,0x0,0x8}, // 0 RCC              x
    {0x0,0x0,0x8}, // 0 RSQ              x
    {0x0,0x0,0x8}, // 0 EXP              x
    {0x0,0x0,0x8}, // 0 LOG              x
    {0x0,0x0,0xd}, // 0 LIT              xy w
};

const UCHAR kILUInputRegColumnatedComponentUse[8][3] = {
                   //       a---- b---- c----
    {0x0,0x0,0x0}, // 0 NOP
    {0x0,0x0,0xf}, // 0 MOV              xyzw
    {0x0,0x0,0x0}, // 0 RCP               
    {0x0,0x0,0x0}, // 0 RCC               
    {0x0,0x0,0x0}, // 0 RSQ               
    {0x0,0x0,0x0}, // 0 EXP               
    {0x0,0x0,0x0}, // 0 LOG               
    {0x0,0x0,0x0}, // 0 LIT               
};

static const char* kOutNames[] = {"oPos",
    "o1?", "o2?",
    "oD0", "oD1",
    "oFog", "oPts",
    "oB0", "oB1",
    "oT0", "oT1", "oT2", "oT3", "???"
};

struct OutPair {
    unsigned char m; // Mask
    unsigned char r; // register
    bool used;
};

void ExpandRegisterOutputMasks(OutPair* masks, const D3DVsInstruction* a);






//verifier: double-checks that the optimizers didn't break something
//The code can also be used to compare two vertex shaders for similarity.
//Basically, it builds a tree of all calculations performed in the shader,
//and includes code to compare two trees.
//Several more optimizations could be performed in the future, using these tree structures

#ifndef DISABLE_VERIFIER

//verifier debug-code
#ifdef DBG_VERIFIER
#define VER_ERR(_a_) do { DPF2##_a_ ; DPF2("\n"); _asm {int 3}; } while(0)
#define VER_MSG(_a_) do { DPF2##_a_ ; DPF2("\n"); } while(0)
#else
#ifdef DBG
#define VER_ERR(_a_) do { DPF2##_a_ ; DPF2("\n"); } while (0)
#define VER_MSG(_a_) do { } while (0)
#else
#define VER_ERR(_a_) do { } while (0)
#define VER_MSG(_a_) do { } while (0)
#endif
#endif

#define VER_NUM_REG ((REG_ARL * 4) + 2) //all regs, ARL, constlist
#define VER_NUM_CONSTS (96 * 2 * 4)

typedef int VerReg; //range from (0 to ARL * 4), inclusive

enum VerOpcode {
    VEROP_NOP =0,
    VEROP_MOV  ,
    VEROP_MUL ,
    VEROP_ADD ,
    VEROP_MAD ,
    VEROP_DP3 ,
    VEROP_DPH ,
    VEROP_DP4 ,
    VEROP_DST , //should not be used. Use dstx, dsty, dstz, dstw, or simplify to (ONE,MUL,MOV,MOV)
    VEROP_MIN ,
    VEROP_MAX ,
    VEROP_SLT ,
    VEROP_SGE ,
    VEROP_ARL ,
    VEROP_UNU1,
    VEROP_UNU2,
    VEROP_NOPX, //nop* == nop
    VEROP_NOPY, 
    VEROP_NOPZ, 
    VEROP_NOPW,
    VEROP_IMVX, //imv* == mov
    VEROP_IMVY, 
    VEROP_IMVZ, 
    VEROP_IMVW,
    VEROP_RCPX, //all destination registers equal ("X" form used for all calculations)
    VEROP_RCPY, 
    VEROP_RCPZ, 
    VEROP_RCPW,
    VEROP_RCCX, 
    VEROP_RCCY, 
    VEROP_RCCZ, 
    VEROP_RCCW,
    VEROP_RSQX, 
    VEROP_RSQY, 
    VEROP_RSQZ, 
    VEROP_RSQW,
    VEROP_EXPX, //destination registers not equal. Some duplicates exist (LOGW == ONE)
    VEROP_EXPY, 
    VEROP_EXPZ, 
    VEROP_EXPW,
    VEROP_LOGX, 
    VEROP_LOGY, 
    VEROP_LOGZ, 
    VEROP_LOGW,
    VEROP_LITX, 
    VEROP_LITY, 
    VEROP_LITZ, 
    VEROP_LITW,
    VEROP_DSTX, //The MAC exception: the 4 outputs involve different calculations...
    VEROP_DSTY, 
    VEROP_DSTZ, 
    VEROP_DSTW,
    VEROP_INVL,
    VEROP_ZERO,
    VEROP_ONE ,

    VEROP_FORCE_DWORD = 0x7fffffff
}; // enum VerOpcode

#ifdef DBG
void PrintVerOpcode(VerOpcode o) {
    switch (o) {
    case VEROP_NOP : DPF2("VEROP_NOP "); break;
    case VEROP_MOV : DPF2("VEROP_MOV "); break;
    case VEROP_MUL : DPF2("VEROP_MUL "); break;
    case VEROP_ADD : DPF2("VEROP_ADD "); break;
    case VEROP_MAD : DPF2("VEROP_MAD "); break;
    case VEROP_DP3 : DPF2("VEROP_DP3 "); break;
    case VEROP_DPH : DPF2("VEROP_DPH "); break;
    case VEROP_DP4 : DPF2("VEROP_DP4 "); break;
    case VEROP_DST : DPF2("VEROP_DST "); break;
    case VEROP_MIN : DPF2("VEROP_MIN "); break;
    case VEROP_MAX : DPF2("VEROP_MAX "); break;
    case VEROP_SLT : DPF2("VEROP_SLT "); break;
    case VEROP_SGE : DPF2("VEROP_SGE "); break;
    case VEROP_ARL : DPF2("VEROP_ARL "); break;
    case VEROP_UNU1: DPF2("VEROP_UNU1"); break;
    case VEROP_UNU2: DPF2("VEROP_UNU2"); break;
    case VEROP_NOPX: DPF2("VEROP_NOPX"); break;
    case VEROP_NOPY: DPF2("VEROP_NOPY"); break;
    case VEROP_NOPZ: DPF2("VEROP_NOPZ"); break;
    case VEROP_NOPW: DPF2("VEROP_NOPW"); break;
    case VEROP_IMVX: DPF2("VEROP_IMVX"); break;
    case VEROP_IMVY: DPF2("VEROP_IMVY"); break;
    case VEROP_IMVZ: DPF2("VEROP_IMVZ"); break;
    case VEROP_IMVW: DPF2("VEROP_IMVW"); break;
    case VEROP_RCPX: DPF2("VEROP_RCPX"); break;
    case VEROP_RCPY: DPF2("VEROP_RCPY"); break;
    case VEROP_RCPZ: DPF2("VEROP_RCPZ"); break;
    case VEROP_RCPW: DPF2("VEROP_RCPW"); break;
    case VEROP_RCCX: DPF2("VEROP_RCCX"); break;
    case VEROP_RCCY: DPF2("VEROP_RCCY"); break;
    case VEROP_RCCZ: DPF2("VEROP_RCCZ"); break;
    case VEROP_RCCW: DPF2("VEROP_RCCW"); break;
    case VEROP_RSQX: DPF2("VEROP_RSQX"); break;
    case VEROP_RSQY: DPF2("VEROP_RSQY"); break;
    case VEROP_RSQZ: DPF2("VEROP_RSQZ"); break;
    case VEROP_RSQW: DPF2("VEROP_RSQW"); break;
    case VEROP_EXPX: DPF2("VEROP_EXPX"); break;
    case VEROP_EXPY: DPF2("VEROP_EXPY"); break;
    case VEROP_EXPZ: DPF2("VEROP_EXPZ"); break;
    case VEROP_EXPW: DPF2("VEROP_EXPW"); break;
    case VEROP_LOGX: DPF2("VEROP_LOGX"); break;
    case VEROP_LOGY: DPF2("VEROP_LOGY"); break;
    case VEROP_LOGZ: DPF2("VEROP_LOGZ"); break;
    case VEROP_LOGW: DPF2("VEROP_LOGW"); break;
    case VEROP_LITX: DPF2("VEROP_LITX"); break;
    case VEROP_LITY: DPF2("VEROP_LITY"); break;
    case VEROP_LITZ: DPF2("VEROP_LITZ"); break;
    case VEROP_LITW: DPF2("VEROP_LITW"); break;
    case VEROP_DSTX: DPF2("VEROP_DSTX"); break;
    case VEROP_DSTY: DPF2("VEROP_DSTY"); break;
    case VEROP_DSTZ: DPF2("VEROP_DSTZ"); break;
    case VEROP_DSTW: DPF2("VEROP_DSTW"); break;
    case VEROP_INVL: DPF2("VEROP_INVL"); break;
    case VEROP_ZERO: DPF2("VEROP_ZERO"); break;
    case VEROP_ONE : DPF2("VEROP_ONE "); break;

    }
}
#endif

int VerOpParamCount(VerOpcode op) {
    switch (op) {
    case VEROP_NOP:
    case VEROP_NOPX: case VEROP_NOPY: case VEROP_NOPZ: case VEROP_NOPW:
    case VEROP_ZERO:
    case VEROP_ONE:
        return 0;

    case VEROP_MOV:
    case VEROP_IMVX: case VEROP_IMVY: case VEROP_IMVZ: case VEROP_IMVW:
        return 1;

    case VEROP_ADD:
    case VEROP_MUL:
        return 2;

    case VEROP_MAD:
        return 3;

    case VEROP_SLT:
    case VEROP_SGE:
    case VEROP_MIN:
    case VEROP_MAX:
        return 2;

    case VEROP_DP3:
        return 6;

    case VEROP_DPH:
        return 7;

    case VEROP_DP4:
        return 8;

    case VEROP_DSTX:
        return 0;
    case VEROP_DSTY:
        return 2;
    case VEROP_DSTZ:
    case VEROP_DSTW:
        return 1;

    
    case VEROP_EXPX:
    case VEROP_EXPY:
    case VEROP_EXPZ:
        return 1;
    case VEROP_EXPW:
        return 0;

    case VEROP_LITX:
        return 0;
    case VEROP_LITY:
        return 1; //x
    case VEROP_LITZ:
        return 3; //x,y,w
    case VEROP_LITW:
        return 0;

    case VEROP_LOGX:
    case VEROP_LOGY:
    case VEROP_LOGZ:
        return 1;
    case VEROP_LOGW:
        return 0;


    case VEROP_RCPX: case VEROP_RCPY: case VEROP_RCPZ: case VEROP_RCPW:
    case VEROP_RSQX: case VEROP_RSQY: case VEROP_RSQZ: case VEROP_RSQW:
    case VEROP_RCCX: case VEROP_RCCY: case VEROP_RCCZ: case VEROP_RCCW:
        return 1;

    case VEROP_DST:
        VER_ERR(("OpParamCount: DST performs different calculations for each output param. Use DST* instead."));
        return 0;

    default:
        VER_ERR(("OpParamCount: invalid op: %d", op));
        return 0;
    }
}

//contains calculations for all constant registers. 
//A new list is created when a constant is written in a shader program
class VerConstTableList {
public:
    class VerTable* mpData;
    VerConstTableList* mpNext;
    VerConstTableList():mpData(0),mpNext(0){}
    bool SetData(VerTable*ptable);
    ~VerConstTableList();
};

//just a single-linked list used in class VerOp, to keep track of similar operations
class VerOpList {
public:
    class VerOp* mpData;
    VerOpList* mpNext;
    VerOpList():mpData(0),mpNext(0){}

	//add more data just after this list node
    void SetData(VerOp*pOp) {
        VerOpList *pl = mpNext;
        mpNext = new VerOpList;
        if(mpNext) {
            mpNext->mpNext = pl;
            mpNext->mpData = pOp;
        }
        else {
            mpNext = pl; //yikes, tiny allocation failed!
        }

    }

	//find data "pOp" in this element, or after this element, in the list
	//return true if it is found, false if not.
    bool FindData(VerOp*pOp) {
        for(VerOpList* pl = this; pl; pl = pl->mpNext) {
            if(pl->mpData == pOp) {
                return true;
            }
        }
        return false;
    }

	//delete the entire list
    ~VerOpList() {
        delete mpNext;
    }
};


//VerOp: a "branch" in the opration tree. Keeps track of parameters to the operation performed, the type of operation, etc
class VerOp {
private:
    VerOp():mRefCount(0),mNumParams(0),mpParams(0),mpSame(0) {} //yes, private. Use Copy, MakeOp, or MakeEmptyOp to create a new op.
    int mRefCount;
    ~VerOp(){ Cleanup(); }
    //cleanup
    void Cleanup(); //cleans up all pointers & stuff inside of this VerOp.

public:
    int AddRef() { return ++mRefCount; } 
    int Release() { 
        if(!--mRefCount) {
            delete this;
            return 0;
        }
        return mRefCount;
    }

    //data
    VerOpcode mOp; //the operation performed
    int mNumParams; //number of params used in the operation
    class VerNode* mpParams; //pointer to array of params
    
    //creation
    static VerOp* MakeOpV(VerOpcode op, int numparams, class VerNode** ppv); //same as below, except pass a pointer to array of numparams VerNode*'s.
    static VerOp* MakeOp(VerOpcode op, int numparams, ...); //Create a new VerOp, passing the specified number of VerNode* for the ...'s. Return NULL if no mem.
    static VerOp* MakeEmptyOp(VerOpcode op, int numparams = -1); //create a new empty VerOp. NULL if no mem.
    VerOp* Copy(); //AddRefs this, returns this. This should be called when a node is duplicated, so we don't end up with duplicate trees.
    VerOp* CopyTree(); //create a duplicate of this VerOp. Returns pointer to the new VerOp, or NULL if no memory
    
    VerOpList* mpSame; //list of ops that are the same as this one. Note: this is used and clobbered by Verify()
};

typedef bool (*VERNODEFUNC)(VerNode*);

//VerNode: holds the state of a register at a given time. 
//These correspond to parameters used in VerOps

class VerNode {
public:
    bool mbHasReg;   //if this register is either untouched, or has been the destination of only a MOV
    bool mbHasOp;    //if this register contains the result of an opration
    bool mbHasTable; //true if the register uses a0.x after a constant has been written
    bool mbNegated;  
    VerReg mReg;	//uses a slight variation of the register enumeration: multiply the enum value by 4, and add 0 for .x, 1 for .y, etc.
    class VerOp*    mpOp; //null if mbHasOp is false

    class VerTable* mpTable; //if table exists, arl is also required.
    class VerNode*  mpArl;   //pointer to a0.x, if it is used in the contents of this register

    VerNode():mbHasReg(false),mbHasOp(false),mbHasTable(false), mbNegated(false), 
        mReg(-1), mpOp(NULL), mpTable(NULL), mpArl(NULL)
    {
    }

    ~VerNode() { Cleanup(); }
    
	//duplicate the contents of another VerNode into the current one 
    bool CopyFrom(VerNode* p)
    {
        Cleanup();
        mbHasOp = p->mbHasOp;
        mbHasReg = p->mbHasReg;
        mbHasTable = p->mbHasTable;
        mpTable = p->mpTable;
        mbNegated = p->mbNegated;
        mReg = p->mReg;
        if(mbHasOp) {
            mpOp = p->mpOp->Copy();
            if(mpOp ==  NULL) {
                VER_ERR(("VerNode::CopyFrom: mpOp->Copy failed."));
                mbHasOp = false;
                Cleanup();
                return false;
            } 
        } else {
            mpOp = NULL; //should be null anyway...
        }
        if(mbHasTable) {
            mpArl = p->mpArl->Copy();
        }
        return true;
    }
    
	//create a new VerNode that is a duplicate of "this"
    VerNode* Copy() 
    {
        VerNode* p = new VerNode;
        if(p == NULL) {
            VER_ERR(("VerNode::Copy: couldn't create new node!"));
            return NULL;
        }
        if(false == p->CopyFrom(this)) {
            VER_ERR(("VerNode::Copy: CopyFrom(this) failed!"));
            delete p;
            return NULL;
        }
        return p;
    }
    
	//create a new node, point it to the VerOp provided
	//note: the verop is assumed to have just been created, so we don't add-ref it here
    static VerNode* EncaseOpWithNode(VerOp* pop) {
        VerNode* p = new VerNode;
        if(!p) {
            VER_ERR(("VerNode::EncaseOpWithNode: new VerNode returned NULL!"));
            return NULL;
        }
        if(pop) {
            p->mbHasOp = true;
            p->mpOp = pop;
        } else {
            VER_MSG(("VerNode: EncaseOpWithNode was passed a NULL VerOp*!"));
            //nothing. Already initialized to NULL...
        }
        return p;
    }

	//let "this" use the VerOp provided. AddRef the verop, since others are using it already
    void UseOp(VerOp* pop) { 
        Cleanup();
        mbHasOp = true;
        mpOp = pop;
        if(pop) pop->AddRef();
    }

    void Cleanup() 
    {
        if(mbHasOp) {
            if(mpOp) mpOp->Release();
            mpOp = NULL;
            mbHasOp = false;
        }
        
        if(mbHasTable) {
            //Tables are global, and are freed at the end of verification, 
            //so that we only need one table if no constant registers are 
            //written to. However, mpArl is unique to this VerNode, so delete it.
            delete mpArl;
        } 
        
        mbHasReg = mbHasOp = mbHasTable = mbNegated = false;
        mReg = -1;
        mpOp = NULL;
        mpTable = NULL;
        mpArl = NULL;
    }

	//some puny validation code, to make sure the verifier didn't mess up
    bool TinyValidate()
    {
        if(mbHasReg && (mReg == -1 || (mReg >= REG_R0 * 4 && mReg < REG_R15 * 4 + 4))) {
            return false;
        }
        if(mbHasOp && (mpOp->mOp == VEROP_NOPX || mpOp->mOp == VEROP_NOP)) {
            return false;
        }
        return true;
    }
};


bool BreakUpMad(VerNode* pn) //breaks up MAD instruction into MUL + ADD. Returns true if any were changed.
{
    if(!pn->mbHasOp) return false;

    if(pn->mpOp->mOp == VEROP_MAD) {
        if(pn->mpOp->mNumParams != 3) {
            VER_ERR(("mad with %d params???", pn->mpOp->mNumParams));
            return false;
        }
        
        //create the MUL op, using params 0 and 1
        VerOp* pop = VerOp::MakeOp(VEROP_MUL, 2, &pn->mpOp->mpParams[0], &pn->mpOp->mpParams[1]);
        VerNode* pmulnode;
        if(!pop) {
            VER_ERR(("BreakUpMad: MakeOp returned NULL!"));
            return false;
        }
        pmulnode = VerNode::EncaseOpWithNode(pop); //encase the mul Op with a Node so it can be used in other Ops
        if(!pmulnode) {
            pop->Release();
            VER_ERR(("BreakUpMad: EncaseOpWithNode returned NULL!"));
            return false;
        }
        
        //make the add node, adding param2 to results of the mul
        pop = VerOp::MakeOp(VEROP_ADD, 2, pmulnode, &pn->mpOp->mpParams[2]);
        delete pmulnode;  //makeop makes a copy of the params... we don't need pmulnode any more.
        
        if(!pop) {
            VER_ERR(("BreakUpMad: MakeOp returned NULL!"));
            return false;
        }
        
        pn->mpOp->Release();
        pn->mpOp = pop; 
        return true;
    }
    return false;
}

//delete the "buddies" list, of a VerNode. 
//Call this if the node ever changes...
bool ClobberSameList(VerNode* pn) 
{
    if(!pn->mbHasOp) return false;

    if(pn->mpOp->mpSame) {
        delete pn->mpOp->mpSame;
        pn->mpOp->mpSame = 0;
        return true;
    }
    return false;
}

//create a VerOp, retrieving the parameters using an array of their addresses
VerOp* VerOp::MakeOpV(VerOpcode op, int numparams, VerNode** ppv) 
{
    VerOp* p = MakeEmptyOp(op, numparams);
    if(p == NULL) {
        VER_ERR(("VerOp::MakeOpV : MakeEmptyOp failed!"));
        return p;
    }
    
    int i;

    for(i = 0; i < p->mNumParams; i++) {
        if(false == p->mpParams[i].CopyFrom(*ppv++)) {
            VER_ERR(("VerOp::MakeOpV : CopyFrom failed!"));
            for(i--; i >= 0; i--) {
                p->mpParams[i].Cleanup();
            }
            p->mNumParams = 0;
            p->Release();
            return NULL;
        }
    }
    return p;
}

//see MakeOpV
VerOp* VerOp::MakeOp(VerOpcode op, int numparams, ...) //all params after op should be VerNode*'s.
{
    VerOp* p;
    p = MakeOpV(op, numparams, (VerNode**)((&numparams) + 1) );
    if(!p) {
        VER_ERR(("VerOp::MakeOp: MakeOpV failed!"));
    }
    return p;
}

//allocate an empty VerOp
VerOp* VerOp::MakeEmptyOp(VerOpcode op, int numparams) 
{
    int c;
    VerOp* p = new VerOp;
    if(p == NULL) {
        VER_ERR(("VerOp::MakeEmptyOp: new VerOp failed!"));
        return NULL;
    }
    p->AddRef();
    if(numparams == -1) {
        c = VerOpParamCount(op);
    } else {
        c = numparams;
    }
    p->mpParams = new VerNode[c];
    if(!p->mpParams) { 
        VER_ERR(("VerOp::MakeEmptyOp: new VerNode[%d] failed", c));
        p->Release(); 
        return NULL; 
    }
    p->mOp = op;
    p->mNumParams = c;
    return p;
}

//duplicate a VerOp and all parameters used by it (not recursive though)
//this isn't used, but could be useful...
VerOp* VerOp::CopyTree()
{
    int i;
    VerOp* p = MakeEmptyOp(mOp, mNumParams);
    if(!p) {
        VER_ERR(("VerOp::Copy: MakeEmptyOp(%d, %d) returned NULL!", mOp, mNumParams));
        return NULL;
    }
    for(i = 0; i < p->mNumParams; i++) {
        if(false == p->mpParams[i].CopyFrom(&mpParams[i])) {
            VER_ERR(("VerOp::Copy: CopyFrom failed!"));
            for(i--; i >= 0; i--) {
                p->mpParams[i].Cleanup();
            }
            p->Release();
            return NULL;
        }
    }
    return p;
}

//The majority of the time that you want to "copy" a VerOp, the original won't be changing, so
//a simple "addref" will do. If you need to duplicate a VerOp and its parameters, use CopyTree above
VerOp* VerOp::Copy() 
{
    AddRef();
    return this;
}

void VerOp::Cleanup() 
{
    int i;
    for(i = 0; i < mNumParams; i++) {
        mpParams[i].Cleanup();
    }
    mNumParams = 0;
    mOp = VEROP_INVL;
    delete[] mpParams;
    mpParams = NULL;
    delete mpSame;
    mpSame = NULL;

}

class VerTable {
private:
    VerTable(){}; //yes, private. Use MakeTable to create a VerTable.

	//call f() for all VerNodes in the tree. 
	//Recursive.
    bool Propegate2(VERNODEFUNC f, VerNode* pn, void* p) 
    {
        bool ret = false;
        int i;
        VerOpList* pv = *((VerOpList**)p);
        if(pv == 0) {
            pv = new VerOpList;
            if(!pv) return false;
        }
    

        if(pn->mbHasOp) {
            if(pv->FindData(pn->mpOp)) return ret;
            pv->SetData(pn->mpOp);
            for(i = 0; i < pn->mpOp->mNumParams; i++) {
                ret |= Propegate2(f, &pn->mpOp->mpParams[i], &pv);
            }
        }

        ret |= f(pn);

        if(*((VerOpList**)p) == 0) {
            delete pv;
        }
        
        return ret;
    }

public:
    ~VerTable(){ Cleanup(); }
    int mNodeCount;
    VerNode* mpNodes;

	//call f() once for every VerNode in all trees in the table
    bool Propegate(VERNODEFUNC f) 
    {
        int i;
        int j = 0; //used for scratch space by propegate2
        bool br = false;
        for(i = 0; i < mNodeCount; i++) {
            br |= Propegate2(f, &mpNodes[i], &j);
        }
        return br;
    }

	//create a table to keep track of "count" registers (192 constants * 4 elements = 368)
    static VerTable* MakeTable(int count) 
    {
        VerTable* pt = new VerTable;
        if(!pt) {
            VER_ERR(("VerTable::MakeTable: new VerTable failed!"));
            return NULL;
        }
        pt->mpNodes = new VerNode[count];
        if(!pt->mpNodes) { 
            VER_ERR(("VerTable::MakeTable: new VerNode[%d] failed!", count));
            delete pt; 
            return NULL; 
        }
        pt->mNodeCount = count;
        return pt;
    }
    
	//copy the array of registers pointed to by psrc into the table.
    //passing NULL of psrc will just initialize table element i to i.
    bool InitTable(VerNode* psrc = NULL) 
    {
        int i;
        if(psrc == NULL) {
            for(i = 0; i < mNodeCount; i++) {
                mpNodes[i].mbHasReg = true;
                mpNodes[i].mReg = i;
            }
            return true;
        } 
        else { //we have something to init from
            for(i = 0; i < mNodeCount; i++) {
                if(false == mpNodes[i].CopyFrom(psrc + i)) {
                    VER_ERR(("VerTable::InitTable(%p): CopyFrom(scr + %d) failed", psrc, i));
                    return false;
                }
            }
            return true;
        }
    }

    void Cleanup() 
    {
        delete[] mpNodes;
        mNodeCount = 0;
        mpNodes = 0;
    }

	//build a VerTable that contains all calculations performed in the vertex shader pointed to by pcode
    static HRESULT BuildTable(D3DVertexShaderProgram*pcode, VerConstTableList** ppc, VerTable** ppOutTable) //build a table & constant list. To delete it, use delete.
    {
        VerTable* pt = NULL;        //pointer to the table
        VerConstTableList* pc = NULL;  //holds the base of the constant list
        VerConstTableList* pc2 = NULL; //pointer to most-recently added constant table
        bool br;
        int ret;
        int i = 0;
        HRESULT hr = S_OK;

        if(!ppOutTable) { VER_ERR(("Called BuildTable with NULL ppOutTable")); return E_FAIL; }
        if(!ppc)        { VER_ERR(("Called BuildTable with NULL ppc"));        return E_FAIL; }
        *ppOutTable = NULL;
        *ppc = NULL;

        pt = VerTable::MakeTable(VER_NUM_REG);
        if(!pt) {
            hr = E_OUTOFMEMORY;
            VER_ERR(("BuildTable: pt = MakeTable(%d) failed", VER_NUM_REG));
            goto cleanup;
        }

        br = pt->InitTable();
        if(!br) {
            hr = E_OUTOFMEMORY;
            VER_ERR(("BuildTable: pt->InitTable() failed"));
            goto cleanup;
        }

        pc = pc2 = new VerConstTableList;
        if(!pc) {
            hr = E_OUTOFMEMORY;
            VER_ERR(("BuildTable: new VerConstTableList failed"));
            goto cleanup;
        }

        if(!pc->SetData(pt)) {
            hr = E_OUTOFMEMORY;
            VER_ERR(("BuildTable: pc->SetData(pt) failed"));
            goto cleanup;
        }

        do {
            ret = pt->Interpret(&pcode->ucode[i], pc2->mpData);
            if(ret == 1) {
                if(!pc2->SetData(pt)) {
                    hr = E_OUTOFMEMORY;
                    VER_ERR(("BuildTable: pc2->SetData(pt) failed"));
                    goto cleanup;
                }
                pc2 = pc2->mpNext;
            } else if (ret == -1) {
                hr = E_OUTOFMEMORY;
                VER_ERR(("BuildTable: Interpret ran out of memory"));
                goto cleanup;
            } else if (ret == E_ASSEMBLER_FAILED) {
                hr = ret;
                goto cleanup;
            }
        } while ((++i < pcode->length) && !(pcode->ucode[i].eos));

        if(SUCCEEDED(hr)) {
            *ppc = pc;
            *ppOutTable = pt;
        }
        return hr;

//code will only get here if something went wrong
cleanup:
        VER_MSG(("deleting pt."));
        delete pt;
        VER_MSG(("deleting pc."));
        delete pc;
        *ppc = NULL;
        return hr;
    }


	//interpret a vertex shader instruction, and modify the tree to store its operation appropriately
	//LastConstData contains a pointer to the last known state of all constant registers
    int Interpret(D3DVsInstruction*pcode, VerTable* LastConstData) //-1 if error. 0 if good. 1 if wrote to a constant register.
    {
        VerNode a[3][4];   //input (from a, b, c ... swizzled & negated appropriately). These are copied to the params of Out, then deleted.
        VerOp*  pOutOp[2][4] = {0}; //Output from the two operations. This is copied to the appropriate output registers, then deleted.
        VerNode Out[2][4]; //node form of OutOp. Note: Mov writes directly to Out. If OutOp is NULL, it is not used when creating Out.
        VerNode*pOutArl = 0; //if ARL instruction is encountered, this receives the new ARL, and holds it until the end...
        bool br = false;        
        bool WrittenToConstant = false;
        int iIluOutReg;
        int rwm;

#ifdef DBG
        char* ErrStr;
#define SetErrStr(a) ErrStr = a
#else
#define SetErrStr(a)
#endif

        int iin[3];
        switch (pcode->amx) {
        case MX_R:
            //ainput is register
            iin[0] = (pcode->arr == 12) ? REG_oPos : (REG_R0 + pcode->arr); //r12 is mapped to oPos. 
            break;
        case MX_V:
            iin[0] = REG_V0 + pcode->va;  //ainput is input
            break;
        case MX_C:
            iin[0] = REG_C0 + pcode->ca;  //ainput is constant
            //deal with ARL stuff later
            break;
        default:
            VER_ERR(("Unknown amx: %d", pcode->amx));
            break;
        }
        switch (pcode->bmx) {
        case MX_R:
            //binput is register
            iin[1] = (pcode->brr == 12) ? REG_oPos : (REG_R0 + pcode->brr); //r12 is mapped to oPos
            break;
        case MX_V:
            iin[1] = REG_V0 + pcode->va;   //binput is input
            break;
        case MX_C:
            iin[1] = REG_C0 + pcode->ca;   //binput is constant
            //deal with ARL stuff later
            break;
        default:
            VER_ERR(("Unknown bmx: %d", pcode->bmx));
            break;
        }
        switch (pcode->cmx) {
        case MX_R:
            //cinput is register
            iin[2] = (pcode->crr == 12) ? REG_oPos : (REG_R0 + pcode->crr); //r12 is mapped to oPos
            break;
        case MX_V:
            iin[2] = REG_V0 + pcode->va;
            break;
        case MX_C:
            iin[2] = REG_C0 + pcode->ca;
            //deal with ARL stuff later
            break;
        default:
            VER_ERR(("Unknown cmx: %d", pcode->cmx));
            break;
        }
    
        //copy the state of the input to our temp buffers. code->**s takes care of swizzling
        br = true;
        br &= a[0][0].CopyFrom(&mpNodes[iin[0] * 4 + pcode->axs]);
        br &= a[0][1].CopyFrom(&mpNodes[iin[0] * 4 + pcode->ays]);
        br &= a[0][2].CopyFrom(&mpNodes[iin[0] * 4 + pcode->azs]);
        br &= a[0][3].CopyFrom(&mpNodes[iin[0] * 4 + pcode->aws]);
        br &= a[1][0].CopyFrom(&mpNodes[iin[1] * 4 + pcode->bxs]);
        br &= a[1][1].CopyFrom(&mpNodes[iin[1] * 4 + pcode->bys]);
        br &= a[1][2].CopyFrom(&mpNodes[iin[1] * 4 + pcode->bzs]);
        br &= a[1][3].CopyFrom(&mpNodes[iin[1] * 4 + pcode->bws]);
        br &= a[2][0].CopyFrom(&mpNodes[iin[2] * 4 + pcode->cxs]);
        br &= a[2][1].CopyFrom(&mpNodes[iin[2] * 4 + pcode->cys]);
        br &= a[2][2].CopyFrom(&mpNodes[iin[2] * 4 + pcode->czs]);
        br &= a[2][3].CopyFrom(&mpNodes[iin[2] * 4 + pcode->cws]);
        if(!br) {
            VER_ERR(("Interpret: one of the calls to a[*][*].CopyFrom failed!"));
            return -1;
        }


        int i;

        //if we're doing indexed stuff, link to the table and the ARL register
        if(pcode->cin) {
            if(pcode->amx == MX_C) {
                for(i = 0; i < 4; i++) {
                    a[0][i].Cleanup(); //get rid of any operation fluff, since we don't know what constant to read from
                    a[0][i].mbHasTable = true;
                    a[0][i].mpTable = LastConstData;
                    a[0][i].mpArl = mpNodes[REG_ARL * 4].Copy();
                    a[0][i].mbHasReg = true;
                    if(NULL == a[0][i].mpArl) {
                        VER_ERR(("a[0][%d].mpArl is null after mpNodes[REG_ARL*4].Copy!"));
                        return -1;
                    }
                }
                a[0][0].mReg = pcode->ca * 4 + pcode->axs; //isn't a valid VerOpcode, but works as an index instead...
                a[0][1].mReg = pcode->ca * 4 + pcode->ays; //isn't a valid VerOpcode, but works as an index instead...
                a[0][2].mReg = pcode->ca * 4 + pcode->azs; //isn't a valid VerOpcode, but works as an index instead...
                a[0][3].mReg = pcode->ca * 4 + pcode->aws; //isn't a valid VerOpcode, but works as an index instead...
            }
            if(pcode->bmx == MX_C) {
                for(i = 0; i < 4; i++) {
                    a[1][i].Cleanup(); //get rid of any operation fluff, since we don't know what constant to read from
                    a[1][i].mbHasTable = true;
                    a[1][i].mpTable = LastConstData;
                    a[1][i].mpArl = mpNodes[REG_ARL * 4].Copy();
                    a[1][i].mbHasReg = true;
                    if(NULL == a[1][i].mpArl) {

                        VER_ERR(("a[1][%d].mpArl is null after mpNodes[REG_ARL*4].Copy!"));
                        return -1;
                    }
                }
                a[1][0].mReg = pcode->ca * 4 + pcode->bxs; //isn't a valid VerOpcode, but works as an index instead...
                a[1][1].mReg = pcode->ca * 4 + pcode->bys; //isn't a valid VerOpcode, but works as an index instead...
                a[1][2].mReg = pcode->ca * 4 + pcode->bzs; //isn't a valid VerOpcode, but works as an index instead...
                a[1][3].mReg = pcode->ca * 4 + pcode->bws; //isn't a valid VerOpcode, but works as an index instead...
            }
            if(pcode->cmx == MX_C) {
                for(i = 0; i < 4; i++) {
                    a[2][i].Cleanup(); //get rid of any operation fluff, since we don't know what constant to read from
                    a[2][i].mbHasTable = true;
                    a[2][i].mpTable = LastConstData;
                    a[2][i].mpArl = mpNodes[REG_ARL * 4].Copy();
                    a[2][i].mbHasReg = true;
                    if(NULL == a[2][i].mpArl) {

                        VER_ERR(("a[2][%d].mpArl is null after mpNodes[REG_ARL*4].Copy() !"));
                        return -1;
                    }
                }
                a[2][0].mReg = pcode->ca * 4 + pcode->cxs; //isn't a valid VerOpcode, but works as an index instead...
                a[2][1].mReg = pcode->ca * 4 + pcode->cys; //isn't a valid VerOpcode, but works as an index instead...
                a[2][2].mReg = pcode->ca * 4 + pcode->czs; //isn't a valid VerOpcode, but works as an index instead...
                a[2][3].mReg = pcode->ca * 4 + pcode->cws; //isn't a valid VerOpcode, but works as an index instead...
            }
        }

        //if negated, negate the negated flag
        for(i = 0; i < 4; i++) {
            if(pcode->ane) {                //if the input is negated
                a[0][i].mbNegated ^= true; //swap the "negated" flag
            }
            if(pcode->bne) {                //if the input is negated
                a[1][i].mbNegated ^= true; //swap the "negated" flag
            }
            if(pcode->cne) {                //if the input is negated
                a[2][i].mbNegated ^= true; //swap the "negated" flag
            }
        }

        //look at the MAC opcode... create the approptate output branches
        switch(pcode->mac) {
        case MAC_NOP:
            pOutOp[0][0] = VerOp::MakeOp(VEROP_NOP, 0);
            if(!pOutOp[0][0]) {
                SetErrStr("Interpret: VerOp::MakeOp(VEROP_NOP,0) failed");
                goto cleanupbad;
            }
            for(i = 1; i < 4; i++) {
                pOutOp[0][i] = pOutOp[0][0]->Copy();
            }
            break;
        case MAC_MOV:
            for(i = 0; i < 4; i++) {
                if(!Out[0][i].CopyFrom(&a[0][i])) {
                    SetErrStr("Interpret: CopyFrom for Mov instruction failed");
                    goto cleanupbad;
                }
            }
            break;
        case MAC_MUL:
            for(i = 0; i < 4; i++) {
                pOutOp[0][i] = VerOp::MakeOp(VEROP_MUL, 2, &a[0][i], &a[1][i]);
                if(!pOutOp[0][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_MUL,2) failed");
                    goto cleanupbad;
                }
            }
            break;
        case MAC_ADD:
            for(i = 0; i < 4; i++) {
                pOutOp[0][i] = VerOp::MakeOp(VEROP_ADD, 2, &a[0][i], &a[2][i]);
                if(!pOutOp[0][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_ADD,2) failed");
                    goto cleanupbad;
                }
            }
            break;
        case MAC_MAD:
            for(i = 0; i < 4; i++) {
                pOutOp[0][i] = VerOp::MakeOp(VEROP_MAD, 3, &a[0][i], &a[1][i], &a[2][i]);
                if(!pOutOp[0][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_MAD,3) failed");
                    goto cleanupbad;
                }
            }
            break;
        case MAC_DP3:
            pOutOp[0][0] = VerOp::MakeOp(VEROP_DP3, 6, &a[0][0], &a[1][0], &a[0][1], &a[1][1], &a[0][2], &a[1][2]);
            if(!pOutOp[0][0]) {
                SetErrStr("Interpret: VerOp::MakeOp(VEROP_DP3,6) failed");
                goto cleanupbad;
            }
            for(i = 1; i < 4; i++) {
                pOutOp[0][i] = pOutOp[0][0]->Copy();
            }
            break;
        case MAC_DPH:
            pOutOp[0][0] = VerOp::MakeOp(VEROP_DPH, 7, &a[0][0], &a[1][0], &a[0][1], &a[1][1], &a[0][2], &a[1][2], &a[1][3]);
            if(!pOutOp[0][0]) {
                SetErrStr("Interpret: VerOp::MakeOp(VEROP_DPH,7) failed");
                goto cleanupbad;
            }
            for(i = 1; i < 4; i++) {
                pOutOp[0][i] = pOutOp[0][0]->Copy();
            }
            break;
        case MAC_DP4:
            pOutOp[0][0] = VerOp::MakeOp(VEROP_DP4, 8, &a[0][0], &a[1][0], &a[0][1], &a[1][1], &a[0][2], &a[1][2], &a[0][3], &a[1][3]);
            if(!pOutOp[0][0]) {
                SetErrStr("Interpret: VerOp::MakeOp(VEROP_DP4,8) failed");
                goto cleanupbad;
            }
            for(i = 1; i < 4; i++) {
                pOutOp[0][i] = pOutOp[0][0]->Copy();
            }
            break;
        case MAC_DST:
            pOutOp[0][0] = VerOp::MakeOp(VEROP_DSTX, 0);
            pOutOp[0][1] = VerOp::MakeOp(VEROP_DSTY, 2, &a[0][1], &a[1][1]);
            pOutOp[0][2] = VerOp::MakeOp(VEROP_DSTZ, 1, &a[0][2]);
            pOutOp[0][3] = VerOp::MakeOp(VEROP_DSTW, 1, &a[1][3]);
            for(i = 0; i < 4; i++) {
                if(!pOutOp[0][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_DST,*) failed");
                    goto cleanupbad;
                }
            }
            break;
        case MAC_MIN:
            for(i = 0; i < 4; i++) {
                pOutOp[0][i] = VerOp::MakeOp(VEROP_MIN, 2, &a[0][i], &a[1][i]);
                if(!pOutOp[0][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_MIN,2) failed");
                    goto cleanupbad;
                }
            }
            break;
        case MAC_MAX:
            for(i = 0; i < 4; i++) {
                pOutOp[0][i] = VerOp::MakeOp(VEROP_MAX, 2, &a[0][i], &a[1][i]);
                if(!pOutOp[0][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_MAX,2) failed");
                    goto cleanupbad;
                }
            }
            break;
        case MAC_SLT:
            for(i = 0; i < 4; i++) {
                pOutOp[0][i] = VerOp::MakeOp(VEROP_SLT, 2, &a[0][i], &a[1][i]);
                if(!pOutOp[0][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_SLT,2) failed");
                    goto cleanupbad;
                }
            }
            break;
        case MAC_SGE:
            for(i = 0; i < 4; i++) {
                pOutOp[0][i] = VerOp::MakeOp(VEROP_SGE, 2, &a[0][i], &a[1][i]);
                if(!pOutOp[0][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_SGE,2) failed");
                    goto cleanupbad;
                }
            }
            break;
        case MAC_ARL:
            pOutOp[0][0] = VerOp::MakeOp(VEROP_NOP, 0);
            if(!pOutOp[0][0]) {
                SetErrStr("Interpret: VerOp::MakeOp(VEROP_NOP,0) failed");
                goto cleanupbad;
            }
            for(i = 1; i < 4; i++) {
                pOutOp[0][i] = pOutOp[0][0]->Copy();
            }

            //WEIRDNESS: write to arl in the table here. (Must occur after ARL is read from, above)
            if(false == mpNodes[REG_ARL * 4].CopyFrom(&a[0][0])) { // read from 'x' channel of input
                SetErrStr("CopyFrom, for ARL instruction failed");
                goto cleanupbad;
            }

            break;
        default:
            //unknown mac instruction
            SetErrStr("unknown mac instruction");
            break;
        }

        //look at the ILU instruction... create the appropriate output branch...
        switch(pcode->ilu) {
        case ILU_NOP:
            pOutOp[1][0] = VerOp::MakeOp(VEROP_NOPX, 0); 
            if(!pOutOp[1][0]) {
                SetErrStr("Interpret: VerOp::MakeOp(VEROP_NOPX,0) failed");
                goto cleanupbad;
            }
            for(i = 1; i < 4; i++) {
                pOutOp[1][i] = pOutOp[1][0]->Copy();
            }
            break;
        case ILU_MOV:
            for(i = 0; i < 4; i++) {
                if(!Out[1][i].CopyFrom(&a[2][i])) {
                    SetErrStr("Interpret: CopyFrom, for IMV failed");
                    goto cleanupbad;
                }
            }
            break;
        case ILU_RCP:
            pOutOp[1][0] = VerOp::MakeOp((VerOpcode)(VEROP_RCPX), 1, &a[2][0]);
            if(!pOutOp[1][0]) {
                SetErrStr("Interpret: VerOp::MakeOp(VEROP_RCPX,1) failed");
                goto cleanupbad;
            }
            for(i = 1; i < 4; i++) {
                pOutOp[1][i] = pOutOp[1][0]->Copy();
            }
            break;
        case ILU_RCC:
            pOutOp[1][0] = VerOp::MakeOp((VerOpcode)(VEROP_RCCX), 1, &a[2][0]);
            if(!pOutOp[1][0]) {
                SetErrStr("Interpret: VerOp::MakeOp(VEROP_RCCX,1) failed");
                goto cleanupbad;
            }
            for(i = 1; i < 4; i++) {
                pOutOp[1][i] = pOutOp[1][0]->Copy();
            }
            break;
        case ILU_RSQ:
            pOutOp[1][0] = VerOp::MakeOp((VerOpcode)(VEROP_RSQX), 1, &a[2][0]);
            if(!pOutOp[1][0]) {
                SetErrStr("Interpret: VerOp::MakeOp(VEROP_RSQX,1) failed");
                goto cleanupbad;
            }
            for(i = 1; i < 4; i++) {
                pOutOp[1][i] = pOutOp[1][0]->Copy();
            }
            break;
        case ILU_EXP:
            for(i = 0; i < 4; i++) {
                pOutOp[1][i] = VerOp::MakeOp((VerOpcode)(VEROP_EXPX + i), 1, &a[2][0]);
                if(!pOutOp[1][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_EXP,1) failed");
                    goto cleanupbad;
                }
            }
            break;
        case ILU_LOG:
            for(i = 0; i < 4; i++) {
                pOutOp[1][i] = VerOp::MakeOp((VerOpcode)(VEROP_LOGX + i), 1, &a[2][0]);
                if(!pOutOp[1][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_LOG,1) failed");
                    goto cleanupbad;
                }
            }
            break;
        case ILU_LIT:
            pOutOp[1][0] = VerOp::MakeOp(VEROP_LITX, 0);
            pOutOp[1][1] = VerOp::MakeOp(VEROP_LITY, 1, &a[2][0]); //x
            pOutOp[1][2] = VerOp::MakeOp(VEROP_LITZ, 3, &a[2][0], &a[2][1], &a[2][3]); //x, y, w
            pOutOp[1][3] = VerOp::MakeOp(VEROP_LITW, 0);
            for(i = 0; i < 4; i++) {
                if(!pOutOp[1][i]) {
                    SetErrStr("Interpret: VerOp::MakeOp(VEROP_LIT,*) failed");
                    goto cleanupbad;
                }
            }

            break;
        default:
            SetErrStr("unknown ilu instruction");
            break;
        }
        //all done with calculating instructions!
        //Now, build Out[0] and Out[1]. Only use OutOp if it isn't NULL!

        for(i = 0; i < 4; i++) {
            if(pOutOp[0][i]) {
                Out[0][i].UseOp(pOutOp[0][i]);
                pOutOp[0][i]->Release();
            } //else {} nothing. MOVs leave the OutOp as NULL, but modify Out directly.
            if(pOutOp[1][i]) {
                Out[1][i].UseOp(pOutOp[1][i]);
                pOutOp[1][i]->Release();
            } //else {} nothing. MOVs leave the OutOp as NULL, but modify Out directly.

        }

        //Out[0] contains results from the MAC instruction, Out[1] contains results from ILU. Now, update the register table...
        //if MAC != NOP, ILU writes to r1. 
        //if rw == 1 and MAC != NOP, and ILU != NOP, the MAC instruction is ignored.
        //if rw == 1, MAC != NOP, ILU == NOP, MAC writes to 1.

        rwm = pcode->rwm;
        iIluOutReg = (pcode->mac != 0) ? REG_R1 : (REG_R0 + pcode->rw);
        if(pcode->rw == 1 && pcode->mac != 0 && pcode->ilu != 0 && pcode->swm != 0) rwm = 0; //if ilu & mac conflict, ilu wins; mac writes nothing.


        for(i = 0; i < 4; i++) {
            //write to primary register
            if(rwm & (1 << (3 - i))) {
                if(!mpNodes[(REG_R0 + pcode->rw) * 4 + i].CopyFrom(&Out[0][i])) {
                        VER_ERR(("CopyFrom to Write to primary register failed. "));
                        return -1;
                }

                if(!mpNodes[(REG_R0 + pcode->rw) * 4 + i].TinyValidate()) {
                    return E_ASSEMBLER_FAILED;
                }
            }
            //write to secondary register
            if(pcode->swm & (1 << (3 - i))) {
                if(!mpNodes[(iIluOutReg) * 4 + i].CopyFrom(&Out[1][i])) {
                        VER_ERR(("CopyFrom to Write to ilu (or secondary) register failed. "));
                        return -1;
                }

                if(!mpNodes[(iIluOutReg) * 4 + i].TinyValidate()) {
                    return E_ASSEMBLER_FAILED;
                }
            }
            //write to output/constant register
            if(pcode->owm & (1 << (3 - i))) {
                if(pcode->oc & 0x100) {
                    //output register
                    if(!mpNodes[(REG_O0 + (pcode->oc & 0xff)) * 4 + i].CopyFrom(&Out[pcode->om][i])) {
                        VER_ERR(("CopyFrom to Write to output register failed. "));
                        return -1;
                    }

                    if(!mpNodes[(REG_O0 + (pcode->oc & 0xff)) * 4 + i].TinyValidate()) {
                        return E_ASSEMBLER_FAILED;
                    }
                } else {
                    //constant register!
                    br = true; //we have written to a constant!
                    if(!mpNodes[(REG_C0 + (pcode->oc & 0xff)) * 4 + i].CopyFrom(&Out[pcode->om][i])) {
                        VER_ERR(("CopyFrom to Write to constant register failed. "));
                        return -1;
                    }

                    if(!mpNodes[(REG_C0 + (pcode->oc & 0xff)) * 4 + i].TinyValidate()) {
                        return E_ASSEMBLER_FAILED;
                    }
                }
            }
        }

		//return 1 if we wrote to a constant, 0 if we didn't, or something else something bad happened
        return br ? 1 : 0;

cleanupbad:
        //this cleans up the ops that were created, since they won't be destroyed by c++ unless they're part of a node...
        VER_ERR((ErrStr));
        for(i = 0; i < 4; i++) {
            pOutOp[0][i]->Release();
            pOutOp[1][i]->Release();
        }
        return -1;

    }
};

//compare two VerNodes
//return true if they do the same thing, false if they're different
bool Compare(VerNode* pa, VerNode* pb)
{
#ifdef DBG_VERIFIER
#define Dif(_a_) do { VER_MSG(_a_); return false; } while (0)
#else
#define Dif(_a_) do { return false; } while (0)
#endif
    int i;
    if(pb->mbHasOp != pa->mbHasOp) Dif(("mbHasOp"));
    if(pb->mbHasReg != pa->mbHasReg) Dif(("mbHasReg"));
    if(pb->mbHasTable != pa->mbHasTable) Dif(("mbHasTable"));
    if(pb->mbNegated != pa->mbNegated) Dif(("mbNegated")); //bugbug: for later, the ZERO op won't matter if it's negated.

    if(pb->mbHasReg) {
        if(pb->mReg != pa->mReg) Dif(("mReg"));
    }
    if(pb->mbHasOp) {
        if(pb->mpOp == pa->mpOp) { //pointer to the same op...
        } else if(pa->mpOp->mpSame && pa->mpOp->mpSame->FindData(pb->mpOp)) {
        } else {
            if(pb->mpOp->mOp != pa->mpOp->mOp) {
                if(pb->mpOp->mOp == VEROP_MAD) (BreakUpMad(pb));
                if(pa->mpOp->mOp == VEROP_MAD) (BreakUpMad(pa));
                if(pb->mpOp->mOp != pa->mpOp->mOp) {
                    Dif(("mOp")); //bugbug:? make sure MADs are broken up first, and maybe DP*s.
                }
            }
            if(pb->mpOp->mNumParams != pa->mpOp->mNumParams) Dif(("mNumParams"));
            if(pb->mpOp->mOp == VEROP_ADD || pb->mpOp->mOp == VEROP_MUL || pb->mpOp->mOp == VEROP_MAX || pb->mpOp->mOp == VEROP_MIN) {
                //deal with the associative law in addition, mul, min, max...
                //bugbug: this only works if adding 2 things together. (add with >= 3 inputs will require more code when that is implimented)
                if(!Compare(&pa->mpOp->mpParams[0], &pb->mpOp->mpParams[0])) {
                    if(!Compare(&pa->mpOp->mpParams[1], &pb->mpOp->mpParams[0])) Dif(("%d: b[0] != a[1]", pb->mpOp->mOp));
                    if(!Compare(&pa->mpOp->mpParams[0], &pb->mpOp->mpParams[1])) Dif(("%d: b[1] != a[0]", pb->mpOp->mOp));
                } else {
                    if(!Compare(&pa->mpOp->mpParams[1], &pb->mpOp->mpParams[1])) Dif(("%d: b[1] != a[1]", pb->mpOp->mOp));
                }
            } else {
                for(i = 0; i < pb->mpOp->mNumParams; i++) {
                    if(!Compare(&pa->mpOp->mpParams[i], &pb->mpOp->mpParams[i])) Dif(("param check"));
                }
            }

            //ops are the same! add them to their list of "same buddies"
            if(!pa->mpOp->mpSame) {
                pa->mpOp->mpSame = new VerOpList; //bugbug: if allocation fails, it won't matter too much, but could slow things down a bit. This function has no way to return errors though.
                if(pa->mpOp->mpSame) pa->mpOp->mpSame->mpData = pb->mpOp;
            } else {
                pa->mpOp->mpSame->SetData(pb->mpOp);
            }
            if(!pb->mpOp->mpSame) {
                pb->mpOp->mpSame = new VerOpList;
                if(pb->mpOp->mpSame) pb->mpOp->mpSame->mpData = pa->mpOp;
            } else {
                pb->mpOp->mpSame->SetData(pa->mpOp);
            }
        }
    }
    if(pb->mbHasTable) {
        VER_MSG(("checking ARL"));
        if(!Compare(pa->mpArl, pb->mpArl)) Dif(("arl check"));

        if(pb->mpTable != pa->mpTable) {
            if(pb->mpTable->mNodeCount != pa->mpTable->mNodeCount) Dif(("table->mNodeCount"));
            for(i = (pb->mReg & 3); i < pb->mpTable->mNodeCount; i += 4) { //check only elements that could be used here.
                if(!Compare(&pa->mpTable->mpNodes[i], &pb->mpTable->mpNodes[i])) Dif(("table check"));
            }               
        } else { 
            //table pointers are the same - no point in checking individual elements...
        }
    }
    return true;
}

//update the table of constant registers to hold the most recent values
//keep the old tables around: if a0.x was previously used in an instruction, the VerNode that 
//is used in that instruction points to the state of the constant registers at that time. This 
//is used to prevent "aliasing" problems in the optimizers
bool VerConstTableList::SetData(VerTable*ptable) //returns false if allocations failed, true if worked
{ 
    bool br;
    if(mpData == 0) {
        mpData = VerTable::MakeTable(VER_NUM_CONSTS);
        if(!mpData) {
            VER_ERR(("VerConstTableList::SetData: MakeTable failed!"));
            return false;
        }
        br = mpData->VerTable::InitTable(&ptable->mpNodes[REG_C0 * 4]);       
        if(br == false) {
            VER_ERR(("VerConstTableList::SetData: InitTable failed"));
            return false;
        }
        
    } else {
        VerConstTableList* pc = this;
        while(pc->mpNext) { pc = pc->mpNext; }
        pc->mpNext = new VerConstTableList;
        if(!pc->mpNext) {
            VER_ERR(("VerConstTableList::SetData: new VerConstTableList failed"));
            return false;
        }
        br = pc->mpNext->SetData(ptable);
        if(br == false) {
            VER_ERR(("VerConstTableList::SetData failed"));
            return false;
        }
    }
    return true;
}

VerConstTableList::~VerConstTableList() {
    delete mpNext;
    mpNext = 0;
    delete mpData;
    mpData = 0;
}

//VERIFY! returns true if the two tables are the same, false if not.
bool Verify(VerTable* pa, VerTable* pb, XD3DXErrorLog* pErrorLog) 
{
    if(!pa || !pb) {
        VER_ERR(("Verify was passed null table pointers"));
        return false;
    }

    int i;

    //bugbug: Merge multiple Add, Mul, Min, Max ops together, to appropriately deal with commutitive/associative laws

    //verify output registers
    for(i = REG_O0 * 4; i < REG_O15 * 4 + 4; i++) {
        if(!Compare(&pa->mpNodes[i], &pb->mpNodes[i])) {
            char buf[100];
            int index = (i >> 2) - REG_O0;
            sprintf(buf, "Output register %s o[%d][%d] didn't match!", kOutNames[index], index, i & 3);
            if(pErrorLog){
                pErrorLog->Log(true, 1, 0, 0, buf);
            }
            VER_ERR((buf));
            return false;
        } else {
//          VER_MSG(("compare worked!"));
        }
    }
    //verify constant registers
    for(i = REG_C0 * 4; i < REG_C0 * 4 + VER_NUM_CONSTS; i++) {
        if(!Compare(&pa->mpNodes[i], &pb->mpNodes[i])) {
            char buf[100];
            sprintf(buf, "Constant register[%d][%d] didn't match!", (i >> 2) - REG_C0, i & 3);
            if(pErrorLog){
                pErrorLog->Log(true, 2, 0, 0, buf);
            }
            VER_ERR((buf));
            return false;
        }
    }
    //we don't care about r0-r15, or v* registers, since they either aren't used at the end of a shader, or never change

    return true;
}

//build a table for the vertex shader pb, compare it with table pa
bool Verify(VerTable* pa, D3DVertexShaderProgram * pb, const char* title) 
{
    VerConstTableList* pbc = 0;
    VerTable* pbt = 0;
    bool br = false;
    HRESULT hr = S_OK;

    hr = VerTable::BuildTable(pb, &pbc, &pbt);
    if(!pbt || FAILED(hr)) {
        if(hr == E_ASSEMBLER_FAILED) {
            VER_ERR(("It looks like %s broke something", title));
        } else {
            VER_ERR(("Verify: BuildTable failed"));
        }
        br = false;
        goto cleanup;
    }

    br = Verify(pa, pbt, NULL);
    if(!br) {
        VER_ERR(("It looks like %s broke something", title));
    } else {
        VER_MSG(("%s worked!", title));
    }

cleanup:
    pa->Propegate(ClobberSameList);
    delete pbt;
    delete pbc;

    return br;
}

//create tables for vertex shaders pa and pb. Compare the tables, delete them.
//returns true if the two D3DVertexShaderPrograms do the same thing, false otherwise
bool Verify(D3DVertexShaderProgram * pa, D3DVertexShaderProgram * pb, XD3DXErrorLog* pErrorLog) 
{
    VerConstTableList* pc[2] = {0, 0};
    VerTable* pt[2] = {0, 0};
    bool br = false;
    HRESULT hr = S_OK;

    hr = VerTable::BuildTable(pa, &pc[0], &pt[0]);
    if(SUCCEEDED(hr)) {
        hr = VerTable::BuildTable(pb, &pc[1], &pt[1]);
    }

    if(!pt[0] || !pt[1]) {
        if(hr == E_ASSEMBLER_FAILED) {
            VER_ERR(("Assembler failed. (bad value in register)"));
        }
        VER_ERR(("BuildTable failed. pt[0]:%p t[1]:%p  c[0]:%p, c[1]:%p", pt[0],pt[1],pc[0],pc[1]));
        goto cleanup;
    }
    
    br = Verify(pt[0], pt[1], pErrorLog);

cleanup:
    delete pt[0];
    delete pt[1];
    delete pc[0];
    delete pc[1];

    return br;
}

#endif //ndef DISABLE_VERIFIER






enum StallType {
	eNone               = ' ',  //no stall
	eRegInit            = 'A',  //stall from using a temp register too early
	eBypass             = 'B',  //bypass was used - can cause small shadow-stalls if a stall occurs next
	eStandard           = 'C',  //stall from trying to read something that isn't ready (pipeline issue)
	eShadow             = 'D',  //stall from reading a register that was written to in a stalling instruction after 1 or 2 instructions
	eStandardAndShadow  = 'E',  //stall from standard + using a stalled register
	eBypassAndShadow    = 'F',  //bypass used, shadow also occurred
	eStandardAndBypShad = 'G',  //stall, plus shadow from "... bypass, stall, ..."
	eArl                = 'H',  //stall from using a0.x right after it is written
	eArlShadow          = 'I',  //stall from using a0.x right after it is written + a shadow
	eXOutputsIsXCycles  = 'J',  //stall from other area of gpu
	eSingleThreaded     = 'K',  //stall from other area of gpu
	eSmallShader        = 'L'   //stall from 
};



const char* StallDescription[] = {
	"register initialization (first two instructions only)",
	"bypass (no stall)",
	"standard stall",
	"shadow-stall from using a register two instructions after it stalled",
	"standard stall plus shadow-stall from using a register the instruction after it stalled",
	"bypass plus shadow-stall from using a register the instruction after it stalled",
	"stall from stalling immediately after using a bypass",
	"stall from using a0.x the instruction after writing to it",
	"stall from using a0.x the instruction after writing to it stalled",
	"writing to X output registers causes a shader to take X cycles to execute",
	"18-cycle stall for using xvsw or xvss",
	"stall for shaders smaller than 2.5 cycles",
};

// Low level simulator.

class TLEngineSim{
public:
    TLEngineSim(){
    }

    ~TLEngineSim(){
    }


    HRESULT Initialize(DWORD shaderType, int NumInst){
        isMultiThreaded = SASMT_SHADERTYPE(shaderType) == SASMT_VERTEXSHADER;
        HRESULT hr = Clear(NumInst);
        return hr;
    }

    HRESULT Clear(int NumInst){
        HRESULT hr = S_OK;
        
        {
            for(int i = 0; i < 2; i++){
                c[i] = -6;
            }
        }

		for(int i = 0; i < 16; i++) {
			OutRegWritten[i] = false;
		}

        STscoreboard.Clear();
		scoreboard.Clear(NumInst);
        currentCycle = 0;
        return hr;
    }

    HRESULT Do(const D3DVsInstruction* pI, float& stall, StallType& reason) {
		reason = eNone;
        HRESULT hr = S_OK;
		if(isMultiThreaded) {
			float realStall = CalculateRealStall(pI, reason);
			currentCycle += realStall;
			if(realStall > .19) {
				//DoOut always calls StartNewInstruction. 
				//This call here is to eliminate a duplicate shadow in the following case:
				//dp4 r2, v0, v0  
				//dp4 r2, r2, r2   //stall
				//dp4 oPos, r2, r2 //shadow + stall
				//mul oD0, r2, r2  //this doesn't shadow on the hardware.
				scoreboard.StartNewInstruction();
			}
			hr = DoOut(pI, reason);
			currentCycle += 0.5f;
			stall = realStall;
			return hr;
		} else {
			DWORD realStall = CalculateRealStall(pI, reason);
			currentCycle += realStall;
			hr = STDoOut(pI);
			if(isMultiThreaded){
	            currentCycle += .5;
				stall = realStall / 3;
			}
			else {
	            currentCycle += 1;
				stall = (float)realStall;
			}
			return hr;
		}
    }

    bool IsStall(const D3DVsInstruction* pI){
		StallType dummyreason = eNone;
        return CalculateStall(pI, dummyreason) > 0;
    }

    float CalculateStall(const D3DVsInstruction* pI, StallType& reason){
        float stall = CalculateRealStall(pI, reason);
        return stall;
    }

	float CalculateFinalStall(StallType& reason)
	{
		if(isMultiThreaded) {
			int i;
			float numregswritten = 0;
			float stall = 0;
			reason = eNone;

			for (i = 0; i < 16; i++) {
				if(OutRegWritten[i]) {
					numregswritten += 1.0;
				}
			}
			if(numregswritten > currentCycle) {
				stall = numregswritten - currentCycle;
				reason = eXOutputsIsXCycles;
			}

			if(currentCycle < 2.2) {
				float newstall = 2.2 - currentCycle;
				if(stall < newstall) {
					stall = newstall;
					reason = eSmallShader;
				}
			}

			return stall;

		}
		else {
			reason = eSingleThreaded;
			return 18.0f;
		}
	}

    // Which type of bypass 
    enum OpType {
        eALU,
        eMLU,
        eILU,
		eARL,
        eOther // Includes NOPs
    };
    OpType MacOpType(const D3DVsInstruction* pI){
        static const OpType kOpType[] = {
            eOther,  // MAC_NOP
            eMLU,  // MAC_MOV
            eMLU,  // MAC_MUL
            eALU,   // MAC_ADD
            eALU,   // MAC_MAD
            eALU,   // MAC_DP3
            eALU,   // MAC_DPH
            eALU,   // MAC_DP4
            eMLU,  // MAC_DST
            eMLU,  // MAC_MIN
            eMLU,  // MAC_MAX
            eMLU,  // MAC_SLT
            eMLU,  // MAC_SGE
            eOther,  // MAC_ARL
            eOther,  // ?0xe Unused
            eOther   // ?0xf Unused
        };

		if(pI->ilu) {
			return eALU; //if ILU is active, MLU operations don't write to output until ILU is finished.
		} else {
			return kOpType[pI->mac];
		}
    }

    OpType ILUOpType(const D3DVsInstruction* pI){
        static const OpType kOpType[] = {
            eOther,   // ILU_NOP
            eILU,   // ILU_MOV
            eILU,   // ILU_RCP
            eILU,   // ILU_RCC
            eILU,   // ILU_RSQ
            eILU,   // ILU_EXP
            eILU,   // ILU_LOG
            eILU,   // ILU_LIT
        };
        return kOpType[pI->ilu];
    }

private:

    float CalculateRealStall(const D3DVsInstruction* pI, StallType& reason){
		reason = eNone;
        HRESULT hr = S_OK;
        
        // From emperical evidence on an NV20 we know that the stall is determined
        // by the time taken to fetch the A, B, and C
        // arguments, independent of whether the arguments are used by the opcodes.
        //
        // However, we also have been told (but haven't verified this) that the
        // bypass logic tracks which components have been read and written.
        //
        // That's why we compute the use masks here.
        //
        // See the Understanding the Xbox Vertex Shader Processor whitepaper for
        // details.
        
		//if the multi-threaded scoreboard is causing problems, change this to, "if (0)".
		if(isMultiThreaded) {

			float stall = 0.0f;
			UCHAR useMasks[3];
			for(int i = 0; i < 3; i++){
				useMasks[i] = ComputeEffectiveReadMask(pI, i);
			}
        
			float aready, bready, cready;
			StallType StallReason = eNone;

			aready = AReady(pI, useMasks[0], currentCycle, StallReason);
			reason = StallReason;
			stall = aready;

			bready = BReady(pI, useMasks[1], currentCycle, StallReason);
			if(bready > stall || reason == eNone) {
				reason = StallReason;
				stall = bready;
			}

			cready = CReady(pI, useMasks[2], currentCycle, StallReason);
			if(cready > stall || reason == eNone) {
				reason = StallReason;
				stall = cready;
			}

			return stall;
		} else {

			DWORD stall;
			UCHAR useMasks[3];
			for(int i = 0; i < 3; i++){
				useMasks[i] = ComputeEffectiveReadMask(pI, i);
			}
        
			int stallInc = isMultiThreaded ? 3 : 1;
			// We know we never stall more than five cycles....
			for(stall = 0; stall < 5; stall+= stallInc){
				if(STAReady(pI, useMasks[0], currentCycle + stall)
					&& STBReady(pI, useMasks[1], currentCycle + stall)
					&& STCReady(pI, useMasks[2], currentCycle + stall)) {
					break;
				}
			}
        
			return stall;
		}
    }

    DWORD STAReady(const D3DVsInstruction* pI, UCHAR useMask, int cycle){
        return STMuxReady(pI, pI->amx, pI->arr, pI->ane, pI->axs, pI->ays, pI->azs, pI->aws, 0, useMask, cycle);
    }
    DWORD STBReady(const D3DVsInstruction* pI, UCHAR useMask, int cycle){
        return STMuxReady(pI, pI->bmx, pI->brr, pI->bne, pI->bxs, pI->bys, pI->bzs, pI->bws, 1, useMask, cycle);
    }
    DWORD STCReady(const D3DVsInstruction* pI, UCHAR useMask, int cycle){
        return STMuxReady(pI, pI->cmx, pI->crr, pI->cne, pI->cxs, pI->cys, pI->czs, pI->cws, 2, useMask, cycle);
    }

    float AReady(const D3DVsInstruction* pI, UCHAR useMask, float cycle, StallType& reason){
        return MuxReady(pI, pI->amx, pI->arr, pI->ane, pI->axs, pI->ays, pI->azs, pI->aws, 0, useMask, cycle, reason);
    }
    float BReady(const D3DVsInstruction* pI, UCHAR useMask, float cycle, StallType& reason){
        return MuxReady(pI, pI->bmx, pI->brr, pI->bne, pI->bxs, pI->bys, pI->bzs, pI->bws, 1, useMask, cycle, reason);
    }
    float CReady(const D3DVsInstruction* pI, UCHAR useMask, float cycle, StallType& reason){
        return MuxReady(pI, pI->cmx, pI->crr, pI->cne, pI->cxs, pI->cys, pI->czs, pI->cws, 2, useMask, cycle, reason);
    }
    // Constant registers are divided into two banks

    static bool Bank(DWORD address){
        return (address >> 2) & 1;
    }

    HRESULT STDoOut(const D3DVsInstruction* pI){
        HRESULT hr = S_OK;
        if(pI->rwm){
            if(pI->rw >= 12){
                SETERROR(hr, E_FAIL);
            }
            STscoreboard.Start(MacOpType(pI), pI->rw, pI->rwm, currentCycle);
        }
        if(pI->swm){
            DWORD ilu_rw = (pI->mac && pI->ilu) ? 1 : pI->rw;
            STscoreboard.Start(ILUOpType(pI), ilu_rw, pI->swm, currentCycle);
        }
        if(pI->owm){
            bool oc_output = (pI->oc & 0x0100) != 0;
            DWORD oc_index = pI->oc & 0xff;
            if(oc_output){
                if(oc_index == 0){ // oPos is shadowed as r[12]
					//this block changed 23 Aug 2001 - to allow mac writes to oPos to use the MLU bypass in the simulation
                    if(pI->om == 0) { //if MAC writes to oPos
						STscoreboard.Start(MacOpType(pI), 12, pI->owm, currentCycle);
					} else { //if ILU writes to oPos
						STscoreboard.Start(ILUOpType(pI), 12, pI->owm, currentCycle);
					}
                }
            }
            else {
                if(oc_index >= 192){
                    SETERROR(hr, E_FAIL);
                }
                else {
                    c[Bank(oc_index)] = true;
                }
            }
        }
        return hr;
    }

	bool OutRegWritten[16];

	HRESULT DoOut(const D3DVsInstruction* pI, StallType reason){
        HRESULT hr = S_OK;

		scoreboard.StartNewInstruction();

        if(pI->rwm){
            if(pI->rw >= 12){
                SETERROR(hr, E_FAIL);
            }
            scoreboard.Start(MacOpType(pI), pI->rw, pI->rwm, currentCycle, reason);
        }
        if(pI->swm){
            DWORD ilu_rw = (pI->mac && pI->ilu) ? 1 : pI->rw;
            scoreboard.Start(ILUOpType(pI), ilu_rw, pI->swm, currentCycle, reason);
        }
        if(pI->owm){
            bool oc_output = (pI->oc & 0x0100) != 0;
            DWORD oc_index = pI->oc & 0xff;
            if(oc_output){
				OutRegWritten[oc_index] = true;
                if(oc_index == 0){ // oPos is shadowed as r[12]
                    if(pI->om == 0) { //if MAC writes to oPos
						scoreboard.Start(MacOpType(pI), 12, pI->owm, currentCycle, reason);
					} else { //if ILU writes to oPos
						scoreboard.Start(ILUOpType(pI), 12, pI->owm, currentCycle, reason);
					}
                }
            }
        }

		if(pI->mac == 13) {
			scoreboard.Start(eARL, 13, 1, currentCycle, reason); //for the .16-cycle arl stall
		}

        return hr;
    }

    bool STMuxReady(const D3DVsInstruction* pI,
        DWORD mx, UCHAR rr, DWORD ne, DWORD xs, DWORD ys, 
        DWORD zs, DWORD ws, UCHAR muxIndex, UCHAR useMask, int cycle){
        // we don't care about ne at all
        switch(mx){
        case MX_R:
            {
                // See how many cycles until we're ready
                return STRegReady(pI, muxIndex, rr, useMask, cycle);
            }
        case MX_V:
            return true; // reading from inputs doesn't stall.
        case MX_C:
            {
                int age = cycle - c[Bank(pI->crr)];
                return age >= 6;
            }
        default:
            return true;
        }
    }

	float MuxReady(const D3DVsInstruction* pI,
        DWORD mx, UCHAR rr, DWORD ne, DWORD xs, DWORD ys, 
        DWORD zs, DWORD ws, UCHAR muxIndex, UCHAR useMask, float cycle, StallType& reason){
		
		reason = eNone;
        // we don't care about ne at all
        switch(mx){
        case MX_R:
            {
                // See how many cycles until we're ready
                return RegReady(pI, muxIndex, rr, useMask, cycle, reason);
            }
        case MX_V:
            return 0.0f; // reading from inputs doesn't stall.
        case MX_C:
			//reading from constants doesn't stall unless you just wrote to a0.x

			if(pI->cin) {
				return scoreboard.Ready(13, 0 /*, cycle*/, false, reason);
			}
            return 0.0f; 

        default:
            return 0.0f; //???
        }
    }

    bool STRegReady(const D3DVsInstruction* pI, UCHAR muxIndex, UCHAR rr, UCHAR useMask, int cycle){
        for(int c = 0; c < 4; c++){
            UCHAR cMask = 1 << c;
            if(useMask & cMask){
                if( STscoreboard.Ready(rr, c, cycle)) {
                    continue; // No stall from this component
                }
                else if (STscoreboard.TakeBypass(pI, muxIndex, rr, cMask, cycle))
                {
                    continue; // Bypass worked
                }
                return false; // not ready
            }
        }
        return true;
    }

	float RegReady(const D3DVsInstruction* pI, UCHAR muxIndex, UCHAR rr, UCHAR useMask, float cycle, StallType& reason){
		reason = eNone;
		float stall = 0.0f;
        for(int c = 0; c < 4; c++){
            UCHAR cMask = 1 << c;
            if(useMask & cMask){
				bool IsAluCInput;
				if(muxIndex == 2 && !pI->ilu) {
					IsAluCInput = true;
				} else {
					IsAluCInput = false;
				}

				StallType tempreason = eNone;
				float whenready = scoreboard.Ready(rr, c /*,cycle*/, IsAluCInput, tempreason);
				if(stall <= whenready) {
					stall = whenready;
					reason = tempreason;
				}
            }
        }
        return stall;
    }


//    bool isVertexShader;
//    bool isReadWriteShader;
    bool isMultiThreaded;

    float currentCycle; // In cycles, what cycle we're currently on. Starts at zero

    int c[2]; // For vertex state shaders and read/write vertex shaders
                // c[0] means constants in bank 0 (c[0]..c[3], etc.)
                // c[1] means constants in bank 1 (c[4]..c[7], etc.)

    // for calculating bypass.

    struct STRegScoreboard {
        int resultCycle[13][4];
        OpType resultOpType[13][4];
        void Clear() {
            for(int i = 0; i < 13;i++){
                for(int j = 0; j < 4; j++){
                    resultCycle[i][j] = -6;
                    resultOpType[i][j] = eOther;
                }
            }
        }

        void Start(OpType opType, UCHAR r, UCHAR useMask, int cycle){
            for(int c = 0; c < 4;c++){
                if((1 << c) & useMask){
                    resultCycle[r][c] = cycle + 6;
                    resultOpType[r][c] = opType;
                }
            }
        }

        bool Ready(UCHAR r, int c, int cycle){
            return resultCycle[r][c] <= cycle;
        }

        bool TakeBypass(const D3DVsInstruction* pI, DWORD muxIndex, UCHAR r, UCHAR useMask, int cycle){
            bool bTakeBypass = false;
            for(int c = 0; c < 4; c++){
                if((1 << c) & useMask){
                    // Is it 3 cycles since the operation occured
                    if(cycle != resultCycle[r][c] - 3){
                        return false; // Nope, not the bypass cycle
                    }
                    // MLU bypass
                    if(resultOpType[r][c] == eMLU){
                        bTakeBypass = true;
                        continue;
                    }
                    // Thes other two bypasses are for the C input of ADD and MAD only
                    if(muxIndex != 2){
                        return false;
                    }
                    if(pI->mac != MAC_ADD && pI->mac != MAC_MAD){
                        return false;
                    }

                    if(pI->ilu ){
                        // Check whether the ILU instruction actually uses this component
                        // before bailing.
                        UCHAR useMasks[3];
                        D3DVsInstruction test = *pI;
                        StripMacInstruction(&test);
                        ComputePostSwizzleUseMasks(&test, useMasks);
                        if(useMasks[2] & (1 << c)){
                            // Need to wait for ILU instruction, even if mac instruction's OK
                            return false;
                        }
                    }

                    // ALU bypass
                    if(resultOpType[r][c] == eALU && muxIndex == 2){
                        bTakeBypass = true;
                        continue;
                    }
                    // ILU bypass
                    if(resultOpType[r][c] == eILU && muxIndex == 2){
                        bTakeBypass = true;
                        continue;
                    }
                    return false;
                }
            }
            return bTakeBypass;
        }
    };

    STRegScoreboard STscoreboard;


    struct RegScoreboard {
        float resultCycle       [14][4];     //stall if used by MLU or ILU
        float resultCycleAlu    [14][4];  //stall if used by ALU's second input
        float resultNextCycle   [14][4]; //stall if used in the next instruction 
        float resultNextCycleAlu[14][4]; //stall if used in the next instruction's Alu (note: usually the same as NextCycle non-alu) 

		StallType resultReason       [14][4];
		StallType resultReasonAlu    [14][4];
		StallType resultNextReason   [14][4];
		StallType resultNextReasonAlu[14][4];

		int numCycles;

        void Clear(int NumCycles) {
            for(int i = 0; i < 13;i++){
                for(int j = 0; j < 4; j++){
                    resultCycle       [i][j] = 2.5f; //the first instruction, temp registers stall 2.5 cycles (if you read them uninitialized)
                    resultCycleAlu    [i][j] = 2.5f; 
					//If you write a temp register in the first inst, then read it in the second, it stalls about 1.5
					//However, if it stalls in this manner, it will remove any stall in the next instruction
                    resultNextCycle   [i][j] = 1.5f; 
                    resultNextCycleAlu[i][j] = 1.5f;
					resultReason       [i][j] = eRegInit;
					resultReasonAlu    [i][j] = eRegInit;
					resultNextReason   [i][j] = eRegInit;
					resultNextReasonAlu[i][j] = eRegInit;

                }
            }
			resultCycle[13][0] = 0.0f;
			resultReason[13][0] = eNone;
			numCycles = NumCycles;
        }

		void StartNewInstruction()
		{
			resultCycle[13][0] = 0.0f;
			resultReason[13][0] = eNone;
			
            for(int i = 0; i < 14;i++){
                for(int j = 0; j < 4; j++){
					resultCycle[i][j] = resultNextCycle[i][j];
					resultReason[i][j] = resultNextReason[i][j];
					resultCycleAlu[i][j] = resultNextCycleAlu[i][j];
					resultReasonAlu[i][j] = resultNextReasonAlu[i][j];

                    resultNextCycle   [i][j] = 0.0f;
                    resultNextCycleAlu[i][j] = 0.0f;
					resultNextReason   [i][j] = eNone;
					resultNextReasonAlu[i][j] = eNone;
				}
			}
		}

        void Start(OpType opType, UCHAR r, UCHAR useMask, float cycle, StallType reason){
			//reason == reason the current instruction stalled / didn't stall
			//opType: if eMLU, it can use the MLU bypass
			//r: the particular register being written
			//usemask: xyzw components of the register being written
			//cycle: the current cycle of the program.
			
			//a0.x's stall and shadow are smaller than everything else's, so deal with that seperately.
			int c; 
			if(r == 13) {
				float a0xstall = 0.17f;
				StallType a0xreason = eArl;
				
				if(reason == eStandard || reason == eStandardAndShadow || reason == eStandardAndBypShad) {
					//a0.x's shadow only appears in larger shaders, when writing to a0.x stalls after the third instruction.
					//its shadow also does not occur in the second instruction, like standard stall's shadows do.
					if(numCycles > 10 && cycle > 2) {
						a0xstall = 0.33f;
					}
					StallType a0xreason = eArlShadow;
				}

				resultCycle[r][0] = a0xstall;
				resultReason[r][0] = a0xreason;
				resultCycleAlu[r][0] = a0xstall;
				resultReasonAlu[r][0] = a0xreason;

				resultNextCycle[r][0]    = 0.0f;
				resultNextReason[r][0]   = eNone;
				resultNextCycleAlu[r][0] = 0.0f;
				resultNextReasonAlu[r][0] = eNone;
				return;
			}

            for(c = 0; c < 4;c++){
                if((1 << c) & useMask){

					switch (reason) 
					{
					case eRegInit:  //doesn't cuase any further problems, and does eliminate any future initialization issues
						resultNextCycle[r][c]    = 0.0f;
						resultNextReason[r][c]   = eNone;
						resultNextCycleAlu[r][c] = 0.0f;
						resultNextReasonAlu[r][c] = eNone;
						resultCycle[r][c]    = 0.0f;
						resultReason[r][c]   = eBypass;
						resultCycleAlu[r][c] = 0.0f;
						resultReasonAlu[r][c] = eBypass;
						break;

					case eNone:      //no stall or bypass
					case eShadow:    //we just dealt with a shadow, but no normal stalls. Continue as normal.
					case eArl:       //a0.x stalled, but that won't cause shadow-stalls though
					case eArlShadow: //a0.x stalled & dropped a shadow, but that won't cause shadow-stalls though

						if(opType == eMLU) { //(mac instructions that only use the mlu)

							resultNextCycle[r][c]    = 0.0f;
							resultNextReason[r][c]   = eNone;
							resultNextCycleAlu[r][c] = 0.0f;
							resultNextReasonAlu[r][c] = eNone;

							//if we just waited for register initialization, we don't have to worry about it again.
							//If the register hasn't been initialized by hardware yet, we can't make that stall go away.

							if(resultReason[r][c] != eRegInit && reason != eRegInit) {
								resultCycle[r][c]    = 0.0f;
								resultCycleAlu[r][c] = 0.0f;
								resultReason[r][c]   = eBypass;
								resultReasonAlu[r][c] = eBypass;

							} else {
								//Registers suffering from initialization stalls don't cause shadows.
								//In fact, if a register stalls from both initialization and normal stall, 
								//  it casts a 0.1 cycle anti-shadow. I'm not sure how to deal with that 
								//  though, and it is somewhat minor in a situation that should be avoided anyway...
								//Nothing can change how long register initialization takes though
							}

						} else //(ilu) or (mac instruction that uses alu)
						{
							resultNextCycle[r][c]    = 0.0f;
							resultNextReason[r][c]   = eNone;
							resultNextCycleAlu[r][c] = 0.0f;
							resultNextReasonAlu[r][c] = eNone;
							
							if(resultReason[r][c] != eRegInit && reason != eRegInit) {
								resultCycle[r][c]    = 0.5f;
								resultReason[r][c]   = eStandard;
								resultCycleAlu[r][c] = 0.0f;
								resultReasonAlu[r][c] = eBypass;
							}
						}
						break;


					case eStandard:
					case eStandardAndShadow:
					case eStandardAndBypShad:
						//can cast lots of different shadows

						{
							float twoshadow = 0.4f;
							if((numCycles < 10) || (cycle < 4)) twoshadow = 0.2f;

							resultNextCycle[r][c]    = twoshadow;
							resultNextReason[r][c]   = eShadow;
							resultNextCycleAlu[r][c] = twoshadow;
							resultNextReasonAlu[r][c] = eShadow;
						}

						if(opType == eMLU) { //(mac instructions that only use the mlu)

							if(resultReason[r][c] != eRegInit) 
							{
								//... stall, bypass, ...

								//the rule: if three-instruction group starts on the third, fifth, or seventh cycle, the stall is only .2.
								//"fifth" being one-based, not zero-based. Meaning, cycle == 2.0 on the previous instruction, so
								//3.0 for the current instruction (hence, 2, 3, 4)...

								if(Epsilon(cycle, 2, .3f) || Epsilon(cycle, 3, .3f) || Epsilon(cycle, 4, .3f) || (numCycles >= 5 && numCycles <= 9)) 
								{
									resultCycle[r][c] = 0.2f;
									resultReason[r][c] = eBypassAndShadow;
									resultCycleAlu[r][c] = 0.2f;
									resultReasonAlu[r][c] = eBypassAndShadow;
								} else {
									resultCycle[r][c] = 0.5f;
									resultReason[r][c] = eBypassAndShadow;
									resultCycleAlu[r][c] = 0.5f;
									resultReasonAlu[r][c] = eBypassAndShadow;
								}
							}
						} 
						else { //ALU, MLU+ALU, or ILU instructions 

							if(resultReason[r][c] != eRegInit) {

								//if next inst is ALU-only,
								//   ... stall, bypass, ...
								//else
								//   ... stall, stall, ...
								//

								if(Epsilon(cycle, 2, .3f) || Epsilon(cycle, 3, .3f) || Epsilon(cycle, 4, .3f) || (numCycles >= 5 && numCycles <= 9)) 
								{
									resultCycleAlu[r][c] = 0.2f;
									resultReasonAlu[r][c] = eBypassAndShadow;
								} else {
									resultCycleAlu[r][c] = 0.5f;
									resultReasonAlu[r][c] = eBypassAndShadow;
								}

								if(numCycles < 10) {
									resultCycle[r][c]    = 0.5f;
									resultReason[r][c]   = eStandard;
								} else if (Epsilon(cycle, 2, .3f) || Epsilon(cycle, 3, .3f)) {
									resultCycle[r][c]    = 1.3f;
									resultReason[r][c]   = eStandardAndShadow;									
								} else {
									resultCycle[r][c]    = 1.05f;
									resultReason[r][c]   = eStandardAndShadow;									
								}
							}
						}

						break;

					case eBypass:
					case eBypassAndShadow:
						//mostly normal, unless a stall occurs

						if(opType == eMLU) { //(mac instructions that only use the mlu)

							//... bypass, bypass, ...

							//we can't stall, and bypasses don't case shadows unless followed by a stall
							resultNextCycle[r][c]    = 0.0f;
							resultNextReason[r][c]   = eNone;
							resultNextCycleAlu[r][c] = 0.0f;
							resultNextReasonAlu[r][c] = eNone;

							if(resultReason[r][c]  != eRegInit) {
								resultCycle[r][c]    = 0.0f;
								resultReason[r][c]   = eBypass;
								resultCycleAlu[r][c] = 0.0f;
								resultReasonAlu[r][c] = eBypass;

							}

						} else { //ALU, MLU+ALU, or ILU instructions.
							//if next inst is ALU only, 
							// ... bypass, bypass ...
							//else
							// ... bypass, stall ...

							resultNextCycle[r][c]    = 0.0f;
							resultNextReason[r][c]   = eNone;
							resultNextCycleAlu[r][c] = 0.0f;
							resultNextReasonAlu[r][c] = eNone;

							if(resultReason[r][c] != eRegInit) {

								if(Epsilon(cycle, 1, .3f)) {
									resultCycle[r][c]    = 0.6f;
									resultReason[r][c]   = eStandardAndBypShad;
								} else if (Epsilon(cycle, 3, .3f) || Epsilon(cycle, 3.5, .3f)) {
									resultCycle[r][c]    = 0.72f;
									resultReason[r][c]   = eStandardAndBypShad;
								} else {
									resultCycle[r][c]    = 0.83f;
									resultReason[r][c]   = eStandardAndBypShad;
								}

								resultCycleAlu[r][c] = 0.0f;
								resultReasonAlu[r][c] = eBypass;
							}
						}

						break;
					}
                }
            }
        }

        float Ready(UCHAR r, int c, /*int cycle,*/ bool IsAluCInput, StallType& reason){
			float stall;
			if(IsAluCInput) {
				reason = resultReasonAlu[r][c];
				stall  = resultCycleAlu[r][c];
			} else {
				reason = resultReason[r][c];
				stall  = resultCycle[r][c];
			}
			return stall;
        }


    };

    RegScoreboard scoreboard;


};


// Dead code stripper

class DeadCodeStripper {
    struct RegMask {
        UCHAR reg;
        UCHAR mask;
    };
    struct OutRegMaskSet {
        UCHAR CombinedWriteMask() const { return o.mask | r.mask | s.mask; }
        UCHAR MacWriteMask() const { return r.mask | ( outIsMac ? o.mask : 0); }
        UCHAR IluWriteMask() const { return s.mask | (!outIsMac ? o.mask : 0); }
        bool outIsMac;
        RegMask o;
        RegMask r; // mac (also ARL)
        RegMask s; // ilu
    };
    struct InRegMaskSet {
        RegMask a;
        RegMask b;
        RegMask c;
        RegMask a0; // Index register
    };
    struct FullRegMaskSet {
        InRegMaskSet in;
        OutRegMaskSet out;
    };

public:
    DeadCodeStripper(){
    }
    
    ~DeadCodeStripper(){
    }
    
    HRESULT Run(DWORD shaderType, D3DVertexShaderProgram* ucode){
        HRESULT hr = S_OK;
        hr = InitRegisters(shaderType);

        // Copy the ucode program backwards into a temporary buffer,
        // skipping instructions with no effect, and narrowing masks
        // as we go.
        D3DVsInstruction* temp = new D3DVsInstruction[MAX_MICROCODE_LENGTH];
		if(temp == NULL) {
			return E_OUTOFMEMORY;
		}
        D3DVsInstruction* pIn = & ucode->ucode[ucode->length - 1];
        D3DVsInstruction* pOut = temp;
        D3DVsInstruction* pInEnd = & ucode->ucode[-1];
        for(;pIn > pInEnd && SUCCEEDED(hr); pIn--){
#ifdef DBG_STRIPPER
            DPF2("%d: ", pIn - pInEnd + 1);
            Print(*pIn);
#endif
            hr = ProcessInstruction(pIn,pOut);
        }
        if(SUCCEEDED(hr)){
            // Copy the stripped program backwards into the original program
            DWORD newLength = pOut - temp;
            for(DWORD i = 0; i < newLength; i++){
                ucode->ucode[i] = temp[newLength - (i+1)];
            }
            ucode->length = newLength;
        }
#ifdef DBG_STRIPPER
        if(SUCCEEDED(hr)){
            DPF2("Post-stripped code\n");
            Print(shaderType, *ucode);
        }
#endif
        delete [] temp;
        return hr;
    }

    HRESULT ProcessInstruction(const D3DVsInstruction* pIn, D3DVsInstruction*& pOut){
        HRESULT hr = S_OK;
        D3DVsInstruction temp = *pIn;
        FullRegMaskSet masks;
        memset(&masks,0,sizeof(masks));
        hr = CalcOutputMasks(temp, masks.out);
#ifdef DBG_STRIPPER
            PrintOutMasks("initial out", masks.out);
#endif
        if(SUCCEEDED(hr)){
            hr = NarrowOutputMasks(temp, masks.out);
        }
#ifdef DBG_STRIPPER
            PrintOutMasks("after NarrowOutputMasks out", masks.out);
#endif
        if(SUCCEEDED(hr)){
            hr = UpdateCode(temp, masks.out);
        }
        if(SUCCEEDED(hr)){
            hr = RemoveNullMoves(temp);
        }

#ifdef DBG_STRIPPER
        if(SUCCEEDED(hr)){
            DPF2("updated: ");
            Print(temp);
        }
#endif
        if(SUCCEEDED(hr)){
            if(! IsNOP(temp) && masks.out.CombinedWriteMask() ) {
                hr = CalcInputMasks(temp, masks.out, masks.in);
                // We're moving backwards in time, so we record the outputs before the inputs.
                // This is important when the same register is used for both input and output.
                if(SUCCEEDED(hr)){
                    hr = RecordOutputMasks(temp, masks.out);
                }
                if(SUCCEEDED(hr)){
                    hr = RecordInputMasks(temp, masks.in);
                }
                if(SUCCEEDED(hr) && (temp.ilu || temp.mac)){
                    *pOut++ = temp;
                }
            }
            else {
#ifdef DBG_STRIPPER
                DPF2("Removing dead code instruction\n");
#endif
            }
        }
        return hr;
    }

    // Is the temp-register mac portion of this instruction is a mov of a register to itself
    static bool IsMacMovRegNOP(const D3DVsInstruction& code){
        return code.mac == MAC_MOV
            && code.amx == MX_R
            && code.rwm
            && code.arr == code.rw
            && ! code.ane
            && (! (code.rwm & 8) || code.axs == CSW_X)
            && (! (code.rwm & 4) || code.ays == CSW_Y)
            && (! (code.rwm & 2) || code.azs == CSW_Z)
            && (! (code.rwm & 1) || code.aws == CSW_W)
            ;
    }

    // Is the output-register mac portion of this instruction is a mov of a constant to itself
    static bool IsMacMovConstNOP(const D3DVsInstruction& code){
        // TODO: Check for mov oPos,r12
        return code.mac == MAC_MOV
            && code.amx == MX_C
            && ! code.cin
            && code.owm && code.om == OM_MAC && (code.oc & 0x100) == 0 // writing to constant register
            && (code.oc & 0xff) == code.ca // reading & writing from same constant register
            && ! code.ane
            && (! (code.owm & 8) || code.axs == CSW_X)
            && (! (code.owm & 4) || code.ays == CSW_Y)
            && (! (code.owm & 2) || code.azs == CSW_Z)
            && (! (code.owm & 1) || code.aws == CSW_W)
            ;
    }

    // Is the temp-register ilu portion of this instruction is a mov of a register to itself
    static bool IsIluMovRegNOP(const D3DVsInstruction& code){
        return code.ilu == ILU_MOV
            && code.cmx == MX_R
            && code.swm
            && ((code.crr == code.rw && ! code.mac) || (code.mac && code.crr == 1))
            && ! code.cne
            && (! (code.swm & 8) || code.cxs == CSW_X)
            && (! (code.swm & 4) || code.cys == CSW_Y)
            && (! (code.swm & 2) || code.czs == CSW_Z)
            && (! (code.swm & 1) || code.cws == CSW_W)
            ;
    }

    // Is the output-register ilu portion of this instruction is a mov of a constant to itself
    static bool IsIluMovConstNOP(const D3DVsInstruction& code){
        // TODO: Check for mov oPos,r12
        return code.ilu == ILU_MOV
            && code.cmx == MX_C
            && ! code.cin
            && code.owm && code.om == OM_ILU && (code.oc & 0x100) == 0 // writing to constant register
            && (code.oc & 0xff) == code.ca // reading & writing from same constant register
            && ! code.cne
            && (! (code.owm & 8) || code.cxs == CSW_X)
            && (! (code.owm & 4) || code.cys == CSW_Y)
            && (! (code.owm & 2) || code.czs == CSW_Z)
            && (! (code.owm & 1) || code.cws == CSW_W)
            ;
    }

    static HRESULT RemoveNullMoves(D3DVsInstruction& code){
        if(IsMacMovRegNOP(code)){
            code.rwm = 0;
        }
        if(IsMacMovConstNOP(code)){
            code.owm = 0;
        }
        if(IsIluMovRegNOP(code)){
            code.swm = 0;
        }
        if(IsIluMovConstNOP(code)){
            code.owm = 0;
        }
        
        return S_OK;
    }
    
    static bool IsNOP(const D3DVsInstruction& code){
        bool macIsNOP = code.mac == 0;
        bool iluIsNOP = code.ilu == 0;
        if(!macIsNOP){
            macIsNOP = ((! (code.om == OM_MAC && code.owm)) && IsMacMovRegNOP(code))
                   || ((! code.rwm) && IsMacMovConstNOP(code));
        }
        if(macIsNOP && !iluIsNOP){
            iluIsNOP = ((! (code.om == OM_ILU && code.owm)) && IsIluMovRegNOP(code))
                   || ((! code.swm) && IsIluMovConstNOP(code));
        }
        if (macIsNOP && iluIsNOP){
            return true;
        }
        return false;
    }

    static HRESULT CalcOutputMasks(const D3DVsInstruction& code, OutRegMaskSet& out){
        HRESULT hr = S_OK;
        memset(&out,0,sizeof(out));
        if(code.mac){
            // MAC output masks
            if(code.mac == MAC_ARL){
                out.r.mask = 8; // Just x register.
                out.r.reg = REG_ARL;
            }
            if(code.rwm) {
                out.r.mask = code.rwm;
                out.r.reg = REG_R0 + code.rw;
            }
            if(code.om == OM_MAC && code.owm){
                out.outIsMac = true;
                out.o.mask = code.owm;
                out.o.reg = (code.oc & 0x100) ? REG_O0 + (code.oc & 0xff) : REG_C0 + (code.oc & 0xff);
            }
        }
        
        if(SUCCEEDED(hr)){
            if(code.ilu){
                if(code.om == OM_ILU && code.owm){
                    out.outIsMac = false;
                    out.o.mask = code.owm;
                    out.o.reg = (code.oc & 0x100) ? REG_O0 + (code.oc & 0xff) : REG_C0 + (code.oc & 0xff);
                }
                if(code.swm) {
                    out.s.mask = code.swm;
                    if(code.mac){
                        out.s.reg = REG_R1;
                    }
                    else {
                        out.s.reg = REG_R0 + code.rw;
                    }
                }
            }
        }
        return hr;
    }

    static HRESULT UpdateCode(D3DVsInstruction& code, const OutRegMaskSet& out){
        HRESULT hr = S_OK;

        bool oldMac = code.mac != 0;
        
        if(code.mac){
            if(!out.MacWriteMask()){
                code.mac = 0;
                code.rwm = 0;
                if(code.om == OM_MAC){
                    code.owm = 0;
                    code.oc = 0x1ff;
                }
            }
            else {
                // MAC output masks
                if(code.mac == MAC_ARL){
                    // do nothing
                }
                if(code.om == OM_MAC && code.owm){
                    code.owm = out.o.mask;
                }
                if(code.rwm) {
                    code.rwm = out.r.mask;
                }
            }
        }
        
        if(SUCCEEDED(hr)){
            if(code.ilu){
                if(!out.IluWriteMask()){
                    code.ilu = 0;
                    code.swm = 0;
                    if(code.om == OM_ILU){
                        code.owm = 0;
                        code.oc = 0x1ff;
                    }
                }
                else {
                    if(code.om == OM_ILU && code.owm){
                        code.owm = out.o.mask;
                    }
                    if(code.swm) {
                        code.swm = out.s.mask;
                        // Check for special case - was paired instruction,
                        // but is now not a paired instruction, so we're now
                        // responsible for setting rw to 1.
                        if(oldMac && ! code.mac){
                            code.rw = 1;
                        }
                    }
                }
            }
        }
        return hr;
    }

    // See also CVSInstruction::CalculateComponentReadMasks

    static HRESULT CalcInputMasks(const D3DVsInstruction& code, const OutRegMaskSet& out, InRegMaskSet& in){
        HRESULT hr = S_OK;

        in.a0.reg = REG_ZER; //unused enumeration

        UCHAR aMask = 0;
        UCHAR bMask = 0;
        UCHAR cMask = 0;
        switch(code.mac){
        case MAC_NOP:
            // nothing to do
            break;

            // Passthru - read what they write
        case MAC_MOV:
            aMask = out.MacWriteMask();
            break;
        case MAC_MUL:
        case MAC_MIN:
        case MAC_MAX:
        case MAC_SLT:
        case MAC_SGE:
            aMask =
                bMask = out.MacWriteMask();
            break;
        case MAC_ADD:
            aMask =
                cMask = out.MacWriteMask();
            break;
        case MAC_MAD:
            aMask =
                bMask =
                cMask = out.MacWriteMask();
            break;

        case MAC_DP3:
            aMask = 0xe; // xyz
            bMask = 0xe; // xyz
            break;
        case MAC_DPH:
            aMask = 0xe; // xyz
            bMask = 0xf; // xyzw
            break;
        case MAC_DP4:
            aMask = 0xf; // xyzw
            bMask = 0xf; // xyzw
            break;
        case MAC_DST:
            aMask = 0x6; //  yz
            bMask = 0x5; //  y w
            break;

        case MAC_ARL:
            aMask = 0x8; // x
            break;
        default:
            DXGRIP("Unknown MAC opcode");
            SETERROR(hr, E_FAIL);
            break;
        }

        switch(code.ilu){
        case ILU_NOP:
            break;
        case ILU_MOV:
            cMask |= out.IluWriteMask();
            break;
        case ILU_RCP:
        case ILU_RCC:
        case ILU_RSQ:
        case ILU_EXP:
        case ILU_LOG:
            cMask |= 0x8; // x , yes x not w
            break;
        case ILU_LIT:
            cMask |= 0xd; // xy w
            break;
        default:
            DXGRIP("Unknown ILU opcode");
            SETERROR(hr, E_FAIL);
            break;
        }

        // aMask, bMask, and cMask are the post-swizzle masks. Un-swizzle them
        if(SUCCEEDED(hr)){
            hr = UnswizzleMask(in.a.mask, aMask, code.axs, code.ays, code.azs, code.aws);
        }
        if(SUCCEEDED(hr)){
            hr = UnswizzleMask(in.b.mask, bMask, code.bxs, code.bys, code.bzs, code.bws);
        }
        if(SUCCEEDED(hr)){
            hr = UnswizzleMask(in.c.mask, cMask, code.cxs, code.cys, code.czs, code.cws);
        }

        if(SUCCEEDED(hr)){
            hr = CalcInputReg(aMask, in, in.a.reg, code, code.arr, code.amx);
        }
        if(SUCCEEDED(hr)){
            hr = CalcInputReg(bMask, in, in.b.reg, code, code.brr, code.bmx);
        }
        if(SUCCEEDED(hr)){
            hr = CalcInputReg(cMask, in, in.c.reg, code, code.crr, code.cmx);
        }
        return hr;
    }

    static HRESULT UnswizzleMask(UCHAR& mask, UCHAR swizMask,
        WORD xs,  WORD ys, WORD zs,  WORD ws)
    {
        HRESULT hr = S_OK;
        // Mask means "reads this component. bit 0 == w, 1 = z, 2 = y, 3 = x"
        // swizzle s# means "the component # comes from this other component (0 = take source from X)
        // So identity swizzle is xs = 0, ys = 1, zs = 2, ws = 3
        UCHAR useX = (swizMask & 8) >> 3;
        UCHAR useY = (swizMask & 4) >> 2;
        UCHAR useZ = (swizMask & 2) >> 1;
        UCHAR useW = (swizMask & 1) >> 0;
        mask =
            (useX << (3-xs)) |  // If we use X, then where does X come from
            (useY << (3-ys)) |  // If we use Y, then where does Y come from
            (useZ << (3-zs)) |  // If we use Z, then where does Z come from
            (useW << (3-ws));   // If we use W, then where does W come from

        return hr;
    }

    static HRESULT CalcInputReg(UCHAR operationMask, InRegMaskSet& masks, UCHAR& reg,
        const D3DVsInstruction& code, WORD rr, WORD mx)
    {
        HRESULT hr = S_OK;
        if(operationMask){
            switch(mx){
            case MX_R:
                reg = REG_R0 + rr;
                break;
            case MX_V:
                reg = REG_V0 + code.va;
                break;
            case MX_C:
                reg = REG_C0 + code.ca;
                if(code.cin){
                    masks.a0.reg = REG_ARL;
                    masks.a0.mask = 8;// Just X
                }
                break;
            default:
                DXGRIP("Unexpected mx");
                break;
            }
        }
        else {
            reg = REG_ZER; // This input register is not used.
        }
        return hr;
    }

    HRESULT NarrowOutputMasks(const D3DVsInstruction& code, OutRegMaskSet& out){
        HRESULT hr = S_OK;

        // Narrow input masks based on computed 

        if(code.mac){
            // MAC instruction
            hr = NarrowOutputMask(code, out.r);
            if(SUCCEEDED(hr)){
                if(out.outIsMac){
                    hr = NarrowOutputMask(code, out.o);
                }
            }
        }
        if(SUCCEEDED(hr)){
            if(code.ilu){
                hr = NarrowOutputMask(code, out.s);
            }
            if(SUCCEEDED(hr)){
                if(!out.outIsMac){
                    hr = NarrowOutputMask(code, out.o);
                }
            }
        }

        return hr;
    }


    HRESULT NarrowOutputMask(const D3DVsInstruction& code, RegMask& mask){
        HRESULT hr = S_OK;
        // Working backwards in time
        // if code writes to a register that's in the written state, that's dead code
        // because nobody reads the value before it's overwritten.

        if(mask.reg >= REG_C0 && mask.reg < REG_C0+192){
            // This register might have been read by a later a0.x read.
            // Need to track a0.x reads at a finer granularity
            mask.mask &= ( m_anyPRRead | (0xf & (~ m_regLastWritten[mask.reg])) );
        }
        else {
            mask.mask &= ( 0xf & (~ m_regLastWritten[mask.reg]) );
        }

        // if code writes to a temp register (including ARL) that's in the unused state,
        // that's dead code because nobody reads the result.

        if(mask.reg >= REG_R0 && mask.reg <= REG_R11 || mask.reg == REG_ARL){
            mask.mask &= (0xf &  m_regUsed[mask.reg]);
        }
        return hr;
    }

    HRESULT RecordOutputMasks(const D3DVsInstruction& code, const OutRegMaskSet& out){
        HRESULT hr = S_OK;
        if(code.mac){
            hr = RecordOutputMask(code, out.r);
            if(SUCCEEDED(hr)){
                if(out.outIsMac){
                    hr = RecordOutputMask(code, out.o);
                }
            }
        }
        if(SUCCEEDED(hr)){
            if(code.ilu){
                hr = RecordOutputMask(code, out.s);
            }
            if(SUCCEEDED(hr)){
                if(!out.outIsMac){
                    hr = RecordOutputMask(code, out.o);
                }
            }
        }
        return hr;
    }

#ifdef DBG_STRIPPER
    void PrintMask(const char* label, const RegMask& mask){
        char regName;
        int regOffset;
        if(mask.reg >= REG_V0 && mask.reg <= REG_V15){
            regName = 'v';
            regOffset = mask.reg - REG_V0;
        }
        else if(mask.reg >= REG_O0 && mask.reg <= REG_O15){
            regName = 'o';
            regOffset = mask.reg - REG_O0;
        }
        else if(mask.reg >= REG_C0 && mask.reg < REG_C0+192){
            regName = 'c';
            regOffset = mask.reg - REG_C0 - 96;
        }
        else if(mask.reg >= REG_R0 &&mask.reg <= REG_R15){
            regName = 'r';
            regOffset = mask.reg - REG_R0;
        }
        else if(mask.reg == REG_ARL){
            regName = 'a';
            regOffset = mask.reg - REG_ARL;
        }
        else {
            regName = '?';
            regOffset = mask.reg - REG_ZER;
        }
        static const char* kDecodeOutputMask[16] = {
            "____", // 0
            "   w", // 1
            "  z ", // 2
            "  zw", // 3
            " y  ", // 4
            " y w", // 5
            " yz ", // 6
            " yzw", // 7
            "x   ", // 8
            "x  w", // 9
            "x z ", // a
            "x zw", // b
            "xy  ", // c
            "xy w", // d
            "xyz ", // e
            "xyzw"  // f
        };

        DPF2("%s %c[%3d] = 0x%x %s\n", label, regName, regOffset, mask.mask, kDecodeOutputMask[mask.mask]);
    }
    void PrintOutMasks(const char* label, const OutRegMaskSet& maskSet){
        DPF2("%s:\n", label);
        if(maskSet.r.reg){
            PrintMask(" .r",maskSet.r);
        }
        if(maskSet.s.reg){
            PrintMask(" .s",maskSet.s);
        }
        if(maskSet.o.reg){
            PrintMask(maskSet.outIsMac ? " .o[OM_MAC]" : ".o[OM_ILU]", maskSet.o );
        }
    }
#endif

    HRESULT RecordOutputMask(const D3DVsInstruction& code, const RegMask& mask){
        HRESULT hr = S_OK;
#ifdef DBG_STRIPPER
        PrintMask("out", mask);
#endif
        m_regUsed[mask.reg] |= mask.mask;
        m_regLastWritten[mask.reg] |= mask.mask;
        if(mask.reg == REG_oPos){ // Writing to REG_oPos also writes to temporary register 12
            m_regUsed[REG_R12] |= mask.mask;
            m_regLastWritten[REG_R12] |= mask.mask;
        }
        return hr;
    }

    HRESULT RecordInputMasks(const D3DVsInstruction& code, const InRegMaskSet& in){
        HRESULT hr = S_OK;
        hr = RecordInputMask(code, in.a, "a");
        if(SUCCEEDED(hr)){
            hr = RecordInputMask(code, in.b, "b");
        }
        if(SUCCEEDED(hr)){
            hr = RecordInputMask(code, in.c, "c");
        }
        if(SUCCEEDED(hr)){
            hr = RecordInputMask(code, in.a0, "a0");
        }
        return hr;
    }

    HRESULT RecordInputMask(const D3DVsInstruction& code, const RegMask& mask,
                            const char* name)
    {
        HRESULT hr = S_OK;
        if(mask.reg != REG_ZER){
#ifdef DBG_STRIPPER
            PrintMask(name, mask);
#endif
            if(code.cin && mask.reg >= REG_C0 && mask.reg < REG_C0 + 192) {
#ifdef DBG_STRIPPER
                DPF2(" +a0.x. Dirtying all c registers.\n");
#endif
                // An indexed read could be reading from any c register
                // so dirty them all.
                m_anyPRRead |= mask.mask;
            }
            else {
                m_regUsed[mask.reg] |= mask.mask;
                m_regLastWritten[mask.reg] &= ~mask.mask;
                // A read of R12 counts as a read of o[0], too
                if(mask.reg == REG_R12){
                    m_regUsed[REG_oPos] |= mask.mask;
                    m_regLastWritten[REG_oPos] &= ~mask.mask;
                }
            }
        }
        return hr;
    }

    HRESULT InitRegisters(DWORD shaderType){
        HRESULT hr = S_OK;
        m_shaderType = shaderType;
        memset(m_regUsed,0,sizeof(m_regUsed));
        memset(m_regLastWritten,0,sizeof(m_regLastWritten));
        if(SASMT_SHADERTYPE(shaderType) != SASMT_VERTEXSHADER) {
            // Mark all the constant registers as read, because
            // the CPU can read the constant registers after the
            // shader runs.
            // Not sure if it makes a difference to avoid
            // doing this for normal Vertex Shaders. But we
            // have to do it for shaders that can write to constant registers.
            
            for(int i = REG_C0; i < REG_C0 + 192; i++){
                m_regUsed[i] = 0xf;
            }
        }
        m_anyPRRead = 0;
        return hr;
    }
private:
    // Track the usage of a register backwards from the future to the past.
    // All registers start out unused
    // reading from a register sets bits in m_regUsed, and clears bits from m_regLastWritten
    // writing to a register sets bits in both m_regUsed and m_regLastWritten
    // Working backwards in time, if code writes to a temp register that's in the unused or written state,
    // that's dead code.
    // if code writes to an out or const register that's in the written state, that's dead code.
    // Unify all registers into one big happy address space, use Register_t to address them
    UCHAR m_regUsed[256];
    UCHAR m_regLastWritten[256];
    UCHAR m_anyPRRead; // Tracks reads using a0.x
    DWORD m_shaderType;
};

//----------- Reorderer ----------------------
//
// Attempt to reorder instructions to reduce stalls.
//
// This version does not rename registers.

class Reorderer {
private:
    class RegSet {
    public:
        RegSet(){
        }
        
        void Clear(){
            memset(r, 0, sizeof(r));
            memset(c, 0, sizeof(c));
            memset(o, 0, sizeof(o));
            a0x = 0;
            anyCReg = 0;
        }

        bool DirtyConflict(const RegSet* r2) const{
            return DirtyConflictTest(r, r2->r, sizeof(r))
                || DirtyConflictTest(c, r2->c, sizeof(c))
                || DirtyConflictTest(o, r2->o, sizeof(o))
                || (a0x && r2->a0x); 
        }

        bool DirtyConflictTest(const UCHAR* a, const UCHAR* b, DWORD size) const{
            for(DWORD i = 0; i < size; i++){
                if(a[i] & b[i]){
                    return true;
                }
            }
            return false;
        }

        // Dirty the registers that are affected by this microcode instruction

        HRESULT DirtyOut(D3DVsInstruction* pI){
            HRESULT hr = S_OK;
            if(pI->mac == MAC_ARL){
                a0x = true;
            }
            if(pI->rwm){
                if(pI->rw > 11){
                    SETERROR(hr, E_FAIL);
                }
                else {
                    r[pI->rw] |= pI->rwm;
                }
            }
            if(pI->swm){
                // When this is a double opcode, and the ilu is writing to
                // a register, then the register has to be 1.
                DWORD ilu_rw = (pI->mac && pI->ilu) ? 1 : pI->rw;
                r[ilu_rw] |= pI->swm;
            }
            if(pI->owm){
                bool oc_output = (pI->oc & 0x0100) != 0;
                DWORD oc_index = pI->oc & 0xff;
                if(oc_output){
                    if(oc_index >= sizeof(o)){
                        SETERROR(hr, E_FAIL);
                    }
                    else {
                        o[oc_index] = pI->owm;
                        if(oc_index == 0){ // oPos is shadowed as r[12]
                            r[12] |= pI->owm;
                        }
                    }
                }
                else {
                    if(oc_index >= 192){
                        SETERROR(hr, E_FAIL);
                    }
                    else {
                        c[oc_index] |= pI->owm;
                        anyCReg |= pI->owm;
                    }
                }
            }
            return hr;
        }

        // Is this instruction reading from any dirty registers?

        bool IsReadDirty(D3DVsInstruction* pI){
            HRESULT hr = S_OK;
            bool dirty = false;
            bool macDirty = false;
            if(pI->mac){
                hr = DoMac(pI, macDirty);
            }
            bool iluDirty = false;
            if(SUCCEEDED(hr)){
                if(pI->ilu){
                    hr = DoILU(pI, iluDirty);
                }
            }
            bool a0Dirty = a0x && pI->cin;
            if(SUCCEEDED(hr)){
                dirty = macDirty || iluDirty || a0Dirty;
            }
            return dirty;
        }

    private:


        bool ADirty(const D3DVsInstruction* pI, UCHAR usedMask){
            return MuxReadDirty(pI, pI->amx, pI->arr, pI->ane, pI->axs, pI->ays, pI->azs, pI->aws, usedMask);
        }
        bool BDirty(const D3DVsInstruction* pI, UCHAR usedMask){
            return MuxReadDirty(pI, pI->bmx, pI->brr, pI->bne, pI->bxs, pI->bys, pI->bzs, pI->bws, usedMask);
        }
        bool CDirty(const D3DVsInstruction* pI, UCHAR usedMask){
            return MuxReadDirty(pI, pI->cmx, pI->crr, pI->cne, pI->cxs, pI->cys, pI->czs, pI->cws, usedMask);
        }

        bool MuxReadDirty(const D3DVsInstruction* pI,
            DWORD mx, DWORD rr, DWORD ne, DWORD xs, DWORD ys, 
            DWORD zs, DWORD ws, UCHAR usedMask){
            // we don't care about ne at all
            HRESULT hr = S_OK;
            bool dirty = false;
            UCHAR usedMask2 = 0;
            if(usedMask & 8) { // x
                usedMask2 = (1<<(3-xs));
            }
            if(usedMask & 4) { // y
                usedMask2 |= (1<<(3-ys));
            }
            if(usedMask & 2) { // z
                usedMask2 |= (1<<(3-zs));
            }
            if(usedMask & 1) { // w
                usedMask2 |= (1<<(3-ws));
            }
            usedMask = usedMask2;
            
            switch(mx){
            case MX_R:
                if(r[rr] & usedMask){
                    dirty = true;
                }
                break;
            case MX_V:
                break; // no way to write to V, so always clean.
            case MX_C:
                if(c[pI->ca] & usedMask){
                    dirty = true;
                }
                if(pI->cin){ // c[a0.x] 
                    if(anyCReg & usedMask){
                        dirty = true; // If we've written to any c register, we might be reading it now.
                    }
                }
                break;
            default:
                SETERROR(hr, E_FAIL);
                break;
            }
            return dirty;
        }
        HRESULT DoMac(const D3DVsInstruction* pI, bool& dirty){
            HRESULT hr = S_OK;
            UCHAR usedMask = pI->rwm | pI->owm;
            switch(pI->mac){
                // No-op
            case MAC_NOP:
                break;
                // One argument - A
            case MAC_ARL:
                dirty = ADirty(pI, 8); // Just uses the x component
                break;
            case MAC_MOV:
                dirty = ADirty(pI, usedMask);
                break;
            case MAC_ADD:
                dirty = ADirty(pI, usedMask);
                dirty = dirty || CDirty(pI, usedMask);
                break;
            case MAC_MAD:
                dirty = ADirty(pI, usedMask);
                dirty = dirty || BDirty(pI, usedMask);
                dirty = dirty || CDirty(pI, usedMask);
                break;
            case MAC_MUL:
            case MAC_MIN:
            case MAC_MAX:
            case MAC_SLT:
            case MAC_SGE:
                dirty = ADirty(pI, usedMask) || BDirty(pI, usedMask);
                break;
            case MAC_DP3: // Uses xyz from source
                dirty = ADirty(pI, 0xe) || BDirty(pI, 0xe);
                break;
            case MAC_DPH: // Uses xyz for a, xyzw for b
                dirty = ADirty(pI, 0xe) || BDirty(pI, 0xf);
                break;
            case MAC_DP4:
                dirty = ADirty(pI, 0xf) || BDirty(pI, 0xf);
                break;
            case MAC_DST:
                dirty = ADirty(pI, 0x6) || BDirty(pI, 0x5);
                break;
            default:
                // Unknown opcode
                SETERROR(hr, E_FAIL);
                break;
            }
            return hr;
        }

        HRESULT DoILU(const D3DVsInstruction* pI, bool& dirty){
            HRESULT hr = S_OK;
            UCHAR usedMask = pI->swm | pI->owm;
            switch(pI->ilu){
            case ILU_NOP:
                break;
            case ILU_MOV:
                dirty = CDirty(pI, usedMask);
                break;
            case ILU_RCP:
            case ILU_RCC:
            case ILU_RSQ:
            case ILU_EXP:
            case ILU_LOG:
                dirty = CDirty(pI, 0x8); // x
                break;
            case ILU_LIT:
                dirty = CDirty(pI, 0xd); // xy w
                break;
            default:
                SETERROR(hr, E_FAIL);
                break;
            }
            return hr;
        }

        // A set bit means this register was written to.
        // bit 0 = w, bit 1 = z, bit 2 = y, bit 3 = x 
        UCHAR r[13]; // r12 is a way of reading from oPos
        UCHAR c[192]; // c registers
        UCHAR o[16]; // o registers
        UCHAR a0x;  // a0.x register
        UCHAR anyCReg; // Used for read/write dirty when a0.x is used
    };

public:

#ifndef DISABLE_VERIFIER
    VerTable* pPreOptimizedTable;
#endif

    Reorderer(){
    }

    HRESULT Run(DWORD shaderType, D3DVertexShaderProgram* ucode){
#ifdef DISABLE_MOVES_TOWARDS_BEGINNING_AFTER_MOVE_NUMBER
        static int gMoveCount;
#endif
#ifdef DISABLE_PAIRS_AFTER_PAIR_NUMBER
        static int gPairCount;
#endif
        HRESULT hr = S_OK;
        m_ucode = ucode;
        hr = m_sim.Initialize(shaderType, ucode->length);
        bool tryPairingAgain = false;
        if(SUCCEEDED(hr)){
            for(int pc = 0; pc < m_ucode->length - 1 && SUCCEEDED(hr); pc++){
                D3DVsInstruction* pI = &m_ucode->ucode[pc];
                if(m_sim.IsStall(pI)){
                    // Look thru rest of instruction stream to see if we can
                    // find an instruction to put here.
#ifndef DISABLE_MOVES_TOWARDS_BEGINNING
                    int pc2;
                    if(FindInstruction(pc,&pc2)){
#ifdef DISABLE_MOVES_TOWARDS_BEGINNING_AFTER_MOVE_NUMBER
                        if(gMoveCount <= DISABLE_MOVES_TOWARDS_BEGINNING_AFTER_MOVE_NUMBER){
                            DPF2("move-towards-beginning #%d\n", gMoveCount);
                            hr = MoveInstruction(pc,pc2);
                        }
                        else {
                        DPF2("Skipping disabled move-towards-beginning #%d\n", gMoveCount);
                        }
                        gMoveCount++;
#else
                        hr = MoveInstruction(pc,pc2);
#endif
                    }
#endif // DISABLE_MOVES_TOWARDS_BEGINNING
                }

                // Look to see if this instruction can be paired with another instruction
                if(SUCCEEDED(hr)){
#ifndef DISABLE_MOVE_AND_PAIR
                    int pc3;
                    D3DVsInstruction pair;
                    if(FindPairableInstruction(pc,&pc3, &pair)){
                        if(m_sim.IsStall(&pair)){
                            // Look thru rest of instruction stream to see if we can
                            // find an instruction to put here.
                            int pc2;
                            if(FindInstruction(pc,&pc2)){
#ifdef DISABLE_MOVES_TOWARDS_BEGINNING_AFTER_MOVE_NUMBER
                                if(gMoveCount <= DISABLE_MOVES_TOWARDS_BEGINNING_AFTER_MOVE_NUMBER){
                                    DPF2("move-towards-beginning #%d\n", gMoveCount);
                                    hr = MoveInstruction(pc,pc2);
                                }
                                else {
                                    DPF2("Skipping disabled move-towards-beginning #%d\n", gMoveCount);
                                }
                                gMoveCount++;
#else
                                hr = MoveInstruction(pc,pc2);
#endif
                                tryPairingAgain = true;
                                goto skipPair;
                            }
                        }
#ifdef DISABLE_PAIRS_AFTER_PAIR_NUMBER
                                if(gPairCount <= DISABLE_PAIRS_AFTER_PAIR_NUMBER){
                                    DPF2("pairs #%d\n", gPairCount);
                                    hr = PairInstructions(pc, pc3, &pair);
                                }
                                else {
                                    DPF2("Skipping disabled pair #%d\n", gPairCount);
                                }
                                gPairCount++;
#else
                        hr = PairInstructions(pc, pc3, &pair);
#endif
skipPair:;
                    }
#endif // DISABLE_MOVE_AND_PAIR
                }
                
                if(SUCCEEDED(hr)){
                    float stall; // dummy arg
					StallType reason = eNone;
                    hr = m_sim.Do(pI, stall, reason);
                }
            }
        }

        if(SUCCEEDED(hr) && tryPairingAgain){
            // It's better to be paired than to be stall free, because
            // a pair saves an instruction slot. Go back through the
            // code and see if there is anything left to pair
            hr = m_sim.Initialize(shaderType, m_ucode->length);
            if(SUCCEEDED(hr)){
                for(int pc = 0; pc < m_ucode->length - 1 && SUCCEEDED(hr); pc++){
#ifndef DISABLE_MOVE_AND_PAIR
                    int pc2;
                    D3DVsInstruction pair;
                    if(FindPairableInstruction(pc,&pc2, &pair)){
#ifdef DISABLE_PAIRS_AFTER_PAIR_NUMBER
                        if(gPairCount <= DISABLE_PAIRS_AFTER_PAIR_NUMBER){
                            DPF2("pairs #%d\n", gPairCount);
                            hr = PairInstructions(pc, pc2, &pair);
                        }
                        else {
                            DPF2("Skipping disabled pair #%d\n", gPairCount);
                        }
                        gPairCount++;
#else
                        hr = PairInstructions(pc, pc2, &pair);
#endif
                    }
#endif // DISABLE_MOVE_AND_PAIR
                }
            }
        }
        
        return hr;
    }
private:

    // Move an instruction frome one location to another

    HRESULT MoveInstruction(int pc, int pc2){
        HRESULT hr = S_OK;
        ASSERT(pc != pc2);
        if(pc2 < pc){
#ifdef DBG_REORDERER
            DPF2("moving %d towards the end, to %d, to avoid stall.\n", pc2, pc);
            for(int ii = pc2; ii <= pc; ii++){
                PrintInstruction(ii, m_ucode);
            }
#endif
            // Bring the candidate towards end
            D3DVsInstruction temp = m_ucode->ucode[pc2];
            for(int i = pc2; i < pc; i++){
                m_ucode->ucode[i] = m_ucode->ucode[i+1];
            }
            m_ucode->ucode[pc] = temp;
        }
        else {
#ifdef DBG_REORDERER
            DPF2("moving %d towards the beginning, to %d, to avoid stall.\n", pc2, pc);
            for(int ii = pc; ii <= pc2; ii++){
                PrintInstruction(ii, m_ucode);
            }
#endif
            // Bring the candidate forward
            D3DVsInstruction temp = m_ucode->ucode[pc2];
            for(int i = pc2; i > pc; i--){
                m_ucode->ucode[i] = m_ucode->ucode[i-1];
            }
            m_ucode->ucode[pc] = temp;
        }
        if(SUCCEEDED(hr)){
#ifndef DISABLE_VERIFIER
            hr = VerifyStep(pPreOptimizedTable, m_ucode, "Reorderer0");
#endif
        }
        return hr;
    }

    HRESULT PairInstructions(int pc, int pc3, const D3DVsInstruction* pPair){
        HRESULT hr = S_OK;
#ifdef DBG_REORDERER
        DPF2("moving %d to pair with %d. Now there are %d instructions\n", pc3, pc, m_ucode->length-1);
        int pcLow = pc3 < pc ? pc3 : pc;
        int pcHigh = pc3 < pc ? pc : pc3;
        for(int ii = pcLow; ii <= pcHigh; ii++){
            PrintInstruction(ii, m_ucode);
        }
        PrintInstruction(pc, pPair);
#endif
        m_ucode->ucode[pc] = *pPair;
        // remove paired instruction
        int i;
        for(i = pc3 + 1; i < m_ucode->length; i++){
            m_ucode->ucode[i-1] = m_ucode->ucode[i];
        }
        m_ucode->length--;
        if(SUCCEEDED(hr)){
#ifndef DISABLE_VERIFIER
            hr = VerifyStep(pPreOptimizedTable, m_ucode, "Reorderer1");
#endif
        }
        return hr;
    }

    // Find an instruction that can be moved forward into a stall slot.
    // In order to be moved, an instruction has to:
    // (a) Not stall
    // (b) Not read any output computed in the region between pc and pc2
    // (c) Not write to a register that's read or written between pc and pc2

    bool FindInstruction(int pc, int* p_pc2){
        RegSet r;
        r.Clear();
        r.DirtyOut(&m_ucode->ucode[pc]);
        for(int pc2 = pc+1;
            pc2 < m_ucode->length;
            pc2++){
            if( !r.IsReadDirty(&m_ucode->ucode[pc2]) // Condition (b)
                && !m_sim.IsStall(&m_ucode->ucode[pc2])
                && OKToMoveConditionC(&r, pc, pc2) ){ // Condition (a)
                *p_pc2 = pc2;
                return true;
            }
            r.DirtyOut(&m_ucode->ucode[pc2]);
        }
        return false;
    }

    bool OKToMoveConditionC(const RegSet* r, int pc, int pc2) const{
        // Now Check condition (c) - pc2 doesn't write to a register that's used
        // between pc and pc2
        RegSet r2;
        r2.Clear();
        r2.DirtyOut(&m_ucode->ucode[pc2]);
        if(r->DirtyConflict(&r2)){ // pc2-write-everything-else-write conflict
            return false;
        }
        for(int pc3 = pc; pc3 < pc2; pc3++){
            if(r2.IsReadDirty(&m_ucode->ucode[pc3])){ // pc2-Write-pc3-read conflict
                return false;
            }
        }
        return true;
    }

    bool FindPairableInstruction(int pc, int* p_pc2, D3DVsInstruction* pPair){
        RegSet r;
        r.Clear();
        r.DirtyOut(&m_ucode->ucode[pc]);
        for(int pc2 = pc+1;
            pc2 < m_ucode->length;
            pc2++){
            if( !r.IsReadDirty(&m_ucode->ucode[pc2])
                && Pairable(pPair, &m_ucode->ucode[pc], &m_ucode->ucode[pc2])
                && OKToMoveConditionC(&r,pc,pc2) )
            {
                *p_pc2 = pc2;
                return true;
            }
            r.DirtyOut(&m_ucode->ucode[pc2]);
        }
        return false;
    }

    D3DVertexShaderProgram* m_ucode;
    TLEngineSim m_sim;
};

//----------- Microcode disassembler utility -----------

class InstructionDisassembler {
public:
    InstructionDisassembler(){
    }
    ~InstructionDisassembler(){
    }

    HRESULT Disassemble(const D3DVsInstruction* pI, char* outBuf, DWORD outLength){
        HRESULT hr = S_OK;
        Buffer buf;
        hr = buf.Initialize(100);
        if(SUCCEEDED(hr)){
            hr = Disassemble(pI, buf);
        }
        if(SUCCEEDED(hr)){
            DWORD size = min(outLength-1,buf.GetUsed());
            memcpy(outBuf, buf.GetText(), size);
            outBuf[size] = '\0';
        }
        return hr;
    }
    HRESULT Disassemble(const D3DVsInstruction* pI, Buffer& out){
        m_pOut = &out;
        HRESULT hr = S_OK;
        // Each microcode instruction disassembles into up to three
        // normal instructions, depending upon how many outputs it generates
        bool bIsFirstInstruction = true;
        static const char* kIndent = "\r\n                 +   ";

        // nop

        if(SUCCEEDED(hr) && !pI->mac && !pI->ilu){
            hr = Disassemble(pI, false, false);
            bIsFirstInstruction = false;
        }

        // mac register instruction (or ARL instruction.)

        if(SUCCEEDED(hr) && (pI->mac && (pI->rwm || pI->mac == MAC_ARL))){
            hr = Disassemble(pI, false, false);
            bIsFirstInstruction = false;
        }

        // mac owm instruction

        if(SUCCEEDED(hr) && (pI->mac && pI->owm && pI->om == OM_MAC)){
            if(!bIsFirstInstruction){
                out.Append(kIndent);
            }
            hr = Disassemble(pI, false, true);
            bIsFirstInstruction = false;
        }
            
        // ilu register instruction

        if(SUCCEEDED(hr) && (pI->ilu && pI->swm)){
            if(!bIsFirstInstruction){
                out.Append(kIndent);
            }
            hr = Disassemble(pI, true, false);
            bIsFirstInstruction = false;
        }

        // ilu owm instruction

        if(SUCCEEDED(hr) && (pI->ilu && pI->owm && pI->om == OM_ILU)){
            if(!bIsFirstInstruction){
                out.Append(kIndent);
            }
            hr = Disassemble(pI, true, true);
            bIsFirstInstruction = false;
        }

        if(SUCCEEDED(hr)){
            if(pI->eos){
                hr = m_pOut->Printf("%s// end", kIndent); //todo: backspace a few times?
            }
        }
        return hr;
    }

private:

    HRESULT Disassemble(const D3DVsInstruction* pI, bool bDoILU, bool bDoOWM){
        HRESULT hr = S_OK;

        if(SUCCEEDED(hr)){
            static const char* iluOps[] = { //      inversion unit operation                    
                "nop",  //  0x0         
                "mov",  //  0x1                 
                "rcp",  //  0x2                 
                "rcc",  //  0x3                 
                "rsq",  //  0x4                 
                "expp", //  0x5                 
                "logp", //  0x6                 
                "lit",  //  0x7
            };
                            
            static const char* macOps[] = { //      multiply / add operation                    
                "nop",  //      0x0                 
                "mov",  //      0x1                 
                "mul",  //      0x2                 
                "add",  //      0x3                 
                "mad",  //      0x4                 
                "dp3",  //      0x5                 
                "dph",  //      0x6                 
                "dp4",  //      0x7                 
                "dst",  //      0x8                 
                "min",  //      0x9                 
                "max",  //      0xA                 
                "slt",  //      0xB                 
                "sge",  //      0xC                 
                "mov",  //      0xD
                "??e",
                "??f",
            };

            hr = m_pOut->Printf("%s ",
                bDoILU ? iluOps[pI->ilu] : macOps[pI->mac]);

        }
        if(SUCCEEDED(hr)){
            hr = ParseOut(pI, bDoILU, bDoOWM);       
        }
        static const UCHAR iluArgMask[] = {
            0x0, // nop
            0x4, // mov
            0x4, // rcp
            0x4, // rcc
            0x4, // rsq
            0x4, // exp
            0x4, // log
            0x4, // lit
        };

        static const UCHAR macArgMask[] = {
            0x0, // nop
            0x1, // mov
            0x3, // mul
            0x5, // add
            0x7, // mad
            0x3, // dp3
            0x3, // dph
            0x3, // dp4
            0x3, // dst
            0x3, // min
            0x3, // max
            0x3, // slt
            0x3, // sge
            0x1, // arl
            0x0, // ??e
            0x0, // ??f
        };

        UCHAR argMask = bDoILU ? iluArgMask[pI->ilu] : macArgMask[pI->mac];

        static const bool kExpandXChannel[] = {
            false, // nop
            false, // mov
            true, // rcp
            true, // rcc
            true, // rsq
            true, // exp
            true, // log
            false, // lit
        };
        bool bExpandXChannel = bDoILU && kExpandXChannel[pI->ilu];

        if(SUCCEEDED(hr)){
            if(argMask & 1) {
                hr = ParseMux('a', 
                    pI->amx,        /* 1:26 a mux (NA,r0,v,c) */
                    pI->arr,        /* 1:28 a register read */
                    pI->aws,        /* 2:00 a w swizzle */
                    pI->azs,        /* 2:02 a z swizzle */
                    pI->ays,        /* 2:04 a y swizzle */
                    pI->axs,        /* 2:06 a x swizzle */
                    pI->ane,        /* 2:08 a negate */
                    pI, false);
            }
        }
        if(SUCCEEDED(hr)){
            if(argMask & 2) {
                hr = ParseMux('b', 
                    pI->bmx,        /* 1:11 b mux (NA,r1,v,c) */
                    pI->brr,        /* 1:13 b register read */
                    pI->bws,        /* 1:17 b w swizzle */
                    pI->bzs,        /* 1:19 b z swizzle */
                    pI->bys,        /* 1:21 b y swizzle */
                    pI->bxs,        /* 1:23 b x swizzle */
                    pI->bne,        /* 1:25 b negate */
                    pI, false);
            }
        }
        if(SUCCEEDED(hr)){
            if(argMask & 4) {
                hr = ParseMux('c', 
                    pI->cmx,        /* 0:28 c mux (NA,r1,v,c) */
                    pI->crr,        /* 0:30 c register read */
                    pI->cws,        /* 1:02 c w swizzle */
                    pI->czs,        /* 1:04 c z swizzle */
                    pI->cys,        /* 1:06 c y swizzle */
                    pI->cxs,        /* 1:08 c x swizzle */
                    pI->cne,        /* 1:10 c negate */
                    pI, bExpandXChannel);
            }
        }
        return hr;
    }

    static const char* RegisterWriteMask(DWORD m){
        const char* masks[] = {
            ".null",
            ".w",
            ".z",
            ".zw",
            ".y",
            ".yw",
            ".yz",
            ".yzw",
            ".x",
            ".xw",
            ".xz",
            ".xzw",
            ".xy",
            ".xyw",
            ".xyz",
            "", // all
            "error"
        };
        if(m>15){
            m=16;
        }
        return masks[m];
    }

    HRESULT ParseOut(const D3DVsInstruction* pI, bool bDoILU, bool bDoOWM)
    {
        HRESULT hr = S_OK;
        // Do register write
        if(! bDoILU && ! bDoOWM && pI->rwm){
            hr = m_pOut->Printf("r%d%s", pI->rw, RegisterWriteMask(pI->rwm));
        }
        if(SUCCEEDED(hr) && ! bDoILU && pI->mac == MAC_ARL){
            hr = m_pOut->Printf("a0.x");
        }
        if(SUCCEEDED(hr) && bDoILU && ! bDoOWM && pI->swm){
            // When this is a double opcode, and the ilu is writing to
            // a register, then the register has to be 1.
            DWORD ilu_rw = (pI->mac && pI->ilu) ? 1 : pI->rw;
            hr = m_pOut->Printf("r%d%s", ilu_rw, RegisterWriteMask(pI->swm));
        }
        if(SUCCEEDED(hr) && bDoOWM && pI->owm && ((pI->om == OM_ILU) == bDoILU)){
            bool oc_output = (pI->oc & 0x0100) != 0;
            DWORD oc_index = pI->oc & 0xff;
            if(oc_output){
                if(oc_index > 12){
                    oc_index = 13;
                }
                hr = m_pOut->Printf("%s%s", kOutNames[oc_index], RegisterWriteMask(pI->owm));
            }
            else {
                hr = m_pOut->Printf("c%d%s", oc_index-96, RegisterWriteMask(pI->owm));
            }
        }
        return hr;
    }

    HRESULT ParseMux(char /* mux */, int mx, int rr, int ws, int zs, int ys, int xs, int ne,
        const D3DVsInstruction* pI, bool bExpandXChannel){
        static const char s[] = "xyzw"; // Swizzle
        static const char m[] = "?rvc";
        HRESULT hr = m_pOut->Append(", ");
        if(SUCCEEDED(hr)){
            if(ne){
                hr = m_pOut->Append('-');
            }
            if(mx == 1){
                hr = m_pOut->Printf("r");
            }
            else {
                hr = m_pOut->Printf("%c", m[mx]);
            }
        }
        if(SUCCEEDED(hr)){
            switch(mx){
            default:
            case 0:
                hr = m_pOut->Printf("error");
                break;
            case 1:
                hr = m_pOut->Printf("%d", rr);
                break;
            case 2:
                hr = m_pOut->Printf("%d", pI->va);
                break;
            case 3:
                {
                    int userReg = pI->ca-96;
                    if(pI->cin){
                        hr = m_pOut->Printf("[a0.x");
                        if(SUCCEEDED(hr)){
                            if(userReg < 0){
                                hr = m_pOut->Printf("%d", userReg);
                            }
                            else if(userReg > 0){
                                hr = m_pOut->Printf("+%d", userReg);
                            }
                        }
                        if(SUCCEEDED(hr)){
                            hr = m_pOut->Printf("]");
                        }
                    }
                    else {
                        hr = m_pOut->Printf("%d", userReg);
                    }
                }
                break;
            }
        }
        if(SUCCEEDED(hr)){
            if(bExpandXChannel){ // To convert microcode-level rules back into assembly-language-level rules
                ys = xs;
                zs = xs;
                ws = xs;
            }
            if(xs == 0 && ys == 1 && zs == 2 && ws == 3){
                // Do nothing
            }
            else if(xs == ys && ys == zs  && zs == ws){
                hr = m_pOut->Printf(".%c", s[xs]);
            }
            else if(ys == zs && zs == ws){
                hr = m_pOut->Printf(".%c%c", s[xs], s[ys]);
            }
            else if(zs == ws){
                hr = m_pOut->Printf(".%c%c%c", s[xs], s[ys], s[zs]);
            }
            else {
                hr = m_pOut->Printf(".%c%c%c%c", s[xs], s[ys], s[zs], s[ws]);
            }
        }
        return hr;
    }

    Buffer* m_pOut;
};

extern "C" 
HRESULT WINAPI DisassembleInstruction(const D3DVsInstruction* pI, char* outBuf, DWORD outLength){
    InstructionDisassembler d;
    return d.Disassemble(pI, outBuf, outLength);
}

HRESULT DisassembleInstruction(const D3DVsInstruction* pI, Buffer& buffer){
    InstructionDisassembler d;
    return d.Disassemble(pI, buffer);
}

#ifdef DBG
void PrintInstruction(int address, const D3DVsInstruction* pI){
    Buffer outBuf;
    outBuf.Initialize(200);
    InstructionDisassembler d;
    d.Disassemble(pI, outBuf);
    DPF2("%d: %s\n", address, outBuf.GetText());
}

void PrintInstruction(int address, const D3DVertexShaderProgram* pUcode){
    PrintInstruction(address, &pUcode->ucode[address]);
}

#endif

//---------- Renamer ------------------

// Tries to free up R1 for ILU operations
//
// Theory of Operation:
//
// Go through the code, treating each write of a temporary register as a unique "value".
// Because of masked writes, at any one time a temp register can contain data from up to four
// values, one value in each component.
//
// Then, go through the code again, looking at how the instructions read data from temporary
// registers. Assign values to "VRegisters", based upon whether or not the code
// reads data from multiple "values" at once.
//
// Re-assign the mapping from VRegisters to temporary registers so that R1 is used as
// often as possible for ILU operations.Depending upon which ILU instructions are being used,
// up to four VRegisters can be assigned to R1 at once.
//
// Emit the new code, using the new temporary registers.

// Swizzle renaming is buggy - disable until we have a better verifier.
// (See bug 3038 for a test case.)

#define DISABLE_SWIZZLE_RENAMING

class Renamer {
    // A Value is a single computed temporary register component.
    // The first value computed
    // by a program is value 1, the next is value 2, and so on.
    // 0 is the NULL value

    typedef WORD Value;
    typedef WORD VReg;

    // Annotations to the microcode instructions. There is one of these
    // records allocated for each microcode instruction.
    struct PRegSet {
        Value in[3][4];   // [abc = 012] [wzyx = 0123]
        Value rw;         // What value the mlu writes, if any
        Value sw;         // What value the ilu writes, if any
    };

    struct ValueInfo {
        WORD first; // First instruction that this value is used (where it's created)
        WORD last;  // last instruction that register is used (last time it's read, or when created if never read)
        UCHAR reg;  // Which register this was originally
        UCHAR mask; // which compontents of this register were written by this value
        VReg owner;
        Value next; // Next value in this virtual register. 0 at end of chain
    };

    struct VRegInfo {
        WORD first;         // First instruction where this virtual register is used.
        WORD last;          // Last instruction where this virtual register is used.
        Value headValue;    // Head Value for this virtual register. (A linked list)
        UCHAR mask;         // Which components of a register are use by this value;
        UCHAR reg;          // Which register this was originally
        bool prefersR1;     // Prefers R1 because of an unpaired ilu swm instruction.
        bool requiresR1;    // Requires R1, because already has a paired ilu swm instruction in it.
        bool cantBeR1;      // Can't be R1 because is used in a paired mac instruction.
        bool fixedComponents; // Component are fixed, can't be swizzled.
        UCHAR newReg;       // Post-register-assignment register
        UCHAR sw[4];        // sw[wzyx = 0123] is the mapping for component. Uses CSW_X..CSW_W codes
                             
    };

    PRegSet* m_pRegSet;

    ValueInfo* m_pValueInfo;
    Value m_valueCount;

    VRegInfo* m_pVregInfo;
    VReg m_vregCount;

    UCHAR m_renameRegRotor; // Where to start searching from

    D3DVertexShaderProgram* m_pUcode;

public:
    Renamer(){
    }

    HRESULT Run(DWORD shaderType, D3DVertexShaderProgram* ucode){
        HRESULT hr = S_OK;
        // First, is it worth renaming
        bool somethingToDo;
        hr = EligableInstructions(ucode, &somethingToDo);
        if(SUCCEEDED(hr) ){
            if(somethingToDo){
                hr = Rename(ucode);
            }
            else {
#ifdef DBG_RENAMER
                DPF2("Renamer thinks there's nothing to do.\n");
#endif
            }
        }
        return hr;
    }

private:

    HRESULT EligableInstructions(const D3DVertexShaderProgram* ucode, bool* pSomethingToDo){
        bool somethingToDo = false;
        for(int i = 0; i < ucode->length; i++){
            const D3DVsInstruction* pI = ucode->ucode + i;
            if(!pI->mac && pI->ilu && pI->swm && pI->rw != 1){
                somethingToDo = true;
                break;
            }
			//make renamer work if an alu instruction writes to r1. This will allow movs to be paired more easily in some cases.
			if(pI->mac && !pI->ilu && pI->rwm && pI->rw == 1){
				somethingToDo = true;
				break;
			}
        }
        *pSomethingToDo = somethingToDo;
        return S_OK;
    }

    HRESULT Rename(D3DVertexShaderProgram* ucode){
        HRESULT hr = S_OK;
        m_pUcode = ucode;
        D3DVertexShaderProgram* p2 = 0;
        m_pRegSet = 0;
        m_pValueInfo = 0;
        m_valueCount = 0;
        m_pVregInfo = 0;
        m_vregCount = 0;
        m_renameRegRotor = 0;

        if(SUCCEEDED(hr)){
            p2 = new D3DVertexShaderProgram;
            if(!p2){
                SETERROR(hr,E_OUTOFMEMORY);
            }
        }
        if(SUCCEEDED(hr)){
            m_pRegSet = new PRegSet[ucode->length];
            if(!m_pRegSet){
                SETERROR(hr,E_FAIL);
            }
            for(int i = 0; i < ucode->length; i++){
                memset(&m_pRegSet[i], 0, sizeof(PRegSet));
            }
        }
        if(SUCCEEDED(hr)){
            // Can't write more than two values per instruction, and Value #0 is reserved
            int valueTotalCount = 2*ucode->length + 1;
            m_pValueInfo = new ValueInfo[valueTotalCount];
            if(!m_pValueInfo){
                SETERROR(hr,E_FAIL);
            }
            for(int i = 0; i < valueTotalCount; i++){
                memset(&m_pValueInfo[i], 0, sizeof(ValueInfo));
            }
        }
        if(SUCCEEDED(hr)){
            // No more than two vregisters per instruction, and vregister #0 is reserved
            int vregTotalCount = 2*ucode->length + 1;
            m_pVregInfo = new VRegInfo[vregTotalCount];
            if(!m_pVregInfo){
                SETERROR(hr,E_FAIL);
            }
            for(int i = 0; i < vregTotalCount; i++){
                memset(&m_pVregInfo[i], 0, sizeof(VRegInfo));
            }
        }
        if(SUCCEEDED(hr)){
            hr = MapRegisterUse();
        }
        // Now that we know the lifetime of each register,
        // remap the registers to maximize potential parallelism
        if(SUCCEEDED(hr)){
            HRESULT hr2 = RemapVRegs();
#ifdef DBG_RENAMER
            DumpRegisterUse();
#endif
            // If we fail to remap, then don't report an error, just don't modify the code
            if(SUCCEEDED(hr2)){
                hr = RemapCode();
#ifdef DBG_RENAMER
                DumpNewCode();
#endif
            }
        }
        delete [] m_pRegSet;
        m_pRegSet = 0;
        delete [] m_pValueInfo;
        m_pValueInfo = 0;
        delete [] m_pVregInfo;
        m_pVregInfo = 0;
        delete p2;
        return hr;
    }

#ifdef DBG_RENAMER

    void DumpRegisterUse(){
        InstructionDisassembler d;
        Buffer b;
        b.Initialize(1024);
        {
            for(int i = 0; i < m_pUcode->length; i++){
                b.Printf("%3d: ", i);
                d.Disassemble(&m_pUcode->ucode[i], b);
                DPF2("%s\n", b.GetText());
                b.Clear();
                for(int inr = 0; inr < 3; inr++){
                    Value* v = m_pRegSet[i].in[inr];
                    VReg vr = 0;
                    for(int iv = 0; iv < 4; iv++){
                        if(v[iv]){
                            vr = m_pValueInfo[v[iv]].owner;
                            break;
                        }
                    }
                    if(vr){
                        b.Printf("       %c: %3d.x %3d.y %3d.z %3d.w [%d]", 'a'+inr, v[3], v[2], v[1], v[0], vr);
                        DPF2("%s\n", b.GetText());
                        b.Clear();
                    }
                }
                if(m_pRegSet[i].rw || m_pRegSet[i].sw){
                    b.Printf("      rw: %3d sw: %3d", m_pRegSet[i].rw,  m_pRegSet[i].sw);
                    DPF2("%s\n", b.GetText());
                    b.Clear();
                }
            }
        }
        {
            DPF2("---- Values ---\n");
            DPF2(" id   vreg fir-las nex reg[comp]\n");
            // Start at 1 because that's the first Value
            for(int i = 1; i < m_valueCount; i++){
                ValueInfo* pV = &m_pValueInfo[i];
                DPF2("%3d: [%3d] %3d-%3d %3d %sr%d[%c%c%c%c]\n",
                    i,
                    pV->owner,
                    pV->first, pV->last,
                    pV->next,
                    pV->reg >= 10 ? "" : " ",
                    pV->reg,
                    pV->mask & 8 ? 'x' : ' ',
                    pV->mask & 4 ? 'y' : ' ',
                    pV->mask & 2 ? 'z' : ' ',
                    pV->mask & 1 ? 'w' : ' '
                    );
            }
        }
        {
            DPF2("---- VRegisters ---\n");
            // Start at 1 because that's the first VReg
            DPF2(" id  fir-las hea reg[comp] new[swiz] (attribs)\n"); 

            for(int i = 1; i < m_vregCount; i++){
                VRegInfo* pV = &m_pVregInfo[i];
                if(pV->headValue) {
                    static const char kSwiz[4] = {'x','y','z','w'};
                    DPF2("%3d: %3d-%3d %3d %sr%d[%c%c%c%c] -> %sr%d[%c%c%c%c] %s %s %s %s\n",
                        i,
                        pV->first, pV->last,
                        pV->headValue,
                        pV->reg >= 10 ? "" : " ",
                        pV->reg,
                        pV->mask & 8 ? 'x' : ' ',
                        pV->mask & 4 ? 'y' : ' ',
                        pV->mask & 2 ? 'z' : ' ',
                        pV->mask & 1 ? 'w' : ' ',
                        pV->newReg >= 10 ? "" : " ",
                        pV->newReg,
                        pV->mask & 8 ? kSwiz[pV->sw[3]] : ' ',
                        pV->mask & 4 ? kSwiz[pV->sw[2]] : ' ',
                        pV->mask & 2 ? kSwiz[pV->sw[1]] : ' ',
                        pV->mask & 1 ? kSwiz[pV->sw[0]] : ' ',
                        pV->prefersR1 ? "prefers r1" : "",
                        pV->requiresR1 ? "requires r1" : "",
                        pV->cantBeR1 ? "can't be r1" : "",
                        pV->fixedComponents ? "fixed" : ""
                        );
                }
            }
        }
    }

    void DumpNewCode(){
        DPF2("--- New Code ---\n");
        InstructionDisassembler d;
        Buffer b;
        b.Initialize(1024);
        {
            for(int i = 0; i < m_pUcode->length; i++){
                b.Printf("%3d: ", i);
                d.Disassemble(&m_pUcode->ucode[i], b);
                DPF2("%s\n", b.GetText());
                b.Clear();
            }
        }
    }

#endif

    HRESULT MapRegisterUse(){
        HRESULT hr = true;
        Value currentReg[16*4]; // [reg][0..3 == wzyx]
        m_valueCount = 1; // One-based
        m_vregCount = 1; // One-based

        // Clear current register set
        memset(currentReg,0,sizeof(currentReg));
        // Simulate
        WORD length = (WORD) m_pUcode->length;
        for(WORD i = 0; i < length; i++){
            const D3DVsInstruction* pI = m_pUcode->ucode + i;
            
            // Update input
            UCHAR inMasks[3];
            ComputePostSwizzleUseMasks(pI, inMasks);
            
            UpdateInput(inMasks[0], pI->amx, pI->arr, pI->axs, pI->ays, pI->azs, pI->aws, m_pRegSet[i].in[0], currentReg, i);
            UpdateInput(inMasks[1], pI->bmx, pI->brr, pI->bxs, pI->bys, pI->bzs, pI->bws, m_pRegSet[i].in[1], currentReg, i);
            UpdateInput(inMasks[2], pI->cmx, pI->crr, pI->cxs, pI->cys, pI->czs, pI->cws, m_pRegSet[i].in[2], currentReg, i);

            // Update output

            if(pI->rwm){
                UpdateOutput(pI->rwm, pI->rw, & m_pRegSet[i].rw, currentReg, i, true);
            }
            if(pI->swm){
                UCHAR reg = pI->mac ? 1 : pI->rw; // Any MAC instruction at all forces swm to write to r1
                UpdateOutput(pI->swm, reg, & m_pRegSet[i].sw, currentReg, i, false);
            }            
        }
        return hr;
    }

    void UpdateInput(UCHAR inMask, UCHAR mx, UCHAR rr, UCHAR xs, UCHAR ys, UCHAR zs, UCHAR ws, Value* prr,
        const Value* currentReg, WORD pc){
        if(inMask && mx == MX_R){ // Reading from a register
            {
                const Value* v = currentReg + rr * 4;
                for(int j = 0; j < 4; j++){
                    if(inMask & (1 << j)){
                        // We use the value in the ith component, w = 0. Where did that value come from?
                        int source; // Swizzle mask value: 0 = X .. 3 = W
                        switch(j){
                        case 0: source = ws; break;
                        case 1: source = zs; break;
                        case 2: source = ys; break;
                        case 3: source = xs; break;
                        }

                        Value v2 = v[3-source];
                        ValueInfo* pV2 = &m_pValueInfo[v2];
                        pV2->last = pc; // This register was used here
                        m_pVregInfo[pV2->owner].last = pc;
                        prr[j] = v2;
                    }
                    else {
                        prr[j] = 0; // Not used in this instruction
                    }
                }
            }
            // Do we need to combine virtual registers together?
            VReg winner = 0;
            int i;
            for(i = 0; i < 4; i++){
                Value v = prr[i];
                if(v) {
                    VReg vreg = m_pValueInfo[v].owner;
                    if(!winner || vreg < winner){
                        winner = vreg;
                    }
                }
            }
            for(i = 0; i < 4; i++){
                Value v = prr[i];
                if(v) {
                    VReg vreg = m_pValueInfo[v].owner;
                    if(winner != vreg){
                        // Move all of the loser's Values over to the winner
                        VRegInfo* pWinner = &m_pVregInfo[winner];
                        VRegInfo* pLoser = &m_pVregInfo[vreg];
                        if(! pLoser->first) {
                            continue; // We've alreadly cleaned out this register.
                        }
#ifdef DBG_RENAMER
                        DPF2("Merging vreg %d into vreg %d at instruction %d\n", vreg, winner, pc);
#endif
                        // Merge the loser's values with the winner's values
                        Value winnerVal = pWinner->headValue;
                        Value loserVal = pLoser->headValue;
                        bool firstTime = true;
                        Value tailVal = 0;
                        // Merge two linked lists, keep sorted
                        // (Also update the headValue of the loser)
                        while(winnerVal || loserVal){
                            // Which value is numericly smaller?
                            if(! winnerVal || (winnerVal && loserVal && loserVal < winnerVal)){
                                // Link the next loserVal into the list
                                if(firstTime){
                                    pWinner->headValue = loserVal;
                                }
                                if(tailVal){
                                    m_pValueInfo[tailVal].next = loserVal;
                                }
                                m_pValueInfo[loserVal].owner = winner;
                                tailVal = loserVal;
                                loserVal = m_pValueInfo[loserVal].next;
                            }
                            else {
                                // Link the next winnerVal ino the list
                                if(tailVal){
                                    m_pValueInfo[tailVal].next = winnerVal;
                                }
                                tailVal = winnerVal;
                                ASSERT(winnerVal);
                                winnerVal = m_pValueInfo[winnerVal].next;
                            }
                            firstTime = false;
                        }
                        if(tailVal){
                            m_pValueInfo[tailVal].next = 0;
                        }

                        // Update winner info
                        pWinner->first = min(pWinner->first, pLoser->first);
                        pWinner->last = max(pWinner->last, pLoser->last);
                        pWinner->mask = pWinner->mask | pLoser->mask;
                        // pWinner->reg is the same as pLoser->reg
                        pWinner->prefersR1 = pWinner->prefersR1 || pLoser->prefersR1;
                        pWinner->requiresR1 = pWinner->requiresR1 || pLoser->requiresR1;
                        pWinner->cantBeR1 = pWinner->cantBeR1 || pLoser->cantBeR1;
                        pWinner->fixedComponents = pWinner->fixedComponents || pLoser->fixedComponents;
                        
                        // Clear out loser's registers
                        memset(pLoser, 0, sizeof(VRegInfo));
                    }
                }
            }
        }
    }

    void UpdateOutput(UCHAR outMask, UCHAR rw, Value* prw,
        Value* currentReg, WORD pc, bool isMac){
        WORD vid = m_valueCount++;
        Value* v = currentReg + rw * 4;
        for(int j = 0; j < 4; j++){
            if(outMask & (1 << j)){
                v[j] = vid;
            }
        }
        VReg vr =  m_vregCount++;
        *prw = vid;
        ValueInfo* pV = &m_pValueInfo[vid];
        pV->first = pc;
        pV->last = pc;
        pV->reg = rw;
        pV->mask = outMask;
        pV->owner = vr;
        // pV->next = 0; // Already zero

        VRegInfo* pVr = &m_pVregInfo[vr];
        pVr->first = pc;
        pVr->last = pc;
        pVr->headValue = vid;
        pVr->mask = outMask;
        pVr->reg = rw;
        // pVr->prefersR1 = 0; // Already zero
        // pVr->requiresR1 = 0; // Already zero
        // pVr->cantBeR1 = 0; // Already zero
        // pVr->fixedComponents = 0; // Already zero

        // Don't set up remapping info here.
        UpdateComponentUse(vr, pc, outMask, isMac);
    }

    void UpdateComponentUse(VReg vreg, WORD pc, UCHAR outMask, bool isMac){
        // Which output components can't be moved to other
        // channels by swizzleing the inputs?
        // w = 1 .. x == 8
        static const UCHAR kFixedILUOutputs[8] = {
            0x0,   // ILU_NOP
            0x0,   // ILU_MOV
            0x0,   // ILU_RCP
            0x0,   // ILU_RCC
            0x0,   // ILU_RSQ
            0xf,   // ILU_EXP
            0xf,   // ILU_LOG
            0x0,   // ILU_LIT
        };
        static const UCHAR kFixedMACOutputs[16] = {
            0x0,   // MAC_NOP
            0x0,   // MAC_MOV
            0x0,   // MAC_MUL
            0x0,   // MAC_ADD
            0x0,   // MAC_MAD
            0x0,   // MAC_DP3
            0x0,   // MAC_DPH
            0x0,   // MAC_DP4
            0xf,   // MAC_DST
            0x0,   // MAC_MIN
            0x0,   // MAC_MAX
            0x0,   // MAC_SLT
            0x0,   // MAC_SGE
            0x0,   // MAC_ARL
            0x0,   // 0xe
            0x0,   // 0xf
        };

        D3DVsInstruction* pI = & m_pUcode->ucode[pc];
        VRegInfo * pVr = &m_pVregInfo[vreg];
        WORD ilu = pI->ilu;
        if(! isMac && ilu && pI->swm){
            pVr->prefersR1 = true;
            if(pI->mac){
                pVr->requiresR1 = true;
            }
        }
        if(isMac && ilu && pI->rwm){
            pVr->cantBeR1 = true;
        }

#ifdef DISABLE_SWIZZLE_RENAMING
        pVr->fixedComponents = true;
#else
        pVr->fixedComponents = 0 != (kFixedILUOutputs[ilu] | kFixedMACOutputs[pI->mac]);
#endif
    }
    
    // The policy for mapping:
    // Go through the instructions, in order.
    // When a new vreg starts, pick a reg for it from the free registers.
    //
    // (2) 
    HRESULT RemapVRegs(){
        HRESULT hr = S_OK;
        VReg regs[64]; // Active Vreg-to-temp-reg assignments
        memset(regs,0,sizeof(regs));
        for(WORD pc = 0; pc < m_pUcode->length && SUCCEEDED(hr); pc++){
            PRegSet *pRegSet = &m_pRegSet[pc];
            hr = RemapVRegs2Start(pRegSet->rw, regs, pc);
            if(SUCCEEDED(hr)){
                hr = RemapVRegs2Start(pRegSet->sw, regs, pc);
            }
            if(SUCCEEDED(hr)){
                for(int op = 0; op < 3 && SUCCEEDED(hr); op++){
                    hr = RemapVRegs2End(pRegSet->in[op], regs, pc);
                }
            }
        }
        return hr;
    }
    
    HRESULT RemapVRegs2Start(Value v, VReg* pRegs, WORD pc){
        if(v){
            VReg vreg = m_pValueInfo[v].owner;
            VRegInfo* pVr = &m_pVregInfo[vreg];
            if(pVr->first == pc){ // First use of this VReg, assign a register
                if(pVr->prefersR1 && ! pVr->cantBeR1){
                    if(IsRegFree(pRegs, 1, vreg)){
                        return AssignReg(pRegs, 1, vreg);
                    }
                    else {
                        if(pVr->requiresR1){
#ifdef DBG_RENAMER
                            DPF2("Instruction at %d requires r1, but can't get it.\n", pc);
#endif
                            return E_FAIL;
                        }
                        return AssignFreeReg(pRegs,vreg, false);
                    }
                }
                else {
                    return AssignFreeReg(pRegs,vreg, pVr->cantBeR1);
                }
            }
        }
        return S_OK;
    }
    
    bool IsRegFree(const VReg* pRegs, UCHAR reg, VReg vreg){
        const VReg* pComponents = pRegs + reg * 4;
        VRegInfo* pVr = &m_pVregInfo[vreg];
        WORD mask = pVr->mask;
        if ( pVr->fixedComponents ){
            // See if the specific components in the mask are free in this register.
            for(int i = 0; i < 4; i++){
                if( (1 << i) & mask ) {
                    if(pComponents[i]){
                        // This component is in use
                        return false;
                    }
                }
            }
            return true;
        }
        else {
            // Count how many components are free, needed
            UCHAR free = 0;
            UCHAR needed = 0;
            for(int i = 0; i < 4; i++){
                if( (1 << i) & mask ) {
                    needed++;
                }
                if( ! pComponents[i]){
                    free++;
                }
            }
            if(free < needed){
                return false;
            }
            return true;
        }
    }
    
    HRESULT AssignReg(VReg* pRegs, UCHAR reg, VReg vreg){
        VReg* pComponents = pRegs + reg * 4;
        VRegInfo* pVr = &m_pVregInfo[vreg];
        WORD mask = pVr->mask;
        {
            // See if the specific components in the mask are free in this register.
            for(int i = 0; i < 4; i++){
                if( (1 << i) & mask ) {
                    if(pComponents[i]){
                        // This component is in use
                        goto trySwizzled;
                    }
                    pComponents[i] = vreg; // Mark this component as being assigned
                    pVr->newReg = reg;
                    pVr->sw[i] = 3-i; // Convert component index to swizzle constant
                }
            }
#ifdef DBG_RENAMER
            PrintRegisterMove(vreg);
#endif
            return S_OK;
        }
trySwizzled: ;
        if ( pVr->fixedComponents ) {
            return E_FAIL;
        }
        else {
            // Count how many components are free, needed
            UCHAR component = 0;
            for(int i = 0; i < 4; i++){
                if( (1 << i) & mask ) {
                    // TODO - we know that (because of EXPP being used for frac)
                    // R1.Y is the most valuable component. So try to assign it last.
                    
                    while(pComponents[component] && component < 4){
                        component++;
                    }
                    if(component >= 4){
                        // No free components
                        return E_FAIL;
                    }
                    pComponents[component] = vreg; // Mark this component as being assigned
                    pVr->newReg = reg;
                    pVr->sw[i] = 3-component; // Convert component index to swizzle constant
                }
            }
#ifdef DBG_RENAMER
            PrintRegisterMove(vreg);
#endif
            return S_OK;
        }
    }

#ifdef DBG_RENAMER
    void PrintRegisterMove(VReg vreg){
        const VRegInfo* pVr = &m_pVregInfo[vreg];
        if(pVr->reg != pVr->newReg){
            DPF2("Moving vreg %d r%d to r%d.\n", vreg, pVr->reg, pVr->newReg);
        }
        if ( pVr->mask & 1 && pVr->sw[0] != CSW_W
           || pVr->mask & 2 && pVr->sw[1] != CSW_Z
           || pVr->mask & 4 && pVr->sw[2] != CSW_Y
           || pVr->mask & 8 && pVr->sw[3] != CSW_X){
            static const char kSwizzle[4] = {'x','y','z','w'};
            DPF2(" swizzleing vreg %d r%d[%c%c%c%c].\n", vreg, pVr->newReg,
                pVr->mask & 1 ? kSwizzle[pVr->sw[3]] : ' ', // component 3 == x
                pVr->mask & 2 ? kSwizzle[pVr->sw[2]] : ' ',
                pVr->mask & 4 ? kSwizzle[pVr->sw[1]] : ' ',
                pVr->mask & 8 ? kSwizzle[pVr->sw[0]] : ' '
                );
        }
    }
#endif

    HRESULT AssignFreeReg(VReg* pRegs, VReg vreg, bool cantBeR1){
        // To increase the chances of being able to move instructions around,
        // we rotate the register we start searching at.
        // (This really scrambles the code!)
        
        VRegInfo* pVr = &m_pVregInfo[vreg];
// #define TRY_TO_KEEP_OLD_ASSIGNMENTS
#define USE_ROTOR

#ifdef TRY_TO_KEEP_OLD_ASSIGNMENTS
        if(pVr->reg != 1 && IsRegFree(pRegs, pVr->reg, vreg)){
            return AssignReg(pRegs, pVr->reg, vreg);
        }
#endif
        // Find first free reg - assign R1 last
        static const UCHAR kRegOrder[] = {0,2,3,4,5,6,7,8,9,10,11};
#ifdef USE_ROTOR
        m_renameRegRotor++;
        if(m_renameRegRotor >= sizeof(kRegOrder)) {
                m_renameRegRotor = m_renameRegRotor - sizeof(kRegOrder);
        }
#endif
        for(int i = 0; i < sizeof(kRegOrder); i++){
            int i2 = m_renameRegRotor + i;
            if(i2 >= sizeof(kRegOrder)) {
                i2 = i2 - sizeof(kRegOrder);
            }
            UCHAR reg = kRegOrder[i2];
            if(IsRegFree(pRegs, reg, vreg)){
                return AssignReg(pRegs, reg, vreg);
            }
        }
        // Finally, try R1
        if(!cantBeR1 && IsRegFree(pRegs, 1, vreg)){
            return AssignReg(pRegs, 1, vreg);
        }
        // Out of ideas.
        return E_FAIL;
    }
    
    HRESULT RemapVRegs2End(Value* pInRegComponents, VReg* pRegs, WORD pc){
        VReg vreg = 0;
        for(int c = 0; c < 4; c++){
            Value v = pInRegComponents[c];
            if(v){
                vreg = m_pValueInfo[v].owner;
            }
        }
        if(vreg){
            VRegInfo* pVr = &m_pVregInfo[vreg];
            if(pVr->last == pc){ // End of use of register, clear out pRegs
                UCHAR mask = pVr->mask;
                VReg* pRegBase = &pRegs[pVr->newReg * 4];
                for(int c = 0; c < 4; c++){
                    if((1 << c) & mask){
                        int newc = 3 - pVr->sw[c];
#ifdef DBG_RENAMER
                        // It can be blank if has already been cleared, but
                        // it better not be assigned to another register!
                        if(pRegBase[newc] && pRegBase[newc] != vreg){
                            DebugBreak();
                        }
#endif
                        pRegBase[newc] = 0;
                    }
                }
            }
        }
        return S_OK;
    }
    
    HRESULT RemapCode(){
        HRESULT hr = S_OK;
        for(int i = 0; i < m_pUcode->length; i++){
            D3DVsInstruction* pI = &m_pUcode->ucode[i];
            PRegSet* pRegSet = &m_pRegSet[i];
            // Remap input registers
            for(int j = 0; j < 3; j++){
                Value v = 0;
                for(int k = 0; k < 4; k++){
                    Value v2 = pRegSet->in[j][k];
                    if(v2){
                        v = v2;
                        break;
                    }
                }

                if(v){
                    VReg vreg = m_pValueInfo[v].owner;
                    ASSERT(vreg);
                    VRegInfo* pVr = &m_pVregInfo[vreg];
                    ASSERT(pVr->first <= i && i <= pVr->last);
                    switch(j){
                    case 0:
                        pI->arr = pVr->newReg;
#ifndef DISABLE_SWIZZLE_RENAMING
                        pI->aws = pVr->sw[3 - pI->aws];
                        pI->azs = pVr->sw[3 - pI->azs];
                        pI->ays = pVr->sw[3 - pI->ays];
                        pI->axs = pVr->sw[3 - pI->axs];
#endif
                        break;
                    case 1:
                        pI->brr = pVr->newReg;
#ifndef DISABLE_SWIZZLE_RENAMING
                        pI->bws = pVr->sw[3 - pI->bws];
                        pI->bzs = pVr->sw[3 - pI->bzs];
                        pI->bys = pVr->sw[3 - pI->bys];
                        pI->bxs = pVr->sw[3 - pI->bxs];
#endif
                        break;
                    case 2:
                        pI->crr = pVr->newReg;
#ifndef DISABLE_SWIZZLE_RENAMING
                        pI->cws = pVr->sw[3 - pI->cws];
                        pI->czs = pVr->sw[3 - pI->czs];
                        pI->cys = pVr->sw[3 - pI->cys];
                        pI->cxs = pVr->sw[3 - pI->cxs];
#endif
                        break;
                    }
                }
            }

            // Remap the output registers
            if(pRegSet->rw){
                VReg vreg = m_pValueInfo[pRegSet->rw].owner;
                ASSERT(vreg);
                VRegInfo* pVr = &m_pVregInfo[vreg];
                ASSERT(pVr->first <= i && i <= pVr->last);
                pI->rw = pVr->newReg;
                pI->rwm = CalcSwizzledWriteMask(pI->rwm, pVr);
            }
            if(pRegSet->sw){
                VReg vreg = m_pValueInfo[pRegSet->sw].owner;
                ASSERT(vreg);
                VRegInfo* pVr = &m_pVregInfo[vreg];
                ASSERT(pVr->first <= i && i <= pVr->last);
                if(!pI->mac){
                    pI->rw = pVr->newReg;
                }
                pI->swm = CalcSwizzledWriteMask(pI->swm, pVr);
            }
        }
        return hr;
    }
    
    UCHAR CalcSwizzledWriteMask(UCHAR mask, const VRegInfo* pVr){
        UCHAR result = 0;
        const UCHAR* pSw = pVr->sw;
        for(int c = 0; c < 4; c++){
            if(mask & (1 << c)){
                result |= 1 << (3 - pSw[c]);
            }
        }
        return result;
    }
};



void ConvertPackedDWordsToUCode(const DWORD* pIn, D3DVsInstruction* pucode){
    memset(pucode,0,sizeof(D3DVsInstruction));
    DWORD x = *pIn++;
    DWORD y = *pIn++;
    DWORD z = *pIn++;
    DWORD w = *pIn++;
    // Word X -- ignore
    // Word Y
	pucode->aws = (y >> 0);
	pucode->azs = (y >> 2);
    pucode->aws = (y >> 0);
    pucode->azs = (y >> 2);
    pucode->ays = (y >> 4);
    pucode->axs = (y >> 6);
    pucode->ane = (y >> 8);
    pucode->va  = (y >> 9);
    pucode->ca  = (y >>13);
    pucode->mac = (y >>21);
    pucode->ilu = (y >>25);
    //Word Z
    pucode->crr = ((0xc & (z << 2)) | ((w >> 30) & 0x3));
    pucode->cws = (z >> 2);
    pucode->czs = (z >> 4);
    pucode->cys = (z >> 6);
    pucode->cxs = (z >> 8);
    pucode->cne = (z >>10);
    pucode->bmx = (z >>11);
    pucode->brr = (z >>13);
    pucode->bws = (z >>17);
    pucode->bzs = (z >>19);
    pucode->bys = (z >>21);
    pucode->bxs = (z >>23);
    pucode->bne = (z >>25);
    pucode->amx = (z >>26);
    pucode->arr = (z >>28);

    //Word W
    pucode->eos = (w >> 0);
    pucode->cin = (w >> 1);
    pucode->om  = (w >> 2);
    pucode->oc  = (w >> 3);
    pucode->owm = (w >>12);
    pucode->swm = (w >>16);
    pucode->rw  = (w >>20);
    pucode->rwm = (w >>24);
    pucode->cmx = (w >>28);
} 
 
// This is exported, but isn't documented. It's exported so the xsdasm program can
// easily disassemble the vertex shader.

extern "C" HRESULT DisassembleVertexShader(LPCVOID pUcode, DWORD length, LPXGBUFFER* pDisassembly);

HRESULT DisassembleVertexShader(LPCVOID pUcode, DWORD length, LPXGBUFFER* pDisassembly){
    HRESULT hr = S_OK;
    // Check header
    if ( length < 4 || ! pUcode) {
        SETERROR(hr, E_FAIL);
    }
    WORD shaderLength = 0;
    DWORD shaderType;
    if(SUCCEEDED(hr)){
        switch(* (WORD*) pUcode){
        case 0x2078:
            shaderType = SASMT_VERTEXSHADER;
            break;
        case 0x7778:
            shaderType = SASMT_READWRITE_VERTEXSHADER;
            break;
        case 0x7378:
            shaderType = SASMT_VERTEXSTATESHADER;
            break;
        default:
            SETERROR(hr, E_FAIL); // Unknown shader type
        }
    }
    if(SUCCEEDED(hr)){
        shaderLength = * (WORD*) (((CHAR*) pUcode) + 2);
        if ( 4 + shaderLength * sizeof(PaddedUCode) != length){
            SETERROR(hr, E_FAIL);
        }
    }
    Buffer buf;
    if(SUCCEEDED(hr)){
        hr = buf.Initialize(100+40*shaderLength);
    }
    TLEngineSim sim;
    if(SUCCEEDED(hr)){
        hr = sim.Initialize(shaderType, shaderLength);
    }
    if(SUCCEEDED(hr)){
		DWORD* pI = (DWORD*) (((CHAR*) pUcode) + 4);
		float stall;
		StallType reason = eNone;
		for(WORD i = 0; SUCCEEDED(hr) && i < shaderLength; i++){
			reason = eNone;
            D3DVsInstruction ucode;
            ConvertPackedDWordsToUCode(pI + i * 4, &ucode);
            sim.Do(&ucode, stall, reason);
			if(SASMT_SHADERTYPE(shaderType) == SASMT_VERTEXSHADER)
				buf.Printf("%d: (%c %.2f) ", i, char(reason), stall + .5);
			else
				buf.Printf("%d: (%d) ", i, (int)(stall + 1.1));
            hr = DisassembleInstruction(&ucode, buf);
            buf.Append("\n");
       }
	   stall = sim.CalculateFinalStall(reason);
	   if(stall) {
		   buf.Printf("Final Stall: %c %.2f \n", char(reason), stall);
	   }
    }

    if(SUCCEEDED(hr) && pDisassembly){
        hr = XGBufferCreateFromBuffer(&buf, pDisassembly);
    }

    return hr;
}

#ifdef DBG
void Print(D3DVsInstruction& instruction){
    char buf[100];
    DisassembleInstruction(&instruction, buf, sizeof(buf));
    DPF2("%s\n", buf);
}

void Print(DWORD shaderType, D3DVertexShaderProgram& program){
    Buffer buf;
    HRESULT hr = buf.Initialize(100);
    DPF2("; Program has %d instructions\n", program.length);
    TLEngineSim sim;
    sim.Initialize(shaderType, program.length);
    float stall;
	StallType reason = eNone;
    for(int i = 0; i < program.length && SUCCEEDED(hr); i++){
        buf.Clear();
        hr = DisassembleInstruction(&program.ucode[i], buf);
        sim.Do(&program.ucode[i], stall, reason);
        DPF2("%3d: (%c %.2f) %s\n", i, stall, char(reason), buf.GetText()); 
    }
	stall = sim.CalculateFinalStall(reason);
	if(stall) {
		DPF2("; Final Stall: %c %.2f\n", char(reason), stall);
	}
    DPF2("; ---------\n");
}

#endif

//----------- end of Microcode disassembler ------------

// We're assured that the first op is a mac and the second is an ilu
bool InputsConflict_MAC_ILU(const D3DVsInstruction* mac, const D3DVsInstruction* ilu){

    bool macUsesA = kMacUsesA[mac->mac];
    bool macUsesB = kMacUsesB[mac->mac];
    bool macUsesC = kMacUsesC[mac->mac];
    if(macUsesC && ilu->ilu){
        if(mac->cmx != ilu->cmx){
            return true;
        }
        switch(ilu->cmx){
        case MX_R:
            if(mac->crr != ilu->crr){
                return true;
            }
            break;
        case MX_V:
            if(mac->va != ilu->va ){
                return true;
            }
            break;
        case MX_C:
            if(mac->ca != ilu->ca) {
                return true;
            }
            break;
        default:
            return true;
        }
        // Same inputs, but the swizzle or negate might be different
        if(mac->cne != ilu->cne){
            return true;
        }
        // Check if the effective swizzle is different
        char macSwizzles[12];
        char iluSwizzles[12];
        ComputeEffectiveSwizzles(mac,macSwizzles);
        ComputeEffectiveSwizzles(ilu,iluSwizzles);
        for(int i = 8; i < 12; i++){ // Just check C register
            char ms = macSwizzles[i];
            char is = iluSwizzles[i];
            if( ms != -1 && is != -1 && ms != is ) {
                return true; // Incompatable
            }
        }
    }
    switch(ilu->cmx){
    case MX_V:
        if((macUsesA && (mac->amx == MX_V))
            || (macUsesB && (mac->bmx == MX_V))){
            if(mac->va != ilu->va){
                return true;
            }
        }
        break;
    case MX_C:
        if((macUsesA && (mac->amx == MX_C))
            || (macUsesB && (mac->bmx == MX_C))){
            if(mac->ca != ilu->ca || mac->cin != ilu->cin){
                return true;
            }
        }

    default:
        break;
    }
    return false;
}

bool OutputsConflict_MAC_ILU(const D3DVsInstruction* mac, const D3DVsInstruction* ilu){

    if(ilu->swm){
       if(ilu->rw != 1){
            return true; // Can't be paired, ilu has to write to r1.
        }
    }

    // See Xbox OS bug 2378.
    // It appears that, when pairing instructions, the mac
    // instruction can't write to r1, even if
    // the ILU instruction is not writing to the r registers.
    // For example, the following instruction pair doesn't seem
    // to work, because r1 doesn't seem to be written.
    // sge r1.x,v0,c0.wyzw : mov o[0],v0

    if(mac->rwm){
        if(mac->rw == 1){
            return true; 
        }
    }
	/* //jgould, commented 12 July 2001. This code is unneeded, since the conditions are a subset of the above
    if(mac->rwm && ilu->swm) {
        if(mac->rw == 1 && (mac->rwm & ilu->swm)){
            return true; // Collision - both writing to same part of r1.
                         // ilu will win, but that may not be what the user intended.
        }
    }
	*/
    if(mac->owm && ilu->owm){
        return true; // Only one can write to output
    }
    return false;
}

// Does b depend upon the outputs of a?
bool InputOutputDependency(const D3DVsInstruction* a, const D3DVsInstruction* b){
    // Which output channels does A use
    OutPair outMasks[2];
    ExpandRegisterOutputMasks(outMasks, a);
    // Which input chanels does b use?
    bool aWritesToConst = (a->owm != 0) && ((a->oc & 0x100) == 0);
    bool aWritesToOut = (a->owm != 0) && ((a->oc & 0x100) != 0);
    bool bUsesA = kMacUsesA[b->mac];
    bool bUsesB = kMacUsesB[b->mac];
    bool bUsesC = b->ilu || kMacUsesC[b->mac];
    DWORD baMask = bUsesA ? (1<<(3-b->axs))|(1<<(3-b->ays))|(1<<(3-b->azs))|(1<<(3-b->aws)) : 0;
    DWORD bbMask = bUsesB ? (1<<(3-b->bxs))|(1<<(3-b->bys))|(1<<(3-b->bzs))|(1<<(3-b->bws)) : 0;
    DWORD bcMask = bUsesC ? (1<<(3-b->cxs))|(1<<(3-b->cys))|(1<<(3-b->czs))|(1<<(3-b->cws)) : 0;
    bool bBUsesConst = (bUsesA && b->amx == MX_C)
        || (bUsesB && b->bmx == MX_C)
        || (bUsesC && b->cmx == MX_C);

    for(int i = 0; i < 2; i++){
        if(outMasks[i].used){
            if(bUsesA && b->amx == MX_R && b->arr == outMasks[i].r && (baMask & outMasks[i].m)){
                return true;
            }
            if(bUsesB && b->bmx == MX_R && b->brr == outMasks[i].r && (bbMask & outMasks[i].m)){
                return true;
            }
            if(bUsesC && b->cmx == MX_R && b->crr == outMasks[i].r && (bcMask & outMasks[i].m)){
                return true;
            }
        }
    }
    if(aWritesToConst && bBUsesConst && ((b->ca == a->oc) || b->cin)) {
        return true;
    }
    if(aWritesToOut){
        // Check for the special case of writing to o[oPos] and reading from R12
        if(a->oc == 0x100){ // o[oPos]
            if(bUsesA && b->amx == MX_R && b->arr == 12 && (baMask & a->owm)){
                return true;
            }
            if(bUsesB && b->bmx == MX_R && b->brr == 12 && (bbMask & a->owm)){
                return true;
            }
            if(bUsesC && b->cmx == MX_R && b->crr == 12 && (bcMask & a->owm)){
                return true;
            }
        }
    }
    if(a->mac == MAC_ARL && bBUsesConst && b->cin){
        return true;
    }
    return false;
}

bool ConvertToImv(D3DVsInstruction* ucode, const D3DVsInstruction* mov){
    if(mov->mac != MAC_MOV){
        return false;
    }
    if(mov->ilu){
        return false;
    }
    ucode->mac = MAC_NOP;
    ucode->ilu = ILU_MOV; // A move
    ucode->ca  = mov->ca;
    ucode->va  = mov->va;
    ucode->ane = 0;
    ucode->axs = CSW_X;
    ucode->ays = CSW_Y;
    ucode->azs = CSW_Z;
    ucode->aws = CSW_W;
    ucode->amx = MX_R;
    ucode->arr = 0;
    ucode->bne = 0;
    ucode->bxs = CSW_X;
    ucode->bys = CSW_Y;
    ucode->bzs = CSW_Z;
    ucode->bws = CSW_W;
    ucode->bmx = MX_R;
    ucode->brr = 0;
    ucode->cne = mov->ane;
    ucode->cxs = mov->axs;
    ucode->cys = mov->ays;
    ucode->czs = mov->azs;
    ucode->cws = mov->aws;
    ucode->cmx = mov->amx;
    ucode->crr = mov->arr;
    ucode->rw  = mov->rw;
    ucode->rwm = 0;
    ucode->oc  = mov->oc;
    ucode->om  = mov->owm ? OM_ILU : OM_MAC;
    ucode->eos = mov->eos;
    ucode->cin = mov->cin;
    ucode->swm = mov->rwm;
    ucode->owm = mov->owm;
    return true;
}

bool SwapAC(D3DVsInstruction* pOut, const D3DVsInstruction* pIn){
    bool swapable = pIn->mac == MAC_ADD && ! pIn->ilu;
    if(swapable){
        *pOut = *pIn;
        pOut->amx = pIn->cmx;
        pOut->ane = pIn->cne;
        pOut->arr = pIn->crr;
        pOut->axs = pIn->cxs;
        pOut->ays = pIn->cys;
        pOut->azs = pIn->czs;
        pOut->aws = pIn->cws;

        pOut->cmx = pIn->amx;
        pOut->cne = pIn->ane;
        pOut->crr = pIn->arr;
        pOut->cxs = pIn->axs;
        pOut->cys = pIn->ays;
        pOut->czs = pIn->azs;
        pOut->cws = pIn->aws;
    }
    return swapable;
}

void StripMacInstruction(D3DVsInstruction* pI){
    if(pI->mac){
        if(pI->swm){
            pI->rw = 1;
            pI->rwm = pI->swm;
        }
        else {
            pI->rw = 0;
            pI->rwm = 0;
        }
        pI->mac = 0;
        pI->amx = MX_V;
        pI->arr = 0;
        pI->ane = 0;
        pI->axs = 0;
        pI->ays = 0;
        pI->azs = 0;
        pI->aws = 0;
        pI->bmx = MX_V;
        pI->brr = 0;
        pI->bne = 0;
        pI->bxs = 0;
        pI->bys = 0;
        pI->bzs = 0;
        pI->bws = 0;
    }
}

// pair is guaranteed to not be equal a or b.

bool Pairable(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b){
    // If there is a dependency between the output of a and
    // the input of b, then the instructions can't be paired
    if(InputOutputDependency(a,b)){
        return false;
    }

    DWORD dummyReason;

    if(ForcedPair(pair,a,b, &dummyReason)){
        return true;
    }
    // Special case ADD instructions - see if they're pairable with their arguments reversed
    D3DVsInstruction temp;
    if(SwapAC(&temp,a)){
        if(ForcedPair(pair,&temp,b, &dummyReason)){
            return true;
        }
    }
    else if(SwapAC(&temp,b)){
        if(ForcedPair(pair,a,&temp, &dummyReason)){
            return true;
        }
    }
    return false;
}

HRESULT PeepholeOptimize(DWORD shaderType, D3DVertexShaderProgram* program){
    HRESULT hr = S_OK;

    // in-place optimizations
    {
        TLEngineSim sim;
        hr = sim.Initialize(shaderType, program->length);
        if(SUCCEEDED(hr)){
            for(int pc = 0; pc < program->length && SUCCEEDED(hr); pc++){
                D3DVsInstruction* pI = &program->ucode[pc];
                // The second argument of an ADD instruction doesn't stall,
                // so sometimes swapping the two arguments is faster
                D3DVsInstruction temp;
                if(SwapAC(&temp, pI)){
					StallType dummysr = eNone;
                    if(sim.CalculateStall(pI, dummysr) > sim.CalculateStall(&temp, dummysr)){
                        *pI = temp;
                    }
                }
                float stall; // dummy arg
				StallType reason = eNone;
                hr = sim.Do(pI, stall, reason);
            }
        }
    }
    return hr;
}

HRESULT EstimateCycles(DWORD shaderType, D3DVertexShaderProgram* ucode, float* pCycles){
    HRESULT hr = S_OK;
    TLEngineSim sim;
    float cycles = 0;
    float stall;
    hr = sim.Initialize(shaderType, ucode->length);
    if(SUCCEEDED(hr)){
        stall = 0;
		StallType reason = eNone;
        for(int ip = 0; ip < ucode->length && SUCCEEDED(hr); ip++){
            hr = sim.Do(&ucode->ucode[ip], stall, reason);
            
			if(SASMT_SHADERTYPE(shaderType)==SASMT_VERTEXSHADER) {
				cycles += stall + 0.5;
			} else {
				cycles += stall + 1;
			}
        }

		cycles += sim.CalculateFinalStall(reason);
    }
    if(SUCCEEDED(hr)){
        *pCycles = cycles;
    }

    return hr;
}

/* Super-obscure feature.
 *
 * If we have two instructions in a row that
 * compute the same result, but one instruction
 * writes to an output register, and the
 * other instruction writes to a temporary register,
 * then we can combine those two instructions into one instruction.
 * op o[0].x,...
 * op r#.x,...
 * can be combined into one instruction.
 *
 *
 */

bool PairableMasks1(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b){
    // Do they have the same opcode?
    // ToDo - extend to handle cases where a is already paired and b is compatable.
    bool macOpSame = a->mac == b->mac;
    bool iluOpSame = a->ilu == b->ilu;
    if( ! (macOpSame && iluOpSame) ) {
        return false;
    }

    // Same inputs? TODO - don't look at unused inputs
    bool aSame = a->amx == b->amx && a->ane == b->ane && a->arr == b->arr
        && a->aws == b->aws && a->axs == b->axs && a->ays == b->ays && a->azs == b->azs;
    bool bSame = a->bmx == b->bmx && a->bne == b->bne && a->brr == b->brr
        && a->bws == b->bws && a->bxs == b->bxs && a->bys == b->bys && a->bzs == b->bzs;
    bool cSame = a->cmx == b->cmx && a->cne == b->cne && a->crr == b->crr
        && a->cws == b->cws && a->cxs == b->cxs && a->cys == b->cys && a->czs == b->czs;

    if (! (aSame && bSame && cSame) ){
        return false;
    }

    if ( ! ( a->ca == b->ca && a->va == b->va && a->cin == b->cin ) ) {
        return false;
    }

    // Is there an input-output dependency between a and b? (Does b use the result of a?
    if(InputOutputDependency(a,b)){
        return false;
    }

    // Same calculation, where does the result go?
    bool aUsesReg = (a->rwm || a->swm);
    bool bUsesReg = (b->rwm || b->swm);
    // If this isn't a reg vs. oc situation, then don't pair them.
    // If the user has global optimization turned on, then the
    // PairableMasks3 optimization will handle this case.
    // If the user doesn't have global optimization turned on,
    // then let them live with this code.
    
    if(aUsesReg && bUsesReg || a->owm && b->owm){
        return false;
    }

    // OK it's mergable, so merge it.
    *pair = *a;
    pair->rwm |= b->rwm;
    pair->swm |= b->swm;
    if(bUsesReg && ! aUsesReg){
        pair->rw = b->rw;
    }
    pair->owm |= b->owm;
    if(b->owm && !a->owm){
        pair->oc = b->oc;
        pair->om = b->om;
    }
    return true;

}

bool NullSwizzle(DWORD rwm, DWORD ne, DWORD xs, DWORD ys, DWORD zs, DWORD ws){
    return (! ne)
        && (! (rwm & 1) || ws == CSW_W)
        && (! (rwm & 2) || zs == CSW_Z)
        && (! (rwm & 4) || ys == CSW_Y)
        && (! (rwm & 8) || xs == CSW_X);
}

bool ReadsFromConst(const D3DVsInstruction* pI){
    return kMacUsesA[pI->mac] && pI->amx == MX_C
        || kMacUsesB[pI->mac] && pI->bmx == MX_C
        || kMacUsesC[pI->mac] && pI->cmx == MX_C
        || pI->ilu && pI->cmx == MX_C;
}

bool CompatableConstUsage(const D3DVsInstruction* pA, const D3DVsInstruction* pB){
    // Quick return
    if ( pA->ca == pB->ca && pA->cin == pB->cin){
        return true;
    }

    if (! (ReadsFromConst(pA) && ReadsFromConst(pB)) ){
        return true; 
    }

    return false;
}

/*
 * Alternately, if we have an instruction that writes to a temporary register,
 * and the next instruction is a mov that doesn't swizzle or negate, and just
 * copies the temporary register to an output register, then we can pair that
 * too.
 */

bool PairableMasks2(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b){
    // Is the first instruction a register output instruction
    // Is the second instruction a MOV from the same register to an output reg?
    // TODO: Currently a must be a mac instruction - but paired and ilu instructions
    // should be checked, too.
    bool pairable = ! a->owm
        && a->rwm 
        // b is a simple, non-swizzleing, non-negating move
        && b->mac == MAC_MOV
        && ! b->ilu
        && b->amx == MX_R
        && b->arr == a->rw
        && NullSwizzle(b->owm, b->ane, b->axs, b->ays, b->azs, b->aws)
        && b->om == OM_MAC
        && (b->owm & (~ (a->rwm))) == 0 // It only reads data that was written by A
        && b->owm
        && ! b->rwm
        && ! b->swm
        && CompatableConstUsage(a, b);

    if(pairable){

        // OK it's mergable, so merge it.
        *pair = *a;
        pair->owm = b->owm;
        pair->om = b->om;
        pair->oc = b->oc;
    }

    return pairable;
}

/*
 * If we have two instructions that
 * 1) have the same opcodes
 * 2) take compatable arguments
 * 3) write to compatable output registers
 * 4) don't have input/output dependencies 
 *
 * then they can be merged together.
 *
 * An example would be:
 *
 * add r0.x,r1.x,r2.x
 * add oPos.y,r1.y,r2.y
 *
 * This can be combined into
 * add {r0.x,oPos.y},r1.xy,r2.xy
 *
 * A simpler example would be:
 *
 * mov r0.y,r1.x
 * mov r0.x,r1.y
 *
 * Which would become
 *
 * mov r0.xy,r1.yx
 *
 */

// Are two operends reading data from the same sources (not counting swizzle)?

bool SourcesEqual(DWORD a_mx, DWORD a_rr, DWORD a_ne, const D3DVsInstruction* a,
                  DWORD b_mx, DWORD b_rr, DWORD b_ne, const D3DVsInstruction* b){
    return a_mx == b_mx && a_ne == b_ne &&
        ((a_mx == MX_R && a_rr == b_rr)
        || (a_mx == MX_V && a->va == b->va)
        || (a_mx == MX_C && a->ca == b->ca && a->cin == b->cin));
}

// ignoring swizzle, which compontents of the input arguments are used
// by this opcode?
// Results is a 3 character array, where results[0] = arg a's mask,
// Mask bit order is the same as the output mask

void ComputePostSwizzleUseMasks(const D3DVsInstruction* pI, UCHAR* results){
    UCHAR aMask = 0;
    UCHAR bMask = 0;
    UCHAR cMask = 0;

    UCHAR macMask = pI->rwm | ((pI->om == OM_MAC) ? pI->owm : 0);
    UCHAR iluMask = pI->swm | ((pI->om == OM_ILU) ? pI->owm : 0);

    for(int a = 0; a < 3; a++){
        results[a] =
              kMACInputRegFixedComponentUse[pI->mac][a]
            | kILUInputRegFixedComponentUse[pI->ilu][a]
            | (macMask & kMACInputRegColumnatedComponentUse[pI->mac][a])
            | (iluMask & kILUInputRegColumnatedComponentUse[pI->ilu][a]);
    }
}

void ComputeEffectiveSwizzle(UCHAR mask, UCHAR xs, UCHAR ys, UCHAR zs, UCHAR ws, char* pSwizzles){
    pSwizzles[0] = (mask & 1) ? (char) ws : -1; // We read W from where?
    pSwizzles[1] = (mask & 2) ? (char) zs : -1;
    pSwizzles[2] = (mask & 4) ? (char) ys : -1;
    pSwizzles[3] = (mask & 8) ? (char) xs : -1;
}

void ComputeEffectiveSwizzles(const D3DVsInstruction* a, char* pSwizzles){
    UCHAR masks[3];
    ComputePostSwizzleUseMasks(a, masks);
    ComputeEffectiveSwizzle(masks[0], a->axs, a->ays, a->azs, a->aws, pSwizzles);
    ComputeEffectiveSwizzle(masks[1], a->bxs, a->bys, a->bzs, a->bws, pSwizzles+4);
    ComputeEffectiveSwizzle(masks[2], a->cxs, a->cys, a->czs, a->cws, pSwizzles+8);
}

UCHAR ComputeEffectiveReadMask(UCHAR xs, UCHAR ys, UCHAR zs, UCHAR ws){
    return 1 << (3-xs) | 1 << (3-ys) | 1 << (3-zs) | 1 << (3-ws);
}

UCHAR ComputeEffectiveReadMask(const D3DVsInstruction* a, int channel){
    switch(channel){
    case 0: return ComputeEffectiveReadMask(a->axs, a->ays, a->azs, a->aws);
    case 1: return ComputeEffectiveReadMask(a->bxs, a->bys, a->bzs, a->bws);
    case 2: return ComputeEffectiveReadMask(a->cxs, a->cys, a->czs, a->cws);
    default:
        return 0;
    }
}
    
// Look at four swizzles, pick the last one that's defined
// If they're all undefined, use the default Undef.
//
// The idea here is to generate masks that look good when printed,
// e.g. .w instead of .xxxw (given that w is defined and the other fields aren't)

bool CanUseXYZW(const char* pSwizzleRun){
    const char kXYZW[4] = {CSW_W, CSW_Z, CSW_Y, CSW_X};
    for(int i = 0; i < 4; i++){
        if(pSwizzleRun[i] != -1 && pSwizzleRun[i] != kXYZW[i]){
            return false;
        }
    }
    return true;
}

char PickDefault(const char* pSwizzleRun){
    // First check if the .xyzw swizzle pattern would work
    if(CanUseXYZW(pSwizzleRun)){
        return -1;
    }

    // Otherwise, find last element that's used, so we get a pattern like
    // .w

    for(int i = 0; i < 4; i++){
        if(pSwizzleRun[i] != -1){
            return pSwizzleRun[i];
        }
    }

    // Shouldn't get here.
    return -1;
}

DWORD ChooseBestSwizzle(char swizzle, char defaultSw, char defaultDefault){
    if ( swizzle == -1 ) {
        swizzle = defaultSw;
    }
    if ( swizzle == -1 ) {
        swizzle = defaultDefault;
    }
    return (DWORD) swizzle;
}

void SetSwizzles(D3DVsInstruction* pI, const char* pSwizzles){
    char defaultSw = PickDefault(&pSwizzles[0]);
    pI->aws = ChooseBestSwizzle(pSwizzles[0], defaultSw, CSW_W);
    pI->azs = ChooseBestSwizzle(pSwizzles[1], defaultSw, CSW_Z);
    pI->ays = ChooseBestSwizzle(pSwizzles[2], defaultSw, CSW_Y);
    pI->axs = ChooseBestSwizzle(pSwizzles[3], defaultSw, CSW_X);
    
    defaultSw = PickDefault(&pSwizzles[4]);
    pI->bws = ChooseBestSwizzle(pSwizzles[4], defaultSw, CSW_W);
    pI->bzs = ChooseBestSwizzle(pSwizzles[5], defaultSw, CSW_Z);
    pI->bys = ChooseBestSwizzle(pSwizzles[6], defaultSw, CSW_Y);
    pI->bxs = ChooseBestSwizzle(pSwizzles[7], defaultSw, CSW_X);

    defaultSw = PickDefault(&pSwizzles[8]);
    pI->cws = ChooseBestSwizzle(pSwizzles[8], defaultSw, CSW_W);
    pI->czs = ChooseBestSwizzle(pSwizzles[9], defaultSw, CSW_Z);
    pI->cys = ChooseBestSwizzle(pSwizzles[10], defaultSw, CSW_Y);
    pI->cxs = ChooseBestSwizzle(pSwizzles[11], defaultSw, CSW_X);
}

bool PairableMasks3(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b){
    // Do they have the same opcode?
    // ToDo - extend to handle cases where a is already paired and b is compatable.
    bool macOpSame = a->mac == b->mac;
    bool iluOpSame = a->ilu == b->ilu;
    if( ! (macOpSame && iluOpSame) ) {
        return false;
    }

    // Do they have the same input sources
    // Swizzle can be different - we'll check that later
    if ( ! ( SourcesEqual(a->amx, a->arr, a->ane, a, b->amx, b->arr, b->ane, b)
          && SourcesEqual(a->bmx, a->brr, a->bne, a, b->bmx, b->brr, b->bne, b)
          && SourcesEqual(a->cmx, a->crr, a->cne, a, b->cmx, b->crr, b->cne, b) ) ){
        return false;
    }

    // Is there an input-output dependency between a and b? (Does b use the result of a?
    if(InputOutputDependency(a,b)){
        return false;
    }

    // Do they have compatable outputs?

    bool aUsesReg = (a->rwm || a->swm);
    bool bUsesReg = (b->rwm || b->swm);
    if(aUsesReg && bUsesReg && a->rw != b->rw){
        return false;
    }
    if(a->owm && b->owm && a->oc != b->oc){
        return false;
    }

    // Everything's OK except perhaps the swizzles.
    // Start the merge.
    *pair = *a;

    if(! MergeSwizzles(pair,a, b) ){
        return false;
    }

    pair->rwm |= b->rwm;
    pair->swm |= b->swm;
    if(bUsesReg && ! aUsesReg){
        pair->rw = b->rw;
    }
    pair->owm |= b->owm;
    if(b->owm && !a->owm){
        pair->oc = b->oc;
        pair->om = b->om;
    }

    return true;
}

bool MergeSwizzles(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b){
    char aSwizzles[12];
    char bSwizzles[12];
    char abSwizzles[12];

    ComputeEffectiveSwizzles(a,aSwizzles);
    ComputeEffectiveSwizzles(b,bSwizzles);

    // Are the swizzles incompatable?
    for(int i = 0; i < 12; i++){
        char as = aSwizzles[i];
        char bs = bSwizzles[i];
        if( as != -1 && bs != -1 && as != bs ) {
            return false; // Incompatable
        }
        abSwizzles[i] = as != -1 ? as : bs;
    }

    // Merge swizzles
    SetSwizzles(pair, abSwizzles);
    return true;
}

// A should always be the first instruction in the sequence for PairableMasks to work
// correctly.
bool PairableMasks(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b){
    return PairableMasks1(pair, a, b) || PairableMasks2(pair, a, b);
}

HRESULT PeepholePairOutputMasks(D3DVertexShaderProgram* program){
    HRESULT hr = S_OK;

    // Look to see if any microcode can be paired
    DWORD outPC = 0;
    // The pairing optimization
    for(int pc = 0; pc < program->length; pc++, outPC++){
        // Do the pairing optimization
        D3DVsInstruction pair;
        if((pc < program->length-1)
            && PairableMasks(&pair, &program->ucode[pc], &program->ucode[pc+1]))
        {
#ifdef DBG_PAIRER
            DPF2("PeepholePairOutputMasks: pairing %d with %d.\n", pc, pc+1);
            PrintInstruction(pc, program);
            PrintInstruction(pc+1, program);
            PrintInstruction(pc, &pair);
#endif
            program->ucode[outPC] = pair;
            pc++;
        }
        else {
            program->ucode[outPC] = program->ucode[pc];
        }
    }
    program->length = outPC;
    return S_OK;
}

bool MacUsesMX(const D3DVsInstruction* a, DWORD mxVal){
    return (kMacUsesA[a->mac] && a->amx == mxVal)
        || (kMacUsesB[a->mac] && a->bmx == mxVal)
        || (kMacUsesC[a->mac] && a->cmx == mxVal);
}

bool IluUsesMX(const D3DVsInstruction* a, DWORD mxVal){
    return a->ilu && a->cmx == mxVal;
}

bool UsesMX(const D3DVsInstruction* a, DWORD mxVal){
    return MacUsesMX(a, mxVal) || IluUsesMX(a, mxVal);
}

bool UsesCA(const D3DVsInstruction* a){
    return UsesMX(a, MX_C);
}

bool UsesVA(const D3DVsInstruction* a){
    return UsesMX(a, MX_V);
}

bool PairableMulAdd(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b)
{
    if ( ! (a->mac == MAC_MUL
        && ! a->ilu
        && b->mac == MAC_ADD
        && ! b->ilu
        && a->rwm
        && ! a->owm
        && b->rwm == a->rwm
        && b->rw == a->rw
        && ! (b->owm & ~ b->rwm) // If b outputs to owm, it does so using a subset of rwm
        ) ){
        return false;
    }
    // We know that a is a MUL that outputs to a register, and b is an ADD that
    // outputs to the same register.

    // Now does b input from a's register?
    bool bUsesAOpr = b->amx == MX_R && b->arr == a->rw;
    bool bUsesCOpr = b->cmx == MX_R && b->crr == a->rw;
    if(! (bUsesAOpr ^ bUsesCOpr) ) {
        return false; // One or the other, but not both.
    }

    // Are the swizzles compatable?
    if( bUsesAOpr && ! NullSwizzle(b->rwm, b->ane, b->axs, b->ays, b->azs, b->aws) ){
        return false;
    }
    if( bUsesCOpr && ! NullSwizzle(b->rwm, b->cne, b->cxs, b->cys, b->czs, b->cws) ){
        return false;
    }

    // Are the uses of the va and ca registers compatable?
    bool aUsesVA = UsesVA(a);
    bool bUsesVA = UsesVA(b);
    bool aUsesCA = UsesCA(a);
    bool bUsesCA = UsesCA(b);
    if( aUsesVA && bUsesVA && a->va != b->va){
        return false;
    }

    if(aUsesCA && bUsesCA && a->ca != b->ca){
        return false;
    }
    
    // OK, pair em up!

    *pair = *a;
    pair->mac = MAC_MAD;
    if(bUsesVA){
        pair->va = b->va;
    }
    if(bUsesCA){
        pair->ca = b->ca;
    }
    if(bUsesAOpr){
        // b uses the A operend for the register that's being MAC-ed to, so
        // copy the C register (that's the one that's being added.)
        pair->cmx = b->cmx;
        pair->cne = b->cne;
        pair->crr = b->crr;
        pair->cxs = b->cxs;
        pair->cys = b->cys;
        pair->czs = b->czs;
        pair->cws = b->cws;
    }

    if(bUsesCOpr){
        // b uses the C operend for the register that's being MAC-ed to, so
        // copy the A register (that's the one that's being added.)
        pair->cmx = b->amx;
        pair->cne = b->ane;
        pair->crr = b->arr;
        pair->cxs = b->axs;
        pair->cys = b->ays;
        pair->czs = b->azs;
        pair->cws = b->aws;
    }

    // Copy b's om
    pair->om = b->om;
    pair->owm = b->owm;
    pair->oc = b->oc;

    return true;
}

// a and b are guarenteed to be sequential instructions

bool SequentialPairable(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b)
{
    return Pairable(pair, a, b)
        || PairableMasks(pair, a, b) 
        || PairableMulAdd(pair, a, b)
        || PairableMasks3(pair, a, b);
}

// Force two instructions together, as-is, without re-arrainging instructions

bool UsesA(const D3DVsInstruction* a){
    return kMacUsesA[a->mac];
}

bool UsesB(const D3DVsInstruction* a){
    return kMacUsesB[a->mac];
}

bool UsesC(const D3DVsInstruction* a){
    return kMacUsesC[a->mac] || a->ilu;
}

void ExpandRegisterOutputMasks(OutPair* masks, const D3DVsInstruction* a){
    masks[0].m = (unsigned char) a->rwm;
    masks[0].r = (unsigned char) a->rw;
    masks[0].used = a->rwm && a->mac;
    masks[1].m = (unsigned char) a->swm;
    // 7 is traditionally the default don't-care value. No known reason.
    masks[1].r = (unsigned char) (a->ilu ? (a->mac ? 1 : a->rw): 7);   //r7
    masks[1].used = a->swm && a->ilu;
}

bool SetRegisterOutputMasks(D3DVsInstruction* a, const OutPair* masks){
    // Check to make sure that the resulting masks are expressible
    // If both mac and ilu write to registers, 
    // then mac can't write to r1, and ilu must write to r1.
    if(masks[0].used && masks[1].used){
        if(masks[0].r == 1 || masks[1].r != 1){
            return false;
        }
    }
    a->rwm = masks[0].m;
    a->swm = masks[1].m;
    a->rw = masks[0].used ? masks[0].r : masks[1].r; 
    
    return true;
}

bool MergeRegisterOutputMasks(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b)
{
    OutPair ma[2];
    OutPair mb[2];
    OutPair mp[2];
    ExpandRegisterOutputMasks(ma, a);
    ExpandRegisterOutputMasks(mb, b);
    
    // Merge
    for(int i = 0; i < 2; i++){
        mp[i] = ma[i];
        if(ma[i].used && mb[i].used ) {
            if( ma[i].r != mb[i].r) {
                // Incompatable register use
                return false;
            }
            if( ma[i].m != mb[i].m){
                // incompatable mask use
                return false;
            }
        }
        // if we get here then either ma or mb is used, but not both. We've
        // already stored the ma values, so replace them with the mb values if mb is used
        if(mb[i].used){
            mp[i] = mb[i];
        }
    }

    // If there is both a mac and an ilu instruction,
    if(pair->mac && pair->ilu){
        // if the ilu instruction uses a register,
        // then it must use r1.
        if(mp[1].used && mp[1].r != 1){
            return false;
        }
        // if the mac instruction uses a register,
        // then it must not use r1.
        if(mp[0].used && mp[0].r == 1){
            return false;
        }
    }

    return SetRegisterOutputMasks(pair, mp);
}


bool ForcedPair2(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b,
                 DWORD* pReason)
{
    // Assemble the parts
    *pair = *a;

    // merge mac
    if(b->mac){
        if(a->mac && a->mac != b->mac){
            *pReason = ERROR_ASM_CANT_PAIR_OPCODES;
            return false;
        }
        pair->mac = b->mac;
    }

    // merge ilu
    if(b->ilu){
        if(a->ilu && a->ilu != b->ilu){
            *pReason = ERROR_ASM_CANT_PAIR_OPCODES;
            return false;
        }
        pair->ilu = b->ilu;
    }


    // Merge input a
    if(UsesA(b)){
        if(UsesA(a)){
            if(b->amx != a->amx){
                *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
                return false;
            }
            if(b->ane != a->ane){
                *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
                return false;
            }
            if(b->amx == MX_R && b->arr != a->arr){
                *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
                return false;
            }
        }
        pair->amx = b->amx;
        pair->ane = b->ane;
        pair->arr = b->arr;
    }

    // Merge input b
    if(UsesB(b)){
        if(UsesB(a)){
            if(b->bmx != a->bmx){
                *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
                return false;
            }
            if(b->bne != a->bne){
                *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
                return false;
            }
            if(b->bmx == MX_R && b->brr != a->brr){
                *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
                return false;
            }
        }
        pair->bmx = b->bmx;
        pair->bne = b->bne;
        pair->brr = b->brr;
    }

    // Merge input c
    if(UsesC(b)){
        if(UsesC(a)){
            if(b->cmx != a->cmx){
                *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
                return false;
            }
            if(b->cne != a->cne){
                *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
                return false;
            }
            if(b->cmx == MX_R && b->crr != a->crr){
                *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
                return false;
            }
        }
        pair->cmx = b->cmx;
        pair->cne = b->cne;
        pair->crr = b->crr;
    }

    if(!MergeSwizzles(pair, a, b)){
        *pReason = ERROR_ASM_CANT_PAIR_INPUT_SWIZZLES;
        return false;
    }

    // ca, cin

    if(UsesCA(b)){
        if(UsesCA(a) && (a->ca != b->ca || a->cin != b->cin)){
            *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
            return false;
        }
        else {
            pair->ca = b->ca;
            pair->cin = b->cin;
        }
    }

    // va
    if(UsesVA(b)){
        if(UsesVA(a) && (a->va != b->va)){
            *pReason = ERROR_ASM_CANT_PAIR_INPUTS;
            return false;
        }
        else {
            pair->va = b->va;
        }
    }

    // om, owm, oc
    if(b->owm){
        if(a->owm && (b->om != a->om || b->owm != a->owm || b->oc != a->oc)){
            *pReason = ERROR_ASM_CANT_PAIR_OUTPUTS;
            return false;
        }
        else {
            pair->owm = b->owm;
            pair->om = b->om;
            pair->oc = b->oc;
        }
    }

    // rwm, swm, rw

    if(!MergeRegisterOutputMasks(pair, a, b)){
        *pReason = ERROR_ASM_CANT_PAIR_OUTPUTS;
        return false;
    }
    return true;
}

bool ForcedPair(D3DVsInstruction* pair, const D3DVsInstruction* a, const D3DVsInstruction* b,
                DWORD* pReason)
{
    DWORD dummyReason;
    if( ForcedPair2(pair, a, b, pReason)){
        return true;
    }

    // TODO: we might not want to pair two moves, 
	//because it wastes a potential ILU instruction

    // See if first instruction can be turned into an IMV
    D3DVsInstruction dummy;
    if(ConvertToImv(&dummy,a)){
        if(ForcedPair2(pair,&dummy, b, &dummyReason)){
            return true;
        }
    }

    // See if second instruction can be turned into an IMV
    if(ConvertToImv(&dummy,b)){
        if(ForcedPair2(pair, a, &dummy, &dummyReason)){
            return true;
        }
    }

    return false;
}

// Just combine R1 ILU instructions

HRESULT PeepholePair1(D3DVertexShaderProgram* program){
    HRESULT hr = S_OK;

    // Look to see if any microcode can be paired
    DWORD outPC = 0;
    // The pairing optimization
    for(int pc = 0; pc < program->length; pc++, outPC++){
        // Do the pairing optimization
        D3DVsInstruction pair;
        D3DVsInstruction a = program->ucode[pc];
        while((pc < program->length-1)
            && Pairable(&pair, &a, &program->ucode[pc+1]))
        {
#ifdef DBG_PAIRER
            DPF2("PeepholePair1: pairing %d with %d.\n", pc, pc+1);
            PrintInstruction(pc, program);
            PrintInstruction(pc+1, program);
            PrintInstruction(pc, &pair);
#endif
            a = pair;
            pc++;
        }
        program->ucode[outPC] = a;
    }
    program->length = outPC;
    return S_OK;
}

HRESULT PeepholePair2(D3DVertexShaderProgram* program){
    HRESULT hr = S_OK;

    // Look to see if any microcode can be paired
    DWORD outPC = 0;
    // The pairing optimization
    for(int pc = 0; pc < program->length; pc++, outPC++){
        // Do the pairing optimization
        D3DVsInstruction pair;
        D3DVsInstruction a = program->ucode[pc];
        while((pc < program->length-1)
            && SequentialPairable(&pair, &a, &program->ucode[pc+1]))
        {
#ifdef DBG_PAIRER
            DPF2("PeepholePair2: pairing %d with %d.\n", pc, pc+1);
            PrintInstruction(pc, program);
            PrintInstruction(pc+1, program);
            PrintInstruction(pc, &pair);
#endif
            a = pair;
            pc++;
        }
        program->ucode[outPC] = a;
    }
    program->length = outPC;
    return S_OK;
}


HRESULT ListD3DTokens(Buffer* pBuffer, const DWORD* tokens, DWORD length){
    HRESULT hr = S_OK;
    for(DWORD i = 0; i < length && SUCCEEDED(hr); i++){
        hr = pBuffer->Printf("%d: %08x\n", i, tokens[i]);
    }
    return hr;
}

class MyErrorLog : public XD3DXErrorLog {
public:
    MyErrorLog(){
        m_buffer = NULL;
        m_defaultFileName = NULL;
    }

    ~MyErrorLog(){
        delete m_buffer;
    }

    HRESULT Initialize(const char* defaultFileName){
        HRESULT hr = S_OK;
        m_buffer = new Buffer();
        if(!m_buffer){
            SETERROR(hr, E_OUTOFMEMORY);
        }
        if(SUCCEEDED(hr)){
            hr = m_buffer->Initialize(1024);
        }
        m_defaultFileName = defaultFileName;
        return hr;
    }

    virtual HRESULT Log(bool error, DWORD errorCode, LPCSTR filePath, DWORD line, LPCSTR message){
        HRESULT hr = S_OK;
        const char* messageType = error ? "error" : "warning";

        if(!filePath){
            filePath = m_defaultFileName;
        }
        if(!line){
            line = 1;
        }

#ifdef DBG
        DWORD startOffset = m_buffer->GetUsed();
#endif
        hr = m_buffer->Printf("%s(%d) : %s V%d: %s\n",
            filePath, line, messageType, errorCode, message);
#ifdef DBG
        const char* msg = m_buffer->GetText() + startOffset;
        DPF2("%s",msg);
#endif
        return hr;
    }
    Buffer* m_buffer;
    const char* m_defaultFileName;
};

HRESULT XGBufferCreateFromBuffer(Buffer* pBuf, LPXGBUFFER* ppXGBuf){
    HRESULT hr = S_OK;
    if(pBuf && ppXGBuf){
        // Create the listing object.
        DWORD size = pBuf->GetUsed();
        hr = XGBufferCreate(size, ppXGBuf);
        if(SUCCEEDED(hr)){
            PVOID newData = (*ppXGBuf)->GetBufferPointer();
            memcpy(newData, pBuf->GetText(), size);
//			((char*)newData)[size] = '\0';
        }
    }
    return hr;
}

extern "C" HRESULT XGOptimizeVertexShader(bool optimize, bool globalOptimize, bool verifyOptimizer, 
	DWORD shaderType, D3DVertexShaderProgram* ucode)
{

	HRESULT hr = S_OK;

#ifndef DISABLE_VERIFIER
    VerTable* pPreOptimizedTable = 0;
    VerConstTableList* pPreOptimizedConsts = 0;

    if(SUCCEEDED(hr)){
        if(verifyOptimizer && (optimize || globalOptimize)){
            hr = VerTable::BuildTable(ucode, &pPreOptimizedConsts, &pPreOptimizedTable);
            if(!pPreOptimizedTable) {
                VER_ERR(("Couldn't create PreOptimizedTable for verifier"));
            }
        }
    }
#endif

    int preOptimizationLength;

#ifdef VERIFY_EACH_STEP
    int passNumber = 0;
#endif
    do {
#ifdef VERIFY_EACH_STEP
        DPF2("Optimization pass %d\n", passNumber);
        ++passNumber;
#endif
        preOptimizationLength = ucode->length;
        if(SUCCEEDED(hr)){
            if(optimize){
                // Allow this even when global optimization is
                // turned off. Otherwise people can't hand-optimize
                // their code.

                hr = PeepholePairOutputMasks(ucode);
                if(SUCCEEDED(hr)){
#ifndef DISABLE_VERIFIER
                    hr = VerifyStep(pPreOptimizedTable, ucode, "PeepholePairOutputMasks");
#endif
                }
            }
        }

        // We run the dead code stripper after the PeepholePairOutputMasks because some
        // of the PeepholePairOutputMasks optimizations can generate dead code. Specificly:
        //     mul r1, v0, v0
        //     mov oPos, r1
        // becomes
        //     mul {r1,oPos},v0,v0
        //
        // And if r1 isn't used anywhere else, it is dead code that can be stripped.

        if(SUCCEEDED(hr)){
            if(globalOptimize){
#ifndef DISABLE_STRIPPER
                DeadCodeStripper d;
                hr = d.Run(shaderType, ucode);
                if(SUCCEEDED(hr)){
                    hr = VerifyStep(pPreOptimizedTable, ucode, "DeadCodeStripper");
                }
#endif
            }
        }
        if(SUCCEEDED(hr)){
            if(globalOptimize){
#ifndef DISABLE_RENAMER
                Renamer r;
                hr = r.Run(shaderType, ucode);

                if(SUCCEEDED(hr)){
                    hr = VerifyStep(pPreOptimizedTable, ucode, "Renamer");
                }
#endif
            }
        }
        if(SUCCEEDED(hr)){
            if(globalOptimize){
#ifndef DISABLE_REORDERER
                Reorderer r;
#ifndef DISABLE_VERIFIER
                r.pPreOptimizedTable = pPreOptimizedTable;
#endif
                hr = r.Run(shaderType, ucode);
                if(SUCCEEDED(hr)){
                    hr = VerifyStep(pPreOptimizedTable, ucode, "Reorderer");
                }
#endif
            }
        }
        if(SUCCEEDED(hr)){
            if(globalOptimize){
#ifndef DISABLE_PEEPHOLE
                hr = PeepholeOptimize(shaderType, ucode);

                if(SUCCEEDED(hr)){
#ifndef DISABLE_VERIFIER
                    hr = VerifyStep(pPreOptimizedTable, ucode, "PeepholeOptimize");
#endif
                }
#endif
            }
        }
        if(SUCCEEDED(hr)){
            if(optimize){
#ifndef DISABLE_PAIRER1
                hr = PeepholePair1(ucode);

                if(SUCCEEDED(hr)){
#ifndef DISABLE_VERIFIER
                    hr = VerifyStep(pPreOptimizedTable, ucode, "PeepholePair1");
#endif
                }
#endif
            }
        }
        if(SUCCEEDED(hr)){
            if(globalOptimize){
#ifndef DISABLE_PAIRER2
                hr = PeepholePair2(ucode);

                if(SUCCEEDED(hr)){
#ifndef DISABLE_VERIFIER
                    hr = VerifyStep(pPreOptimizedTable, ucode, "PeepholePair2");
#endif
                }
#endif // DISABLE_PAIRER2
            }
        }

        // If the number of instructions went down, try optimizing everything again,
        // because there may be more dead code, which in turn could allow more
        // pairing, and so on.
    } while(SUCCEEDED(hr) && globalOptimize && ucode->length < preOptimizationLength);

#if !defined(DISABLE_VERIFIER) && ! defined(VERIFY_EACH_STEP)
    // Check if the whole optimization succeeded
    if(SUCCEEDED(hr)){
        if((globalOptimize || optimize) && pPreOptimizedTable){
            if ( ! Verify(pPreOptimizedTable, ucode, "the optimizer") ){
                hr = E_OPTIMIZER_FAILED;
            }
        }
    }
#endif

    #ifndef DISABLE_VERIFIER
        delete pPreOptimizedTable;
        delete pPreOptimizedConsts;
    #endif

	return hr;
}


class Assembler {
public:
    static HRESULT ConvertMicrocodeToVsInstructions(bool bScreenSpace, DWORD srcDataLen, LPCVOID srcData,
        D3DVertexShaderProgram* ucode, DWORD* pShaderType, XD3DXErrorLog* pErrorLog)
    {
        HRESULT hr = S_OK;
        if(srcDataLen < 4){
            pErrorLog->Log(true, ERROR_INPUT_MICROCODE_FORMAT, NULL,1,"Invalid input microcode size.");
            hr = E_FAIL;
        }
        *pShaderType = XGSUCode_GetVertexShaderType(srcData);
        if(*pShaderType == SASMT_INVALIDSHADER){
            pErrorLog->Log(true, ERROR_INPUT_MICROCODE_FORMAT, NULL,1,"Invalid input microcode shader type.");
            hr = E_FAIL;
        }

        WORD numInstructions = 0;
        if(SUCCEEDED(hr)){
            numInstructions = ((WORD*) srcData)[1];
            if(numInstructions*sizeof(PaddedUCode) + 4 != srcDataLen){
                pErrorLog->Log(true, ERROR_INPUT_MICROCODE_FORMAT, NULL,1,"Invalid input microcode size.");
                hr = E_FAIL;
            }
        }
        int totalInstructions = numInstructions;
        if(!bScreenSpace){
            totalInstructions += 2; // For conversion to screen space
        }
        if(SUCCEEDED(hr)){
            if(totalInstructions > MAX_MICROCODE_LENGTH){
                pErrorLog->Log(true, ERROR_INPUT_MICROCODE_FORMAT, NULL,1,"Input microcode has too many instructions.");
                hr = E_FAIL;
            }
        }
        if(SUCCEEDED(hr)){
            ucode->length = numInstructions;
            for(int i = 0; i < numInstructions; i++){
                ConvertPackedDWordsToUCode((DWORD*) ((char*) srcData + i*sizeof(PaddedUCode) + 4),
                    &ucode->ucode[i]);
                ucode->ucode[i].eos = 0;
            }
        }
        if(SUCCEEDED(hr)){
            if(!bScreenSpace){
                static const DWORD kStandardPostfix[] = {
                    0x00000000, 0x0647401b, 0xc4361bff, 0x1078e800,
                    0x00000000, 0x0087601b, 0xc400286c, 0x3070e800
                };
                ConvertPackedDWordsToUCode(kStandardPostfix,
                    &ucode->ucode[ucode->length++]);
                ConvertPackedDWordsToUCode(kStandardPostfix+4,
                    &ucode->ucode[ucode->length++]);
            }
        }
        return hr;
    }
	



private:
    HRESULT ConvertMicrocodeToVsInstructions(D3DVertexShaderProgram* ucode, DWORD* pShaderType, XD3DXErrorLog* pErrorLog){
        return ConvertMicrocodeToVsInstructions(m_bInputMicrocodeIsScreenSpace, m_SrcDataLen,
            m_pSrcData, ucode, pShaderType, pErrorLog);
    }


    void SetToNOP(D3DVsInstruction* pI){
        memset(pI, 0, sizeof(D3DVsInstruction));
    }


    HRESULT InstructionsToMicrocode(bool optimize, bool globalOptimize, bool verifyOptimizer,
                                    DWORD shaderType, 
                                    D3DVertexShaderProgram* ucode,
                                    Buffer* pListing,
                                    XD3DXErrorLog* pErrorLog){
        HRESULT hr = S_OK;
        DWORD instructions = 0;
            
        if(m_bInputMicrocode){
            hr = ConvertMicrocodeToVsInstructions(ucode, &shaderType, pErrorLog);
            instructions = ucode->length;
        }
        else {
            if(! SASMT_ISSCREENSPACE(shaderType) && SASMT_SHADERTYPE(shaderType) != SASMT_VERTEXSTATESHADER){
                static const DWORD kStandardPostfix[] = {
                    0x00000000, 0x0647401b, 0xc4361bff, 0x1078e800,
                    0x00000000, 0x0087601b, 0xc400286c, 0x3070e800 ////must not have the eof flag... optimizers might move the instruction without removing the eof
                };
				ucode->ucode[ucode->length].eos = 0;
                ConvertPackedDWordsToUCode(kStandardPostfix,
                    &ucode->ucode[ucode->length++]);
                ConvertPackedDWordsToUCode(kStandardPostfix+4,
                    &ucode->ucode[ucode->length++]);
            }

			instructions = ucode->length;
		}

        DWORD preOptLength = ucode->length;
        float preOptCycles;
        if(SUCCEEDED(hr)){
            if(pListing){
                hr = pListing->Printf("// instructions: %d\r\n", instructions);
                if(SUCCEEDED(hr)){
                    hr = EstimateCycles(shaderType, ucode, &preOptCycles);
                }
            }
        }
    #ifdef DBG_PRINT_PRE_OPTIMIZED
        if(SUCCEEDED(hr)){
            Print(shaderType, *ucode);
        }
    #endif


		hr = XGOptimizeVertexShader(optimize, globalOptimize, verifyOptimizer, shaderType, ucode);

        // Generate the listing

        if(SUCCEEDED(hr)){
            if(pListing){
                switch(SASMT_SHADERTYPE(shaderType)){
                case SASMT_VERTEXSTATESHADER:
                    hr = pListing->Append("xvss.1.1\r\n");
                    break;
                case SASMT_READWRITE_VERTEXSHADER:
                    hr = pListing->Append("xvsw.1.1\r\n#pragma screenspace\r\n");
                    break;
                case SASMT_VERTEXSHADER:
                default:
                    hr = pListing->Append("xvs.1.1\r\n#pragma screenspace\r\n");
                    break;
                }
                if(SUCCEEDED(hr)){
                    hr = pListing->Append("/*slot stall */\r\n");
                }
                if(SUCCEEDED(hr)){
                    InstructionDisassembler dis;
                    TLEngineSim sim;
                    hr = sim.Initialize(shaderType, ucode->length);
					bool usedreasons[256] = {false};
                    for(int i = 0;i < ucode->length && SUCCEEDED(hr); i++){
                        float stall = 0;
						StallType reason = eNone;
                        const D3DVsInstruction* pI = &ucode->ucode[i];
                        hr = sim.Do(pI, stall, reason);
                        if(SUCCEEDED(hr)){
                            if(stall || (reason != ' ')){
								hr = pListing->Printf("/* %3d %c %.2f */  ", i, char(reason), stall);
								usedreasons[reason] = true;
								usedreasons[0] = true;
                            }
                            else { 
								hr = pListing->Printf("/* %3d        */  ", i);
                            }
                        }
                        if(SUCCEEDED(hr)){
                            hr = dis.Disassemble(pI, *pListing);
                        }
                        if(SUCCEEDED(hr)){
                            hr = pListing->Append("\r\n");
                        }
                    }
                    float stall;
					StallType reason = eNone;

					stall = sim.CalculateFinalStall(reason);
					if(stall) {
						hr = pListing->Printf("/* Final Stall: %c %.2f */ \r\n", char(reason), stall);
					}

					if(usedreasons[0] == true) {
						pListing->Printf("\r\nStall Types:\r\n");
						for(i = 1; i < 255; i++) {
							if((i != ' ') && (usedreasons[i])) {
								pListing->Printf("%c: %s\r\n", i, StallDescription[i - 'A']);
							}
						}
						pListing->Printf("\r\n");
					}

                }
            }
        }

        DWORD postOptLength = ucode->length;
        float postOptCycles;
        if(SUCCEEDED(hr)){
            if(pListing ){


                if(optimize){
                    hr = EstimateCycles(shaderType, ucode, &postOptCycles);
                    double cpi = instructions ? 
                        ((float) postOptCycles) / (instructions ): 0.f;
                    if(SUCCEEDED(hr)){
                        int deltaLength = preOptLength - postOptLength;
                        float deltaCycles = preOptCycles - postOptCycles;
                        hr = pListing->Printf("//    microcode: %d slots, %g cycles, cpi = %g\r\n",
                            postOptLength, (float)(postOptCycles), cpi);  
                        if(SUCCEEDED(hr)) {
                            hr = pListing->Printf("// Optimization saved %d slots, %g cycles.\r\n",
                                deltaLength, (float)(deltaCycles));
                        }
                    }
                }
                else {
                    double cpi = instructions ? 
                        ((float) preOptCycles) / (instructions ) : 0.f;
                        hr = pListing->Printf("//    microcode: %d slots, %g cycles, cpi = %g\r\n",
                            preOptLength, (float)preOptCycles, cpi);    
                }

            }
        }
        if(SUCCEEDED(hr)){
            if(ucode->length > 136){
                char buf[256];
                sprintf(buf, "Too many microcode instructions: %d. Max is 136.",
                    ucode->length);
                pErrorLog->Log(true, ERROR_TOOMANYINSTRUCTIONS, NULL, 0, buf);
                SETERROR(hr, E_FAIL);
            }
        }
        if(SUCCEEDED(hr)){
            if(ucode->length){
                ucode->ucode[ucode->length-1].eos = 1; // Last instruction
            }
        }

        if(hr == E_OPTIMIZER_FAILED){
            pErrorLog->Log(true,ERROR_ASM_OPTIMIZER_INTERNAL_ERROR,NULL,1,"Internal vertex shader optimizer error. Disable optimizations and try again.");
            hr = E_FAIL; // E_OPTIMIZER_FAILED is an internal failure code, don't want it getting out.
        }
        if(hr == E_ASSEMBLER_FAILED){
            pErrorLog->Log(true,ERROR_ASM_INTERNAL_ASSEMBLER_ERROR,NULL,1,"Read from uninitialized register. Turn on validator and try again.");
            hr = E_FAIL; // E_ASSEMBLER_FAILED is an internal failure code, don't want it getting out.
        }

        return hr;
    }

    HRESULT CompileVertexShaderToUCode(bool optimize, bool globalOptimize, bool verifyOptimizer,
                                       DWORD shaderType,
                                 LPXGBUFFER pTokenizedShader, 
                                 LPXGBUFFER* ppCompiledShader,
                                 Buffer* pListing,
                                 XD3DXErrorLog* pErrorLog)
    {
        HRESULT hr = S_OK;

        D3DVertexShaderProgram* pUcode = new D3DVertexShaderProgram;
        if(!pUcode){
            SETERROR(hr,E_OUTOFMEMORY);
        }

        if(SUCCEEDED(hr)){
            if(!m_bInputMicrocode){
                DWORD* pTokens = (DWORD*) pTokenizedShader->GetBufferPointer();
                DWORD tokenCount = pTokenizedShader->GetBufferSize() / sizeof(DWORD);
				hr = D3DTokensToUCode(pTokens, pUcode, pErrorLog);
            }
        }


        if(SUCCEEDED(hr)){
            hr = InstructionsToMicrocode(optimize, globalOptimize, verifyOptimizer, shaderType,
                pUcode, pListing, pErrorLog);
        }



    #ifdef DBG_PRINT_OPTIMIZED
        Print(shaderType, *pUcode);
    #endif
        if(SUCCEEDED(hr)) {
            // Return the microcode in a buffer

            // Format of vertex shader microcode is (little endian):
            // Byte offset
            //   0..1   WORD - magic number describing type of shader:
            //                 0x2078 - ordinary vertex shader
            //                 0x7778 - read/write vertex shader
            //                 0x7378 - vertex state shader
            //   2..3   WORD - length of shader, in instructions, ranges from 1 to 136
            //   4..end the shader microcode.

            DWORD headerSize = sizeof(WORD) + sizeof(WORD);
            DWORD ucodeSize = sizeof(PaddedUCode) * pUcode->length;
            DWORD size = headerSize + ucodeSize;

            LPXGBUFFER pCompiledShader;
            hr = XGBufferCreate(size, &pCompiledShader);

            if(SUCCEEDED(hr)){
                unsigned char* pC = (unsigned char*) pCompiledShader->GetBufferPointer();
                static const char kShaderTypes[3][2]
                    = {{'x',' '}, // ordinary vertex shader
                       {'x','w'}, // read/write vertex shader
                       {'x','s'}  // vertex state shader
                    };
                int shaderTypeIndex = 0;
                int shaderTypeIgnoringScreenspace = SASMT_SHADERTYPE(shaderType);
                if(shaderTypeIgnoringScreenspace == SASMT_READWRITE_VERTEXSHADER){
                    shaderTypeIndex = 1;
                }
                if(shaderTypeIgnoringScreenspace == SASMT_VERTEXSTATESHADER){
                    shaderTypeIndex = 2;
                }
                const char* pHeader = kShaderTypes[shaderTypeIndex];
                memcpy(pC, pHeader, 2);
                WORD numInstructions = pUcode->length;

                memcpy(pC + 2, &numInstructions, sizeof(numInstructions));
                DWORD* pOutCode = (DWORD*) (pC + headerSize);
                const D3DVsInstruction* pI = pUcode->ucode;
                for(int i = 0; i < pUcode->length; i++, pI++){
                    *pOutCode++ = PGM_UWORDX(pI);
                    *pOutCode++ = PGM_UWORDY(pI);
                    *pOutCode++ = PGM_UWORDZ(pI);
                    *pOutCode++ = PGM_UWORDW(pI);
                }
                *ppCompiledShader = pCompiledShader;
            }
        }

        delete pUcode;

        return hr;
    }

    HRESULT CompileShaderToUCode(bool optimize, bool globalOptimize, bool verifyOptimizer,
                                 DWORD shaderType,
                                 LPXGBUFFER pTokenizedShader, 
                                 LPXGBUFFER* ppCompiledShader,
                                 Buffer* pListing,
                                 XD3DXErrorLog* pErrorLog)
    {
        ASSERT(pTokenizedShader || m_bInputMicrocode);
        ASSERT(ppCompiledShader);
        // OK, what kind of shader is this?
        if((shaderType & 0xff) == SASMT_PIXELSHADER){
            return CompilePixelShaderToUCode(optimize, shaderType, pTokenizedShader, ppCompiledShader, pListing, pErrorLog);
        }
        else {
            return CompileVertexShaderToUCode(optimize, globalOptimize, verifyOptimizer,
                shaderType, pTokenizedShader, ppCompiledShader, pListing, pErrorLog);
        }
    }

    HRESULT StripDebugInfo(LPXGBUFFER pBuffer){
        HRESULT hr = S_OK;

        // We strip the debug info out in-place, and fix up the
        // size of the pBuffer after the fact. We can get away with this because
        // it's a private API

        const DWORD* pSource = (const DWORD*) pBuffer->GetBufferPointer();
        const DWORD* pSrcEnd = (const DWORD*) ((char*) pBuffer->GetBufferPointer() 
            + pBuffer->GetBufferSize());
        DWORD* pDest = (DWORD*) pBuffer->GetBufferPointer();
        bool bDone = false;

        // header
        *pDest++ = *pSource++;

        while(pSource < pSrcEnd && !bDone){
            DWORD token = *pSource++;
            DWORD opCode = token & D3DSI_OPCODE_MASK;
            switch (opCode){
                // No operands, no result
                case D3DSIO_NOP:
                    *pDest++ = token;
                    break;
                    // One operand
                case D3DSIO_TEXCOORD:
                case D3DSIO_TEXKILL:
                case D3DSIO_TEX:
    #if 0 // Not on XBOX
                case D3DSIO_TEXREG2AB:
                case D3DSIO_TEXREG2GR:
    #endif
                    // One total args
                    *pDest++ = token;
                    *pDest++ = *pSource++;
                    break;
                // One operand, one result
                case D3DSIO_EXP:
                case D3DSIO_EXPP:
                case D3DSIO_FRC:
                case D3DSIO_LIT:
                case D3DSIO_LOG:
                case D3DSIO_LOGP:
                case D3DSIO_MOV:
                case D3DSIO_RCC:
                case D3DSIO_RCP:
                case D3DSIO_RSQ:

                // Two operands, no results
                case D3DSIO_TEXBEM:
                case D3DSIO_TEXBEML:
                case D3DSIO_TEXM3x2PAD:
                case D3DSIO_TEXM3x2TEX:
                case D3DSIO_TEXM3x3PAD:
                case D3DSIO_TEXM3x3TEX:
                case D3DSIO_TEXM3x3DIFF:
                case D3DSIO_TEXM3x3VSPEC:

                    // Two total args
                    *pDest++ = token;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    break;

                // Two operands, one result
                case D3DSIO_ADD:
                case D3DSIO_DP3:
                case D3DSIO_DP4:
                case D3DSIO_DPH:
                case D3DSIO_DST:
                case D3DSIO_M3x2:
                case D3DSIO_M3x3:
                case D3DSIO_M3x4:
                case D3DSIO_M4x3:
                case D3DSIO_M4x4:
                case D3DSIO_MAX:
                case D3DSIO_MIN:
                case D3DSIO_MUL:
                case D3DSIO_SGE:
                case D3DSIO_SLT:
                case D3DSIO_SUB:
                    // Three operends, no result
                case D3DSIO_TEXM3x3SPEC:
                    *pDest++ = token;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    break;
                // Three operands, one result
                case D3DSIO_CND:
                case D3DSIO_LRP:
                case D3DSIO_MAD:
                    *pDest++ = token;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    break;
                    // Six total args
                case D3DSIO_XDM: // Four ops, two results
                case D3DSIO_XDD: // Four ops, two results
                    *pDest++ = token;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    break;
                    // Seven total args
                case D3DSIO_XMMA: // Four ops, Three results
                case D3DSIO_XMMC: // Four ops, Three results
                case D3DSIO_XFC:  // Seven operands, zero results
                    *pDest++ = token;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    *pDest++ = *pSource++;
                    break;
                case D3DSIO_COMMENT:
                    {
                        DWORD numDWORDs = (token & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;
                        pSource += numDWORDs;
                    }
                    break;
                case D3DSIO_END:
                    *pDest++ = token;
                    bDone = TRUE;
                    break;
                default:
                    // unhandled instruction
    #ifdef DBG
                    DPF2("Unknown opcode (%d)", token & D3DSI_OPCODE_MASK);
    #endif
                    DXGRIP("Unknown opcode");
                    break;
            }  // switch
        }

        pBuffer->size = ((char*) pDest) - (char*) pBuffer->GetBufferPointer();
        return hr;
    }

public:
    HRESULT WINAPI
    AssembleShader(
      LPCSTR pSourceFileName,
      LPCVOID pSrcData,
      UINT SrcDataLen,
      DWORD Flags,
      LPXGBUFFER* ppConstants,
      LPXGBUFFER* ppCompiledShader,
      LPXGBUFFER* ppErrorLog,
      LPXGBUFFER* ppListing,
      SASM_ResolverCallback pResolver,
      LPVOID pResolverUserData,
      LPDWORD pShaderType
      )
    {
        HRESULT hr = S_OK;

        static const char* kDefaultSourceFileName = "";

        if ( ! pSourceFileName ) {  // Simplifies developer's lives by handleing an empty file name.
            pSourceFileName = kDefaultSourceFileName;
        }
        m_pSourceFileName = pSourceFileName;
        m_pSrcData = pSrcData;
        m_SrcDataLen = SrcDataLen;
        
        bool debug = (Flags & SASM_DEBUG) != 0;
        bool optimize = (Flags & SASM_DONOTOPTIMIZE) == 0;
        bool verifyOptimizer = (Flags & SASM_VERIFY_OPTIMIZATIONS) != 0;
        bool globalOptimize = optimize && ((Flags & SASM_DISABLE_GLOBAL_OPTIMIZATIONS) == 0);
        bool validate = (Flags & SASM_SKIPVALIDATION) == 0;
        bool listing = ppListing != 0;
        bool inputTokens = 0 != (Flags & (SASM_INPUT_PIXELSHADER_TOKENS
                | SASM_INPUT_VERTEXSHADER_TOKENS
                | SASM_INPUT_READWRITE_VERTEXSHADER_TOKENS
                | SASM_INPUT_VERTEXSTATESHADER_TOKENS));
        m_bInputMicrocode = 0 != (Flags & (SASM_INPUT_MICROCODE
                | SASM_INPUT_SCREENSPACE_MICROCODE));
        bool outputShaderTokens = (Flags & SASM_OUTPUTTOKENS) != 0;
        bool preprocessOnly = (Flags & SASM_PREPROCESSONLY) != 0;
        bool skipPreprocessor = (Flags & SASM_SKIPPREPROCESSOR) != 0;
        bool isXboxShader = false;

        DWORD shaderType = 0;
        if(inputTokens){
            if((Flags & SASM_INPUT_PIXELSHADER_TOKENS) != 0){
                shaderType = SASMT_PIXELSHADER;
            }
            else if((Flags & SASM_INPUT_VERTEXSHADER_TOKENS) != 0){
                shaderType = SASMT_VERTEXSHADER;
            }
            else if((Flags & SASM_INPUT_READWRITE_VERTEXSHADER_TOKENS) != 0){
                shaderType = SASMT_READWRITE_VERTEXSHADER;
            }
            else if((Flags & SASM_INPUT_VERTEXSTATESHADER_TOKENS) != 0){
                shaderType = SASMT_VERTEXSTATESHADER;
            }
            if((Flags & SASM_INPUT_SCREENSPACE_VERTEXSHADER_TOKENS) != 0){
                shaderType |= SASMT_SCREENSPACE;
            }
            if((Flags & SASM_INPUT_NONXBOX_TOKENS) == 0){
                isXboxShader = true;
            }
        }
 
        m_bInputMicrocodeIsScreenSpace = false;
        if(m_bInputMicrocode){
            shaderType = SASMT_VERTEXSHADER;
            if((Flags & SASM_INPUT_SCREENSPACE_MICROCODE) != 0){
                m_bInputMicrocodeIsScreenSpace = true;
                shaderType |= SASMT_SCREENSPACE;
            }
            outputShaderTokens = false;
            validate = false;
            preprocessOnly = false;
            debug = false;
            skipPreprocessor = true;
        }

        // An error log
        MyErrorLog errorLog;
        hr = errorLog.Initialize(pSourceFileName);

        // Preprocessor
        LPXGBUFFER pPreProcessedSource = NULL;

        if(!(inputTokens || m_bInputMicrocode)){
            if ( skipPreprocessor ) {
                if(SUCCEEDED(hr)){
                    hr = XGBufferCreate(SrcDataLen, &pPreProcessedSource);
                    if(SUCCEEDED(hr)){
                        memcpy(pPreProcessedSource->GetBufferPointer(), pSrcData, SrcDataLen);
                    }
                }
            }
            else {
                XGBuffer inputHelper; // A dummy XGBuffer
                inputHelper.refCount = 5000; // To make sure nobody tries to delete it.
                inputHelper.pData = (LPVOID) pSrcData;
                inputHelper.size = SrcDataLen;

                if(SUCCEEDED(hr)){
                    hr = XGPreprocess(pSourceFileName,
                               pResolver,
                               pResolverUserData,
                               &inputHelper,
                               &pPreProcessedSource,
                               &errorLog);
                }
            }

            if(preprocessOnly){
                if(SUCCEEDED(hr)){
                    if(ppCompiledShader){
                        *ppCompiledShader = pPreProcessedSource;
                    }
                }
            }
        }

        // We will post-process the compiled code, to convert it from
        // opcodes to microcode. so pass in our own buffer to catch
        // the compiled code.
        
        LPXGBUFFER pTokenizedShader = 0;
        LPXGBUFFER* ppTokenizedShader = 0;
        if(!preprocessOnly){
            if( ppCompiledShader) {
                *ppCompiledShader = NULL;
                ppTokenizedShader = &pTokenizedShader;
            }

            if(SUCCEEDED(hr) && !(inputTokens||m_bInputMicrocode)){
                CD3DXAssembler assembler;
                DWORD dwFlags = 0;
                dwFlags |= 1; // Turn on debugging always. No specific constant for this
                hr = assembler.Assemble(pPreProcessedSource->GetBufferPointer(),
                    pPreProcessedSource->GetBufferSize(),
                    pSourceFileName, 0, dwFlags,
                    ppConstants, ppTokenizedShader, &errorLog, &shaderType);
                if(SUCCEEDED(hr)){
                    if(assembler.m_bScreenSpace){
                        shaderType |= SASMT_SCREENSPACE;
                    }
                    if(pShaderType){
                        *pShaderType = shaderType;
                    }
                    if(assembler.m_bXbox){
                        isXboxShader = true;
                    }
               }
            }

            if(SUCCEEDED(hr) && inputTokens){
                // Copy the input tokens to the output tokens
                ppTokenizedShader = &pTokenizedShader;
                hr = XGBufferCreate(SrcDataLen, ppTokenizedShader);
                if(SUCCEEDED(hr)){
                    memcpy((*ppTokenizedShader)->GetBufferPointer(), pSrcData, SrcDataLen);
                }
            }

#ifndef DISABLE_VALIDATOR
            if(SUCCEEDED(hr) && validate){
                if(SASMT_SHADERTYPE(shaderType) == SASMT_PIXELSHADER){
                    hr = ValidatePixelShader((DWORD*) pTokenizedShader->GetBufferPointer(),
                        NULL, 0, &errorLog);
                }
                else {
                    DWORD* pDeclaration = 0;
                    DWORD flags = 0;
                    if(isXboxShader) {
                        flags |= VSF_XBOXEXTENSIONS;
                    }
                    switch(SASMT_SHADERTYPE(shaderType)){
                    case SASMT_VERTEXSHADER:
                        // Nothing special
                        break;
                    case SASMT_READWRITE_VERTEXSHADER:
                        flags |= VSF_READWRITESHADER;
                        break;
                    case SASMT_VERTEXSTATESHADER:
                        flags |= VSF_READWRITESHADER | VSF_STATESHADER;
                        break;
                    default: DXGRIP("Unknown shader type.");
                        break;
                    }

                    hr = ValidateVertexShader((DWORD*) pTokenizedShader->GetBufferPointer(),
                        pDeclaration,
                        NULL, flags, &errorLog);
                }
            }
#endif // ndef DISABLE_VALIDATOR

            if(SUCCEEDED(hr) && outputShaderTokens && !debug){
                // Strip the debugging information out of the shader tokens.
                hr = StripDebugInfo(pTokenizedShader);
            }

            Buffer* pListBuffer = NULL;
            if(listing){
                pListBuffer = new Buffer;
                if ( ! pListBuffer ) {
                    SETERROR(hr, E_OUTOFMEMORY);
                }
                if(SUCCEEDED(hr)){
                    hr = pListBuffer->Initialize(20000);
                }
            }

            if(SUCCEEDED(hr) ){
                if(pListBuffer){
                    hr = pListBuffer->Printf("// Xbox Shader Assembler %s\r\n", VER_PRODUCTVERSION_STR);
                }
                if ( SUCCEEDED(hr) && pTokenizedShader && ppCompiledShader) {
                    if(SUCCEEDED(hr)){
                        if(outputShaderTokens){
                            if(ppCompiledShader){
                                *ppCompiledShader = pTokenizedShader;
                                pTokenizedShader->AddRef();
                            }
                            if(pListBuffer){
                                hr = ListD3DTokens(pListBuffer, (DWORD*) pTokenizedShader->GetBufferPointer(),
                                    pTokenizedShader->GetBufferSize() / sizeof(DWORD));
                            }
                        }
                    }
                }
            }

            if(SUCCEEDED(hr) && !outputShaderTokens){
                hr = CompileShaderToUCode(optimize, globalOptimize, verifyOptimizer, shaderType, pTokenizedShader, ppCompiledShader,
                    pListBuffer, &errorLog );
            }

            RELEASE(pPreProcessedSource);
            RELEASE(pTokenizedShader);

            if(SUCCEEDED(hr) && ppListing){
                hr = XGBufferCreateFromBuffer(pListBuffer, ppListing);
            }
            delete pListBuffer;
        }

        // We always want to create the error log, even if there's already an error.
        if(ppErrorLog) {
            HRESULT hr2 = XGBufferCreateFromBuffer(errorLog.m_buffer, ppErrorLog);
            if(SUCCEEDED(hr)){ // If we failed, and we aren't in an error state, report it.
                hr = hr2;      
            }
        }

        return hr;
    }
private:
    bool m_bInputMicrocode;
    bool m_bInputMicrocodeIsScreenSpace;
    LPCSTR m_pSourceFileName;
    LPCVOID m_pSrcData;
    UINT m_SrcDataLen;
};

extern "C" 
HRESULT WINAPI
XGAssembleShader(
  LPCSTR pSourceFileName,
  LPCVOID pSrcData,
  UINT SrcDataLen,
  DWORD Flags,
  LPXGBUFFER* ppConstants,
  LPXGBUFFER* ppCompiledShader,
  LPXGBUFFER* ppErrorLog,
  LPXGBUFFER* ppListing,
  SASM_ResolverCallback pResolver,
  LPVOID pResolverUserData,
  LPDWORD pShaderType
  )
{
    Assembler a;
    return a.AssembleShader(pSourceFileName, pSrcData, SrcDataLen, Flags,
        ppConstants, ppCompiledShader, ppErrorLog, ppListing, pResolver,
        pResolverUserData, pShaderType);
}

extern "C"
DWORD WINAPI XGSUCode_GetVertexShaderType(LPCVOID pMicrocode ){
    DWORD shaderType;
    WORD cookie = *(const WORD*) pMicrocode;
    switch(cookie){
    case 0x2078: // ordinary vertex shader
        shaderType = SASMT_VERTEXSHADER;
        break;
    case 0x7778: // read/write vertex shader
        shaderType = SASMT_READWRITE_VERTEXSHADER;
        break;
    case 0x7378: // vertex state shader
        shaderType = SASMT_VERTEXSTATESHADER;
        break;
    default:
        shaderType = SASMT_INVALIDSHADER;
        break;
    }
    return shaderType;
}

extern "C"
DWORD WINAPI XGSUCode_GetVertexShaderLength(LPCVOID pMicrocode ){
    return *(const WORD*) (((const UCHAR*) pMicrocode) + 2);
}

extern "C"
HRESULT WINAPI XGSUCode_CompareVertexShaders(LPCVOID pMicrocodeA, LPCVOID pMicrocodeB,
                                             LPXGBUFFER* ppErrorLog){
    HRESULT hr = S_OK;
    MyErrorLog errorLog;    
    D3DVertexShaderProgram* pA = 0;
    D3DVertexShaderProgram* pB = 0;

    hr = errorLog.Initialize("");
    if(SUCCEEDED(hr)){
        pA = new D3DVertexShaderProgram;
        if(!pA){
            hr = E_OUTOFMEMORY;
        }
    }
    if(SUCCEEDED(hr)){
        pB = new D3DVertexShaderProgram;
        if(!pB){
            hr = E_OUTOFMEMORY;
        }
    }
    DWORD shaderTypeA;
    DWORD shaderTypeB;
    if(SUCCEEDED(hr)){
        hr = Assembler::ConvertMicrocodeToVsInstructions(true, XGSUCode_GetVertexShaderLength(pMicrocodeA),
            pMicrocodeA, pA, &shaderTypeA, &errorLog);
    }
    if(SUCCEEDED(hr)){
        hr = Assembler::ConvertMicrocodeToVsInstructions(true, XGSUCode_GetVertexShaderLength(pMicrocodeA),
            pMicrocodeA, pA, &shaderTypeB, &errorLog);
    }
    if(SUCCEEDED(hr)){
        if(shaderTypeA != shaderTypeB){
            errorLog.Log(true, 0,0,0, "The two shaders are of different types.");
            hr = E_FAIL;
        }
    }
    if(SUCCEEDED(hr)){
        bool result = Verify(pA, pB, &errorLog);
        if(!result){
            hr = E_FAIL;
        }
    }
    // We always want to create the error log, even if there's already an error.
    if(ppErrorLog) {
        HRESULT hr2 = XGBufferCreateFromBuffer(errorLog.m_buffer, ppErrorLog);
        if(SUCCEEDED(hr)){ // If we failed, and we aren't in an error state, report it.
            hr = hr2;      
        }
    }
    delete pB;
    delete pA;
    return hr;

}

DWORD BASE_REG(DWORD type)
{
    switch (type  & D3DSP_REGTYPE_MASK)
    {
    case D3DSPR_INPUT:
        return REG_V0;
    case D3DSPR_RASTOUT:
        if ((type & D3DSP_REGNUM_MASK )==0) {
            return REG_O0;
        } else {
            return REG_O4;
        }
    case D3DSPR_ATTROUT:
        if(type & 0x100){
            return REG_oB0;
        }
        return REG_oD0; 
    case D3DSPR_TEXCRDOUT:
        return REG_oT0; 
    case D3DSPR_ADDR:
        return REG_ARL;
    case D3DSPR_CONST:
        return REG_C0;
    case D3DSPR_TEMP:
        return REG_R0;
    default:
        return REG_ZER; //unused register enumeration
    }
}

//re-number a dx8-style constant register number (-96 to 95) to the gpu's registers: 0 to 95
//question: wouldn't it just be easier to just always add 96? is there a case where that wouldn't work?
int MapDX8ToUcode(DWORD reg){
    if(reg <= 95){
        reg += 96;
    }
    else {
        reg = 96 - (0xfff & (~reg + 1));
    }
    return reg;
}

//input: D3d token. Output: enumerated register number
Register_t GetEnumeratedReg(DWORD token) 
{
    DWORD final;
    if ((token & D3DSP_REGTYPE_MASK) == D3DSPR_RASTOUT) {
        // psz, fog, pts
        if ((token & D3DSP_REGNUM_MASK) == 0) {
            final = REG_O0;
        } else {
            final = REG_O4;
        }
        final += (DWORD)(token & D3DSP_REGNUM_MASK);
    } else {
        // everything else
        final = BASE_REG(token);
		if(1) { // ! (token & D3DVS_ADDRESSMODE_MASK) ){
            if( final == REG_C0){
			    final += (DWORD)MapDX8ToUcode((token & D3DSP_REGNUM_MASK));
            }
            else {
                if(final == REG_oB0){ // We use 0x100 as a flag, so mask it off
                    final += (DWORD)(token & 0x01);
                }
                else {
                    final += (DWORD)(token & D3DSP_REGNUM_MASK);
                }
            }
		}
	}
    return (Register_t)final;
}

//return true if the input (which contains something from the register enumeration is of the approprate type of registers)
bool IsTempReg(Register_t reg) {
	return reg >= REG_R0 && reg <= REG_R15;
}
bool IsConstantReg(Register_t reg) {
	return reg >= REG_C0 && reg <= REG_C95 + 96;
}
bool IsInputReg(Register_t reg) {
	return reg >= REG_V0 && reg <= REG_V15;
}
bool IsOutputReg(Register_t reg) {
	return reg >= REG_O0 && reg <= REG_O15;
}

//mark as used the appropriate components of temporary registers (r0, etc)
void MarkAsUsed(DWORD token, char * pHasBeenWritten) {
	if(IsTempReg(GetEnumeratedReg(token))) {
		pHasBeenWritten[token & D3DSP_REGNUM_MASK] |= (token & D3DSP_WRITEMASK_ALL) >> 16;
	}
}

//return true if Y has been used. "c" is an element from the array passed to MarkAsUsed above
bool HasYBeenUsed(char c) {
	return (c & (D3DSP_WRITEMASK_1 >> 16))? true : false;
}
bool HasZBeenUsed(char c) {
	return (c & (D3DSP_WRITEMASK_2 >> 16))? true : false;
}

//return true if the instruction is a Scalar ILU instruction (something that reads from only .x / .w)
bool IsScalarIluInstruction(int inst) {
	switch(inst) {
	case ILU_RCP:
	case ILU_RCC:
	case ILU_RSQ:
	case ILU_EXP:
	case ILU_LOG:
		return true;
	default: 
		return false;
	}

}

//this + TempTokens is used to deal with the ugly macros, like m3x4 and frc
#define TOKEN_RETURN 0xFFFD

//convert d3d tokens to semi-final vertex shader code
//input: pTokens, pErrorLog.   Output: pProg, pErrorLog
HRESULT D3DTokensToUCode (const DWORD* pTokens, D3DVertexShaderProgram* pProg, XD3DXErrorLog* pErrorLog)
{
	char bHasBeenWritten[12] = {0};

	pTokens++;

	HRESULT hr= S_OK;
	if (!pTokens || !pProg) return E_POINTER;

	int i = 0; //index into instructions
	D3DVsInstruction* inst;
	D3DVsInstruction* lastinst = &pProg->ucode[0];

	int inputs = 0;
	int outputs = 0;

	//initially clean up the program's instructions in case we forget something later...
	memset(pProg->ucode, 0, sizeof(pProg->ucode));
	bool bDone = false;
	bool bPair;

	DWORD TempTokens[33]; //used to build new d3d tokens from the macro instructions (D3DSIO_M3x2, FRC, etc)

	//holds the line number and file name of the source of each d3d token
	DWORD       dwLineNumber = 0;
	const char* pFileName = 0;

	//start parsing!
	while(!bDone) {
		bPair =		false;

		outputs = 0;
		//inst points to current VsInstruction 
		inst = &pProg->ucode[i];

			inst->mac = MAC_NOP;
			inst->ilu = ILU_NOP;
			inst->ca  = 0;
			inst->va  = 0;
			inst->ane = 0;
			inst->axs = CSW_X;
			inst->ays = CSW_Y;
			inst->azs = CSW_Z;
			inst->aws = CSW_W;
			inst->amx = MX_V;
			inst->arr = 0;
			inst->bne = 0;
			inst->bxs = CSW_X;
			inst->bys = CSW_Y;
			inst->bzs = CSW_Z;
			inst->bws = CSW_W;
			inst->bmx = MX_V;
			inst->brr = 0;
			inst->cne = 0;
			inst->cxs = CSW_X;
			inst->cys = CSW_Y;
			inst->czs = CSW_Z;
			inst->cws = CSW_W;
			inst->cmx = MX_V;
			inst->crr = 0;
			inst->rw  = 7;      //r7
			inst->rwm = 0;
			inst->oc  = 0x1ff;
			inst->om  = OM_MAC;
			inst->eos = 0;
			inst->cin = 0;
			inst->swm = 0;
			inst->owm = 0; 


		if(*pTokens & D3DSI_COISSUE) {
			bPair = true; //paired at the end of this loop
		}

		int iRepeat = 0; //used in matrix macro instructions - holds the number of times to repeat the dp? instruction
		DWORD op = 0;	 //used in matrix macro instructions - holds the appropriate dp? instruction to generate

		//get instruction...
		switch (pTokens[0] & D3DSI_OPCODE_MASK) 
		{
		case D3DSIO_MOV: 
			inputs = 1;
			if(((pTokens[1] & D3DSP_REGTYPE_MASK) == D3DSPR_ADDR) && ((pTokens[1]&D3DSP_REGNUM_MASK) == 0)) {
				inst->mac = MAC_ARL; 
				outputs = 3;
			} else {
				inst->mac = MAC_MOV; 
				outputs = 1;
			}
			break;
		case D3DSIO_NOP: inst->mac = MAC_NOP; outputs = 0; inputs = 0; break;
		case D3DSIO_ADD: inst->mac = MAC_ADD; outputs = 1; inputs = 5; break;
		case D3DSIO_SUB: inst->mac = MAC_ADD; outputs = 1; inputs = 5; inst->cne = true; break;
		case D3DSIO_MAD: inst->mac = MAC_MAD; outputs = 1; inputs = 7; break;
		case D3DSIO_MUL: inst->mac = MAC_MUL; outputs = 1; inputs = 3; break;
		case D3DSIO_MAX: inst->mac = MAC_MAX; outputs = 1; inputs = 3; break;
		case D3DSIO_MIN: inst->mac = MAC_MIN; outputs = 1; inputs = 3; break;
		case D3DSIO_SGE: inst->mac = MAC_SGE; outputs = 1; inputs = 3; break;
		case D3DSIO_SLT: inst->mac = MAC_SLT; outputs = 1; inputs = 3; break;
		case D3DSIO_DP3: inst->mac = MAC_DP3; outputs = 1; inputs = 3; break;
		case D3DSIO_DP4: inst->mac = MAC_DP4; outputs = 1; inputs = 3; break;
		case D3DSIO_DPH: inst->mac = MAC_DPH; outputs = 1; inputs = 3; break;
		case D3DSIO_DST: inst->mac = MAC_DST; outputs = 1; inputs = 3; break;

		case D3DSIO_RCP : inst->ilu = ILU_RCP; outputs = 2; inputs = 4; break;
		case D3DSIO_RCC : inst->ilu = ILU_RCC; outputs = 2; inputs = 4; break;
		case D3DSIO_RSQ : inst->ilu = ILU_RSQ; outputs = 2; inputs = 4; break;
		case D3DSIO_EXPP: inst->ilu = ILU_EXP; outputs = 2; inputs = 4; break;
		case D3DSIO_LOGP: inst->ilu = ILU_LOG; outputs = 2; inputs = 4; break;
		case D3DSIO_LIT : inst->ilu = ILU_LIT; outputs = 2; inputs = 4; break;

		//careful: don't switch the order of these matrix macro tokens within this switch statement!
		case D3DSIO_M4x4:
			if(!op) op = D3DSIO_DP4;
			//fall through
		case D3DSIO_M3x4:
			if(!op) op = D3DSIO_DP3;
			iRepeat++; 
			//fall through
		case D3DSIO_M4x3:
			if(!op) op = D3DSIO_DP4;
			//fall through
		case D3DSIO_M3x3:
			if(!op) op = D3DSIO_DP3;
			iRepeat++; 
			//fall through
		case D3DSIO_M3x2:
			if(!op) op = D3DSIO_DP3;
			iRepeat += 2; 
			{
				int iT = 0;
				iT++; //version

				for(int j = 0; j < iRepeat; j++) {
					TempTokens[iT++] = op; //dp*

					if(!(pTokens[1] & (~D3DSP_WRITEMASK_ALL | (D3DSP_WRITEMASK_0 << j)))) {
						iT--;
						continue;
					} else {
						//output, masked appropriately (.x in first instruction, .y in second, .z in third, etc)
						TempTokens[iT++] = pTokens[1] & (~D3DSP_WRITEMASK_ALL | (D3DSP_WRITEMASK_0 << j));
					}

					//input 1 stays the same...
					TempTokens[iT++] = pTokens[2];

					//input 2 gets the register number incremened. (Mask is 0xfff, any overflow is handled later) 
					TempTokens[iT++] = pTokens[3] + j;
					if(((pTokens[3] + j + 96) & 0xff) >= 192) {
						pErrorLog->Log(true, ERROR_ASM_INVALID_REGISTER, pFileName, dwLineNumber, "matrix macro expands to use out-of-range constant register (c96, etc)");
					}

				}
				TempTokens[iT++] = TOKEN_RETURN;

				D3DVertexShaderProgram TempProg;

				//call recursively. 
				//This call will process the 2-4 DP* instructions just produced, then return
				D3DTokensToUCode(TempTokens, &TempProg, pErrorLog);
				//copy newly formed instructions to the real VsInstructions array
				for(j = 0; j < iRepeat; j++) {
					pProg->ucode[i + j] = TempProg.ucode[j];
				}

				MarkAsUsed(pTokens[1], bHasBeenWritten);
			}

			i += iRepeat;
			pTokens += 4;
			continue;

		//exp and log macros
		case D3DSIO_EXP :
		case D3DSIO_LOG : 
			{
				if ((pTokens[0] & D3DSI_OPCODE_MASK) == D3DSIO_EXP) {
					pErrorLog->Log(false, WARNING_UNSUPPORTEDMACRO, pFileName, dwLineNumber, "Xbox's EXP macro is no more accurate than EXPP, and it is slower. Consider just using EXPP, or writing your own implementation if you need greater accuracy.");
				} else {
					pErrorLog->Log(false, WARNING_UNSUPPORTEDMACRO, pFileName, dwLineNumber, "Xbox's LOG macro is no more accurate than LOGP, and it is slower. Consider just using LOGP, or writing your own implementation if you need greater accuracy.");
				}

				//produce: 
				//	expp temp.z, input
				//	mov out.mask, temp.zzzz		

				MarkAsUsed(pTokens[1], bHasBeenWritten);
					
				int temp;
				for(temp = 0; temp < 12; temp++) {
					if(!HasZBeenUsed(bHasBeenWritten[temp])) break;
				}
				if(temp == 12) {
					pErrorLog->Log(true, ERROR_NOTEMPORARYREGISERAVAILABLE,
                        pFileName, dwLineNumber, "No temporary register available for EXP / LOG macro.");
                    return E_FAIL;			
				}
				int iT = 0;
				iT++; //header

				TempTokens[iT++] = (pTokens[0] & D3DSI_OPCODE_MASK) == D3DSIO_EXP ? D3DSIO_EXPP : D3DSIO_LOGP; 
				TempTokens[iT++] = D3DSPR_TEMP | temp | D3DSP_WRITEMASK_2;
				TempTokens[iT++] = pTokens[2];
				
				TempTokens[iT++] = D3DSIO_MOV;
				TempTokens[iT++] = pTokens[1];
				TempTokens[iT++] = D3DSPR_TEMP | temp | D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z;

				TempTokens[iT++] = TOKEN_RETURN;
				D3DVertexShaderProgram TempProg;

				D3DTokensToUCode(TempTokens, &TempProg, pErrorLog);
				//copy instructions created back to the real VsInstructions array
				for(int j = 0; j < 2; j++) {
					pProg->ucode[i + j] = TempProg.ucode[j];
				}

				i += 2;
				pTokens += 3;
				continue;

			}
		//frc macro
		case D3DSIO_FRC :
			{
				MarkAsUsed(pTokens[1], bHasBeenWritten);
					
				int temp;
				for(temp = 0; temp < 12; temp++) {
					if(!HasYBeenUsed(bHasBeenWritten[temp])) break;
				}
				if(temp == 12) {
					pErrorLog->Log(true, ERROR_NOTEMPORARYREGISERAVAILABLE,
                        pFileName, dwLineNumber, "No temporary register available for FRC macro.");
                    return E_FAIL;			
				}
				
				//.xy case:
				//  expp temp.y, input.x
				//  expp out.y, input.y
				//  mov out.x, temp.y

				//.y case:
				//  expp out.y, input.y

				if((!(pTokens[1] & D3DSP_WRITEMASK_1)) || (pTokens[1] & (D3DSP_WRITEMASK_2 | D3DSP_WRITEMASK_3))) {
	                pErrorLog->Log(true, ERROR_ASM_INVALID_WRITE_MASK, pFileName,dwLineNumber, "FRC: only valid outputs masks are .xy or .y");
					return E_FAIL;
				}

				int iT = 0;
				int NumInstructions;

				iT++; //version?

				if (!(pTokens[1] & D3DSP_WRITEMASK_0)) {
					NumInstructions = 1;
					TempTokens[iT++] = D3DSIO_EXPP;
					TempTokens[iT++] = pTokens[1];
					int swy = (pTokens[2] & (3 << (D3DVS_SWIZZLE_SHIFT + 2))) >> 2;
					TempTokens[iT++] = (pTokens[2] & ~D3DVS_SWIZZLE_MASK) | (swy) | (swy << 2) | (swy << 4) | (swy << 6);
				} else {
					NumInstructions = 3;
					//we're doing .xy. Build the appropriate instructions.
					TempTokens[iT++] = D3DSIO_EXPP; 
					TempTokens[iT++] = D3DSPR_TEMP | temp | D3DSP_WRITEMASK_1;
					int swx = (pTokens[2] & (3 << (D3DVS_SWIZZLE_SHIFT)));
					TempTokens[iT++] = (pTokens[2] & ~D3DVS_SWIZZLE_MASK) | (swx) | (swx << 2) | (swx << 4) | (swx << 6);

					TempTokens[iT++] = D3DSIO_EXPP;
					TempTokens[iT++] = (pTokens[1] & (~D3DSP_WRITEMASK_ALL)) | D3DSP_WRITEMASK_1;
					int swy = (pTokens[2] & (3 << (D3DVS_SWIZZLE_SHIFT + 2))) >> 2;
					TempTokens[iT++] = (pTokens[2] & ~D3DVS_SWIZZLE_MASK) | (swy) | (swy << 2) | (swy << 4) | (swy << 6);

					TempTokens[iT++] = D3DSIO_MOV;
					TempTokens[iT++] = (pTokens[1] & (~D3DSP_WRITEMASK_ALL)) | D3DSP_WRITEMASK_0;
					TempTokens[iT++] = D3DSPR_TEMP | temp | D3DVS_X_Y | D3DVS_Y_Y | D3DVS_Z_Y | D3DVS_W_Y;
				}

				TempTokens[iT++] = TOKEN_RETURN;

				//recursively call this function, passing the four instructions we just built:
				//expp, expp, mov, return.
				//when it hits the return, we just need to increment things appropraitely.
				D3DVertexShaderProgram TempProg;

				D3DTokensToUCode(TempTokens, &TempProg, pErrorLog);
				//copy instructions created back to the real VsInstructions array
				for(int j = 0; j < NumInstructions; j++) {
					pProg->ucode[i + j] = TempProg.ucode[j];
				}

				i += NumInstructions;
				pTokens += 3;
				continue;

			}

		//token comments: we can extract the file name and line number from these
		case D3DSIO_COMMENT:
			{
				const DWORD* pComment = &pTokens[1];
				switch(*(pComment++))
				{
				case MAKEFOURCC('F','I','L','E'):
	                pFileName = (const char*)pComment;
					break;
				case MAKEFOURCC('L','I','N','E'):
	                dwLineNumber = *pComment;
					break;
				}
	
				pTokens += 1 + ((pTokens[0] & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT);
				continue;
			}

		//this is used to return from a recursive call to this function by the macro tokens, like M4x4
		case TOKEN_RETURN:
			return -7; //return an arbitrary number to make compiler happy

		//used to mark the end of d3d vertex shader tokens
		case D3DSIO_END:

			bDone = true;
			pProg->length = i;
			continue; //break to beginning of loop - we're done!
		}


		pTokens++; //if there is output, then this points to the output token... otherwise, input or next instruction

		//outputs == 0: an instruction like NOP that performs no output
		//outputs == 1: a MAC instruction
		//outputs == 2: an ILU instruction
		//outputs == 3: ARL (mov a0.x), which doesn't do traditional output in the hardware, though d3d tokens think it does
		if(outputs == 3) {
			pTokens++;   //we already parsed the output token above, so ignore it now...
			outputs = 0; 
		} 

		if(outputs) {
			DWORD token = *pTokens;

			Register_t t = GetEnumeratedReg(*pTokens);
			if(IsTempReg(t)) {
				MarkAsUsed(pTokens[0], bHasBeenWritten);
				inst->rw = t - REG_R0;
				int mask = (token & D3DSP_WRITEMASK_ALL) >> 16;
				int mask2 = ((mask & 1) << 3) | ((mask & 2) << 1) | ((mask & 4) >> 1) | ((mask & 8) >> 3);
				if(outputs == 1) {
					inst->rwm = mask2;
				} else {
					inst->swm = mask2;
				}
			} else {
				int mask = (token & D3DSP_WRITEMASK_ALL) >> 16;
				int mask2 = ((mask & 1) << 3) | ((mask & 2) << 1) | ((mask & 4) >> 1) | ((mask & 8) >> 3);
				inst->owm = mask2;
				inst->om = outputs - 1; //set output mux to 0 for MAC instruction, 1 for ILU
				if(IsConstantReg(t)) {
					inst->oc = t - REG_C0;
					if(token & D3DVS_ADDRESSMODE_MASK) inst->cin = true;
				} else {
					inst->oc = 0x100 | (t - REG_O0);
				}
			}

			pTokens++;
		}

	//parse input
		DWORD token = *pTokens;

		//if input uses A
		if(inputs & 1) {
			token = *pTokens;
			Register_t t = GetEnumeratedReg(token);

			//set A mux to pick the right input
			if(IsTempReg(t)) {
				inst->amx = 1;
				inst->arr = t - REG_R0;
			} else if (IsConstantReg(t)) {
				if(token & D3DVS_ADDRESSMODE_MASK) {
					inst->cin = true;
				}
				inst->amx = 3;
				inst->ca = t - REG_C0;
			} else /* if (IsInputReg(t)) */ {
				inst->amx = 2;
				inst->va = t - REG_V0;
			}

			//negated?
			inst->ane = (((token & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT) ? true : false);

			//swizzle
			inst->aws = (token >> (D3DVS_SWIZZLE_SHIFT + 6)) & 3;
			inst->azs = (token >> (D3DVS_SWIZZLE_SHIFT + 4)) & 3;
			inst->ays = (token >> (D3DVS_SWIZZLE_SHIFT + 2)) & 3;
			inst->axs = (token >> (D3DVS_SWIZZLE_SHIFT + 0)) & 3;

			pTokens++;
		} else {
			inst->amx = 2; //read from v0
		}
		
		//if input uses B
		if(inputs & 2) {
			token = *pTokens;
			Register_t t = GetEnumeratedReg(token);

			//set B mux to pick the right input
			if(IsTempReg(t)) {
				inst->bmx = 1;
				inst->brr = t - REG_R0;
			} else if (IsConstantReg(t)) {
				if(token & D3DVS_ADDRESSMODE_MASK) {
					inst->cin = true;
				}
				inst->bmx = 3;
				inst->ca = t - REG_C0;
			} else /* if (IsInputReg(t)) */ {
				inst->bmx = 2;
				inst->va = t - REG_V0;
			}

			//negated?
			inst->bne = (((token & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT) ? true : false);

			//swizzle
			inst->bws = (token >> (D3DVS_SWIZZLE_SHIFT + 6)) & 3;
			inst->bzs = (token >> (D3DVS_SWIZZLE_SHIFT + 4)) & 3;
			inst->bys = (token >> (D3DVS_SWIZZLE_SHIFT + 2)) & 3;
			inst->bxs = (token >> (D3DVS_SWIZZLE_SHIFT + 0)) & 3;

			pTokens++;
		} else {
			inst->bmx = 2; //read from v0
		}

		//if input uses C
		if(inputs & 4) {
			token = *pTokens;
			Register_t t = GetEnumeratedReg(token);

			//set C mux to pick the right input
			if(IsTempReg(t)) {
				inst->cmx = 1;
				inst->crr = t - REG_R0;
			} else if (IsConstantReg(t)) {
				if(token & D3DVS_ADDRESSMODE_MASK) {
					inst->cin = true;
				}
				inst->cmx = 3;
				inst->ca = t - REG_C0;
			} else /* if (IsInputReg(t)) */ {
				inst->cmx = 2;
				inst->va = t - REG_V0;
			}

			//negated? (^= is used to accomidate the SUB instruction)
			inst->cne ^= (((token & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT) ? true : false);


			//swizzle
			if(IsScalarIluInstruction(inst->ilu)) {
				inst->cws = (token >> (D3DVS_SWIZZLE_SHIFT + 6)) & 3; //grab from w
				inst->czs = (token >> (D3DVS_SWIZZLE_SHIFT + 6)) & 3;
				inst->cys = (token >> (D3DVS_SWIZZLE_SHIFT + 6)) & 3;
				inst->cxs = (token >> (D3DVS_SWIZZLE_SHIFT + 6)) & 3;
			} else {
				inst->cws = (token >> (D3DVS_SWIZZLE_SHIFT + 6)) & 3;
				inst->czs = (token >> (D3DVS_SWIZZLE_SHIFT + 4)) & 3;
				inst->cys = (token >> (D3DVS_SWIZZLE_SHIFT + 2)) & 3;
				inst->cxs = (token >> (D3DVS_SWIZZLE_SHIFT + 0)) & 3;
			}

			pTokens++;
		} else {
			inst->cmx = 2; //read from v0
		}

		//if we were supposed to pair this instruction with the previous one
		if(bPair){
            if(i == 0){
                pErrorLog->Log(true, ERROR_ASM_INVALID_INSTRUCTION_COMBINATION,
                    pFileName,dwLineNumber, "Can't pair first instruction.");
                hr = E_FAIL;
            }
            else {
                D3DVsInstruction pair;
                DWORD reason = 0;
				//try to force the two instructions to pair
                if(ForcedPair(&pair, &pProg->ucode[i-1], &pProg->ucode[i], &reason)){
                    pProg->ucode[i-1] = pair;
                    i--;
                }
                else {
                    const char* kReasons[] = {"Invalid instruction combination. Opcodes can't be combined.",
                        "Invalid instruction combination. Inputs can't be combined.",
                        "Invalid instruction combination. Input swizzles can't be combined.",
                        "Invalid instruction combination. Outputs can't be combined.",
                        "Invalid instruction combination."
                    };
                    int reasonIndex = reason - ERROR_ASM_CANT_PAIR_OPCODES;
                    if(reasonIndex < 0 || reasonIndex > 4) {
                        reasonIndex = 4;
                    }
                    pErrorLog->Log(true, reason, pFileName,dwLineNumber, kReasons[reasonIndex]);
                    hr = E_FAIL;
                }
            }
        }

		//move to next instruction
		lastinst = inst;
		i++;

	}; //END token will break out

	return hr; 
}


//splice together shaders in the ppShaderArray, return it in *pNewShader.
//return value will be S_OK unless out of memory, or S_FALSE if *pcbNewShaderBufferSize is too small
extern "C" 
HRESULT WINAPI XGSpliceVertexShaders (
/*				OUT  */ DWORD*   pNewShader,			  //pointer to buffer to fill with output
/*	OPTIONAL IN OUT  */ DWORD*   pcbNewShaderBufferSize, //How many bytes long the shader buffer is
/* 	OPTIONAL	OUT  */ DWORD*   pNewInstructionCount,   //how many instrucitons are in the newly-spliced shader
/*      IN*/ CONST DWORD* CONST* ppShaderArray,          //arrray of pointers to shaders to splice together
/*			 IN      */ DWORD    NumShaders,             //num of shaders in ppShaderArray
/*			 IN      */ BOOL     bOptimizeResults        //TRUE to optimize, FALSE to not optimize
)
{
	DWORD i;     //shader index
	int d = 1; //index into pNewShader. (start after the header)
	DWORD InstructionCount = 0;
	HRESULT hr = S_OK;

	if(DBG_CHECK(1)) {
		DWORD i = 0;
		if(NumShaders <= 0) {
			DXGRIP("XGSpliceVertexShaders: Must provide at least 1 shader to splice");
		}
		if(!ppShaderArray) {
			DXGRIP("XGSpliceVertexShaders: ppShaderArray must not be NULL");
		}

		for(i = 0; i < NumShaders; i++) {
			if(!ppShaderArray[i]) {
				DXGRIP("XGSpliceVertexShaders: one of the shader pointers is NULL!");
			}
			if(ppShaderArray[i] == pNewShader && i != 0) {
				DXGRIP("XGSpliceVertexShaders: destination shader may not be used as one of the source shaders, except the first");
			}
		}
		WORD ShaderType = *(WORD*)(ppShaderArray[0]);
		if(ShaderType != 0x2078 && ShaderType != 0x7778 && ShaderType != 0x7378) {
			DXGRIP("XGSpliceVertexShaders: Unknown Shader Type. Uninitialized data? did assembly fail?");
		}
		for(i = 0; i < NumShaders; i++) {
			if(*(WORD*)ppShaderArray[i] != ShaderType) {
				DXGRIP("XGSpliceVertexShaders: two or more source shaders are of different types");
			}
		}
	}

	if(pcbNewShaderBufferSize != NULL) {
		DWORD ByteCount = 4;
		for(i = 0; i < NumShaders; i++) {
			ByteCount += 16 * ((WORD*)(ppShaderArray[i]))[1];
		}
		if(ByteCount > *pcbNewShaderBufferSize) {
			*pcbNewShaderBufferSize = ByteCount;
			return S_FALSE;
		}
	}

	if(DBG_CHECK(pNewShader == NULL)) {
		if(pcbNewShaderBufferSize == NULL) {
			DXGRIP("XGSpliceVertexShaders: if pNewShader is NULL, pcbNewShaderBufferSize can't be. Both are NULL.");
		} else {
			DXGRIP("XGSpliceVertexShaders: pNewShader is NULL, but pcbNewShaderBufferSize is pointing to a valid buffer size");
		}
	}

	WORD OldHeader = (WORD)ppShaderArray[0][0]; //store the type info of the shader

	for(i = 0; i < NumShaders; i++) {
		WORD NumInstructions = ((WORD*)(ppShaderArray[i]))[1]; //extract the size: The second WORD of the header
		memcpy(&pNewShader[d], &ppShaderArray[i][1], NumInstructions * 16); //copy instructions, start reading after the header
		d += NumInstructions * 4; //4 == sizeof(instruction) / sizeof(DWORD)
		InstructionCount += NumInstructions;
		pNewShader[d-1] &= ~0x00000001; //clear "end-of-shader" flag
		if(DBG_CHECK(((InstructionCount > 136) && !bOptimizeResults) || (InstructionCount > 272))) {
			DXGRIP("XGSpliceVertexShaders: Resulting vertex shader is too large! Must be <= 136 instructions");
		}
	}


#ifdef DBG_SPLICER
	HANDLE hPre = CreateFile("t:\\pre.xvu", GENERIC_WRITE |GENERIC_READ,0,0,CREATE_ALWAYS,0,0);
	HANDLE hPost = CreateFile("t:\\Post.xvu", GENERIC_WRITE |GENERIC_READ,0,0,CREATE_ALWAYS,0,0);
	DWORD blah;
	WriteFile(hPre, pNewShader, d*4, &blah,0);
#endif

	if(bOptimizeResults) {
		//get the shader into D3DVertexShaderProgram format: remove spacer dword and reorder the bits appropriately
		for(i = 0; i < InstructionCount; i++) {
			//note: ins is required, since when i == 0, 1, or 2, input overlaps output.
			D3DVsInstruction ins;
			ConvertPackedDWordsToUCode(&pNewShader[i * 4 + 1], &ins);
			*(D3DVsInstruction*)&pNewShader[i * 3 + 1] = ins;
		}

		pNewShader[0] = InstructionCount; //set up the dword array like a D3DShaderProgram structure...

		//optimize, don't verify
		HRESULT hr = XGOptimizeVertexShader(true, true, false, XGSUCode_GetVertexShaderType(&OldHeader), (D3DVertexShaderProgram*)&pNewShader[0]);

		InstructionCount = pNewShader[0]; //get new instruction count
		d = InstructionCount * 4 + 1;    //change index into dword array to hold the number of dwords we're now using

		//put the shader back into the hardware's format
        for(i = InstructionCount - 1; i != 0-1; i--){
			//note: ins is required because when i == 0, 1, or 2, input overlaps output
			//also, we must go backwards so we don't overwrite our input before we've read it
			D3DVsInstruction ins = *(D3DVsInstruction*)&pNewShader[i * 3 + 1];
			D3DVsInstruction*pd=&ins;
			pNewShader[1 + i * 4] = PGM_UWORDX(pd);
			pNewShader[2 + i * 4] = PGM_UWORDY(pd);
			pNewShader[3 + i * 4] = PGM_UWORDZ(pd);
			pNewShader[4 + i * 4] = PGM_UWORDW(pd);
		}
	}

	if(!FAILED(hr)) {
		pNewShader[0] = (InstructionCount << 16) | ((DWORD)OldHeader);
		pNewShader[d-1] |= 0x00000001; //set "end-of-shader" flag
	}

#ifdef DBG_SPLICER
	WriteFile(hPost, pNewShader, d*4, &blah,0);
	CloseHandle(hPre); 
	CloseHandle(hPost);
#endif

	if(pNewInstructionCount != NULL) {
		*pNewInstructionCount = InstructionCount;
	}

	//we will only fail in low-mem situations when optimizing
	return hr;
}








#ifdef DBG

// This is a hack so that users get a reasonable error message when they
// link with D3D8XD and don't link with XGraphicsD. It could be defined
// in any source file that's included in the XGraphics library.

extern "C" {
    DWORD you_must_also_link_with_xgraphicsd_lib;
};

#endif
} // namespace XGRAPHICS

#ifndef DISABLE_TREE_PRINT
//the following functions are used when debugging the optimizers, using msdev's watch window, to print out a tree to the debug window. 
//Msdev doesn't like them being in the D3D namespace though...
void _D3DPrintTree(XGRAPHICS::VerNode*pn, int tab)
{
    if(pn == NULL) { XGRAPHICS::DPF2("PrintTree: NULL\n"); return; }
    char spaces[1024]={0};
    int i;
    for(i = 0; i < tab; i++) { spaces[i] = ' '; }
    char xyzw[4] = {'x','y','z','w'};

    if(pn->mbNegated) {
        XGRAPHICS::DPF2("%s- \n", spaces);
    }
    if(pn->mbHasReg) {
        if(pn->mbHasTable) {
            XGRAPHICS::DPF2("%s C[%d + a0.x]\n", spaces, pn->mReg-96);
        } else {
            if(pn->mReg < 0x40) {
                XGRAPHICS::DPF2("%s Reg: V%d.%c\n", spaces, pn->mReg/4, xyzw[pn->mReg&3]);
            } else if (pn->mReg < 0x80) {
                XGRAPHICS::DPF2("%s Reg: O%d.%c\n", spaces, pn->mReg/4 - 16, xyzw[pn->mReg&3]);
            } else if (pn->mReg < 0x380) {
                XGRAPHICS::DPF2("%s Reg: C%d.%c\n", spaces, pn->mReg/4 - 32, xyzw[pn->mReg&3]);
            } else {
                XGRAPHICS::DPF2("%s Reg: R%d.%c\n", spaces, pn->mReg/4 - 0xe0, xyzw[pn->mReg&3]);
            }
        }
    }
    if(pn->mbHasTable) {
        XGRAPHICS::DPF2("%s TABLE.\n", spaces);
        _D3DPrintTree(pn->mpArl, tab + 4);
    }
    if(pn->mbHasOp) {
        XGRAPHICS::DPF2("%s ", spaces); XGRAPHICS::PrintVerOpcode(pn->mpOp->mOp); XGRAPHICS::DPF2("\n");
        for(i = 0; i < pn->mpOp->mNumParams; i++) {
            _D3DPrintTree(&pn->mpOp->mpParams[i], tab + 3);
        }
    }
}

void _D3DDebugSpewNewLine(){
    XGRAPHICS::DPF2("\n");
}
#endif //ndef DISABLE_TREE_PRINT


// Used to track down a build issue - I beleive that the windows tools may be being built against
// older versions of the xgraphics library. This should help ensure that the code is built
// against a consistent version.

extern "C" const char* XGraphicsBuildVersion() {
    return VER_PRODUCTVERSION_STR;
}




