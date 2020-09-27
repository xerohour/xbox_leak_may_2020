///////////////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 2000.
//
// pshdrval.cpp
//
// Direct3D Reference Device - PixelShader validation
//
///////////////////////////////////////////////////////////////////////////////
#include "pchshadeasm.h"

namespace XGRAPHICS {


// Use these macros when looking at CPSInstruction derived members of the current instruction (CBaseInstruction)
#define _CURR_PS_INST   ((CPSInstruction*)m_pCurrInst)
#define _PREV_PS_INST   (m_pCurrInst?((CPSInstruction*)(m_pCurrInst->m_pPrevInst)):NULL)

//-----------------------------------------------------------------------------
// PixelShader Validation Rule Coverage
//
// Below is the list of rules in "DX8 PixelShader Version Specification",
// matched to the function(s) in this file which enforce them.
// Note that the mapping from rules to funtions can be 1->n or n->1
//
// Generic Rules
// -------------
//
// PS-G1:           Rule_R0Written
// PS-G2:           Rule_SrcInitialized
// PS-G3:           Rule_ValidDstParam
//
// TEX Op Specific Rules
// ---------------------
//
// PS-T1:           Rule_TexOpAfterNonTexOp
// PS-T2:           Rule_ValidDstParam
// PS-T3:           Rule_ValidDstParam, Rule_ValidSrcParams
// PS-T4:           Rule_TexRegsDeclaredInOrder
// PS-T5:           Rule_SrcInitialized
// PS-T6:           Rule_ValidTEXM3xSequence, Rule_ValidTEXM3xRegisterNumbers, Rule_InstructionSupportedByVersion
// PS-T7:           Rule_ValidSrcParams
//
// Co-Issue Specific Rules
// -----------------------
//
// PS-C1:           Rule_ValidInstructionPairing
// PS-C2:           Rule_ValidInstructionPairing
// PS-C3:           Rule_ValidInstructionPairing
// PS-C4:           Rule_ValidInstructionPairing
// PS-C5:           Rule_ValidInstructionPairing
//
// Instruction Specific Rules
// --------------------------
//
// PS-I1:           Rule_ValidLRPInstruction
// PS-I2:           Rule_ValidCNDInstruction
// PS-I3:           Rule_ValidDstParam
// PS-I4:           Rule_ValidDP3Instruction
// PS-I5:           Rule_ValidInstructionCount
//
// Pixel Shader Version 1.0 Rules
// ------------------------------
//
// PS.1.0-1:        InitValidation,
//                  Rule_SrcInitialized
// PS.1.0-2:        Rule_ValidInstructionPairing
// PS.1.0-3:        <empty rule>
// PS.1.0-4:        Rule_ValidInstructionCount
// PS.1.0-5:        <empty rule>

//
// Pixel Shader Version 1.1 Rules
// ------------------------------
//
// PS.1.1-1:        Rule_ValidDstParam
// PS.1.1-2:        Rule_ValidSrcParams
// PS.1.1-3:        Rule_SrcNoLongerAvailable
// PS.1.1-4:        Rule_SrcNoLongerAvailable
// PS.1.1-5:        Rule_SrcNoLongerAvailable
// PS.1.1-6:        Rule_ValidDstParam
// PS.1.1-8:        Rule_MultipleDependentTextureReads
// PS.1.1-9:        <not validated - implemented by refrast though>
//
// Pixel Shader Version 255.255 Rules
// ----------------------------------
//
// PS.255.255-1:    Rule_ValidTEXM3xSequence, Rule_ValidTEXM3xRegisterNumbers, Rule_InstructionSupportedByVersion
// PS.255.255-2:    Rule_TexOpAfterNonTexOp, Rule_TexRegsDeclaredInOrder
// PS.255.255-3:    Rule_ValidCNDInstruction
// PS.255.255-4:    <not validated - implemented by refrast though>
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPSInstruction::CalculateComponentReadMasks()
//-----------------------------------------------------------------------------
void CPSInstruction::CalculateComponentReadMasks()
{
    BOOL bR, bG, bB, bA;

    // The only instruction that has source registers but no destination register is the 
    // XFC instruction.  If this is the XFC instruction, and the source swizzle is identity,
    // assume the read mask is rgb for all sources except G which is b.
    for( UINT i = 0; i < m_SrcParamCount; i++ )
    {
        bR=FALSE, bG=FALSE; bB=FALSE; bA=FALSE;
        if(m_DstParam[0].m_bParamUsed)
        {
            // destination is used
            if(m_DstParam[0].m_WriteMask & D3DSP_WRITEMASK_0) bR = TRUE;
            if(m_DstParam[0].m_WriteMask & D3DSP_WRITEMASK_1) bG = TRUE;
            if(m_DstParam[0].m_WriteMask & D3DSP_WRITEMASK_2) bB = TRUE;
            if(m_DstParam[0].m_WriteMask & D3DSP_WRITEMASK_3) bA = TRUE;
        }
        else
        {
            // first six xfc instruction read .rgb by default, last one reads just .b
            if(i <= 5)
                bR = bG = bB = TRUE;
            else
                bB = TRUE;
        }
        
        // DP3 instruction does not read alpha
        if(D3DSIO_DP3 == m_Type)
            bA = FALSE;

        BOOL read[4] = {FALSE, FALSE, FALSE, FALSE};
        if(bR)
            read[(m_SrcParam[i].m_SwizzleShift >> (D3DSP_SWIZZLE_SHIFT + 0)) & 0x3] = TRUE;
        if(bG)
            read[(m_SrcParam[i].m_SwizzleShift >> (D3DSP_SWIZZLE_SHIFT + 2)) & 0x3] = TRUE;
        if(bB)
            read[(m_SrcParam[i].m_SwizzleShift >> (D3DSP_SWIZZLE_SHIFT + 4)) & 0x3] = TRUE;
        if(bA)
            read[(m_SrcParam[i].m_SwizzleShift >> (D3DSP_SWIZZLE_SHIFT + 6)) & 0x3] = TRUE;
        
        m_SrcParam[i].m_ComponentReadMask = 0;
        if(read[0])
            m_SrcParam[i].m_ComponentReadMask |= D3DSP_WRITEMASK_0;
        if(read[1])
            m_SrcParam[i].m_ComponentReadMask |= D3DSP_WRITEMASK_1;
        if(read[2])
            m_SrcParam[i].m_ComponentReadMask |= D3DSP_WRITEMASK_2;
        if(read[3])
            m_SrcParam[i].m_ComponentReadMask |= D3DSP_WRITEMASK_3;
    }
}

//-----------------------------------------------------------------------------
// CPShaderValidator::CPShaderValidator
//-----------------------------------------------------------------------------
CPShaderValidator::CPShaderValidator(   const DWORD* pCode,
                                        const D3DCAPS8* pCaps,
                                        DWORD Flags,
                                        XD3DXErrorLog* pLog)
                                        : CBaseShaderValidator( pCode, pCaps, Flags, pLog )
{
    // Note that the base constructor initialized m_ReturnCode to E_FAIL.
    // Only set m_ReturnCode to S_OK if validation has succeeded,
    // before exiting this constructor.

    m_bXFCUsed              = FALSE;

    m_TexOpCount            = 0;
    m_BlendOpCount          = 0;
    m_TotalOpCount          = 0;
    m_TexMBaseDstReg        = 0;

    m_pTempRegFile          = NULL;
    m_pInputRegFile         = NULL;
    m_pConstRegFile         = NULL;
    m_pTextureRegFile       = NULL;

    if( !m_bBaseInitOk )
        return;

    ValidateShader(); // If successful, m_ReturnCode will be set to S_OK.
                      // Call GetStatus() on this object to determine validation outcome.
}

//-----------------------------------------------------------------------------
// CPShaderValidator::~CPShaderValidator
//-----------------------------------------------------------------------------
CPShaderValidator::~CPShaderValidator()
{
    delete m_pTempRegFile;
    delete m_pInputRegFile;
    delete m_pConstRegFile;
    delete m_pTextureRegFile;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::AllocateNewInstruction
//-----------------------------------------------------------------------------
CBaseInstruction* CPShaderValidator::AllocateNewInstruction(CBaseInstruction*pPrevInst)
{
    return new CPSInstruction((CPSInstruction*)pPrevInst);
}

//-----------------------------------------------------------------------------
// CPShaderValidator::DecodeNextInstruction
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::DecodeNextInstruction()
{
    m_pCurrInst->m_Type = (D3DSHADER_INSTRUCTION_OPCODE_TYPE)(*m_pCurrToken & D3DSI_OPCODE_MASK);

    if( m_pCurrInst->m_Type == D3DSIO_COMMENT )
    {
        ParseCommentForAssemblerMessages(m_pCurrToken); // does not advance m_pCurrToken

        // Skip comments
        DWORD NumDWORDs = ((*m_pCurrToken) & D3DSI_COMMENTSIZE_MASK) >> D3DSI_COMMENTSIZE_SHIFT;
        m_pCurrToken += (NumDWORDs+1);
        return TRUE;
    }

    // If the assembler has sent us file and/or line number messages,
    // received by ParseCommentForAssemblerMesssages(),
    // then bind this information to the current instruction.
    // This info can be used in error spew to direct the shader developer
    // to exactly where a problem is located.
    m_pCurrInst->SetSpewFileNameAndLineNumber(m_pLatestSpewFileName,m_pLatestSpewLineNumber);

    if( *m_pCurrToken & D3DSI_COISSUE )
    {
        _CURR_PS_INST->m_bCoIssue = TRUE;
    }
    else
    {
        m_CycleNum++; // First cycle is 1. (co-issued instructions will have same cycle number)
    }
    _CURR_PS_INST->m_CycleNum = m_CycleNum;

    m_SpewInstructionCount++; // only used for spew, not for any limits
    m_pCurrInst->m_SpewInstructionCount = m_SpewInstructionCount;

    if( (*m_pCurrToken) & PS_INST_TOKEN_RESERVED_MASK )
    {
        Spew(SPEW_INSTRUCTION_ERROR,m_pCurrInst,"Reserved bit(s) set in instruction parameter token!  Aborting validation.");
        return FALSE;
    }

    m_pCurrToken++;

    // Decode dst param
    DWORD dstCount;
    switch (m_pCurrInst->m_Type)
    {
    case D3DSIO_NOP:
    case D3DSIO_XFC:
        dstCount = 0;
        break;
    case D3DSIO_XMMA:
    case D3DSIO_XMMC:
        dstCount = 3;
        break;
    case D3DSIO_XDM:
    case D3DSIO_XDD:
        dstCount = 2;
        break;
    default:
        dstCount = 1;
        break;
    }
    while ((*m_pCurrToken & (1L<<31)) && (dstCount-- > 0))
    {
        (m_pCurrInst->m_DstParamCount)++;
        DecodeDstParam( m_pCurrInst->m_Type, &m_pCurrInst->m_DstParam[m_pCurrInst->m_DstParamCount - 1], *m_pCurrToken );
        if( (*m_pCurrToken) & PS_DSTPARAM_TOKEN_RESERVED_MASK )
        {
            Spew(SPEW_INSTRUCTION_ERROR,m_pCurrInst,"Reserved bit(s) set in destination parameter token!  Aborting validation.");
            return FALSE;
        }
        m_pCurrToken++;
        if( D3DSIO_DEF == m_pCurrInst->m_Type )
        {
            // Skip source params (float vector) - nothing to check
            // This is the only instruction with 4 source params,
            // and further, this is the only instruction that has
            // raw numbers as parameters.  This justifies the
            // special case treatment here - we pretend
            // D3DSIO_DEF only has a dst param (which we will check).
            m_pCurrToken += 4;
            return TRUE;
        }
    }

    // Decode src param(s)
    while (*m_pCurrToken & (1L<<31))
    {
        (m_pCurrInst->m_SrcParamCount)++;
        if( (m_pCurrInst->m_SrcParamCount + m_pCurrInst->m_DstParamCount) > SHADER_INSTRUCTION_MAX_PARAMS )
        {
            m_pCurrToken++; // eat up extra parameters and skip to next
            continue;
        }

        // Below: index is [SrcParamCount - 1] because m_SrcParam array needs 0 based index.
        DecodeSrcParam( &(m_pCurrInst->m_SrcParam[m_pCurrInst->m_SrcParamCount - 1]),*m_pCurrToken );

        if( (*m_pCurrToken) & PS_SRCPARAM_TOKEN_RESERVED_MASK )
        {
            Spew(SPEW_INSTRUCTION_ERROR,m_pCurrInst,"Reserved bit(s) set in source %d parameter token!  Aborting validation.",
                            m_pCurrInst->m_SrcParamCount);
            return FALSE;
        }
        m_pCurrToken++;
    }

    // Figure out which components of each source operand actually need to be read,
    // taking into account destination write mask, the type of instruction, source swizzle, etc.
    m_pCurrInst->CalculateComponentReadMasks();

    // Find out if the instruction is a TexOp and/or TexMOp.  Needed by multiple validation rules.
    IsCurrInstTexOp();

    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::IsCurrInstTexOp
//-----------------------------------------------------------------------------
void CPShaderValidator::IsCurrInstTexOp()
{
    DXGASSERT(m_pCurrInst);

    switch (m_pCurrInst->m_Type)
    {
    case D3DSIO_TEXM3x2PAD:
    case D3DSIO_TEXM3x2TEX:
    case D3DSIO_TEXM3x3PAD:
    case D3DSIO_TEXM3x3TEX:
    case D3DSIO_TEXM3x3DIFF:
    case D3DSIO_TEXM3x3SPEC:
    case D3DSIO_TEXM3x3VSPEC:
    case D3DSIO_TEXM3x2DEPTH:
        _CURR_PS_INST->m_bTexMOp = TRUE;
        // fall through
    case D3DSIO_TEXCOORD:
    case D3DSIO_TEXKILL:
    case D3DSIO_TEX:
    case D3DSIO_TEXBEM:
    case D3DSIO_TEXBEML:
    case D3DSIO_TEXBEM_LEGACY:
    case D3DSIO_TEXBEML_LEGACY:
    case D3DSIO_TEXREG2AR:
    case D3DSIO_TEXREG2GB:
    case D3DSIO_TEXBRDF:
#if 0 // Doesn't seem to be defined in Xbox
    case D3DSIO_TEXDP3:
    case D3DSIO_TEXREG2RGB:
#endif
        _CURR_PS_INST->m_bTexOp = TRUE;
        break;
    }

    switch(m_pCurrInst->m_Type)
    {
    case D3DSIO_TEXM3x2PAD:
    case D3DSIO_TEXM3x3PAD:
    case D3DSIO_TEXM3x2DEPTH:
    case D3DSIO_TEXCOORD:
    case D3DSIO_TEXKILL:
#if 0 // Doesn't seem to be defined in Xbox
    case D3DSIO_TEXDP3:
#endif
        _CURR_PS_INST->m_bTexOpThatReadsTexture = FALSE;
        break;
    case D3DSIO_TEX:
    case D3DSIO_TEXM3x3DIFF:
    case D3DSIO_TEXM3x2TEX:
    case D3DSIO_TEXM3x3TEX:
    case D3DSIO_TEXM3x3SPEC:
    case D3DSIO_TEXM3x3VSPEC:
    case D3DSIO_TEXBEM:
    case D3DSIO_TEXBEML:
    case D3DSIO_TEXBEM_LEGACY:
    case D3DSIO_TEXBEML_LEGACY:
    case D3DSIO_TEXREG2AR:
    case D3DSIO_TEXREG2GB:
    case D3DSIO_TEXBRDF:
#if 0 // Doesn't seem to be defined in Xbox
    case D3DSIO_TEXREG2RGB:
#endif
        _CURR_PS_INST->m_bTexOpThatReadsTexture = TRUE;
        break;
    }
}

//-----------------------------------------------------------------------------
// CPShaderValidator::InitValidation
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::InitValidation()
{
    switch( m_Version >> 16 )
    {
    case 0xfffe:
        Spew( SPEW_GLOBAL_ERROR, NULL, "Version Token: 0x%x indicates a vertex shader.  Pixel shader version token must be of the form 0xffff****.",
                m_Version);
        return FALSE;
    case 0xffff:
        break; // pixelshader - ok.
    default:
        Spew( SPEW_GLOBAL_ERROR, NULL, "Version Token: 0x%x is invalid. Pixel shader version token must be of the form 0xffff****. Aborting pixel shader validation.",
                m_Version);
        return FALSE;
    }

    if( m_pCaps )
    {
        if( (m_pCaps->PixelShaderVersion & 0x0000FFFF) < (m_Version & 0x0000FFFF)
            && !(m_Version == D3DPS_VERSION(0xff,0xff))) // not a 255.255 shader - that is always allowed
        {
            Spew( SPEW_GLOBAL_ERROR, NULL, "Version Token: Pixel shader version %d.%d is too high for device.  Maximum supported version is %d.%d. Aborting shader validation.",
                    D3DSHADER_VERSION_MAJOR(m_Version),D3DSHADER_VERSION_MINOR(m_Version),
                    D3DSHADER_VERSION_MAJOR(m_pCaps->PixelShaderVersion),D3DSHADER_VERSION_MINOR(m_pCaps->PixelShaderVersion));
            return FALSE;
        }
    }

    switch(m_Version)
    {
    case D3DPS_VERSION(1,0):    // DX7.5
        m_pTempRegFile      = new CRegisterFile(2,TRUE,2); // #regs, bWritable, max# reads/instruction
        m_pInputRegFile     = new CRegisterFile(2,FALSE,1);
        m_pConstRegFile     = new CRegisterFile(8,FALSE,2);
        m_pTextureRegFile   = new CRegisterFile(4,FALSE,2);
        m_bXbox             = FALSE;
        break;
    case D3DPS_VERSION(1,1):    // DX8.0
        m_pTempRegFile      = new CRegisterFile(2,TRUE,2); // #regs, bWritable, max# reads/instruction
        m_pInputRegFile     = new CRegisterFile(2,FALSE,2);
        m_pConstRegFile     = new CRegisterFile(16,FALSE,2);
        m_pTextureRegFile   = new CRegisterFile(4,TRUE,2);
        m_bXbox             = FALSE;
        break;
    case D3DPS_VERSION(1,10):   // Xbox 1.0 Pixel Shader
    case D3DPS_VERSION(1,11):   // Xbox 1.1 Pixel Shader
        m_pTempRegFile      = new CRegisterFile(4,TRUE,4); // #regs, bWritable, max# reads/instruction
        m_pInputRegFile     = new CRegisterFile(4,TRUE,4);
        m_pConstRegFile     = new CRegisterFile(16,FALSE,4);
        m_pTextureRegFile   = new CRegisterFile(4,TRUE,4);
        m_bXbox             = TRUE;
        break;
    case D3DPS_VERSION(254,254):  // Legacy shader
        m_pTempRegFile      = new CRegisterFile(2,TRUE,5); // #regs, bWritable, max# reads/instruction
        m_pInputRegFile     = new CRegisterFile(2,TRUE,5);
        m_pConstRegFile     = new CRegisterFile(2,TRUE,5);
        m_pTextureRegFile   = new CRegisterFile(8,TRUE,5); // 5 is just arbitrarily large value
        m_bXbox             = FALSE;
        break;
    case D3DPS_VERSION(255,255):
        m_pTempRegFile      = new CRegisterFile(6,TRUE,2); // #regs, bWritable, max# reads/instruction
        m_pInputRegFile     = new CRegisterFile(8,FALSE,2);
        m_pConstRegFile     = new CRegisterFile(16,FALSE,2);
        m_pTextureRegFile   = new CRegisterFile(8,TRUE,2);
        m_bXbox             = FALSE;
        break;
    default:
        Spew( SPEW_GLOBAL_ERROR, NULL, "Version Token: %d.%d is not a supported pixel shader version. Aborting pixel shader validation.",
                D3DSHADER_VERSION_MAJOR(m_Version),D3DSHADER_VERSION_MINOR(m_Version));
        return FALSE;
    }
    if( NULL == m_pTempRegFile ||
        NULL == m_pInputRegFile ||
        NULL == m_pConstRegFile ||
        NULL == m_pTextureRegFile )
    {
        Spew( SPEW_GLOBAL_ERROR, NULL, "Out of memory.");
        return FALSE;
    }

    // The CRegisterFile constructor assumes its registers are initialized if
    // !bWritable && NumReadPorts > 0 is passed into the constructor above.
    // This is a valid assumption in most cases - e.g. input and const register files.
    //
    // This is not a correct assumption for texture registers.  Tex ops can always write to
    // texture regsters, regardless of what bWritable is set to.  In this case, bWritable is meant
    // to indicate whether blend ops can write the register.
    //
    // We never want to assume texture registers are initialized pre-shader, so below
    // they are set to uninitialized.
    for( UINT i = 0; i < m_pTextureRegFile->GetNumRegs(); i++ )
    {
        for( UINT Component = 0; Component < 4; Component++ )
        {
            m_pTextureRegFile->m_pAccessHistory[Component][i].m_bPreShaderInitialized = FALSE;
        }
    }
    
    if(m_bXbox)
    {
        // for XBox pixel shaders, the Zero register (r2) and the fog register (r3) are pre-shader initialized
        m_pTempRegFile->m_pAccessHistory[0][2].m_bPreShaderInitialized = TRUE;
        m_pTempRegFile->m_pAccessHistory[1][2].m_bPreShaderInitialized = TRUE;
        m_pTempRegFile->m_pAccessHistory[2][2].m_bPreShaderInitialized = TRUE;
        m_pTempRegFile->m_pAccessHistory[3][2].m_bPreShaderInitialized = TRUE;
        m_pTempRegFile->m_pAccessHistory[0][3].m_bPreShaderInitialized = TRUE;
        m_pTempRegFile->m_pAccessHistory[1][3].m_bPreShaderInitialized = TRUE;
        m_pTempRegFile->m_pAccessHistory[2][3].m_bPreShaderInitialized = TRUE;
        m_pTempRegFile->m_pAccessHistory[3][3].m_bPreShaderInitialized = TRUE;
    }


    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::ApplyPerInstructionRules
//
// Returns FALSE if shader validation must terminate.
// Returns TRUE if validation may proceed to next instruction.
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::ApplyPerInstructionRules()
{
    if( !   Rule_InstructionRecognized()            ) return FALSE;   // Bail completely on unrecognized instr.
    if( !   Rule_InstructionSupportedByVersion()    ) goto EXIT;
    if( !   Rule_ValidParamCount()                  ) goto EXIT;

   // Rules that examine source parameters
    if( !   Rule_ValidSrcParams()                   ) goto EXIT;
    if( !   Rule_MultipleDependentTextureReads()    ) goto EXIT; // needs to be after _ValidSrcParams(), and before _ValidDstParam(), _SrcInitialized()
    if( !   Rule_SrcNoLongerAvailable()             ) goto EXIT; // needs to be after _ValidSrcParams(), and before _ValidDstParam(), _SrcInitialized()
    if( !   Rule_SrcInitialized()                   ) goto EXIT; // needs to be before _ValidDstParam()

    if( !   Rule_ValidDstParam()                    ) goto EXIT;
    if( !   Rule_ValidRegisterPortUsage()           ) goto EXIT;
    if( !   Rule_TexRegsDeclaredInOrder()           ) goto EXIT;
    if( !   Rule_TexOpAfterNonTexOp()               ) goto EXIT;
    if( !   Rule_ValidTEXM3xSequence()              ) goto EXIT;
    if( !   Rule_ValidTEXM3xRegisterNumbers()       ) goto EXIT;
    if( !   Rule_ValidCNDInstruction()              ) goto EXIT;
    if( !   Rule_ValidLRPInstruction()              ) goto EXIT;
    if( !   Rule_ValidDEFInstruction()              ) goto EXIT;
    if( !   Rule_ValidDP3Instruction()              ) goto EXIT;
    if( !   Rule_ValidInstructionPairing()          ) goto EXIT;
    if( !   Rule_ValidInstructionCount()            ) goto EXIT;
EXIT:
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::ApplyPostInstructionsRules
//-----------------------------------------------------------------------------
void CPShaderValidator::ApplyPostInstructionsRules()
{
    Rule_ValidTEXM3xSequence(); // check once more to see if shader ended dangling in mid-sequence
    Rule_ValidInstructionCount(); // see if we went over the limits
    if(!m_bXFCUsed) // r0 need not be written if XFC instruction is used
        Rule_R0Written();
}

//-----------------------------------------------------------------------------
//
// Per Instruction Rules
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_InstructionRecognized
//
// ** Rule:
// Is the instruction opcode known? (regardless of shader version)
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// FALSE when instruction not recognized.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_InstructionRecognized()
{
    switch(m_pCurrInst->m_Type)
    {
    case D3DSIO_MOV:
    case D3DSIO_ADD:
    case D3DSIO_SUB:
    case D3DSIO_MUL:
    case D3DSIO_MAD:
    case D3DSIO_LRP:
    case D3DSIO_DP3:
    case D3DSIO_TEX:
    case D3DSIO_TEXBEM_LEGACY:
    case D3DSIO_TEXBEML_LEGACY:
    case D3DSIO_TEXBEM:
    case D3DSIO_TEXBEML:
    case D3DSIO_CND:
    case D3DSIO_TEXCOORD:
    case D3DSIO_TEXM3x2PAD:
    case D3DSIO_TEXM3x2TEX:
    case D3DSIO_TEXM3x3PAD:
    case D3DSIO_TEXM3x3TEX:
    case D3DSIO_TEXM3x3DIFF:
    case D3DSIO_TEXM3x3SPEC:
    case D3DSIO_TEXM3x3VSPEC:
    case D3DSIO_TEXREG2AR:
    case D3DSIO_TEXREG2GB:
    case D3DSIO_TEXKILL:
    case D3DSIO_END:
    case D3DSIO_NOP:
    case D3DSIO_DEF:
    case D3DSIO_XMMA:
    case D3DSIO_XMMC:
    case D3DSIO_XDM:
    case D3DSIO_XDD:
    case D3DSIO_TEXM3x2DEPTH:
    case D3DSIO_TEXBRDF:
#if 0 // Not on Xbox
    case D3DSIO_TEXDP3:
    case D3DSIO_TEXREG2RGB:
#endif
    case D3DSIO_DP4:
        return TRUE; // instruction recognized - ok.

    case D3DSIO_XFC:
        m_bXFCUsed = TRUE;
        return TRUE; // instruction recognized, remember it.
    }

    // if we get here, the instruction is not recognized
    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Unrecognized instruction. Aborting pixel shader validation.");
    m_ErrorCount++;
    return FALSE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_InstructionSupportedByVersion
//
// ** Rule:
// Is the instruction supported by the current pixel shader version?
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// FALSE when instruction not supported by version.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_InstructionSupportedByVersion()
{
    if( D3DPS_VERSION(0xfe,0xfe) == m_Version )
    {
    // Special case - instructions used in legacy pixel processing by refdev.
    // Note that D3DSIO_TEX shows up below under 0.5 as well.

        switch(m_pCurrInst->m_Type)
        {
        case D3DSIO_TEX:
        case D3DSIO_TEXBEM_LEGACY:
        case D3DSIO_TEXBEML_LEGACY:
            return TRUE; // instruction supported - ok.
        }
    }
    else // "real" instructions
    {
        if( D3DPS_VERSION(1,0) <= m_Version ) // 1.0 and above
        {
            switch(m_pCurrInst->m_Type)
            {
            case D3DSIO_MOV:
            case D3DSIO_ADD:
            case D3DSIO_SUB:
            case D3DSIO_MUL:
            case D3DSIO_MAD:
            case D3DSIO_LRP:
            case D3DSIO_DP3:
            case D3DSIO_TEX:
            case D3DSIO_DEF:
            case D3DSIO_TEXBEM:
            case D3DSIO_TEXBEML:
            case D3DSIO_CND:
            case D3DSIO_TEXKILL:
            case D3DSIO_TEXCOORD:
            case D3DSIO_TEXM3x2PAD:
            case D3DSIO_TEXM3x2TEX:
            case D3DSIO_TEXM3x3PAD:
            case D3DSIO_TEXM3x3TEX:
            case D3DSIO_TEXM3x3SPEC:
            case D3DSIO_TEXM3x3DIFF:
            case D3DSIO_TEXM3x3VSPEC:
            case D3DSIO_TEXREG2AR:
            case D3DSIO_TEXREG2GB:
                return TRUE; // instruction supported - ok.
            }
        }
        if( D3DPS_VERSION(1,10) <= m_Version ) // 10.0 and above (Xbox)
        {
            switch(m_pCurrInst->m_Type)
            {
            case D3DSIO_XMMA:
            case D3DSIO_XMMC:
            case D3DSIO_XDM:
            case D3DSIO_XDD:
            case D3DSIO_XFC:
            case D3DSIO_TEXM3x2DEPTH:
            case D3DSIO_TEXBRDF:
                return TRUE;
            }
        }

        if( D3DPS_VERSION(255,255) == m_Version )
        {
            switch(m_pCurrInst->m_Type)
            {
            case D3DSIO_TEXM3x3DIFF:
            case D3DSIO_DP4:
                return TRUE; // instruction supported - ok.
            }
        }
    }

    switch(m_pCurrInst->m_Type)
    {
    case D3DSIO_END:
    case D3DSIO_NOP:
        return TRUE; // instruction supported - ok.
    }

    // if we get here, the instruction is not supported.
    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Instruction not supported by version %d.%d pixel shader.",
                D3DSHADER_VERSION_MAJOR(m_Version),D3DSHADER_VERSION_MINOR(m_Version));
    m_ErrorCount++;
    return FALSE;  // no more checks on this instruction
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidParamCount
//
// ** Rule:
// Is the parameter count correct for the instruction?
//
// DEF is a special case that is treated as having only 1 dest parameter,
// even though there are also 4 source parameters.  The 4 source params for DEF
// are immediate float values, so there is nothing to check, and no way of
// knowing whether or not those parameter tokens were actually present in the
// token list - all the validator can do is skip over 4 DWORDS (which it does).
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// FALSE when the parameter count is incorrect.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidParamCount()
{
    BOOL bBadParamCount = FALSE;

    if (m_pCurrInst->m_SrcParamCount + m_pCurrInst->m_DstParamCount > SHADER_INSTRUCTION_MAX_PARAMS)  bBadParamCount = TRUE;
    switch (m_pCurrInst->m_Type)
    {
    case D3DSIO_NOP:
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 0) || (m_pCurrInst->m_SrcParamCount != 0); break;
    case D3DSIO_MOV:
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 1) || (m_pCurrInst->m_SrcParamCount != 1); break;
    case D3DSIO_ADD:
    case D3DSIO_SUB:
    case D3DSIO_MUL:
    case D3DSIO_DP3:
    case D3DSIO_DP4:
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 1) || (m_pCurrInst->m_SrcParamCount != 2); break;
    case D3DSIO_MAD:
    case D3DSIO_LRP:
    case D3DSIO_CND:
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 1) || (m_pCurrInst->m_SrcParamCount != 3); break;
    case D3DSIO_TEXCOORD:
    case D3DSIO_TEXKILL:
    case D3DSIO_TEX:
    case D3DSIO_DEF: // we skipped the last 4 parameters (float vector) - nothing to check
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 1) || (m_pCurrInst->m_SrcParamCount != 0); break;
    case D3DSIO_TEXBEM:
    case D3DSIO_TEXBEM_LEGACY:
    case D3DSIO_TEXBEML:
    case D3DSIO_TEXBEML_LEGACY:
    case D3DSIO_TEXREG2AR:
    case D3DSIO_TEXREG2GB:
    case D3DSIO_TEXM3x2PAD:
    case D3DSIO_TEXM3x2TEX:
    case D3DSIO_TEXM3x2DEPTH:
    case D3DSIO_TEXM3x3PAD:
    case D3DSIO_TEXM3x3TEX:
    case D3DSIO_TEXM3x3DIFF:
    case D3DSIO_TEXM3x3VSPEC:
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 1) || (m_pCurrInst->m_SrcParamCount != 1); break;
    case D3DSIO_TEXM3x3SPEC:
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 1) || (m_pCurrInst->m_SrcParamCount != 2); break;
    case D3DSIO_XMMA:
    case D3DSIO_XMMC:
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 3) || (m_pCurrInst->m_SrcParamCount != 4); break;
    case D3DSIO_XDM:
    case D3DSIO_XDD:
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 2) || (m_pCurrInst->m_SrcParamCount != 4); break;
    case D3DSIO_XFC:
        bBadParamCount = (m_pCurrInst->m_DstParamCount != 0) || (m_pCurrInst->m_SrcParamCount != 7); break;
    }

    if (bBadParamCount)
    {
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid parameter count.");
        m_ErrorCount++;
        return FALSE;  // no more checks on this instruction
    }

    return TRUE;

}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidSrcParams
//
// ** Rule:
// for each source parameter,
//      if current instruction is a texture instruction, then
//          source register type must be texture register
//          (with the exception of D3DSIO_SPEC, where Src1 must be c#), and
//          register # must be within range for texture registers, and
//          modifier must be D3DSPSM_NONE (or _SIGN for TexMatrixOps), and
//          swizzle must be D3DSP_NOSWIZZLE
//      else (non texture instruction)
//          source register type must be D3DSPR_TEMP/_INPUT/_CONST/_TEXTURE
//          register # must be within range for register type
//          modifier must be D3DSPSM_NONE/_NEG/_BIAS/_BIASNEG/_SIGN/_SIGNNEG/_COMP
//          swizzle must be D3DSP_NOSWIZZLE/_REPLICATEALPHA
//
// Note that the parameter count for D3DSIO_DEF is treated as 1
// (dest only), so this rule does nothing for it.
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
// Errors in any of the source parameters causes m_bSrcParamError[i]
// to be TRUE, so later rules that only apply when a particular source
// parameter was valid know whether they need to execute or not.
// e.g. Rule_SrcInitialized.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidSrcParams()  // could break this down for more granularity
{
    for( UINT i = 0; i < m_pCurrInst->m_SrcParamCount; i++ )
    {
        BOOL bFoundSrcError = FALSE;
        SRCPARAM* pSrcParam = &(m_pCurrInst->m_SrcParam[i]);
        char* SourceName[7] = {"first", "second", "third", "fourth", "fifth", "sixth", "seventh"};
        if( _CURR_PS_INST->m_bTexOp )
        {
            if( D3DSPR_TEXTURE != pSrcParam->m_RegType )
            {
                if( D3DSIO_TEXM3x3SPEC == m_pCurrInst->m_Type && (1 == i) )
                {
                    // for _SPEC, last source parameter must be c#
                    if( D3DSPR_CONST != pSrcParam->m_RegType ||
                        D3DSP_NOSWIZZLE != pSrcParam->m_SwizzleShift ||
                        D3DSPSM_NONE != pSrcParam->m_SrcMod )
                    {
                        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Second source parameter for texm3x3spec must be c#.");
                        m_ErrorCount++;
                        bFoundSrcError = TRUE;
                        goto LOOP_CONTINUE;
                    }
                }
                else
                {
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                                    "Src reg for tex ops must be TEXTURE register (%s source param).",
                                    SourceName[i]);
                    m_ErrorCount++;
                    bFoundSrcError = TRUE;
                }
            }

            UINT ValidRegNum = 0;
            switch(pSrcParam->m_RegType)
            {
            case D3DSPR_CONST:      ValidRegNum = m_pConstRegFile->GetNumRegs(); break;
            case D3DSPR_TEXTURE:    ValidRegNum = m_pTextureRegFile->GetNumRegs(); break;
            default:
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid reg type (%s source param).", SourceName[i]);
                m_ErrorCount++;
                bFoundSrcError = TRUE;
                goto LOOP_CONTINUE;
            }

            if( pSrcParam->m_RegNum >= ValidRegNum )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid reg num %d (%s source param).  Max allowed for this type is %d.",
                        pSrcParam->m_RegNum, SourceName[i], ValidRegNum - 1);
                m_ErrorCount++;
                bFoundSrcError = TRUE;
            }

            switch (pSrcParam->m_SwizzleShift)
            {
            case D3DSP_NOSWIZZLE:
                break;
            default:
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid src swizzle for tex op (%s source param).", SourceName[i]);
                m_ErrorCount++;
                bFoundSrcError = TRUE;
            }
        }
        else // not a tex op
        {
            if( D3DSIO_XFC != m_pCurrInst->m_Type)
            {
                // not an xfc instruction, can't use prod or sum
                if(pSrcParam->m_RegType == D3DSPR_INPUT)
                {
                    if(pSrcParam->m_RegNum >= 2)
                    {
                        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid register: prod and sum can only be used with xfc instruction.");
                        m_ErrorCount++;
                        bFoundSrcError = TRUE;
                    }
                }
            }
            else if(i>3)
            {
                // xfc instruction but not src 0 through 3, can't use prod or sum
                if(pSrcParam->m_RegType == D3DSPR_INPUT)
                {
                    if(pSrcParam->m_RegNum >= 2)
                    {
                        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid register: prod and sum can only be used as source registers 0 through 3 with xfc instruction.");
                        m_ErrorCount++;
                        bFoundSrcError = TRUE;
                    }
                }
            }
            UINT ValidRegNum = 0;
            switch(pSrcParam->m_RegType)
            {
            case D3DSPR_TEMP:       ValidRegNum = m_pTempRegFile->GetNumRegs(); break;
            case D3DSPR_INPUT:      ValidRegNum = m_pInputRegFile->GetNumRegs(); break;
            case D3DSPR_CONST:      ValidRegNum = m_pConstRegFile->GetNumRegs(); break;
            case D3DSPR_TEXTURE:    ValidRegNum = m_pTextureRegFile->GetNumRegs(); break;
            default:
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid reg type for %s source param.", SourceName[i]);
                m_ErrorCount++;
                bFoundSrcError = TRUE;
            }

            if( (!bFoundSrcError) && (pSrcParam->m_RegNum >= ValidRegNum) )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid reg num: %d for %s source param. Max allowed for this type is %d.",
                    pSrcParam->m_RegNum, SourceName[i], ValidRegNum - 1);
                m_ErrorCount++;
                bFoundSrcError = TRUE;
            }

            if( D3DSIO_XFC == m_pCurrInst->m_Type)
            {
                // instruction is xfc
                switch( pSrcParam->m_SrcMod )
                {
                case D3DSPSM_NONE:
                case D3DSPSM_COMP:
                case D3DSPSM_SAT:
                    break;
                default:
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid src mod for %s source param.",
                                        SourceName[i]);
                    m_ErrorCount++;
                    bFoundSrcError = TRUE;
                }
            }
            else
            {
                // instruction is not xfc
                switch( pSrcParam->m_SrcMod )
                {
                case D3DSPSM_NONE:
                case D3DSPSM_NEG:
                case D3DSPSM_BIAS:
                case D3DSPSM_BIASNEG:
                case D3DSPSM_SIGN:
                case D3DSPSM_SIGNNEG:
                case D3DSPSM_COMP:
                case D3DSPSM_SAT:
                    break;
                default:
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid src mod for %s source param.",
                                        SourceName[i]);
                    m_ErrorCount++;
                    bFoundSrcError = TRUE;
                }
    
            }
            switch( pSrcParam->m_SwizzleShift )
            {
            case D3DSP_NOSWIZZLE:
            case D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_Z:
            case D3DSP_REPLICATEALPHA:
            case D3DVS_X_Z | D3DVS_Y_Z | D3DVS_Z_Z | D3DVS_W_Z:
                break;
            default:
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid src swizzle for %s source param.",
                                   SourceName[i]);
                m_ErrorCount++;
                bFoundSrcError = TRUE;
            }
        }
LOOP_CONTINUE:
        if( bFoundSrcError )
        {
            m_bSrcParamError[i] = TRUE; // needed in Rule_SrcInitialized
        }
    }


    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_SrcNoLongerAvailable
//
// ** Rule:
// for each source parameter,
//     if it refers to a texture register then
//          for each component of the source register that needs to be read,
//              the src register cannot have been written by TEXKILL or TEXM*PAD instructions, and
//              if the instruction is a tex op then
//                  the src register cannot have been written by TEXBEM or TEXBEML
//              else
//                  the src register cannot have been read by any tex op (unless the tex op
//                          
//
// ** When to call:
// Per instruction. This rule must be called before Rule_ValidDstParam(),
//                  and before Rule_SrcInitialized(),
//                  but after Rule_ValidSrcParams()
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_SrcNoLongerAvailable()
{
    for( UINT i = 0; i < m_pCurrInst->m_SrcParamCount; i++ )
    {
        SRCPARAM* pSrcParam = &(m_pCurrInst->m_SrcParam[i]);
        UINT RegNum = pSrcParam->m_RegNum;
        char* SourceName[3] = {"first", "second", "third"};
        DWORD AffectedComponents = 0;

        if( m_bSrcParamError[i] ) continue;

        for( UINT Component = 0; Component < 4; Component++ )
        {
            if( !(pSrcParam->m_ComponentReadMask & COMPONENT_MASKS[Component]) )
                continue;

            if( D3DSPR_TEXTURE == pSrcParam->m_RegType )
            {
                CAccessHistoryNode* pMostRecentWriter = m_pTextureRegFile->m_pAccessHistory[Component][RegNum].m_pMostRecentWriter;
                if( pMostRecentWriter && pMostRecentWriter->m_pInst  )
                {
                    switch( pMostRecentWriter->m_pInst->m_Type )
                    {
                    case D3DSIO_TEXKILL:
                    case D3DSIO_TEXM3x2PAD:
                    case D3DSIO_TEXM3x3PAD:
                        AffectedComponents |= COMPONENT_MASKS[Component];
                    }
                }
            }
        }
        if( AffectedComponents )
        {
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                "Texture register result of TEXKILL or TEXM*PAD instructions must not be read. Affected components(*) of %s source param: %s",
                SourceName[i],MakeAffectedComponentsText(AffectedComponents));
            m_ErrorCount++;
            m_bSrcParamError[i] = TRUE;
        }

        if( _CURR_PS_INST->m_bTexOp )
        {
            AffectedComponents = 0;
            for( UINT Component = 0; Component < 4; Component++ )
            {
                if( !(pSrcParam->m_ComponentReadMask & COMPONENT_MASKS[Component]) )
                    continue;

                if( D3DSPR_TEXTURE == pSrcParam->m_RegType )
                {
                    CAccessHistoryNode* pMostRecentWriter = m_pTextureRegFile->m_pAccessHistory[Component][RegNum].m_pMostRecentWriter;
                    if( pMostRecentWriter && pMostRecentWriter->m_pInst )
                    {
                        switch( pMostRecentWriter->m_pInst->m_Type )
                        {
                        case D3DSIO_TEXBEM:
                        case D3DSIO_TEXBEML:
                            AffectedComponents |= COMPONENT_MASKS[Component];
                            break;
                        }
                    }
                }
            }
            // This is OK for Xbox
            /*
            if( AffectedComponents )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                    "Texture register result of TEXBEM or TEXBEML instruction must not be read by TEX* instruction. Affected components(*) of %s source param: %s",
                    SourceName[i],MakeAffectedComponentsText(AffectedComponents));
                m_ErrorCount++;
                m_bSrcParamError[i] = TRUE;
            }
            */
        }
        else
        {
            AffectedComponents = 0;
            for( UINT Component = 0; Component < 4; Component++ )
            {
                if( !(pSrcParam->m_ComponentReadMask & COMPONENT_MASKS[Component]) )
                    continue;

                if( D3DSPR_TEXTURE == pSrcParam->m_RegType )
                {
                    CAccessHistoryNode* pMostRecentAccess = m_pTextureRegFile->m_pAccessHistory[Component][RegNum].m_pMostRecentAccess;
                    if( pMostRecentAccess &&
                        pMostRecentAccess->m_pInst &&
                        pMostRecentAccess->m_bRead &&
                        ((CPSInstruction*)(pMostRecentAccess->m_pInst))->m_bTexOp )
                    {
                        AffectedComponents |= COMPONENT_MASKS[Component];
                    }
                }
            }
// This is OK for Xbox
/*
            if( AffectedComponents )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                    "Texture register that has been read by a tex op cannot be read by a non-tex op. Affected components(*) of %s source param: %s",
                    SourceName[i],MakeAffectedComponentsText(AffectedComponents));
                m_ErrorCount++;
                m_bSrcParamError[i] = TRUE;
            }
*/
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_MultipleDependentTextureReads
//
// ** Rule:
//
// Multiple dependent texture reads are disallowed.  So texture read results
// can be used as an address in a subsequent read, but the results from that
// second read cannot be used as an address in yet another subsequent read.
//
// As pseudocode:
//
// if current instruction (x) is a tex-op that reads a texture
//     for each source param of x
//         if the register is a texture register
//         and there exists a previous writer (y),
//         and y is a tex op that reads a texture
//         if there exists a souce parameter of y that was previously
//              written by an instruction that reads a texture (z)
//              SPEW(Error)
//
// NOTE that it is assumed that tex ops must write to all components, so
// only the read/write history for the R component is being checked.
//
// ** When to call:
// Per instruction. This rule must be called before Rule_ValidDstParam(),
//                  and Rule_SrcInitialized()
//                  but after Rule_ValidSrcParams()
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_MultipleDependentTextureReads()
{
    if( !_CURR_PS_INST->m_bTexOpThatReadsTexture )
        return TRUE;

    for( UINT i = 0; i < m_pCurrInst->m_SrcParamCount; i++ )
    {
        SRCPARAM* pSrcParam = &(m_pCurrInst->m_SrcParam[i]);
        UINT RegNum = pSrcParam->m_RegNum;
        char* SourceName[3] = {"first", "second", "third"};

        if( m_bSrcParamError[i] ) continue;

        // Just looking at component 0 in this function because we assume tex ops write to all components.
        if( !(pSrcParam->m_ComponentReadMask & COMPONENT_MASKS[0]) )
            continue;

        if( D3DSPR_TEXTURE != pSrcParam->m_RegType )
            continue;

        CAccessHistoryNode* pMostRecentWriter = m_pTextureRegFile->m_pAccessHistory[0][RegNum].m_pMostRecentWriter;
        if( (!pMostRecentWriter) || (!pMostRecentWriter->m_pInst) )
            continue;

        if(!((CPSInstruction*)(pMostRecentWriter->m_pInst))->m_bTexOp)
            continue;

        if(!((CPSInstruction*)(pMostRecentWriter->m_pInst))->m_bTexOpThatReadsTexture)
            continue;

        for( UINT j = 0; j < pMostRecentWriter->m_pInst->m_SrcParamCount; j++ )
        {
            if( D3DSPR_TEXTURE != pMostRecentWriter->m_pInst->m_SrcParam[j].m_RegType )
                continue;

            CAccessHistoryNode* pRootInstructionHistoryNode =
                m_pTextureRegFile->m_pAccessHistory[0][pMostRecentWriter->m_pInst->m_SrcParam[j].m_RegNum].m_pMostRecentWriter;

            CPSInstruction* pRootInstruction = pRootInstructionHistoryNode ? (CPSInstruction*)pRootInstructionHistoryNode->m_pInst : NULL;

            // no error output here because Xbox allows multiple dependent texture reads
            /*
            if( (D3DSPR_TEXTURE == pMostRecentWriter->m_pInst->m_SrcParam[j].m_RegType)
                && pRootInstruction->m_bTexOpThatReadsTexture )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                    "Multiple dependent texture reads are disallowed (%s source param).  Texture read results can be used as an address for subsequent read, but the results from that read cannot be used as an address in yet another subsequent read.",
                    SourceName[i]);
                m_ErrorCount++;
                m_bSrcParamError[i] = TRUE;
                break;
            }
            */
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_SrcInitialized
//
// ** Rule:
// for each source parameter,
//      if source is a TEMP or TEXTURE register then
//          if the source swizzle is D3DSP_NOSWIZZLE then
//              if the current instruction is DP3 (a cross component op) then
//                  the r, g and b components of of the source reg
//                  must have been previously written
//              else if there is a dest parameter, then
//                  the components in the dest parameter write mask must
//                  have been written to in the source reg. previously
//              else
//                  all components of the source must have been written
//          else if the source swizzle is _REPLICATEALPHA then
//              alpha component of reg must have been previously
//              written
//
// When checking if a component has been written previously,
// it must have been written in a previous cycle - so in the
// case of co-issued instructions, initialization of a component
// by one co-issued instruction is not available to the other for read.
//
// Note that the parameter count for D3DSIO_DEF is treated as 1
// (dest only), so this rule does nothing for it.
//
// ** When to call:
// Per instruction. This rule must be called before Rule_ValidDstParam().
//
// ** Returns:
// Always TRUE.
//
// NOTE: This rule also updates the access history to indicate reads of the
// affected components of each source register.
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_SrcInitialized()
{
    DSTPARAM* pDstParam = &(m_pCurrInst->m_DstParam[0]);

    for( UINT i = 0; i < m_pCurrInst->m_SrcParamCount; i++ )
    {
        SRCPARAM* pSrcParam = &(m_pCurrInst->m_SrcParam[i]);
        UINT RegNum = pSrcParam->m_RegNum;
        CRegisterFile* pRegFile = NULL;
        char* RegChar = NULL;
        DWORD UninitializedComponentsMask = 0;
        CAccessHistoryNode* pWriterInCurrCycle[4] = {0, 0, 0, 0};
        UINT NumUninitializedComponents = 0;

        if( m_bSrcParamError[i] ) continue;

        switch( pSrcParam->m_RegType )
        {
            case D3DSPR_TEMP:
                pRegFile = m_pTempRegFile;
                RegChar = "r";
                break;
            case D3DSPR_TEXTURE:
                pRegFile = m_pTextureRegFile;
                RegChar = "t";
                break;
            case D3DSPR_INPUT:
                pRegFile = m_pInputRegFile;
                RegChar = "v";
                break;
            case D3DSPR_CONST:
                pRegFile = m_pConstRegFile;
                RegChar = "c";
                break;
        }
        if( !pRegFile ) continue;

        // check for read of uninitialized components
        if( D3DSPR_TEMP == pSrcParam->m_RegType ||
            D3DSPR_TEXTURE == pSrcParam->m_RegType )
        {
            for( UINT Component = 0; Component < 4; Component++ )
            {
                if( !(pSrcParam->m_ComponentReadMask & COMPONENT_MASKS[Component]) )
                    continue;

                CAccessHistoryNode* pPreviousWriter = pRegFile->m_pAccessHistory[Component][RegNum].m_pMostRecentWriter;
                CBaseInstruction* pCurrInst = m_pCurrInst;

                // If co-issue, find the real previous writer.
                while( pPreviousWriter
                       && ((CPSInstruction*)pPreviousWriter->m_pInst)->m_CycleNum == _CURR_PS_INST->m_CycleNum )
                {
                    pWriterInCurrCycle[Component] = pPreviousWriter; // log read just before this write for co-issue
                    pPreviousWriter = pPreviousWriter->m_pPreviousWriter;
                }

                // Even if pPreviousWriter == NULL, the component could have been initialized pre-shader.
                // So to check for initialization, we look at m_bInitialized below, rather than pPreviousWrite
                if(pPreviousWriter == NULL && !pRegFile->m_pAccessHistory[Component][RegNum].m_bPreShaderInitialized)
                {
                    NumUninitializedComponents++;
                    UninitializedComponentsMask |= COMPONENT_MASKS[Component];
                }
            }

            if( NumUninitializedComponents )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Read of uninitialized component%s(*) in %s%d: %s",
                    NumUninitializedComponents > 1 ? "s" : "",
                    RegChar, RegNum, MakeAffectedComponentsText(UninitializedComponentsMask));
                m_ErrorCount++;
            }
        }

        // Update register file to indicate READ.
        // Multiple reads of the same register component by the current instruction
        // will only be logged as one read in the access history.

        for( UINT Component = 0; Component < 4; Component++ )
        {
            #define PREV_READER(_CHAN,_REG) \
                    ((NULL == pRegFile->m_pAccessHistory[_CHAN][_REG].m_pMostRecentReader) ? NULL :\
                    pRegFile->m_pAccessHistory[_CHAN][_REG].m_pMostRecentReader->m_pInst)

            if( !(pSrcParam->m_ComponentReadMask & COMPONENT_MASKS[Component]) )
                continue;

            if( NULL != pWriterInCurrCycle[Component] )
            {
                if( !pWriterInCurrCycle[Component]->m_pPreviousReader ||
                    pWriterInCurrCycle[Component]->m_pPreviousReader->m_pInst != m_pCurrInst )
                {
                    if( !pRegFile->m_pAccessHistory[Component][RegNum].InsertReadBeforeWrite(
                                            pWriterInCurrCycle[Component], m_pCurrInst ) )
                    {
                        Spew( SPEW_GLOBAL_ERROR, NULL, "Out of memory");
                        m_ErrorCount++;
                    }
                }
            }
            else if( PREV_READER(Component,RegNum) != m_pCurrInst )
            {
                if( !pRegFile->m_pAccessHistory[Component][RegNum].NewAccess(m_pCurrInst,FALSE) )
                {
                    Spew( SPEW_GLOBAL_ERROR, NULL, "Out of memory");
                    m_ErrorCount++;
                }
            }
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidDstParam
//
// ** Rule:
// if instruction is D3DSIO_DEF, then do nothing - this case has its own separate rule
// the dst register must be writable.
// if the instruction has a dest parameter (i.e. every instruction except NOP), then
//      the dst register must be of type D3DSPR_TEMP or _TEXTURE, and
//      register # must be within range for the register type, and
//      the write mask must be: .rgba, .a or .rgb
//      if instruction is a texture instruction, then
//          the dst register must be of type D3DSPR_TEXTURE, and
//          the writemask must be D3DSP_WRITEMASK_ALL, and
//          the dst modifier must be D3DSPDM_NONE (or _SAT on version > 1.1), and
//          the dst shift must be none
//      else (non tex instruction)
//          the dst modifier must be D3DSPDM_NONE or _BIAS, and
//          dst shift must be /2, none, *2, or *4 (for v.255.255, any shift allowed) and
//          multiple dsts must not refer to the same register unless it is discard.
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
// NOTE: After checking the dst parameter, if no error was found,
// the write to the appropriate component(s) of the destination register
// is recorded by this function, so subsequent rules may check for previous
// write to registers.
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidDstParam() // could break this down for more granularity
{
    BOOL   bFoundDstError;
    DSTPARAM* pDstParam;
    UINT RegNum;
    if( D3DSIO_DEF == m_pCurrInst->m_Type )
    {
        // _DEF is a special instruction whose dest is a const register.
        // We do the checking for this in a separate function.
        // Also, we don't need to keep track of the fact that
        // this instruction wrote to a register (done below),
        // since _DEF just declares a constant.
        return TRUE;
    }

    // step through all dst params in instruction
    for(UINT di=0; di<m_pCurrInst->m_DstParamCount; di++)
    {
        pDstParam = &(m_pCurrInst->m_DstParam[di]);
        RegNum = pDstParam->m_RegNum;
        bFoundDstError = FALSE;
    
        if( pDstParam->m_bParamUsed )
        {
            UINT ValidRegNum = 0;
    
            BOOL bWritable = FALSE;
            switch( pDstParam->m_RegType )
            {
            case D3DSPR_INPUT:
                bWritable = m_pInputRegFile->IsWritable();
                ValidRegNum = m_pInputRegFile->GetNumRegs();
                if( m_bXbox )
                {
                    if(RegNum == 2) // sum not writeable
                    {
                        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "sum register invalid dest." );
                        m_ErrorCount++;
                        bFoundDstError = TRUE;
                    }
                    if(RegNum == 3) // prod not writeable
                    {
                        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "prod register invalid dest." );
                        m_ErrorCount++;
                        bFoundDstError = TRUE;
                    }
                }
                break;
    
            case D3DSPR_TEMP:
                bWritable = m_pTempRegFile->IsWritable();
                ValidRegNum = m_pTempRegFile->GetNumRegs();
                if( m_bXbox )
                {
                    if(RegNum == 3) // fog not writeable
                    {
                        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Fog register invalid dest." );
                        m_ErrorCount++;
                        bFoundDstError = TRUE;
                    }
                }
                break;
            case D3DSPR_TEXTURE:
                if( _CURR_PS_INST->m_bTexOp )
                    bWritable = TRUE;
                else
                    bWritable = m_pTextureRegFile->IsWritable();
    
                ValidRegNum = m_pTextureRegFile->GetNumRegs();
                break;
            }
    
            if( !bWritable || !ValidRegNum )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid reg type for dest param." );
                m_ErrorCount++;
                bFoundDstError = TRUE;
            }
            else if( RegNum >= ValidRegNum )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid dest reg num: %d. Max allowed for this reg type is %d.",
                    RegNum, ValidRegNum - 1);
                m_ErrorCount++;
                bFoundDstError = TRUE;
            }
    
            if( _CURR_PS_INST->m_bTexOp )
            {
                // check to see if r0.a is initialized by this instruction
                if((RegNum == 0) && ((D3DSIO_TEX == m_pCurrInst->m_Type) || 
                                     (D3DSIO_TEXCOORD == m_pCurrInst->m_Type)))
                {
                    // r0.a is initialized with t0.a
                    m_pTempRegFile->m_pAccessHistory[3][0].m_bPreShaderInitialized = TRUE;
                }

                if( D3DSPR_TEXTURE != pDstParam->m_RegType )
                {
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Must use texture register a dest param for tex ops." );
                    m_ErrorCount++;
                    bFoundDstError = TRUE;
                }
                if( D3DSP_WRITEMASK_ALL != pDstParam->m_WriteMask )
                {
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "TEX ops must write all components." );
                    m_ErrorCount++;
                    bFoundDstError = TRUE;
                }
                switch( pDstParam->m_DstMod )
                {
                case D3DSPDM_NONE:
                    break;
    
                    // falling through
                default:
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid dst modifier for tex op." );
                    m_ErrorCount++;
                    bFoundDstError = TRUE;
                }
                switch( pDstParam->m_DstShift )
                {
                case DSTSHIFT_NONE:
                    break;
                default:
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Dst shift not allowed for tex op." );
                    m_ErrorCount++;
                    bFoundDstError = TRUE;
                }
            }
            else
            {
                switch( pDstParam->m_DstMod )
                {
                case D3DSPDM_NONE:
                    break;
                case D3DSPDM_BIAS:
                    if(( D3DPS_VERSION(1,0) == m_Version ) || ( D3DPS_VERSION(1,1) == m_Version ))
                    {
                        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid destination modifier (use xps.1.0 shader version to enable this extension)." );
                        m_ErrorCount++;
                        bFoundDstError = TRUE;
                    }
                    break;
                default:
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid dst modifier." );
                    m_ErrorCount++;
                    bFoundDstError = TRUE;
                }
    
                if( D3DPS_VERSION(255,255) != m_Version )
                {
                    switch( pDstParam->m_DstShift )
                    {
                    case DSTSHIFT_NONE:
                    case DSTSHIFT_X2:
                    case DSTSHIFT_X4:
                    case DSTSHIFT_D2:
                        break;
                    default:
                        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid dst shift." );
                        m_ErrorCount++;
                        bFoundDstError = TRUE;
                    }
                }
            }
    
            if( (D3DSP_WRITEMASK_ALL != pDstParam->m_WriteMask)
                && ((D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2) != pDstParam->m_WriteMask )
                && (D3DSP_WRITEMASK_3 != pDstParam->m_WriteMask ) )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Dest write mask must be .rgb, .a, or .rgba (all)." );
                m_ErrorCount++;
                bFoundDstError = TRUE;
            }
    
            DWORD dwWriteMask;
            if(di == 0)
            {
                // first destination parameter
                dwWriteMask = pDstParam->m_WriteMask;
            }
            else
            {
                // check against first dest writemask
                if(dwWriteMask != pDstParam->m_WriteMask)
                {
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Dest write mask must be the same for all dest registers." );
                    m_ErrorCount++;
                    bFoundDstError = TRUE;
                }

                // make sure that multiple writes are not done to the same register
                int ci;
                for(ci=di-1; ci>=0; ci--)
                {
                    DSTPARAM *pChkDstParam;
                    pChkDstParam = &(m_pCurrInst->m_DstParam[ci]);
                    if((pDstParam->m_RegType == pChkDstParam->m_RegType) &&
                       (pDstParam->m_RegNum == pChkDstParam->m_RegNum) &&
                       !((pDstParam->m_RegType == D3DSPR_TEMP) &&
                         (pDstParam->m_RegNum == 2)))
                    {
                        // dst register written multiple times and it's not discard
                        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Dest register other than discard written twice." );
                        m_ErrorCount++;
                        bFoundDstError = TRUE;
                    }
                }
            }
            // Update register file to indicate write.
            if( !bFoundDstError )
            {
                CRegisterFile* pRegFile = NULL;
                switch( pDstParam->m_RegType )
                {
                case D3DSPR_TEMP:       pRegFile = m_pTempRegFile; break;
                case D3DSPR_TEXTURE:    pRegFile = m_pTextureRegFile; break;
                }
    
                if( pRegFile )
                {
                    if( pDstParam->m_WriteMask & D3DSP_WRITEMASK_0 )
                        pRegFile->m_pAccessHistory[0][RegNum].NewAccess(m_pCurrInst,TRUE);
    
                    if( pDstParam->m_WriteMask & D3DSP_WRITEMASK_1 )
                        pRegFile->m_pAccessHistory[1][RegNum].NewAccess(m_pCurrInst,TRUE);
    
                    if( pDstParam->m_WriteMask & D3DSP_WRITEMASK_2 )
                        pRegFile->m_pAccessHistory[2][RegNum].NewAccess(m_pCurrInst,TRUE);
    
                    if( pDstParam->m_WriteMask & D3DSP_WRITEMASK_3 )
                        pRegFile->m_pAccessHistory[3][RegNum].NewAccess(m_pCurrInst,TRUE);
                }
            }
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidRegisterPortUsage
//
// ** Rule:
// Each register class (TEMP,TEXTURE,INPUT,CONST) may only appear as parameters
// in an individual instruction up to a maximum number of times.
//
// Multiple accesses to the same register number (in the same register class)
// only count as one access.
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidRegisterPortUsage()
{
    UINT TempRegPortUsage[SHADER_INSTRUCTION_MAX_SRCPARAMS];
    UINT InputRegPortUsage[SHADER_INSTRUCTION_MAX_SRCPARAMS];
    UINT ConstRegPortUsage[SHADER_INSTRUCTION_MAX_SRCPARAMS];
    UINT TextureRegPortUsage[SHADER_INSTRUCTION_MAX_SRCPARAMS];

    UINT NumUniqueTempRegs = 0;
    UINT NumUniqueInputRegs = 0;
    UINT NumUniqueConstRegs = 0;
    UINT NumUniqueTextureRegs = 0;

    for( UINT i = 0; i < m_pCurrInst->m_SrcParamCount; i++ )
    {
        D3DSHADER_PARAM_REGISTER_TYPE   RegType;
        UINT                            RegNum;
        UINT*                           pRegPortUsage = NULL;
        UINT*                           pNumUniqueRegs = NULL;

        if( !m_pCurrInst->m_SrcParam[i].m_bParamUsed ) continue;

        RegType = m_pCurrInst->m_SrcParam[i].m_RegType;
        RegNum = m_pCurrInst->m_SrcParam[i].m_RegNum;

        switch( RegType )
        {
        case D3DSPR_TEMP:
            pRegPortUsage = TempRegPortUsage;
            pNumUniqueRegs = &NumUniqueTempRegs;
            break;
        case D3DSPR_INPUT:
            pRegPortUsage = InputRegPortUsage;
            pNumUniqueRegs = &NumUniqueInputRegs;
            break;
        case D3DSPR_CONST:
            pRegPortUsage = ConstRegPortUsage;
            pNumUniqueRegs = &NumUniqueConstRegs;
            break;
        case D3DSPR_TEXTURE:
            pRegPortUsage = TextureRegPortUsage;
            pNumUniqueRegs = &NumUniqueTextureRegs;
            break;
        }

        if( !pRegPortUsage ) continue;

        BOOL    bRegAlreadyAccessed = FALSE;
        for( UINT j = 0; j < *pNumUniqueRegs; j++ )
        {
            if( pRegPortUsage[j] == RegNum )
            {
                bRegAlreadyAccessed = TRUE;
                break;
            }
        }
        if( !bRegAlreadyAccessed )
        {
            pRegPortUsage[*pNumUniqueRegs] = RegNum;
            (*pNumUniqueRegs)++;
        }
    }

    if( NumUniqueTempRegs > m_pTempRegFile->GetNumReadPorts() )
    {
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Temp registers (r#) read %d times in one instruction.  Max #reads/instruction is %d.",
                        NumUniqueTempRegs,  m_pTempRegFile->GetNumReadPorts());
        m_ErrorCount++;
    }

    if( NumUniqueInputRegs > m_pInputRegFile->GetNumReadPorts() )
    {
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Input registers (v#) read %d times by instruction.  Max #reads/instruction is %d.",
                        NumUniqueInputRegs,  m_pInputRegFile->GetNumReadPorts());
        m_ErrorCount++;
    }

    if( NumUniqueConstRegs > m_pConstRegFile->GetNumReadPorts() )
    {
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Constant registers (c#) read %d times by instruction.  Max #reads/instruction is %d.",
                        NumUniqueConstRegs, m_pConstRegFile->GetNumReadPorts());
        m_ErrorCount++;
    }

    if( NumUniqueTextureRegs > m_pTextureRegFile->GetNumReadPorts() )
    {
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Texture registers (t#) read %d times by instruction.  Max #reads/instruction is %d.",
                        NumUniqueTextureRegs, m_pTextureRegFile->GetNumReadPorts());
        m_ErrorCount++;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_TexRegsDeclaredInOrder
//
// ** Rule:
// Tex registers must declared in increasing order.
// ex. invalid sequence:    tex t0
//                          tex t3
//                          tex t1
//
//     another invalid seq: tex t0
//                          tex t1
//                          texm3x2pad t1, t0 (t1 already declared)
//                          texm3x2pad t2, t0
//
//     valid sequence:      tex t0
//                          tex t1
//                          tex t3 (note missing t2.. OK)
//
// For v255.255, the above applies, with the following exception:
// A texture register may be redeclared only if it has not been
// previously the destination of a tex-op that did a texture lookup.
//
// ex. valid sequence:  tex         t0
//                      texcoord    t1   
//                      add         t1, t1, t0 // (note 255.255 allows tex/blend op mixing)
//                      texreg2rgb  t1, t1
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_TexRegsDeclaredInOrder()
{
    static DWORD s_TexOpRegDeclOrder; // bit flags

    if( NULL == m_pCurrInst->m_pPrevInst )   // First instruction - initialize static vars
    {
        s_TexOpRegDeclOrder = 0;
    }
    if( !_CURR_PS_INST->m_bTexOp )
        return TRUE;

    DWORD RegNum = m_pCurrInst->m_DstParam[0].m_RegNum;
    if( (D3DSPR_TEXTURE != m_pCurrInst->m_DstParam[0].m_RegType) ||
        (RegNum > m_pTextureRegFile->GetNumRegs()) )
    {
        return TRUE;
    }

    DWORD RegMask = 1 << m_pCurrInst->m_DstParam[0].m_RegNum;
    if( RegMask & s_TexOpRegDeclOrder)
    {
        if( (D3DPS_VERSION(255,255) == m_Version) )
        {
            CAccessHistoryNode* pPrevWriter = m_pTextureRegFile->m_pAccessHistory[0][RegNum].m_pMostRecentWriter;
            DXGASSERT(pPrevWriter); // We only got here if there was a previous writer.
            if( m_pCurrInst == pPrevWriter->m_pInst ) // Already made the current inst. the most recent writer.
                pPrevWriter = pPrevWriter->m_pPreviousWriter;

            while(pPrevWriter)
            {
                if( ((CPSInstruction*)(pPrevWriter->m_pInst))->m_bTexOpThatReadsTexture ||
                     ((CPSInstruction*)(pPrevWriter->m_pInst))->m_bTexMOp )
                {
                    Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Tex register t%d already declared either by a tex op that did a texture lookup, or by a texture matrix op.",
                                    RegNum);
                    m_ErrorCount++;
                    break;
                }
                pPrevWriter = pPrevWriter->m_pPreviousWriter;
            }
        }
        else
        {
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Tex register t%d already declared.",
                            RegNum);
            m_ErrorCount++;
        }
    } 
    else if( s_TexOpRegDeclOrder > RegMask )
    {
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Tex registers must appear in sequence (i.e. t0 before t2 OK, but t1 before t0 not valid)." );
        m_ErrorCount++;
    }
    s_TexOpRegDeclOrder |= (1 << m_pCurrInst->m_DstParam[0].m_RegNum);
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_TexOpAfterNonTexOp
//
// ** Rule:
// Tex ops (see IsTexOp() for which instructions are considered tex ops)
// must appear before any other instruction, with the exception of DEF or NOP.
//
// This rule does not enforce anything in the case of a legacy pixel shader,
// or for version 255.255
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_TexOpAfterNonTexOp()
{
    static BOOL s_bFoundNonTexOp;
    static BOOL s_bRuleDisabled;

    if( NULL == m_pCurrInst->m_pPrevInst ) // First instruction - initialize static vars
    {
        s_bFoundNonTexOp = FALSE;
        s_bRuleDisabled = ( D3DPS_VERSION(0xfe,0xfe) == m_Version ) || // Legacy pixel shader
                          ( D3DPS_VERSION(0xff,0xff) == m_Version);    // 255.255
    }

    if( s_bRuleDisabled )
        return TRUE;

    // Execute the rule.

    if( !(_CURR_PS_INST->m_bTexOp)
        && m_pCurrInst->m_Type != D3DSIO_NOP
        && m_pCurrInst->m_Type != D3DSIO_DEF)
    {
        s_bFoundNonTexOp = TRUE;
        return TRUE;
    }

    if( _CURR_PS_INST->m_bTexOp && s_bFoundNonTexOp )
    {
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Tex ops after non-tex ops." );
        m_ErrorCount++;
        s_bRuleDisabled = TRUE;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidTEXM3xSequence
//
// ** Rule:
// TEXM3x* instructions, if present in the pixel shader, must appear in
// any of the follwing sequences:
//
//      1) texm3x2pad
//      2) texm3x2tex / texdepth
//
// or   1) texm3x3pad
//      2) texm3x3pad
//      3) texm3x3tex
//
// or   1) texm3x3pad
//      2) texm3x3pad / texm3x3diff
//      3) texm3x3spec / texm3x3vspec
//
// ** When to call:
// Per instruction AND after all instructions have been seen.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidTEXM3xSequence()
{
    static UINT s_TexMSequence;
    static UINT s_LastInst;

    if( NULL == m_pCurrInst )
    {
        return TRUE;
    }

    if( NULL == m_pCurrInst->m_pPrevInst ) // First instruction - initialize static vars
    {
       s_TexMSequence = 0;
       s_LastInst = D3DSIO_NOP;
    }

    if( m_bSeenAllInstructions )
    {
        if( s_TexMSequence )
        {
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Incomplete TEXM* sequence." );
            m_ErrorCount++;
        }
        return TRUE;
    }

    // Execute the rule.

    if( _CURR_PS_INST->m_bTexMOp )
    {
        switch( m_pCurrInst->m_Type )
        {
        case D3DSIO_TEXM3x2PAD:
            if( s_TexMSequence ) goto _TexMSeqInvalid;
            m_TexMBaseDstReg = m_pCurrInst->m_DstParam[0].m_RegNum;
            s_TexMSequence = 1;
            break;
        case D3DSIO_TEXM3x2TEX:
        case D3DSIO_TEXM3x2DEPTH:
            // must be one 3x2PAD previous
            if ( (s_TexMSequence != 1) ||
                 (s_LastInst != D3DSIO_TEXM3x2PAD) ) goto _TexMSeqInvalid;
            s_TexMSequence = 0;
            break;
        case D3DSIO_TEXM3x3PAD:
            if (s_TexMSequence)
            {
                // if in sequence, then must be one 3x3PAD previous
                if ( (s_TexMSequence != 1) ||
                     (s_LastInst != D3DSIO_TEXM3x3PAD) ) goto _TexMSeqInvalid;
                s_TexMSequence = 2;
                break;
            }
            m_TexMBaseDstReg = m_pCurrInst->m_DstParam[0].m_RegNum;
            s_TexMSequence = 1;
            break;
        case D3DSIO_TEXM3x3TEX:
            // must be two 3x3PAD previous
            if ( (s_TexMSequence != 2) ||
                 (s_LastInst != D3DSIO_TEXM3x3PAD) ) goto _TexMSeqInvalid;
            s_TexMSequence = 0;
            break;
        case D3DSIO_TEXM3x3DIFF:
            // must be one 3x3PAD previous
            if ( (s_TexMSequence != 1) ||
                 (s_LastInst != D3DSIO_TEXM3x3PAD) ) goto _TexMSeqInvalid;
            s_TexMSequence = 2;
            break;
        case D3DSIO_TEXM3x3SPEC:
        case D3DSIO_TEXM3x3VSPEC:
            // must be at sequence 2 w/ either PAD or DIFF previous
            if ( (s_TexMSequence != 2) ||
                 ( (s_LastInst != D3DSIO_TEXM3x3PAD) &&
                   (s_LastInst != D3DSIO_TEXM3x3DIFF) ) ) goto _TexMSeqInvalid;
            s_TexMSequence = 0;
            break;
        default:
            break;
        }
        goto _TexMSeqOK;
_TexMSeqInvalid:
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid TEXM* sequence." );
        m_ErrorCount++;
    }
_TexMSeqOK:

    s_LastInst = m_pCurrInst->m_Type;
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidTEXM3xRegisterNumbers
//
// ** Rule:
// If instruction is a TEXM3x*, register numbers must be as follows:
//
//      1) texm3x2pad / texdepth        t(x), t(y1)
//      2) texm3x2tex                   t(x+1), t(y2)
//
//      1) texm3x3pad                   t(x), t(y1)
//      2) texm3x3pad                   t(x+1), t(y2)
//      3) texm3x3tex                   t(x+2), t(y3)
//
//      1) texm3x3pad                   t(x), t(y1)
//      2) texm3x3pad / texm3x3diff     t(x+1), t(y2)
//      3) texm3x3spec                  t(x+2), t(y3), c#
//
//      1) texm3x3pad                   t(x), t(y1)
//      2) texm3x3pad / texm3x3diff     t(x+1), t(y2)
//      3) texm3x3vspec                 t(x+2), t(y3)
//
//      where y1, y2, and y3 are all less than x
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidTEXM3xRegisterNumbers()
{
    #define PREV_INST_TYPE(_INST) \
                ((_INST && _INST->m_pPrevInst) ? _INST->m_pPrevInst->m_Type : D3DSIO_NOP)

    if( _CURR_PS_INST->m_bTexMOp )
    {
        DWORD DstParamR = m_pCurrInst->m_DstParam[0].m_RegNum;
        DWORD SrcParam0R = m_pCurrInst->m_SrcParam[0].m_RegNum;
        switch (m_pCurrInst->m_Type)
        {
        case D3DSIO_TEXM3x2PAD:
            break;
        case D3DSIO_TEXM3x2TEX:
        case D3DSIO_TEXM3x2DEPTH:
            if ( DstParamR != (m_TexMBaseDstReg + 1) )
                goto _TexMRegInvalid;
            if( SrcParam0R >= m_TexMBaseDstReg )
                goto _TexMRegInvalid;
            break;
        case D3DSIO_TEXM3x3PAD:
        {
            if ( D3DSIO_TEXM3x3PAD == PREV_INST_TYPE(m_pCurrInst) &&
                 (DstParamR != (m_TexMBaseDstReg + 1) ) )
                    goto _TexMRegInvalid;

            if ( D3DSIO_TEXM3x3PAD == PREV_INST_TYPE(m_pCurrInst) &&
                 (SrcParam0R >= m_TexMBaseDstReg ))
                    goto _TexMRegInvalid;
            break;
        }
        case D3DSIO_TEXM3x3DIFF:
            if ( DstParamR != (m_TexMBaseDstReg + 1) )
                goto _TexMRegInvalid;
            if( SrcParam0R >= m_TexMBaseDstReg )
                    goto _TexMRegInvalid;
            break;
        case D3DSIO_TEXM3x3SPEC:
            // SPEC requires second src param to be from const regs
            if ( m_pCurrInst->m_SrcParam[1].m_RegType != D3DSPR_CONST )
                goto _TexMRegInvalid;
            // fall through
        case D3DSIO_TEXM3x3TEX:
        case D3DSIO_TEXM3x3VSPEC:
            if ( DstParamR != (m_TexMBaseDstReg + 2) )
                goto _TexMRegInvalid;
            if( SrcParam0R >= m_TexMBaseDstReg )
                    goto _TexMRegInvalid;
            break;
        default:
            break;
        }
        goto _TexMRegOK;
_TexMRegInvalid:
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid TEXM* register." );
        m_ErrorCount++;
    }
_TexMRegOK:
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidCNDInstruction
//
// ** Rule:
// First source for cnd instruction must be 'r0.a' (exactly).
// i.e. cnd r1, r0.a, t0, t1
//
// For v255.255, first source can be any register type/number, and
//               .a and no-swizzle are allowed.
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidCNDInstruction()
{
    if( D3DSIO_CND == m_pCurrInst->m_Type )
    {
        SRCPARAM Src0 = m_pCurrInst->m_SrcParam[0];
        if( D3DPS_VERSION(255,255) == m_Version )
        {
            if( Src0.m_bParamUsed &&
                D3DSPSM_NONE == Src0.m_SrcMod &&
                ( (D3DSP_REPLICATEALPHA == Src0.m_SwizzleShift) ||
                  (D3DSP_NOSWIZZLE == Src0.m_SwizzleShift)
                )
              )
            {
                return TRUE;    // Src 0 register has no modifiers and swizzle is either .a or .rgba
            }

            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "First source register for CND instruction must be 'r0.a' or 'r0.rgba'/'r0'(no-swizzle)." );
            m_ErrorCount++;
        }
        else
        {
            if( Src0.m_bParamUsed &&
                D3DSPR_TEMP == Src0.m_RegType &&
                0 == Src0.m_RegNum &&
                D3DSP_REPLICATEALPHA == Src0.m_SwizzleShift &&
                D3DSPSM_NONE == Src0.m_SrcMod )
            {
                return TRUE;    // Src 0 is r0.a
            }

            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "First source for CND instruction must be 'r0.a'." );
            m_ErrorCount++;

        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidLRPInstruction
//
// ** Rule:
// The only valid source modifier for the src0 operand for LRP is complement
// (1-reg)
// i.e. lrp r1, 1-r0, t0, t1
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidLRPInstruction()
{
    if( D3DSIO_LRP == m_pCurrInst->m_Type )
    {
        SRCPARAM Src0 = m_pCurrInst->m_SrcParam[0];
        if( !Src0.m_bParamUsed )
            return TRUE;

        switch( Src0.m_SrcMod )
        {
        case D3DSPSM_NONE:
        case D3DSPSM_COMP:
        case D3DSPSM_SAT:
            break;
        default:
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "The only valid modifiers for the first source parameter of LRP are: reg (no mod) or 1-reg (complement)." );
            m_ErrorCount++;
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidDEFInstruction
//
// ** Rule:
// For the DEF instruction, make sure the dest parameter is a valid constant,
// and it has no modifiers.
//
// NOTE that we are pretending this instruction only has a dst parameter.
// We skipped over the 4 source parameters since they are immediate floats,
// for which there is nothing that can be checked.
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidDEFInstruction()
{

    static BOOL s_bDEFInstructionAllowed;

    if( NULL == m_pCurrInst->m_pPrevInst ) // First instruction - initialize static vars
    {
        s_bDEFInstructionAllowed = TRUE;
    }

    if( D3DSIO_COMMENT != m_pCurrInst->m_Type &&
        D3DSIO_DEF     != m_pCurrInst->m_Type )
    {
        s_bDEFInstructionAllowed = FALSE;
    }
    else if( D3DSIO_DEF == m_pCurrInst->m_Type )
    {
        if( !s_bDEFInstructionAllowed )
        {
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Const declaration (DEF) must appear before other instructions." );
            m_ErrorCount++;
        }
        DSTPARAM* pDstParam = &m_pCurrInst->m_DstParam[0];
        if( D3DSP_WRITEMASK_ALL != pDstParam->m_WriteMask ||
            D3DSPDM_NONE != pDstParam->m_DstMod ||
            DSTSHIFT_NONE != pDstParam->m_DstShift ||
            D3DSPR_CONST != pDstParam->m_RegType
            )
        {
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Destination for DEF instruction must be of the form c# (# = reg number, no modifiers)." );
            m_ErrorCount++;
        }

        // Check that the register number is in bounds
        if( D3DSPR_CONST == pDstParam->m_RegType &&
            pDstParam->m_RegNum >= m_pConstRegFile->GetNumRegs() )
        {
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Invalid const register num: %d. Max allowed is %d.",
                        pDstParam->m_RegNum,m_pConstRegFile->GetNumRegs() - 1);
            m_ErrorCount++;

        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidDP3Instruction
//
// ** Rule:
// The .a result write mask is not valid for the DP3 instruction.
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidDP3Instruction()
{
    if( D3DSIO_DP3 == m_pCurrInst->m_Type )
    {
        if( (D3DSP_WRITEMASK_ALL != m_pCurrInst->m_DstParam[0].m_WriteMask)
            && ((D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2) != m_pCurrInst->m_DstParam[0].m_WriteMask ) )
        {
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Dest write mask must be .rgb, or .rgba (all) for DP3." );
            m_ErrorCount++;
        }
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidInstructionPairing
//
// ** Rule:
// - If an instruction is co-issued with another instruction,
// make sure that both do not write to any of RGB at the same time,
// and that neither instruction individually writes to all of RGBA.
//
// - Co-issue can only involve 2 instructions,
// so consecutive instructions cannot have the "+" prefix (D3DSI_COISSUE).
//
// - Co-issue of instructions only applies to pixel blend instructions (non tex-ops).
//
// - The first color blend instruction cannot have "+" (D3DSI_COISSUE) set either.
//
// - NOP may not be used in a co-issue pair.
//
// - DP3 (dot product) always uses the color/vector pipeline (even if it is not writing
// to color components). Thus:
//      - An instruction co-issued with a dot-product can only write to alpha.
//      - A dot-product that writes to alpha cannot be co-issued.
//      - Two dot-products cannot be co-issued.
//
// - For version <= 1.0, coissued instructions must write to the same register.
//
// ------------------
// examples:
//
//      valid pair:             mov r0.a, c0
//                              +add r1.rgb, v1, c1 (note dst reg #'s can be different)
//
//      another valid pair:     mov r0.a, c0
//                              +add r0.rgb, v1, c1
//
//      another valid pair:     dp3 r0.rgb, t1, v1
//                              +mul r0.a, t0, v0
//
//      another valid pair:     mov r0.a, c0
//                              +add r0.a, t0, t1
//
//      invalid pair:           mov r0.rgb, c0
//                              +add r0, t0, t1  (note the dst writes to rgba)
//
//      another invalid pair:   mov r1.rgb, c1
//                              +dp3 r0.a, t0, t1 (dp3 is using up color/vector pipe)
//
// ** When to call:
// Per instruction.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidInstructionPairing()
{
    static BOOL s_bSeenNonTexOp;

    if( NULL == m_pCurrInst->m_pPrevInst )   // First instruction - initialize static vars
    {
        s_bSeenNonTexOp = FALSE;
    }

    if( !s_bSeenNonTexOp && !_CURR_PS_INST->m_bTexOp )
    {
        // first non-tex op.  this cannot have co-issue set.
        if( _CURR_PS_INST->m_bCoIssue )
        {
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                "First pixel blend instruction cannot have co-issue ('+') set; there is no previous pixel blend instruction to pair with.");
            m_ErrorCount++;
        }
        s_bSeenNonTexOp = TRUE;
    }

    if( _CURR_PS_INST->m_bTexOp && _CURR_PS_INST->m_bCoIssue )
    {
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                "Cannot set co-issue ('+') on a texture instruction.  Co-issue only applies to pixel blend instructions." );
        m_ErrorCount++;
    }

    if( _CURR_PS_INST->m_bCoIssue && m_pCurrInst->m_pPrevInst &&
        _PREV_PS_INST->m_bCoIssue )
    {
        // consecutive instructions cannot have co-issue set.
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Cannot set co-issue ('+') on consecutive instructions." );
        m_ErrorCount++;
    }

    if( _CURR_PS_INST->m_bCoIssue && m_pCurrInst->m_pPrevInst &&
        (D3DSIO_NOP == m_pCurrInst->m_pPrevInst->m_Type))
    {
        // NOP cannot be part of co-issue (previous instruction found to be NOP)
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst->m_pPrevInst, "Nop instruction cannot be co-issued." );
        m_ErrorCount++;
    }

    if( _CURR_PS_INST->m_bCoIssue && D3DSIO_NOP == m_pCurrInst->m_Type )
    {
        // NOP cannot be part of co-issue (current instruction found to be NOP)
        Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Nop instruction cannot be co-issued." );
        m_ErrorCount++;
    }

    if( _CURR_PS_INST->m_bCoIssue && !_CURR_PS_INST->m_bTexOp &&
        NULL != m_pCurrInst->m_pPrevInst && !_PREV_PS_INST->m_bTexOp &&
        !_PREV_PS_INST->m_bCoIssue )
    {
        // instruction and previous instruction are candidate for co-issue.
        // ...do further validation...
        DWORD ColorWriteMask = D3DSP_WRITEMASK_0 | D3DSP_WRITEMASK_1 | D3DSP_WRITEMASK_2;
        DWORD CurrInstWriteMask = 0;
        DWORD PrevInstWriteMask = 0;

        if( m_pCurrInst->m_DstParam[0].m_bParamUsed )
            CurrInstWriteMask = m_pCurrInst->m_DstParam[0].m_WriteMask;
        if( m_pCurrInst->m_pPrevInst->m_DstParam[0].m_bParamUsed )
            PrevInstWriteMask = m_pCurrInst->m_pPrevInst->m_DstParam[0].m_WriteMask;

        if( D3DSIO_DP3 == m_pCurrInst->m_Type &&
            D3DSIO_DP3 == m_pCurrInst->m_pPrevInst->m_Type )
        {
            Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                                "Co-issued instructions cannot both be dot-product, since each require use of the color/vector pipeline to execute." );
            m_ErrorCount++;
        }
        else if( D3DSIO_DP3 == m_pCurrInst->m_Type )
        {
            if( ColorWriteMask & PrevInstWriteMask )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst->m_pPrevInst,
                                    "Dot-product needs color/vector pipeline to execute, so instruction co-issued with it cannot write to color components." );
                m_ErrorCount++;
            }
            if( D3DSP_WRITEMASK_3 & CurrInstWriteMask ) // alpha in addition to the implied rgb for dp3
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                                    "Dot-product which writes alpha cannot co-issue, because both alpha and color/vector pipelines used." );
                m_ErrorCount++;
            }
        }
        else if( D3DSIO_DP3 == m_pCurrInst->m_pPrevInst->m_Type )
        {
            if( ColorWriteMask & CurrInstWriteMask )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                                    "Dot-product needs color/vector pipeline to execute, so instruction co-issued with it cannot write to color components." );
                m_ErrorCount++;
            }
            if( D3DSP_WRITEMASK_3 & PrevInstWriteMask ) // alpha in addition to the implied rgb for dp3
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst->m_pPrevInst,
                                    "Dot-product which writes alpha cannot co-issue, because both alpha and color/vector pipelines used by the dot product." );
                m_ErrorCount++;
            }
        }
        else
        {
            if( PrevInstWriteMask == D3DSP_WRITEMASK_ALL )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst->m_pPrevInst,
                                    "Co-issued instruction cannot write all components - must write either alpha or color." );
                m_ErrorCount++;
            }
            if( CurrInstWriteMask == D3DSP_WRITEMASK_ALL )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                                    "Co-issued instruction cannot write all components - must write either alpha or color." );
                m_ErrorCount++;
            }
            if( (m_pCurrInst->m_DstParam[0].m_RegType == m_pCurrInst->m_pPrevInst->m_DstParam[0].m_RegType) &&
                (m_pCurrInst->m_DstParam[0].m_RegNum == m_pCurrInst->m_pPrevInst->m_DstParam[0].m_RegNum) &&
                ((CurrInstWriteMask & PrevInstWriteMask) != 0) )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst,
                    "Co-issued instructions cannot both write to the same components of a register.  Affected components: %s",
                    MakeAffectedComponentsText(CurrInstWriteMask & PrevInstWriteMask)
                    );
                m_ErrorCount++;
            }
            if( (CurrInstWriteMask & ColorWriteMask) && (PrevInstWriteMask & ColorWriteMask) )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Co-issued instructions cannot both write to color components." );
                m_ErrorCount++;
            }
            if( (CurrInstWriteMask & D3DSP_WRITEMASK_3) && (PrevInstWriteMask & D3DSP_WRITEMASK_3) )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Co-issued instructions cannot both write to alpha component." );
                m_ErrorCount++;
            }
        }

        if( m_Version <= D3DPS_VERSION(1,0) )
        {
            // both co-issued instructions must write to the same register number.
            if( m_pCurrInst->m_DstParam[0].m_RegType != m_pCurrInst->m_pPrevInst->m_DstParam[0].m_RegType )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Co-issued instructions must both write to the same register type for pixelshader version <= 1.0." );
                m_ErrorCount++;
            }
            if( (m_pCurrInst->m_DstParam[0].m_RegNum != m_pCurrInst->m_pPrevInst->m_DstParam[0].m_RegNum) )
            {
                Spew( SPEW_INSTRUCTION_ERROR, m_pCurrInst, "Co-issued instructions must both write to the same register number for pixelshader version <= 1.0." );
                m_ErrorCount++;
            }
        }
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_ValidInstructionCount
//
// ** Rule:
// Make sure instruction count for pixel shader version has not been exceeded.
// Separate counts are kept for texture address instructions, for
// pixel blending instructions, and for the total number of instructions.
// Note that the total may not be the sum of texture + pixel instructions.
//
// For version 1.0+, D3DSIO_TEX counts only toward the tex op limit,
// but not towards the total op count.
//
// TEXBEML takes 3 instructions.
//
// Co-issued pixel blending instructions only
// count as one instruction towards the limit.
//
// The def instruction, nop, and comments (already stripped), do not count
// toward any limits.
//
// This rule does not enforce anything in the case of a legacy pixel shader.
//
// ** When to call:
// Per instruction AND after all instructions seen.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_ValidInstructionCount()
{
    static UINT s_MaxTexOpCount;
    static UINT s_MaxBlendOpCount;
    static UINT s_MaxTotalOpCount;
    static BOOL s_bRuleDisabled;

    if( NULL == m_pCurrInst )
        return TRUE;

    if(( NULL == m_pCurrInst->m_pPrevInst ) &&
       ( FALSE == m_bSeenAllInstructions ))    // First instruction - initialize static vars
    {
        m_TexOpCount = 0;
        m_BlendOpCount = 0;
        s_bRuleDisabled = FALSE;

        switch(m_Version)
        {
        case D3DPS_VERSION(0x254,0x254):  // Legacy shader
            s_bRuleDisabled = TRUE;
            break;
        case D3DPS_VERSION(1,0):    // DX7.5
            s_MaxTexOpCount   = 4;
            s_MaxBlendOpCount = 8;
            s_MaxTotalOpCount = 8;
            break;
        default:
        case D3DPS_VERSION(1,1):    // DX8.0
            s_MaxTexOpCount   = 4;
            s_MaxBlendOpCount = 8;
            s_MaxTotalOpCount = 12;
            break;
        case D3DPS_VERSION(255,255):
            s_MaxTexOpCount   = 16;
            s_MaxBlendOpCount = 32;
            s_MaxTotalOpCount = 48;
            break;
        }
    }

    if( s_bRuleDisabled )
        return TRUE;

    if( m_bSeenAllInstructions )
    {
        if( m_TexOpCount > s_MaxTexOpCount )
        {
            Spew( SPEW_GLOBAL_ERROR, NULL, "Too many texture addressing instruction slots used: %d. Max. allowed is %d. (Note that some texture addressing instructions may use up more than one instruction slot)",
                  m_TexOpCount, s_MaxTexOpCount);
            m_ErrorCount++;
        }
        if( m_BlendOpCount > s_MaxBlendOpCount )
        {
            Spew( SPEW_GLOBAL_ERROR, NULL, "Too many pixel blending instruction slots used: %d. Max. allowed (counting any co-issued instructions as 1) is %d.",
                  m_BlendOpCount, s_MaxBlendOpCount);
            m_ErrorCount++;
        }
        if( !(m_TexOpCount > s_MaxTexOpCount && m_BlendOpCount > s_MaxBlendOpCount) // not already spewed avove 2 errors
            && (m_TotalOpCount > s_MaxTotalOpCount) )
        {
            Spew( SPEW_GLOBAL_ERROR, NULL, "Total number of instruction slots used too high: %d. Max. allowed (counting any co-issued instructions as 1) is %d.",
                  m_TotalOpCount, s_MaxTotalOpCount);
            m_ErrorCount++;
        }
        if(m_BlendOpCount == 0)
        {
            Spew( SPEW_GLOBAL_ERROR, NULL, "At least one pixel blending instruction must be used in addition to XFC.");
            m_ErrorCount++;
        }
        return TRUE;
    }

    switch(m_pCurrInst->m_Type)
    {
    case D3DSIO_TEXBEML:
    case D3DSIO_TEXBEM:
    case D3DSIO_TEX:
    case D3DSIO_TEXBEM_LEGACY:
    case D3DSIO_TEXBEML_LEGACY:
    case D3DSIO_TEXCOORD:
    case D3DSIO_TEXM3x2PAD:
    case D3DSIO_TEXM3x2TEX:
    case D3DSIO_TEXM3x3PAD:
    case D3DSIO_TEXM3x3TEX:
    case D3DSIO_TEXM3x3DIFF:
    case D3DSIO_TEXM3x3SPEC:
    case D3DSIO_TEXM3x3VSPEC:
    case D3DSIO_TEXREG2AR:
    case D3DSIO_TEXREG2GB:
    case D3DSIO_TEXKILL:
    case D3DSIO_TEXM3x2DEPTH:
    case D3DSIO_TEXBRDF:
#if 0 // Not on XBox
    case D3DSIO_TEXDP3:
    case D3DSIO_TEXREG2RGB:
#endif
        m_TexOpCount++;
        m_TotalOpCount++;
        break;
    case D3DSIO_MOV:
    case D3DSIO_ADD:
    case D3DSIO_SUB:
    case D3DSIO_MUL:
    case D3DSIO_MAD:
    case D3DSIO_LRP:
    case D3DSIO_DP3:
    case D3DSIO_CND:
    case D3DSIO_END:
    case D3DSIO_DP4:
    case D3DSIO_XMMA:
    case D3DSIO_XMMC:
    case D3DSIO_XDM:
    case D3DSIO_XDD:
        if( !_CURR_PS_INST->m_bCoIssue )
        {
            m_BlendOpCount++;
            m_TotalOpCount++;
        }
        break;
    case D3DSIO_NOP:
    case D3DSIO_DEF:
    case D3DSIO_XFC:
        break;
    default:
        DXGASSERT(FALSE);
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
// CPShaderValidator::Rule_R0Written
//
// ** Rule:
// All components (r,g,b,a) of register R0 must have been written by the
// pixel shader.
//
// ** When to call:
// After all instructions have been seen.
//
// ** Returns:
// Always TRUE.
//
//-----------------------------------------------------------------------------
BOOL CPShaderValidator::Rule_R0Written()
{
    UINT  NumUninitializedComponents    = 0;
    DWORD UninitializedComponentsMask   = 0;

    for( UINT i = 0; i < NUM_COMPONENTS_IN_REGISTER; i++ )
    {
        if( (NULL == m_pTempRegFile->m_pAccessHistory[i][0].m_pMostRecentWriter ) &&
            (!m_pTempRegFile->m_pAccessHistory[i][0].m_bPreShaderInitialized) )
        {
            NumUninitializedComponents++;
            UninitializedComponentsMask |= COMPONENT_MASKS[i];
        }
    }
    if( NumUninitializedComponents )
    {
        Spew( SPEW_GLOBAL_ERROR, NULL, "r0 must be written by shader. Uninitialized component%s(*): %s",
            NumUninitializedComponents > 1 ? "s" : "", MakeAffectedComponentsText(UninitializedComponentsMask,TRUE,FALSE));
        m_ErrorCount++;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
//
// CPShaderValidator Wrapper Functions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ValidatePixelShaderInternal
//-----------------------------------------------------------------------------
BOOL ValidatePixelShaderInternal( const DWORD* pCode, const D3DCAPS8* pCaps, XD3DXErrorLog* pLog )
{
    CPShaderValidator Validator(pCode,pCaps, 0, pLog);
    return SUCCEEDED(Validator.GetStatus()) ? TRUE : FALSE;
}

//-----------------------------------------------------------------------------
// ValidatePixelShader
//
//-----------------------------------------------------------------------------
HRESULT WINAPI ValidatePixelShader( const DWORD* pCode,
                                    const D3DCAPS8* pCaps,
                                    const DWORD Flags,
                                    XD3DXErrorLog* pLog)
{
    CPShaderValidator Validator(pCode,pCaps,Flags, pLog);
#if 0
    if( ppBuf )
    {
        *ppBuf = (char*)HeapAlloc(GetProcessHeap(), 0, Validator.GetRequiredLogBufferSize());
        if( NULL == *ppBuf )
            OutputDebugString("Out of memory.\n");
        else
            Validator.WriteLogToBuffer(*ppBuf);
    }
#endif
    return Validator.GetStatus();
}

} // namespace XGRAPHICS
