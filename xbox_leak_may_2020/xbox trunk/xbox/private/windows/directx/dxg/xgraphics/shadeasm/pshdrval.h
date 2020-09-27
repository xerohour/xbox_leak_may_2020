///////////////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// pshdrval.hpp
//
// Direct3D Reference Device - PixelShader validation
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __PSHDRVAL_HPP__
#define __PSHDRVAL_HPP__

namespace XGRAPHICS {

#define D3DSIO_TEXBEM_LEGACY    ((D3DSHADER_INSTRUCTION_OPCODE_TYPE)0xC001)
#define D3DSIO_TEXBEML_LEGACY   ((D3DSHADER_INSTRUCTION_OPCODE_TYPE)0xC002)

#define PS_INST_TOKEN_RESERVED_MASK         0xbfff0000 // bits 16-23, 24-29, 31 must be 0
#define PS_DSTPARAM_TOKEN_RESERVED_MASK     0x0000ff00 // bits 8-14 must be 0
#define PS_SRCPARAM_TOKEN_RESERVED_MASK     0x40006000 // bits 13-14, 30 must be 0

//-----------------------------------------------------------------------------
// CPSInstruction
//-----------------------------------------------------------------------------
class CPSInstruction : public CBaseInstruction
{
public:
    CPSInstruction(CPSInstruction* pPrevInst) : CBaseInstruction(pPrevInst)
    {
        m_bTexOp                    = FALSE;
        m_bTexMOp                   = FALSE;
        m_bTexOpThatReadsTexture    = FALSE;
    };

    void CalculateComponentReadMasks();

    BOOL    m_bTexOp;
    BOOL    m_bTexMOp;
    BOOL    m_bTexOpThatReadsTexture;
};

//-----------------------------------------------------------------------------
// CPShaderValidator
//-----------------------------------------------------------------------------
class CPShaderValidator : public CBaseShaderValidator
{
private:
    BOOL            m_bXbox;
    BOOL            m_bXFCUsed;
    UINT            m_TexOpCount;
    UINT            m_BlendOpCount;
    UINT            m_TotalOpCount; // not necessarily the sum of TexOpCount and BlendOpCount....
    UINT            m_TexMBaseDstReg;

    CRegisterFile*  m_pTempRegFile;    
    CRegisterFile*  m_pInputRegFile;
    CRegisterFile*  m_pConstRegFile;
    CRegisterFile*  m_pTextureRegFile;

    CBaseInstruction* AllocateNewInstruction(CBaseInstruction*pPrevInst);
    BOOL DecodeNextInstruction();
    BOOL InitValidation();
    BOOL ApplyPerInstructionRules();
    void ApplyPostInstructionsRules();
    void IsCurrInstTexOp();

    BOOL Rule_InstructionRecognized();
    BOOL Rule_InstructionSupportedByVersion();
    BOOL Rule_ValidParamCount();
    BOOL Rule_ValidSrcParams(); 
    BOOL Rule_NegateAfterSat();
    BOOL Rule_SatBeforeBiasOrComplement();
    BOOL Rule_MultipleDependentTextureReads();
    BOOL Rule_SrcNoLongerAvailable(); 
    BOOL Rule_SrcInitialized();
    BOOL Rule_ValidDstParam();
    BOOL Rule_ValidRegisterPortUsage();
    BOOL Rule_TexRegsDeclaredInOrder();
    BOOL Rule_TexOpAfterNonTexOp();
    BOOL Rule_ValidTEXM3xSequence();               // Call per instruction AND after all instructions seen
    BOOL Rule_ValidTEXM3xRegisterNumbers();
    BOOL Rule_ValidCNDInstruction();
    BOOL Rule_ValidLRPInstruction();
    BOOL Rule_ValidDEFInstruction();
    BOOL Rule_ValidDP3Instruction();
    BOOL Rule_ValidInstructionPairing();
    BOOL Rule_ValidInstructionCount();             // Call per instruction AND after all instructions seen
    BOOL Rule_R0Written();                         // Call after all instructions seen.
        
public:
    CPShaderValidator( const DWORD* pCode, const D3DCAPS8* pCaps, DWORD Flags,
        XD3DXErrorLog* pLog);
    ~CPShaderValidator();
};

HRESULT WINAPI ValidatePixelShader( const DWORD* pCode,
                                    const D3DCAPS8* pCaps,
                                    const DWORD Flags,
                                    XD3DXErrorLog* pLog);

} // namespace XGRAPHICS

#endif __PSHDRVAL_HPP__