/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       ShaderSnapshot.cpp
 *  Content:    Code to capture vertex and pixel shader snapshots for debug
 *
 ***************************************************************************/

#include "precomp.hpp"
#include "dm.h"
#include "rdi.h"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#define SHADER_CAPTURE_BUFFER_SIZE 32768

#define SS_REGISTER_T0IN      0x40000000
#define SS_REGISTER_T1IN      0x40000001
#define SS_REGISTER_T2IN      0x40000002
#define SS_REGISTER_T3IN      0x40000003

#define DRAWVERT_TYPE_VUP     0x1
#define DRAWVERT_TYPE_IVUP    0x2
#define DRAWVERT_TYPE_V       0x3
#define DRAWVERT_TYPE_IV      0x4

// Identifiers for data blocks in the capture buffer
#define PSS_EOF         0x00000000
#define PSS_PIXEL       0x01000000
#define PSS_FINAL       0x02000000
#define PSS_PRIMITIVE   0x03000000
#define PSS_STAGE       0x04000000
#define PSS_FINALSTAGE  0x05000000
#define PSS_IMAGE       0x06000000
#define PSS_NOSHADER    0x07000000
#define PSS_STAGE0      0x08000000

#define VSS_EOF         0x80000000
#define VSS_VERTEX      0x81000000
#define VSS_INSTRUCTION 0x82000000
#define VSS_PROGRAM     0x84000000
#define VSS_STATS       0x85000000

// opcode bits
#define SDOP_PSACTIVE   0x00000001
#define SDOP_VSACTIVE   0x00000002
#define SDOP_XRACTIVE   0x00000004
#define SDOP_HASALPHA   0x00000008

// encodings for Xray flags
#define INCREMENTSHIFT  4
#define INCREMENTMASK   (0xff << INCREMENTSHIFT)

#define FILLMODESHIFT   12
#define FILLMODEMASK    (0x3 << FILLMODESHIFT)

#define ZENABLESHIFT    14
#define ZENABLEMASK     (0x3 << ZENABLESHIFT)

#define STENCILENABLESHIFT    16
#define STENCILENABLEMASK     (0x3 << STENCILENABLESHIFT)

#define CULLMODESHIFT    18
#define CULLMODEMASK     (0x3 << CULLMODESHIFT)

#define GRAYSHIFT       20
#define GRAYMASK        (0x1 << GRAYSHIFT)

typedef struct _regval
{
    short a,r,g,b;
} REGVAL;

typedef struct _pscapture
{
    DWORD dwPreColor, dwPreDepth;
    DWORD dwCombinerCount;
    REGVAL t0in, t1in, t2in, t3in;
    REGVAL v0[9], v1[9];
    REGVAL t0[9], t1[9], t2[9], t3[9];
    REGVAL c0[9], c1[9];
    REGVAL r0[9], r1[9];
    REGVAL fog, sum, prod;
    DWORD dwPostColor, dwPostDepth;
} PSCAPTURE;

typedef struct _userstateentry
{
    D3DRENDERSTATETYPE Enum;
    DWORD Value;
} USERSTATEENTRY;

typedef struct vtxreg
{
    DWORD w,z,y,x;
} VTXREG;

typedef struct vtxio
{
    DWORD regs;     // lower 16 are VAB, upper are temp
    DWORD outs;     // output registers
    DWORD cnst;     // which constant
} VTXIO;

class DebugSnapshot
{
public:
    HRESULT HandleDrawVertices();
    HRESULT DrawVertices();
    
    // pixel shader methods
    HRESULT HandlePSDrawVertices();
    void BeginPSSnapshot(DWORD *args);
    void GetColorAndDepth(DWORD *pColor, DWORD *pDepth);
    void SetColorAndDepth(DWORD Color, DWORD Depth);
    void EndPSSnapshot(void);
    void GetStageData(UINT i);
    void GetRegister(DWORD dwReg, UINT i);
    void SaveUserState(void);
    void RestoreUserState(BOOL bUsingPixelShader);
    void SetPassThroughState(void);
    void WriteCapture(DWORD val);
    void DumpImage(void);
    void WriteColor24(DWORD val);
    
    // vertex shader methods
    HRESULT HandleVSDrawVertices();
    void BeginVSSnapshot(DWORD *args);
    void EndVSSnapshot(void);

    // xray methods
    HRESULT HandleXRDrawVertices();
    
public:    
    DWORD DrawVertType;
    D3DPRIMITIVETYPE PrimitiveType;
    UINT VertexCount;
    CONST void* pIndexData;
    CONST void* pVertexStreamZeroData;
    UINT VertexStreamZeroStride;
    UINT StartVertex;
    DWORD dwCaptureCount;
    DWORD dwFlags;
    DWORD dwMarker;
    DWORD dwXRFlags;

private:    
    // pixel shader capture
    DWORD dwX, dwY;     // pixel to sample
    IDirect3DSurface8 *pBackBuffer;
    IDirect3DSurface8 *pDepthBuffer;
    PSCAPTURE pscpt;
    DWORD dwXScale, dwYScale;
    USERSTATEENTRY UserState[50];
    DWORD UserStateCount;
    DWORD dwScissorRectCount;
    D3DRECT ScissorRects[D3DSCISSORS_MAX];
    BOOL  bScissorExclusive;

    // vertex shader capture
    VTXREG MaskOutputs[16];
    INT a0x;
    DWORD ShaderVerts, OtherVerts;
    DWORD dwProcessedVertexCount;
    DWORD dwFirstVertex, dwLastVertex;
    void DumpVtxInstruction(int inst, VTXREG *VtxProgram, VTXREG *VtxConstant, VTXREG *vals, DWORD *valcnt, BYTE *indexes, DWORD *idxcnt);
};

//------------------------------------------------------------------------------
// The following code and global variables are only needed in debug builds.

DebugSnapshot g_snapshot;

void
DebugSnapshot::WriteColor24(DWORD val)
{
    static DWORD dwByteCnt = 0;
    static DWORD dwData = 0;
    BYTE b;
    UINT i;
    for(i=0; i<3; i++)
    {
        switch(i)
        {
        case 0: b=(BYTE)((val >>  0) & 0xff); break;
        case 1: b=(BYTE)((val >>  8) & 0xff); break;
        case 2: b=(BYTE)((val >> 16) & 0xff); break;
        }
        switch(dwByteCnt)
        {
        case 0: dwData |= (DWORD)b <<  0; dwByteCnt++; break;
        case 1: dwData |= (DWORD)b <<  8; dwByteCnt++; break;
        case 2: dwData |= (DWORD)b << 16; dwByteCnt++; break;
        case 3: 
            dwData |= (DWORD)b << 24;
            WriteCapture(dwData);
            dwData = 0;
            dwByteCnt = 0;
            break;
        }
    }
}

void
DebugSnapshot::WriteCapture(DWORD val)
{
    if(dwCaptureCount >= SHADER_CAPTURE_BUFFER_SIZE/4)
        return;
    
    CDevice* pDevice = g_pDevice;
    *pDevice->m_pShaderCapturePtr = val;
    pDevice->m_pShaderCapturePtr++;
    dwCaptureCount++;
}

void
DebugSnapshot::RestoreUserState(BOOL bUsingPixelShader)
{
    UINT i;
    CDevice* pDevice = g_pDevice;

    if(bUsingPixelShader)
    {
        for(i=0; i<UserStateCount; i++)
            pDevice->SetRenderState(UserState[i].Enum, UserState[i].Value);
    }
    else
    {
        for(i=0; i<UserStateCount; i++)
        {
            if((UserState[i].Enum >= D3DRS_PS_MIN) && (UserState[i].Enum <= D3DRS_PS_MAX))
                continue;
            pDevice->SetRenderState(UserState[i].Enum, UserState[i].Value);
        }
    }

    D3D__DirtyFlags |= (D3DDIRTYFLAG_COMBINERS | D3DDIRTYFLAG_SHADER_STAGE_PROGRAM | D3DDIRTYFLAG_SPECFOG_COMBINER);

    // restore scissor rects
    pDevice->SetScissors(dwScissorRectCount, bScissorExclusive, ScissorRects);
}

void
DebugSnapshot::SaveUserState(void)
{
    UINT i;
    CDevice* pDevice = g_pDevice;

    for(i=0; i<UserStateCount; i++)
        UserState[i].Value = D3D__RenderState[UserState[i].Enum];

    // Get current scissor rects
    pDevice->GetScissors(&dwScissorRectCount, &bScissorExclusive, ScissorRects);
}

void
DebugSnapshot::SetPassThroughState(void)
{
    DWORD dwFCabcd, dwFCefg, dwFCefgSave;
    CDevice* pDevice = g_pDevice;

    // set scissor rects to a single pixel
    D3DRECT rect;
    rect.x1 = dwX;
    rect.y1 = dwY;
    rect.x2 = rect.x1 + 1;
    rect.y2 = rect.y1 + 1;
    pDevice->SetScissors(1, FALSE, &rect);

    // set pass-through mode
    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE); // no alpha test
    pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);   // no stencil test
    pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);         // no z test
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE); // write only source color
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
    pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED |
                                                         D3DCOLORWRITEENABLE_GREEN |
                                                         D3DCOLORWRITEENABLE_BLUE |
                                                         D3DCOLORWRITEENABLE_ALPHA);
    // set up final combiner for pass-through
    dwFCabcd = PS_COMBINERINPUTS( 
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);

    // keep E and F inputs unchanged
    dwFCefg = PS_COMBINERINPUTS(
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_REGISTER_R0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
        PS_FINALCOMBINERSETTING_CLAMP_SUM | 0 | 0);
    pDevice->SetRenderState(D3DRS_PSFINALCOMBINERINPUTSABCD, dwFCabcd);
    dwFCefgSave = D3D__RenderState[D3DRS_PSFINALCOMBINERINPUTSEFG];
    dwFCefg &= 0x0000ffff;  // clear new E and F bits
    dwFCefgSave &= 0xffff0000;  // remove G and flags
    dwFCefg |= dwFCefgSave;     // combine old E and F with new G and flags
    pDevice->SetRenderState(D3DRS_PSFINALCOMBINERINPUTSEFG, dwFCefg);
    
}

void 
DebugSnapshot::BeginPSSnapshot(DWORD *args)
{
    CDevice* pDevice = g_pDevice;
    
    dwCaptureCount = 0;     //initialize count of dwords written

    dwX = *args;     // set pixel offsets for this session
    dwY = *(args+1);
    dwFlags = *(args+2);
    dwMarker = *(args+3);

    WriteCapture(PSS_PIXEL | 2);
    WriteCapture((dwY << 16) | dwX);
    
    // initialize user state storage
    UserStateCount = 0;
    UserState[UserStateCount++].Enum = D3DRS_ALPHATESTENABLE;
    UserState[UserStateCount++].Enum = D3DRS_STENCILENABLE;
    UserState[UserStateCount++].Enum = D3DRS_ZENABLE;
    UserState[UserStateCount++].Enum = D3DRS_ALPHABLENDENABLE;
    UserState[UserStateCount++].Enum = D3DRS_SRCBLEND;
    UserState[UserStateCount++].Enum = D3DRS_DESTBLEND;
    UserState[UserStateCount++].Enum = D3DRS_COLORWRITEENABLE;
    UserState[UserStateCount++].Enum = D3DRS_PSTEXTUREMODES;
    UserState[UserStateCount++].Enum = D3DRS_PSFINALCOMBINERINPUTSABCD;
    UserState[UserStateCount++].Enum = D3DRS_PSFINALCOMBINERINPUTSEFG;
    UserState[UserStateCount++].Enum = D3DRS_PSCOMBINERCOUNT;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBINPUTS0;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBINPUTS1;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBINPUTS2;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBINPUTS3;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBINPUTS4;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBINPUTS5;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBINPUTS6;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBINPUTS7;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAINPUTS0;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAINPUTS1;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAINPUTS2;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAINPUTS3;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAINPUTS4;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAINPUTS5;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAINPUTS6;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAINPUTS7;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBOUTPUTS0;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBOUTPUTS1;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBOUTPUTS2;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBOUTPUTS3;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBOUTPUTS4;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBOUTPUTS5;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBOUTPUTS6;
    UserState[UserStateCount++].Enum = D3DRS_PSRGBOUTPUTS7;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAOUTPUTS0;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAOUTPUTS1;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAOUTPUTS2;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAOUTPUTS3;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAOUTPUTS4;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAOUTPUTS5;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAOUTPUTS6;
    UserState[UserStateCount++].Enum = D3DRS_PSALPHAOUTPUTS7;
    UserState[UserStateCount++].Enum = D3DRS_PSFINALCOMBINERINPUTSABCD;
    UserState[UserStateCount++].Enum = D3DRS_PSFINALCOMBINERINPUTSEFG;

    if(D3D_OK != pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &(pBackBuffer)))
        pBackBuffer = NULL;
    if(D3D_OK != pDevice->GetDepthStencilSurface(&(pDepthBuffer)))
        pDepthBuffer = NULL;

    dwXScale = Round(pDevice->m_AntiAliasScaleX);
    dwYScale = Round(pDevice->m_AntiAliasScaleY);
}

void 
DebugSnapshot::GetColorAndDepth(DWORD *pColor, DWORD *pDepth)
{
    D3DLOCKED_RECT lr;
    
    g_pDevice->BlockUntilIdle();   // kick off the pushbuffer and wait until the hw is idle

    if(pBackBuffer)
    {
        pBackBuffer->LockRect(&lr, NULL, D3DLOCK_TILED);
        *pColor = *((DWORD *)lr.pBits + dwYScale*dwY*lr.Pitch/4 + dwXScale*dwX);
        pBackBuffer->UnlockRect();
    }
    else
        *pColor = 0;

    if(pDepthBuffer)
    {
        pDepthBuffer->LockRect(&lr, NULL, D3DLOCK_TILED);
        *pDepth = *((DWORD *)lr.pBits + dwYScale*dwY*lr.Pitch/4 + dwXScale*dwX);
        pDepthBuffer->UnlockRect();
    }
    else
        pDepth = 0;
}

void 
DebugSnapshot::SetColorAndDepth(DWORD Color, DWORD Depth)
{
    D3DLOCKED_RECT lr;
    
    g_pDevice->BlockUntilIdle();   // kick off the pushbuffer and wait until the hw is idle

    if(pBackBuffer)
    {
        pBackBuffer->LockRect(&lr, NULL, D3DLOCK_TILED);
        *((DWORD *)lr.pBits + dwYScale*dwY*lr.Pitch/4 + dwXScale*dwX) = Color;
        pBackBuffer->UnlockRect();
    }

    if(pDepthBuffer)
    {
        pDepthBuffer->LockRect(&lr, NULL, D3DLOCK_TILED);
        *((DWORD *)lr.pBits + dwYScale*dwY*lr.Pitch/4 + dwXScale*dwX) = Depth;
        pDepthBuffer->UnlockRect();
    }
}

void
DebugSnapshot::DumpImage(void)
{
    // dump 64x64 image surrounding the target
    D3DLOCKED_RECT lr;
    
    if(pBackBuffer)
    {
        WriteCapture(PSS_IMAGE | (3072+2));
        WriteCapture((64 << 16) | 64);          // size of image
        pBackBuffer->LockRect(&lr, NULL, D3DLOCK_TILED);
        
        INT iX, iY;
        for(iY=(dwY+32); iY>(INT)(dwY-32); iY--)
        {
            for(iX=(dwX-32); iX<(INT)(dwX+32); iX++)
            {
                if((iY<0) || (iY>=480) || (iX<0) || (iX>=640))
                    WriteColor24(0);
                else
                    WriteColor24(*((DWORD *)lr.pBits + dwYScale*iY*lr.Pitch/4 + dwXScale*iX));
            }
    
        }
        pBackBuffer->UnlockRect();
    }
}

void 
DebugSnapshot::EndPSSnapshot(void)
{
    // write final color to capture buffer
    DWORD dwColor, dwDepth;
    GetColorAndDepth(&dwColor, &dwDepth);

    WriteCapture(PSS_FINAL | 3);
    WriteCapture(dwColor);
    WriteCapture(dwDepth);
    
    // write an image of the pixels surrounding the target
    DumpImage();

    // mark the end of the buffer
    WriteCapture(PSS_EOF);

    if(pBackBuffer)
        pBackBuffer->Release();
    if(pDepthBuffer)
        pDepthBuffer->Release();
}

HRESULT
DebugSnapshot::DrawVertices()
{
    CDevice* pDevice = g_pDevice;
    DWORD dwSave = pDevice->m_dwSnapshot;
    BOOL bVSD;  // true if we're in the vertex shader debugger

    bVSD = ((pDevice->m_dwSnapshot & SDOP_VSACTIVE) != 0);

    // Leave snapshot mode and call the appropriate D3D DrawVertices method
    pDevice->m_dwSnapshot = 0x0;
    switch(DrawVertType)
    {
    case DRAWVERT_TYPE_VUP:
        pDevice->DrawVerticesUP(PrimitiveType,
                                VertexCount,
                                pVertexStreamZeroData,
                                VertexStreamZeroStride);

        break;
    case DRAWVERT_TYPE_IVUP:
        if(bVSD)
            // only draw 1 vertex so vertex cache doesn't keep shader from running
            pDevice->DrawIndexedVerticesUP(PrimitiveType,
                                           1,
                                           (const WORD *)pIndexData + VertexCount-1,
                                           pVertexStreamZeroData,
                                           VertexStreamZeroStride);
        else
            // draw all vertices
            pDevice->DrawIndexedVerticesUP(PrimitiveType,
                                           VertexCount,
                                           (const WORD *)pIndexData,
                                           pVertexStreamZeroData,
                                           VertexStreamZeroStride);
        break;
    case DRAWVERT_TYPE_V:
        pDevice->DrawVertices(PrimitiveType,
                              StartVertex,
                              VertexCount);

        break;
    case DRAWVERT_TYPE_IV:
        if(bVSD)
            // only draw 1 vertex so vertex cache doesn't keep shader from running
            pDevice->DrawIndexedVertices(PrimitiveType,
                                         1,
                                         (const WORD *)pIndexData + VertexCount-1);
        else
            // draw all vertices
            pDevice->DrawIndexedVertices(PrimitiveType,
                                         VertexCount,
                                         (const WORD *)pIndexData);
        break;
    }
    pDevice->m_dwSnapshot = dwSave;
    return S_OK;
}

static const char* kOutNames[] = {"oPos",
    "o1?", "o2?",
    "oD0", "oD1",
    "oFog", "oPts",
    "oB0", "oB1",
    "oT0", "oT1", "oT2", "oT3", "???"
};

void GetVertexInputsAndOutputs(VTXREG *pInst, VTXIO *pInputs, VTXIO *pOutputs)
{
    // initialize
    pInputs->regs = pOutputs->regs = 0;           // no regs selected
    pInputs->outs = pOutputs->outs = 0;           // no outputs selected
    pInputs->cnst = pOutputs->cnst = 0xffffffff;  // invalid constant

    // decode instruction
    DWORD aws = (pInst->y >>  0) & 0x3;
    DWORD azs = (pInst->y >>  2) & 0x3;
    DWORD ays = (pInst->y >>  4) & 0x3;
    DWORD axs = (pInst->y >>  6) & 0x3;
    DWORD ane = (pInst->y >>  8) & 0x1;
    DWORD va =  (pInst->y >>  9) & 0xf;
    DWORD ca =  (pInst->y >> 13) & 0xff;
    DWORD mac = (pInst->y >> 21) & 0xf;
    DWORD ilu = (pInst->y >> 25) & 0x7;

    DWORD crr = (pInst->z <<  2) & 0xc;
    DWORD cws = (pInst->z >>  2) & 0x3;
    DWORD czs = (pInst->z >>  4) & 0x3;
    DWORD cys = (pInst->z >>  6) & 0x3;
    DWORD cxs = (pInst->z >>  8) & 0x3;
    DWORD cne = (pInst->z >> 10) & 0x1;
    DWORD bmx = (pInst->z >> 11) & 0x3;
    DWORD brr = (pInst->z >> 13) & 0xf;
    DWORD bws = (pInst->z >> 17) & 0x3;
    DWORD bzs = (pInst->z >> 19) & 0x3;
    DWORD bys = (pInst->z >> 21) & 0x3;
    DWORD bxs = (pInst->z >> 23) & 0x3;
    DWORD bne = (pInst->z >> 25) & 0x1;
    DWORD amx = (pInst->z >> 26) & 0x3;
    DWORD arr = (pInst->z >> 28) & 0xf;

    DWORD eos = (pInst->w >>  0) & 0x1;
    DWORD cin = (pInst->w >>  1) & 0x1;
    DWORD om =  (pInst->w >>  2) & 0x1;
    DWORD oc =  (pInst->w >>  3) & 0x1ff;
    DWORD owm = (pInst->w >> 12) & 0xf;
    DWORD swm = (pInst->w >> 16) & 0xf;
    DWORD rw =  (pInst->w >> 20) & 0xf;
    DWORD rwm = (pInst->w >> 24) & 0xf;
    DWORD cmx = (pInst->w >> 28) & 0x3;
    crr |= (pInst->w >> 30) & 0x3;

    // decode inputs
    if(mac != 0)
    {
        switch(amx)
        {
        case 1: pInputs->regs |= (1 << (16+arr)); break;   // temp register input
        case 2: pInputs->regs |= (1 << va); break;       // vab register input
        case 3: pInputs->cnst = ca;                      // constant
        }
    }
    if((mac != 0x1) &&  // MAC_MOV
       (mac != 0x3) &&  // MAC_ADD
       (mac != 0xd))    // MAC_ARL
    {
        switch(bmx)
        {
        case 1: pInputs->regs |= (1 << (16+brr)); break;   // temp register input
        case 2: pInputs->regs |= (1 << va); break;       // vab register input
        case 3: pInputs->cnst = ca;                      // constant
        }
    }
    if((mac == 0x3) ||  // MAC_ADD
       (mac == 0x4) ||  // MAC_MAD
       (ilu != 0))
    {
        switch(cmx)
        {
        case 1: pInputs->regs |= (1 << (16+crr)); break;   // temp register input
        case 2: pInputs->regs |= (1 << va); break;       // vab register input
        case 3: pInputs->cnst = ca;                      // constant
        }
    }

    // decode outputs
    if(rwm)
        pOutputs->regs |= (1 << (16+rw));              // temp register output
    
    if(owm)
    {
        if(oc & 0x100)
            pOutputs->outs |= (1 << (oc & 0xff));
        else
            pOutputs->cnst = oc;
    }
    if(swm)
    {
        if((mac != 0) && (ilu != 0))
            pOutputs->regs |= (1 << (16+1));               // add R1 output
        else
            pOutputs->regs |= (1 << (16+rw));              // temp register output
    }
}

void ReplaceInstWithMoveTemp(int inst, int i)
{
    struct _VtxPgm
    {
        WORD    type;
        WORD    size;
        DWORD   x,y,z,w;
    } VtxPgm;
    
    VtxPgm.type = 0;
    VtxPgm.size = 1;
    VtxPgm.x = 0;
    VtxPgm.y = 0x0020001b; // microcode for mov c-96, r0
    VtxPgm.z = 0x0436106c;
    VtxPgm.w = 0x2070f000;
    
    // Change source register
    VtxPgm.z |= (i & 0xf) << 28;

    // load it
    g_pDevice->LoadVertexShaderProgram((DWORD *)&VtxPgm, inst);
}

void GetConstant0(VTXREG *pC0)
{
    g_pDevice->BlockUntilIdle();   // kick off the pushbuffer and wait until the hw is idle
    D3DRDI_GetRamData(RDI_INDEX_VTX_CONSTANTS0, 0, pC0, sizeof(VTXREG));
}

void ReplaceInst(int inst, VTXREG *pInstruction)
{
    struct _VtxPgm
    {
        WORD    type;
        WORD    size;
        DWORD   x,y,z,w;
    } VtxPgm;
    
    VtxPgm.type = 0;
    VtxPgm.size = 1;
    VtxPgm.x = pInstruction->x;
    VtxPgm.y = pInstruction->y;
    VtxPgm.z = pInstruction->z;
    VtxPgm.w = pInstruction->w;
    
    // load it
    g_pDevice->LoadVertexShaderProgram((DWORD *)&VtxPgm, inst);
}

void SetConstant(int i, VTXREG *pConstant)
{
    CDevice* pDevice = g_pDevice;
    DWORD reverse[4];
    reverse[0] = pConstant->x;
    reverse[1] = pConstant->y;
    reverse[2] = pConstant->z;
    reverse[3] = pConstant->w;

    DWORD save = pDevice->m_ConstantMode;
    pDevice->m_ConstantMode = D3DSCM_192CONSTANTS;
    pDevice->SetVertexShaderConstant(i-96, &reverse, 1);
    pDevice->m_ConstantMode = save;
}

void WriteVTXREG(VTXREG *val, VTXREG *vals, DWORD *valcnt, BYTE *indexes, DWORD *indcnt)
{
    UINT i;
    for(i=0; i<*valcnt; i++)
    {
        if((vals[i].x == val->x) &&
           (vals[i].y == val->y) &&
           (vals[i].z == val->z) &&
           (vals[i].w == val->w))
            break;
    }
    if(i == *valcnt)
    {
        // add new output value
        if(*valcnt == 256)
        {
            i = 0;
        }
        else
        {
            i = *valcnt;
            vals[*valcnt] = *val;
            *valcnt = *valcnt + 1;
        }
    }
    indexes[*indcnt] = (BYTE)i;
    *indcnt = *indcnt + 1;
}

void DebugSnapshot::DumpVtxInstruction(int inst, 
                                       VTXREG *VtxProgram, 
                                       VTXREG *VtxConstant, 
                                       VTXREG *vals, 
                                       DWORD *valcnt, 
                                       BYTE *indexes, 
                                       DWORD *idxcnt)
{
    int i;
    VTXIO inputs, outputs;
    VTXREG C0;
    DWORD ARLType=0, ARLCount, ARLVal=0, ARLSwizzle;
    INT idx;

    if(VtxProgram[inst].w & 0x2)
        idx = a0x;
    else
        idx = 0;

    GetVertexInputsAndOutputs(&(VtxProgram[inst]), &inputs, &outputs);

    if(((VtxProgram[inst].y >> 21) & 0xf) == 0xd)  // is this an ARL instruction?
    {
        ARLSwizzle = ((VtxProgram[inst].y >> 6) & 0x3); // (axs)
        ARLType = ((VtxProgram[inst].z >> 26) & 0x3);  // (amx)
        switch(ARLType)
        {
        case 1: // temp
            ARLCount = ((VtxProgram[inst].z >> 28) & 0xf);  // (arr)
            break;
        case 2: // vab
            ARLCount = ((VtxProgram[inst].y >> 9) & 0xf);   // (va)
            break;
        case 3: // const
            ARLCount = ((VtxProgram[inst].y >> 13) & 0xff); // (ca)
            break;
        }
    }

    // Dump inputs
    // vab
    for(i=0; i<16; i++)
    {
        if(inputs.regs & (1<<i))
        {
            // Get the VAB
            g_pDevice->BlockUntilIdle();   // kick off the pushbuffer and wait until the hw is idle
            VTXREG VtxVAB[16];
            D3DRDI_GetRamData(RDI_INDEX_XL_VAB, 0, VtxVAB, sizeof(VtxVAB));
            WriteVTXREG(&(VtxVAB[i]), vals, valcnt, indexes, idxcnt);
            if((ARLType == 2) && (ARLCount == i))
            {
                switch(ARLSwizzle)
                {
                case 0: ARLVal = VtxVAB[i].x; break;
                case 1: ARLVal = VtxVAB[i].y; break;
                case 2: ARLVal = VtxVAB[i].z; break;
                case 3: ARLVal = VtxVAB[i].w; break;
                }
            }
        }
    }

    // constant
    if(inputs.cnst != 0xffffffff)
    {
        // refresh the constants
        g_pDevice->BlockUntilIdle();   // kick off the pushbuffer and wait until the hw is idle
        D3DRDI_GetRamData(RDI_INDEX_VTX_CONSTANTS0, 0, VtxConstant, 192*sizeof(VTXREG));
        WriteVTXREG(&(VtxConstant[inputs.cnst+idx]), vals, valcnt, indexes, idxcnt);
        if((ARLType == 3) && (ARLCount == inputs.cnst))
        {
            switch(ARLSwizzle)
            {
            case 0: ARLVal = VtxConstant[inputs.cnst+idx].x; break;
            case 1: ARLVal = VtxConstant[inputs.cnst+idx].y; break;
            case 2: ARLVal = VtxConstant[inputs.cnst+idx].z; break;
            case 3: ARLVal = VtxConstant[inputs.cnst+idx].w; break;
            }
        }
    }
    
    // temp
    for(i=0; i<16; i++)
    {
        if(inputs.regs & (1<<(i+16)))
        {
            ReplaceInstWithMoveTemp(inst, i);
            DrawVertices();
            GetConstant0(&C0);
            ReplaceInst(inst, &(VtxProgram[inst]));
            SetConstant(0, &(VtxConstant[0]));
            WriteVTXREG(&(C0), vals, valcnt, indexes, idxcnt);
            if((ARLType == 1) && (ARLCount == i))
            {
                switch(ARLSwizzle)
                {
                case 0: ARLVal = C0.x; break;
                case 1: ARLVal = C0.y; break;
                case 2: ARLVal = C0.z; break;
                case 3: ARLVal = C0.w; break;
                }
            }
        }
    }
    
    // Dump outputs
    // Output registers
    for(i=0; i<16; i++)
    {
        if(outputs.outs & (1<<i))
        {
            // modify instruction to write constant instead of output
            VTXREG newi;
            newi = VtxProgram[inst];
            newi.w &= 0xfffff007;   // output is now c0
            ReplaceInst(inst, &newi);
            DrawVertices();
            GetConstant0(&C0);
            ReplaceInst(inst, &(VtxProgram[inst]));
            SetConstant(0, &(VtxConstant[0]));
            // get write mask for output register
            if((VtxProgram[inst].w >> 12) & 0x1) MaskOutputs[i].w = C0.w;
            if((VtxProgram[inst].w >> 13) & 0x1) MaskOutputs[i].z = C0.z;
            if((VtxProgram[inst].w >> 14) & 0x1) MaskOutputs[i].y = C0.y;
            if((VtxProgram[inst].w >> 15) & 0x1) MaskOutputs[i].x = C0.x;
            WriteVTXREG(&(MaskOutputs[i]), vals, valcnt, indexes, idxcnt);
        }
    }

    // constant
    if(outputs.cnst != 0xffffffff)
    {
        // modify instruction to end program
        VTXREG newi;
        newi = VtxProgram[inst];
        newi.w &= 0xfffff007;   // output is now c0
        ReplaceInst(inst, &newi);
        DrawVertices();
        GetConstant0(&C0);
        ReplaceInst(inst, &(VtxProgram[inst]));
        SetConstant(0, &(VtxConstant[0]));
        WriteVTXREG(&(C0), vals, valcnt, indexes, idxcnt);
    }

    // temp
    for(i=0; i<16; i++)
    {
        // can't get temp reg output for last program slot
        if((outputs.regs & (1<<(i+16))) && (inst < 135))
        {
            VTXREG newi;
            newi = VtxProgram[inst];
            newi.w &= 0xfffffffe;   // clear eos if set
            ReplaceInst(inst, &newi);
            // set next instruction to move temp reg to c0
            ReplaceInstWithMoveTemp(inst+1, i);
            DrawVertices();
            GetConstant0(&C0);
            // restore instructions
            ReplaceInst(inst, &(VtxProgram[inst]));
            ReplaceInst(inst+1, &(VtxProgram[inst+1]));
            SetConstant(0, &(VtxConstant[0]));
            WriteVTXREG(&(C0), vals, valcnt, indexes, idxcnt);
        }
    }
    
    // Update a0.x if necessary
    if(ARLType != 0)
    {
        a0x = (INT)FloatToLong(*((FLOAT *)(&(ARLVal))));
        if((VtxProgram[inst].y >> 8) & 0x1) // (ane)
            a0x = -a0x;
    }
}

HRESULT
DebugSnapshot::HandleVSDrawVertices()
{
    UINT i;
    CDevice* pDevice = g_pDevice;
    
    if (pDevice->m_pVertexShader->Flags & (VERTEXSHADER_PROGRAM))
    {
        ShaderVerts += VertexCount;
    }
    else
    {
        OtherVerts += VertexCount;
        return S_OK;    // no need to draw these
    }

    // *********** Set up for this vertex buffer
    
    // set the vertex shader to write mode
    PPUSH pPush = pDevice->StartPush();

    Push2(pPush, 
          
          NV097_SET_TRANSFORM_EXECUTION_MODE,

          // NV097_SET_TRANSFORM_EXECUTION_MODE:

          (DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |
           DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV)),

          // NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN:

          0xffffffff & VERTEXSHADER_WRITE);
    
    pDevice->EndPush(pPush+3);
    
    
    g_pDevice->BlockUntilIdle();   // kick off the pushbuffer and wait until the hw is idle
    
    // Get the Constants
    VTXREG VtxConstant[192];
    D3DRDI_GetRamData(RDI_INDEX_VTX_CONSTANTS0, 0, VtxConstant, sizeof(VtxConstant));

    // Get the Vertex Program
    VTXREG VtxProgram[136];
    D3DRDI_GetRamData(RDI_INDEX_XL_PROGRAM, 0, VtxProgram, sizeof(VtxProgram));

    // Dump program instructions to buffer
    UINT instcnt = 0;
    for(i=pDevice->m_VertexShaderStart; ; i++)
    {
        instcnt++;
        if(VtxProgram[i].w & 0x1)
            break;
    }
    
    if(dwCaptureCount + instcnt*3+1 >= SHADER_CAPTURE_BUFFER_SIZE/4)
    {
        // Buffer won't hold program, write EOF instead
        WriteCapture(VSS_EOF);
        dwLastVertex = 0;       // finished capturing for this frame
    }
    else
    {
        WriteCapture(VSS_PROGRAM | instcnt*3+1);
        for(i=pDevice->m_VertexShaderStart; ; i++)
        {
            WriteCapture(VtxProgram[i].y);
            WriteCapture(VtxProgram[i].z);
            WriteCapture(VtxProgram[i].w);
            if(VtxProgram[i].w & 0x1)
                break;
        }
    }


    VTXREG vals[256];
    DWORD valcnt;
    BYTE indexes[500];
    DWORD idxcnt;

    // Step through vertices in vertex buffer
    DWORD dwTotalVertices = VertexCount;
    DWORD vert;

    for(vert=1; vert <= dwTotalVertices; vert++)
    {
        if((dwProcessedVertexCount < dwFirstVertex) ||
           (dwProcessedVertexCount > dwLastVertex))
        {
            dwProcessedVertexCount++;
            continue;
        }
        
        VertexCount = vert; // VertexCount determines vertices processed in DrawVertices
        
        //******** Set up to run this vertex through the shader
        // initialize output registers and address offset register
        for(i=0; i<16; i++)
        {
            MaskOutputs[i].x = 0;
            MaskOutputs[i].y = 0;
            MaskOutputs[i].z = 0;
            MaskOutputs[i].w = 0;
        }
        a0x = 0;    // initialize address offset register

        // Get V0
        DrawVertices();
        g_pDevice->BlockUntilIdle();   // kick off the pushbuffer and wait until the hw is idle
        VTXREG VtxVAB[16];
        D3DRDI_GetRamData(RDI_INDEX_XL_VAB, 0, VtxVAB, sizeof(VtxVAB));

        valcnt = idxcnt = 0;
        vals[valcnt++] = VtxVAB[0]; // make sure V0 is in the buffer first

        // Step through the current program
        for(i=pDevice->m_VertexShaderStart; ; i++)
        {
            DumpVtxInstruction(i, VtxProgram, VtxConstant, vals, &valcnt, indexes, &idxcnt);
            
            // Check for EOS
            if(VtxProgram[i].w & 0x1)
                break;
        }
        
        // Finished running shader, write out information for this vertex
        DWORD dwVertexSize = 3 + valcnt*4 + (idxcnt-1)/4 + 1;
        if(dwCaptureCount + dwVertexSize >= SHADER_CAPTURE_BUFFER_SIZE/4)
        {
            // Buffer won't hold vertex, write EOF instead
            WriteCapture(VSS_EOF);
            dwLastVertex = 0;       // finished capturing for this frame
        }
        else
        {
            WriteCapture(VSS_VERTEX | dwVertexSize);
            WriteCapture(dwProcessedVertexCount++);
            WriteCapture(((valcnt & 0xffff) << 16) | ((idxcnt-1)/4 + 1));
            for(i=0; i<valcnt; i++)
            {
                WriteCapture(vals[i].x);
                WriteCapture(vals[i].y);
                WriteCapture(vals[i].z);
                WriteCapture(vals[i].w);
            }
            DWORD *dwptr = (DWORD *)indexes;
            for(i=0; i < ((idxcnt-1)/4 + 1); i++)
                WriteCapture(*dwptr++);
        }
    }
    // restore original VertexCount value
    VertexCount = dwTotalVertices;

    // *********** Finished with this vertex buffer
    
    // reset write mode to original
    pPush = pDevice->StartPush();

    Push2(pPush, 
          
          NV097_SET_TRANSFORM_EXECUTION_MODE,

          // NV097_SET_TRANSFORM_EXECUTION_MODE:

          (DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |
           DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, _PRIV)),

          // NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN:

          pDevice->m_pVertexShader->Flags & VERTEXSHADER_WRITE);
    
    pDevice->EndPush(pPush+3);
    return S_OK;
}

HRESULT 
DebugSnapshot::HandlePSDrawVertices()
{
    UINT i;
    CDevice* pDevice = g_pDevice;
    
    // Save user state at time of DrawVertices call
    SaveUserState();

    // get color and z value before capture
    GetColorAndDepth(&(pscpt.dwPreColor), &(pscpt.dwPreDepth));
    
    // Check if this DrawVertices call affects the sample pixel
    SetPassThroughState();
    DrawVertices();

    DWORD dwTestColor, dwTestDepth;
    GetColorAndDepth(&dwTestColor, &dwTestDepth);

    if((dwTestColor == pscpt.dwPreColor) &&
       (dwTestDepth == pscpt.dwPreDepth))
    {
        // bail out, these primitives don't affect the pixel
        RestoreUserState(pDevice->m_pPixelShader != NULL);
        DrawVertices();
        return S_OK;
    }

    // find out if a pixel shader is active
    if(!pDevice->m_pPixelShader)
    {
        // save primitive information but don't capture registers
        RestoreUserState(pDevice->m_pPixelShader != NULL);
        
        // restore color and depth
        SetColorAndDepth(pscpt.dwPreColor, pscpt.dwPreDepth);
        
        DrawVertices();
        GetColorAndDepth(&(pscpt.dwPostColor), &(pscpt.dwPostDepth));
        
        WriteCapture(PSS_PRIMITIVE | 5);
        WriteCapture(pscpt.dwPreColor);
        WriteCapture(pscpt.dwPreDepth);
        WriteCapture(pscpt.dwPostColor);
        WriteCapture(pscpt.dwPostDepth);
        WriteCapture(PSS_NOSHADER | 1);
        
        return S_OK;
    }

    // find out how many combiner stages are active
    pscpt.dwCombinerCount = D3D__RenderState[D3DRS_PSCOMBINERCOUNT];
    pscpt.dwCombinerCount &= 0xf;

    for(i=0; i<pscpt.dwCombinerCount; i++)    // get texture registers and all stage registers
        GetStageData(i);

    GetStageData(8);    // get final combiner

    // restore color and depth
    SetColorAndDepth(pscpt.dwPreColor, pscpt.dwPreDepth);

    // draw the primitives using the user's state
    RestoreUserState(pDevice->m_pPixelShader != NULL);
    DrawVertices();
    
    // get color and z value after capture
    GetColorAndDepth(&(pscpt.dwPostColor), &(pscpt.dwPostDepth));
    
    WriteCapture(PSS_PRIMITIVE | 5);
    WriteCapture(pscpt.dwPreColor);
    WriteCapture(pscpt.dwPreDepth);
    WriteCapture(pscpt.dwPostColor);
    WriteCapture(pscpt.dwPostDepth);

    for(i=0; i<=pscpt.dwCombinerCount; i++)    // write captured stages to capture buffer
    {
        if(i == pscpt.dwCombinerCount) i = 8;   // final combiner
        switch (i)
        {
        case 0: // combiner stage 0
            WriteCapture(PSS_STAGE0 | 33);
            WriteCapture(D3D__RenderState[D3DRS_PSRGBINPUTS0+i]);
            WriteCapture(D3D__RenderState[D3DRS_PSALPHAINPUTS0+i]);
            WriteCapture(D3D__RenderState[D3DRS_PSRGBOUTPUTS0+i]);
            WriteCapture(D3D__RenderState[D3DRS_PSALPHAOUTPUTS0+i]);
            break;
        case 8: // final combiner stage
            WriteCapture(PSS_FINALSTAGE | 29);
            WriteCapture(D3D__RenderState[D3DRS_PSFINALCOMBINERINPUTSABCD]);
            WriteCapture(D3D__RenderState[D3DRS_PSFINALCOMBINERINPUTSEFG]);
            break;
        default:    // intermediate combiner stages
            WriteCapture(PSS_STAGE | 25);
            WriteCapture(D3D__RenderState[D3DRS_PSRGBINPUTS0+i]);
            WriteCapture(D3D__RenderState[D3DRS_PSALPHAINPUTS0+i]);
            WriteCapture(D3D__RenderState[D3DRS_PSRGBOUTPUTS0+i]);
            WriteCapture(D3D__RenderState[D3DRS_PSALPHAOUTPUTS0+i]);
        }
        WriteCapture(((DWORD)pscpt.r0[i].a << 16) | (pscpt.r0[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.r0[i].g << 16) | (pscpt.r0[i].b & 0xffff));
        WriteCapture(((DWORD)pscpt.r1[i].a << 16) | (pscpt.r1[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.r1[i].g << 16) | (pscpt.r1[i].b & 0xffff));
        WriteCapture(((DWORD)pscpt.c0[i].a << 16) | (pscpt.c0[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.c0[i].g << 16) | (pscpt.c0[i].b & 0xffff));
        WriteCapture(((DWORD)pscpt.c1[i].a << 16) | (pscpt.c1[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.c1[i].g << 16) | (pscpt.c1[i].b & 0xffff));
        WriteCapture(((DWORD)pscpt.v0[i].a << 16) | (pscpt.v0[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.v0[i].g << 16) | (pscpt.v0[i].b & 0xffff));
        WriteCapture(((DWORD)pscpt.v1[i].a << 16) | (pscpt.v1[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.v1[i].g << 16) | (pscpt.v1[i].b & 0xffff));
        WriteCapture(((DWORD)pscpt.t0[i].a << 16) | (pscpt.t0[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.t0[i].g << 16) | (pscpt.t0[i].b & 0xffff));
        WriteCapture(((DWORD)pscpt.t1[i].a << 16) | (pscpt.t1[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.t1[i].g << 16) | (pscpt.t1[i].b & 0xffff));
        WriteCapture(((DWORD)pscpt.t2[i].a << 16) | (pscpt.t2[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.t2[i].g << 16) | (pscpt.t2[i].b & 0xffff));
        WriteCapture(((DWORD)pscpt.t3[i].a << 16) | (pscpt.t3[i].r & 0xffff));
        WriteCapture(((DWORD)pscpt.t3[i].g << 16) | (pscpt.t3[i].b & 0xffff));
        if(i == 0)
        {
            WriteCapture(((DWORD)pscpt.t0in.a << 16) | (pscpt.t0in.r & 0xffff));
            WriteCapture(((DWORD)pscpt.t0in.g << 16) | (pscpt.t0in.b & 0xffff));
            WriteCapture(((DWORD)pscpt.t1in.a << 16) | (pscpt.t1in.r & 0xffff));
            WriteCapture(((DWORD)pscpt.t1in.g << 16) | (pscpt.t1in.b & 0xffff));
            WriteCapture(((DWORD)pscpt.t2in.a << 16) | (pscpt.t2in.r & 0xffff));
            WriteCapture(((DWORD)pscpt.t2in.g << 16) | (pscpt.t2in.b & 0xffff));
            WriteCapture(((DWORD)pscpt.t3in.a << 16) | (pscpt.t3in.r & 0xffff));
            WriteCapture(((DWORD)pscpt.t3in.g << 16) | (pscpt.t3in.b & 0xffff));
        }
        else if (i == 8)
        {
            WriteCapture(((DWORD)pscpt.fog.a << 16) | (pscpt.fog.r & 0xffff));
            WriteCapture(((DWORD)pscpt.fog.g << 16) | (pscpt.fog.b & 0xffff));
            WriteCapture(((DWORD)pscpt.sum.a << 16) | (pscpt.sum.r & 0xffff));
            WriteCapture(((DWORD)pscpt.sum.g << 16) | (pscpt.sum.b & 0xffff));
            WriteCapture(((DWORD)pscpt.prod.a << 16) | (pscpt.prod.r & 0xffff));
            WriteCapture(((DWORD)pscpt.prod.g << 16) | (pscpt.prod.b & 0xffff));
        }
    }
    return S_OK;
}

HRESULT 
DebugSnapshot::HandleXRDrawVertices()
{
    CDevice* pDevice = g_pDevice;

    // save user state
    DWORD savePSHandle;
    pDevice->GetPixelShader(&savePSHandle);
    DWORD saveZEnable = D3D__RenderState[D3DRS_ZENABLE];
    DWORD saveSrcBlend = D3D__RenderState[D3DRS_SRCBLEND];
    DWORD saveDestBlend = D3D__RenderState[D3DRS_DESTBLEND];
    DWORD saveAlphaBlend = D3D__RenderState[D3DRS_ALPHABLENDENABLE];
    DWORD saveFillMode = D3D__RenderState[D3DRS_FILLMODE];
    DWORD saveStencilEnable = D3D__RenderState[D3DRS_STENCILENABLE];
    DWORD saveCullMode = D3D__RenderState[D3DRS_CULLMODE];
    D3DPIXELSHADERDEF *saveUserPSDef = pDevice->m_UserPixelShader.pPSDef;

    D3DPIXELSHADERDEF psd;
    if(dwXRFlags & GRAYMASK)
    {
        ZeroMemory(&psd, sizeof(psd));
        psd.PSCombinerCount=PS_COMBINERCOUNT(
            1,
            PS_COMBINERCOUNT_MUX_MSB | PS_COMBINERCOUNT_UNIQUE_C0 | PS_COMBINERCOUNT_UNIQUE_C1);
        psd.PSTextureModes=PS_TEXTUREMODES(
            PS_TEXTUREMODES_NONE,
            PS_TEXTUREMODES_NONE,
            PS_TEXTUREMODES_NONE,
            PS_TEXTUREMODES_NONE);
        psd.PSInputTexture=PS_INPUTTEXTURE(0,0,0,0);
        psd.PSDotMapping=PS_DOTMAPPING(
            0,
            PS_DOTMAPPING_ZERO_TO_ONE,
            PS_DOTMAPPING_ZERO_TO_ONE,
            PS_DOTMAPPING_ZERO_TO_ONE);
        psd.PSCompareMode=PS_COMPAREMODE(
            PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
            PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
            PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT,
            PS_COMPAREMODE_S_LT | PS_COMPAREMODE_T_LT | PS_COMPAREMODE_R_LT | PS_COMPAREMODE_Q_LT);
        
        //------------- Stage 0 -------------
        psd.PSRGBInputs[0]=PS_COMBINERINPUTS(
            PS_REGISTER_C0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_SIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
        psd.PSAlphaInputs[0]=PS_COMBINERINPUTS(
            PS_REGISTER_C0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_SIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT,
            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
        psd.PSRGBOutputs[0]=PS_COMBINEROUTPUTS(
            PS_REGISTER_R0,
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
        psd.PSAlphaOutputs[0]=PS_COMBINEROUTPUTS(
            PS_REGISTER_R0,
            PS_REGISTER_DISCARD,
            PS_REGISTER_DISCARD,
            PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
        
        //------------- FinalCombiner -------------
        psd.PSFinalCombinerInputsABCD=PS_COMBINERINPUTS(
            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_R0 | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
        psd.PSFinalCombinerInputsEFG=PS_COMBINERINPUTS(
            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_REGISTER_R0 | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
            PS_FINALCOMBINERSETTING_CLAMP_SUM | 0 | 0);
    
        pDevice->SetPixelShaderProgram(&psd);
        pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    }
    
    // read flags and set user-selected state
    // fillmode
    switch((dwXRFlags & FILLMODEMASK) >> FILLMODESHIFT)
    {
    case 1: pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT); break;
    case 2: pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); break;
    case 3: pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID); break;
    }
    // cullmode
    switch((dwXRFlags & CULLMODEMASK) >> CULLMODESHIFT)
    {
    case 1: pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); break;
    case 2: pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); break;
    case 3: pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); break;
    }
    // Z Enable
    switch((dwXRFlags & ZENABLEMASK) >> ZENABLESHIFT)
    {
    case 1: pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE); break;
    case 2: pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE); break;
    case 3: pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW); break;
    }
    // Stencil Enable
    switch((dwXRFlags & STENCILENABLEMASK) >> STENCILENABLESHIFT)
    {
    case 1: pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE); break;
    case 2: pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE); break;
    }
    
    DWORD inc = (dwXRFlags & INCREMENTMASK) >> INCREMENTSHIFT;
    inc |= inc << 8;
    inc |= inc << 16;
    pDevice->SetRenderState(D3DRS_PSCONSTANT0_0, inc);


    // Xray state is set, draw the geometry
    DrawVertices();

    // Now restore the user state
    pDevice->m_UserPixelShader.pPSDef = saveUserPSDef;
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, saveAlphaBlend);
    pDevice->SetRenderState(D3DRS_DESTBLEND, saveDestBlend);
    pDevice->SetRenderState(D3DRS_SRCBLEND, saveSrcBlend);
    pDevice->SetRenderState(D3DRS_ZENABLE, saveZEnable);
    pDevice->SetRenderState(D3DRS_FILLMODE, saveFillMode);
    pDevice->SetRenderState(D3DRS_STENCILENABLE, saveStencilEnable);
    pDevice->SetRenderState(D3DRS_CULLMODE, saveCullMode);
    
    if(dwXRFlags & GRAYMASK)
        pDevice->SetPixelShader(savePSHandle);

    return S_OK;
}
    
HRESULT 
DebugSnapshot::HandleDrawVertices()
{
    CDevice* pDevice = g_pDevice;
    
    // if more than one operation is enabled at a time, the priority is:
    // 1. markers
    // 2. vertex shader debugger
    // 3. pixel shader debugger
    // 4. xray

    // if debug markers are enabled, filter out geometry with different marker.
    if((dwFlags & 0x1) &&
       (pDevice->m_dwDebugMarker != dwMarker))
    {
        return S_OK;
    }

    if(pDevice->m_dwSnapshot & SDOP_VSACTIVE)
    {
        // vertex shader debugger is active
        return HandleVSDrawVertices();
    }

    if(pDevice->m_dwSnapshot & SDOP_PSACTIVE)
    {
        // pixel shader debugger is active
        return HandlePSDrawVertices();
    }
    
    if(pDevice->m_dwSnapshot & SDOP_XRACTIVE)
    {
        // xray utility is active
        return HandleXRDrawVertices();
    }

    return S_OK;
}

void 
DebugSnapshot::GetStageData(UINT i)
{
    DWORD dwCombinerCount;
    CDevice* pDevice = g_pDevice;
    
    RestoreUserState(pDevice->m_pPixelShader != NULL);
    SetPassThroughState();

    if(i != 8)
    {
        // not final combiner, set combiner count
        dwCombinerCount = D3D__RenderState[D3DRS_PSCOMBINERCOUNT];
        dwCombinerCount &= 0xfffffff0;
        dwCombinerCount |= i+1;
        pDevice->SetRenderState(D3DRS_PSCOMBINERCOUNT, dwCombinerCount);
    }
    else
    {
        // final combiner
        GetRegister(PS_REGISTER_FOG, i);
        GetRegister(PS_REGISTER_V1R0_SUM, i);
        GetRegister(PS_REGISTER_EF_PROD, i);
    }
    GetRegister(PS_REGISTER_V0, i);
    GetRegister(PS_REGISTER_V1, i);
    GetRegister(PS_REGISTER_T0, i);
    GetRegister(PS_REGISTER_T1, i);
    GetRegister(PS_REGISTER_T2, i);
    GetRegister(PS_REGISTER_T3, i);
    GetRegister(PS_REGISTER_R0, i);
    GetRegister(PS_REGISTER_R1, i);
    GetRegister(PS_REGISTER_C0, i);
    GetRegister(PS_REGISTER_C1, i);
    if(i == 0)
    {
        // get texture addresses
        GetRegister(SS_REGISTER_T0IN, i);
        GetRegister(SS_REGISTER_T1IN, i);
        GetRegister(SS_REGISTER_T2IN, i);
        GetRegister(SS_REGISTER_T3IN, i);
    }
}

void 
DebugSnapshot::GetRegister(DWORD dwReg, UINT i)
{
    DWORD dwRGBIn, dwRGBOut, dwAlphaIn, dwAlphaOut;
    DWORD dwFCabcd, dwFCefg, dwFCefgSave;
    DWORD dwColor, dwDepth;
    SHORT d;
    CDevice* pDevice = g_pDevice;
    DWORD dwChannel, dwIter, dwStart, dwEnd;
    DWORD dwSampleReg;

    if(pDevice->m_dwSnapshot & SDOP_HASALPHA)
    {
        // framebuffer format has alpha
        dwStart = dwEnd = 3;    // one iteration, save both alpha and rgb
    }
    else
    {
        dwStart = 1;    // first iteration, save rgb
        dwEnd = 2;      // second iteration, save alpha
    }
    
    for(dwIter=dwStart; dwIter<=dwEnd; dwIter++)
    {
        if(dwIter == 2)
        {
            // don't request alpha from these registers
            if(dwReg == PS_REGISTER_V1R0_SUM)
            {
                pscpt.sum.a = 0;
                continue;
            }
            if(dwReg == PS_REGISTER_EF_PROD)
            {
                pscpt.prod.a = 0;
                continue;
            }
            dwChannel = PS_CHANNEL_ALPHA;
        }
        else
            dwChannel = PS_CHANNEL_RGB;

        if(i != 8)
        {
            // setup for texture inputs if requested
            if(dwReg >= SS_REGISTER_T0IN)
            {
                pDevice->SetRenderState(D3DRS_PSTEXTUREMODES, PS_TEXTUREMODES(PS_TEXTUREMODES_PASSTHRU, 
                                                                              PS_TEXTUREMODES_PASSTHRU, 
                                                                              PS_TEXTUREMODES_PASSTHRU, 
                                                                              PS_TEXTUREMODES_PASSTHRU));
                dwSampleReg = dwReg - SS_REGISTER_T0IN + PS_REGISTER_T0;
            }
            else
                dwSampleReg = dwReg;

            // not final combiner
            dwRGBIn = PS_COMBINERINPUTS(
                dwSampleReg      | dwChannel      | PS_INPUTMAPPING_SIGNED_IDENTITY,
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
        
            dwAlphaIn = PS_COMBINERINPUTS(
                dwSampleReg      | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_SIGNED_IDENTITY,
                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
        
            dwRGBOut = PS_COMBINEROUTPUTS(
                PS_REGISTER_R0,
                PS_REGISTER_DISCARD,
                PS_REGISTER_DISCARD,
                PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | 
                PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
        
            dwAlphaOut = PS_COMBINEROUTPUTS(
                PS_REGISTER_R0,
                PS_REGISTER_DISCARD,
                PS_REGISTER_DISCARD,
                PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | 
                PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
        
            pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_PSRGBINPUTS0+i), dwRGBIn);
            pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_PSALPHAINPUTS0+i), dwAlphaIn);
            pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_PSRGBOUTPUTS0+i), dwRGBOut);
            pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_PSALPHAOUTPUTS0+i), dwAlphaOut);

        }
        else
        {
            // final combiner
            dwFCabcd = PS_COMBINERINPUTS( 
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                dwReg            | dwChannel      | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                dwReg            | dwChannel      | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY);
        
            DWORD dwAlphaReg;
            if((dwReg == PS_REGISTER_FOG) || (dwReg == PS_REGISTER_V1R0_SUM) || (dwReg == PS_REGISTER_EF_PROD))
                dwAlphaReg = PS_REGISTER_ZERO;
            else
                dwAlphaReg = dwReg;
            dwFCefg = PS_COMBINERINPUTS(
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                dwAlphaReg       | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_IDENTITY,
                PS_FINALCOMBINERSETTING_CLAMP_SUM | 0 | 0);
            pDevice->SetRenderState(D3DRS_PSFINALCOMBINERINPUTSABCD, dwFCabcd);
            dwFCefgSave = D3D__RenderState[D3DRS_PSFINALCOMBINERINPUTSEFG];
            dwFCefg &= 0x0000ffff;  // clear new E and F bits
            dwFCefgSave &= 0xffff0000;  // remove G and flags
            dwFCefg |= dwFCefgSave;     // combine old E and F with new G and flags
            pDevice->SetRenderState(D3DRS_PSFINALCOMBINERINPUTSEFG, dwFCefg);
        }
    
        // draw the primitives
        DrawVertices();
    
        // get the result of the rendering
        GetColorAndDepth(&dwColor, &dwDepth);
        
        REGVAL *pRv = NULL;
        switch(dwReg)
        {
        case PS_REGISTER_V0: pRv = &pscpt.v0[i]; break;
        case PS_REGISTER_V1: pRv = &pscpt.v1[i]; break;
        case PS_REGISTER_T0: pRv = &pscpt.t0[i]; break;
        case PS_REGISTER_T1: pRv = &pscpt.t1[i]; break;
        case PS_REGISTER_T2: pRv = &pscpt.t2[i]; break;
        case PS_REGISTER_T3: pRv = &pscpt.t3[i]; break;
        case PS_REGISTER_R0: pRv = &pscpt.r0[i]; break;
        case PS_REGISTER_R1: pRv = &pscpt.r1[i]; break;
        case PS_REGISTER_C0: pRv = &pscpt.c0[i]; break;
        case PS_REGISTER_C1: pRv = &pscpt.c1[i]; break;
        case PS_REGISTER_FOG: pRv = &pscpt.fog; break;
        case PS_REGISTER_V1R0_SUM: pRv = &pscpt.sum; break;
        case PS_REGISTER_EF_PROD: pRv = &pscpt.prod; break;
        case SS_REGISTER_T0IN:    pRv = &pscpt.t0in; break;
        case SS_REGISTER_T1IN:    pRv = &pscpt.t1in; break;
        case SS_REGISTER_T2IN:    pRv = &pscpt.t2in; break;
        case SS_REGISTER_T3IN:    pRv = &pscpt.t3in; break;
        }
        if(pRv)
        {
            switch(dwIter)
            {
            case 1:
                pRv->r = (short)(dwColor >> 16) & 0xff;
                pRv->g = (short)(dwColor >>  8) & 0xff;
                pRv->b = (short)(dwColor >>  0) & 0xff;
                break;
            case 2:
                pRv->a = (short)(dwColor >>  0) & 0xff;
                break;
            case 3:
                pRv->a = (short)(dwColor >> 24) & 0xff;
                pRv->r = (short)(dwColor >> 16) & 0xff;
                pRv->g = (short)(dwColor >>  8) & 0xff;
                pRv->b = (short)(dwColor >>  0) & 0xff;
                break;
            }
        }
    
        if(i != 8)
        {
            // not final combiner
            // setup for texture inputs if requested
            if(dwReg >= SS_REGISTER_T0IN)
                dwSampleReg = dwReg - SS_REGISTER_T0IN + PS_REGISTER_T0;
            else
                dwSampleReg = dwReg;

            // set up the combiner stage to add 0xff to the register
            dwRGBIn = PS_COMBINERINPUTS(
                dwSampleReg      | dwChannel      | PS_INPUTMAPPING_SIGNED_IDENTITY,
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_REGISTER_ZERO | PS_CHANNEL_RGB | PS_INPUTMAPPING_UNSIGNED_INVERT);
        
            dwAlphaIn = PS_COMBINERINPUTS(
                dwSampleReg      | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_SIGNED_IDENTITY,
                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT,
                PS_REGISTER_ZERO | PS_CHANNEL_ALPHA | PS_INPUTMAPPING_UNSIGNED_INVERT);
        
            dwRGBOut = PS_COMBINEROUTPUTS(
                PS_REGISTER_DISCARD,
                PS_REGISTER_DISCARD,
                PS_REGISTER_R0,
                PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | 
                PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
        
            dwAlphaOut = PS_COMBINEROUTPUTS(
                PS_REGISTER_DISCARD,
                PS_REGISTER_DISCARD,
                PS_REGISTER_R0,
                PS_COMBINEROUTPUT_IDENTITY | PS_COMBINEROUTPUT_AB_MULTIPLY | 
                PS_COMBINEROUTPUT_CD_MULTIPLY | PS_COMBINEROUTPUT_AB_CD_SUM);
        
            pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_PSRGBINPUTS0+i), dwRGBIn);
            pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_PSALPHAINPUTS0+i), dwAlphaIn);
            pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_PSRGBOUTPUTS0+i), dwRGBOut);
            pDevice->SetRenderState((D3DRENDERSTATETYPE)((DWORD)D3DRS_PSALPHAOUTPUTS0+i), dwAlphaOut);
    
            // draw the primitives again to get the sign bits
            DrawVertices();
        
            // get the result of the rendering
            GetColorAndDepth(&dwColor, &dwDepth);
        
            pRv = NULL;
            switch(dwReg)
            {
            case PS_REGISTER_V0: pRv = &pscpt.v0[i]; break;
            case PS_REGISTER_V1: pRv = &pscpt.v1[i]; break;
            case PS_REGISTER_T0: pRv = &pscpt.t0[i]; break;
            case PS_REGISTER_T1: pRv = &pscpt.t1[i]; break;
            case PS_REGISTER_T2: pRv = &pscpt.t2[i]; break;
            case PS_REGISTER_T3: pRv = &pscpt.t3[i]; break;
            case PS_REGISTER_R0: pRv = &pscpt.r0[i]; break;
            case PS_REGISTER_R1: pRv = &pscpt.r1[i]; break;
            case PS_REGISTER_C0: pRv = &pscpt.c0[i]; break;
            case PS_REGISTER_C1: pRv = &pscpt.c1[i]; break;
            case SS_REGISTER_T0IN: pRv = &pscpt.t0in; break;
            case SS_REGISTER_T1IN: pRv = &pscpt.t0in; break;
            case SS_REGISTER_T2IN: pRv = &pscpt.t0in; break;
            case SS_REGISTER_T3IN: pRv = &pscpt.t0in; break;
            }
            if(pRv)
            {
                switch(dwIter)
                {
                case 1:
                    d = (short)(dwColor >> 16) & 0xff;
                    if(d != 0xff) pRv->r = d - 0xff;
                    d = (short)(dwColor >>  8) & 0xff;
                    if(d != 0xff) pRv->g = d - 0xff;
                    d = (short)(dwColor >>  0) & 0xff;
                    if(d != 0xff) pRv->b = d - 0xff;
                    break;
                case 2:
                    d = (short)(dwColor >>  0) & 0xff;
                    if(d != 0xff) pRv->a = d - 0xff;
                    break;
                case 3:
                    d = (short)(dwColor >> 24) & 0xff;
                    if(d != 0xff) pRv->a = d - 0xff;
                    d = (short)(dwColor >> 16) & 0xff;
                    if(d != 0xff) pRv->r = d - 0xff;
                    d = (short)(dwColor >>  8) & 0xff;
                    if(d != 0xff) pRv->g = d - 0xff;
                    d = (short)(dwColor >>  0) & 0xff;
                    if(d != 0xff) pRv->b = d - 0xff;
                    break;
                }
            }
        }
    }
}

void HandleShaderSnapshotOpcode()
{
#if PROFILE
    CDevice* pDevice = g_pDevice;
    DWORD *args;
    
    switch(pDevice->m_dwOpcode & 0xff000000)
    {
    // Pixel shader capture opcodes
    case PSSNAP_IDLE:           // nothing to do (also VSSNAP_IDLE)
    case PSSNAP_ACK:            // waiting for DM response
    case PSSNAP_ERROR:          // waiting for DM response
        break;
    case PSSNAP_REQUEST:         // start pixel shader snapshot
        // check for pure device
        if (pDevice->m_StateFlags & STATE_PUREDEVICE)
        {
            pDevice->m_dwOpcode = PSSNAP_ERROR | 2; // signal pure device error
            return;
        }

        // check for supported framebuffer format
        IDirect3DSurface8 *pBackBuffer;
        D3DSURFACE_DESC sDesc;
        pDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &(pBackBuffer));
        pBackBuffer->GetDesc(&sDesc);
        pBackBuffer->Release();
        if((sDesc.Format != D3DFMT_LIN_A8R8G8B8) &&
           (sDesc.Format != D3DFMT_LIN_X8R8G8B8))
        {
            pDevice->m_dwOpcode = PSSNAP_ERROR | 1; // signal format error
            return;
        }

        args = (DWORD *)(pDevice->m_pShaderCaptureBuffer);   // save pointer to arguments

        // initialize capture buffer
        pDevice->m_pShaderCaptureBuffer = (DWORD *)MemAllocNoZero(SHADER_CAPTURE_BUFFER_SIZE);
        pDevice->m_pShaderCapturePtr = pDevice->m_pShaderCaptureBuffer; // set write pointer
        if(pDevice->m_pShaderCaptureBuffer == NULL)
        {
            pDevice->m_dwOpcode = PSSNAP_ERROR | 3; // signal memory error
            return;
        }

        // initialize capture state
        g_snapshot.BeginPSSnapshot(args);

        pDevice->m_dwOpcode = PSSNAP_BUSY;
        pDevice->m_dwSnapshot |= SDOP_PSACTIVE; // pixel shader capture active
        if(sDesc.Format == D3DFMT_LIN_A8R8G8B8)
            pDevice->m_dwSnapshot |= SDOP_HASALPHA;
        break;
    case PSSNAP_BUSY:            // finished with pixel shader snapshot
        pDevice->m_dwSnapshot &= ~(SDOP_PSACTIVE | SDOP_HASALPHA);
        g_snapshot.EndPSSnapshot();
        pDevice->m_dwOpcode = PSSNAP_ACK | ((DWORD)(pDevice->m_pShaderCapturePtr) - (DWORD)(pDevice->m_pShaderCaptureBuffer));
        break;
    case PSSNAP_DONE:            // DM has acknowledged receiving data, cleanup
        MemFree(pDevice->m_pShaderCaptureBuffer);
        pDevice->m_pShaderCaptureBuffer = pDevice->m_pShaderCapturePtr = NULL;
        pDevice->m_dwOpcode = PSSNAP_IDLE;
        break;
    
    // Vertex shader capture opcodes
    case VSSNAP_ACK:            // waiting for DM response
    case VSSNAP_ERROR:          // waiting for DM response
        break;
    case VSSNAP_REQUEST:         // start vertex shader snapshot
        // check for pure device
        if (pDevice->m_StateFlags & STATE_PUREDEVICE)
        {
            pDevice->m_dwOpcode = VSSNAP_ERROR | 2; // signal pure device error
            return;
        }

        args = (DWORD *)(pDevice->m_pShaderCaptureBuffer);   // save pointer to arguments

        // initialize capture buffer
        pDevice->m_pShaderCaptureBuffer = (DWORD *)MemAllocNoZero(SHADER_CAPTURE_BUFFER_SIZE);
        pDevice->m_pShaderCapturePtr = pDevice->m_pShaderCaptureBuffer; // set write pointer
        if(pDevice->m_pShaderCaptureBuffer == NULL)
        {
            pDevice->m_dwOpcode = VSSNAP_ERROR | 3; // signal memory error
            return;
        }
        
        // initialize capture state
        g_snapshot.BeginVSSnapshot(args);

        pDevice->m_dwOpcode = VSSNAP_BUSY;
        pDevice->m_dwSnapshot |= SDOP_VSACTIVE;    // vertex shader capture active
        break;
    case VSSNAP_BUSY:            // finished with vertex shader snapshot
        pDevice->m_dwSnapshot &= ~SDOP_VSACTIVE;
        g_snapshot.EndVSSnapshot();
        pDevice->m_dwOpcode = VSSNAP_ACK | ((DWORD)(pDevice->m_pShaderCapturePtr) - (DWORD)(pDevice->m_pShaderCaptureBuffer));
        break;
    case VSSNAP_DONE:            // DM has acknowledged receiving data, cleanup
        MemFree(pDevice->m_pShaderCaptureBuffer);
        pDevice->m_pShaderCaptureBuffer = pDevice->m_pShaderCapturePtr = NULL;
        pDevice->m_dwOpcode = VSSNAP_IDLE;
        break;
    
    // Xray opcodes
    case XRAY_ENDACK:
    case XRAY_BEGINACK:
    case XRAY_ERROR:
        break;            // waiting for DM response

    case XRAY_BEGINREQ:   // Enter Xray mode
        pDevice->m_dwSnapshot |= SDOP_XRACTIVE;
        pDevice->m_dwOpcode = XRAY_BEGINACK;
        args = (DWORD *)(pDevice->m_pShaderCaptureBuffer);   // save pointer to arguments
        g_snapshot.dwXRFlags = *(args+2);
        g_snapshot.dwFlags = *(args+2) & 0xf;
        g_snapshot.dwMarker = *(args+3);
        break;

    case XRAY_ENDREQ:     // Leave Xray mode
        pDevice->m_dwSnapshot &= ~SDOP_XRACTIVE;
        pDevice->m_dwOpcode = XRAY_ENDACK;
        break;
    }
#endif //PROFILE
}

void HandleShaderSnapshot_DrawVerticesUP(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT VertexCount,
    CONST void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride)
{
    g_snapshot.DrawVertType = DRAWVERT_TYPE_VUP;
    g_snapshot.PrimitiveType = PrimitiveType;
    g_snapshot.VertexCount = VertexCount;
    g_snapshot.pVertexStreamZeroData = pVertexStreamZeroData;
    g_snapshot.VertexStreamZeroStride = VertexStreamZeroStride;
    g_snapshot.HandleDrawVertices();
}

void HandleShaderSnapshot_DrawIndexedVerticesUP(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT VertexCount,
    CONST void* pIndexData,
    CONST void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride)
{
    g_snapshot.DrawVertType = DRAWVERT_TYPE_IVUP;
    g_snapshot.PrimitiveType = PrimitiveType;
    g_snapshot.VertexCount = VertexCount;
    g_snapshot.pIndexData = pIndexData;
    g_snapshot.pVertexStreamZeroData = pVertexStreamZeroData;
    g_snapshot.VertexStreamZeroStride = VertexStreamZeroStride;
    g_snapshot.HandleDrawVertices();
}
    
void HandleShaderSnapshot_DrawVertices(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT StartVertex,
    UINT VertexCount)
{
    g_snapshot.DrawVertType = DRAWVERT_TYPE_V;
    g_snapshot.PrimitiveType = PrimitiveType;
    g_snapshot.StartVertex = StartVertex;
    g_snapshot.VertexCount = VertexCount;
    g_snapshot.HandleDrawVertices();
}
    
void HandleShaderSnapshot_DrawIndexedVertices(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT VertexCount,
    CONST WORD* pIndexData)
{
    g_snapshot.DrawVertType = DRAWVERT_TYPE_IV;
    g_snapshot.PrimitiveType = PrimitiveType;
    g_snapshot.VertexCount = VertexCount;
    g_snapshot.pIndexData = pIndexData;
    g_snapshot.HandleDrawVertices();
}

void HandleShaderSnapshot_Clear(DWORD Count,
                                CONST D3DRECT *pRects,
                                DWORD Flags,
                                D3DCOLOR Color,
                                float Z,
                                DWORD Stencil) 
{
    CDevice* pDevice = g_pDevice;
    DWORD dwSave = pDevice->m_dwSnapshot;
    
    // Leave snapshot mode and call the appropriate D3D clear method
    pDevice->m_dwSnapshot = 0x0;
    // clear what the user wants first
    D3DDevice_Clear(Count, pRects, Flags, Color, Z, Stencil);
    if(g_snapshot.dwXRFlags & GRAYMASK)
    {
        // now make sure the buffer is black if user selected grayscale
        D3DDevice_Clear( 0, NULL, D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
    }
    pDevice->m_dwSnapshot = dwSave;
}

void 
DebugSnapshot::BeginVSSnapshot(DWORD *args)
{
    CDevice* pDevice = g_pDevice;
    
    dwCaptureCount = 0;     //initialize count of dwords written
    ShaderVerts = OtherVerts = 0;
    dwProcessedVertexCount = 0; // initialize count of processed vertices
    a0x = 0;
    WriteCapture(VSS_STATS | 0x3);
    WriteCapture(0);    // room for stats
    WriteCapture(0);
    dwFirstVertex = *args;
    dwLastVertex = *(args+1);
    dwFlags = *(args+2);
    dwMarker = *(args+3);
}

void 
DebugSnapshot::EndVSSnapshot(void)
{
    WriteCapture(VSS_EOF | 1);
    *(g_pDevice->m_pShaderCaptureBuffer+1) = ShaderVerts;
    *(g_pDevice->m_pShaderCaptureBuffer+2) = OtherVerts;
}

//------------------------------------------------------------------------------
// SetDebugMarker

extern "C"
DWORD WINAPI D3DDevice_SetDebugMarker(
    DWORD Marker)
{
    CDevice* pDevice = g_pDevice;
    DWORD dwOldMarker = pDevice->m_dwDebugMarker;
    pDevice->m_dwDebugMarker = Marker;
    return dwOldMarker;
}

//------------------------------------------------------------------------------
// GetDebugMarker

extern "C"
DWORD WINAPI D3DDevice_GetDebugMarker()
{
    return g_pDevice->m_dwDebugMarker;
}


} // end namespace
