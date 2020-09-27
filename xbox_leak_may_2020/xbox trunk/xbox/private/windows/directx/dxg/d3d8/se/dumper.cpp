/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dumper.cpp
 *  Content:    Debug help functions for dumping and viewing the push buffer 
 *              contents and current chip state.
 *
 ***************************************************************************/
 
#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#if DBG

//------------------------------------------------------------------------------
// g_ParseState, g_ParsePut
//
// Debug-only allocation for recording all of our state sets:

DWORD g_ParseState[65536 / 4];

// Keeps track of whether a method was modified:

BOOL g_Modified[65536 / 4];

// Debug-only global for tracking our last parse point:

PPUSH g_ParsePut;

// Keeps a copy of the loaded vertex shader programs:

DWORD g_TransformProgram[4*136];

// Current loading index:

DWORD g_TransformProgramLoad;

// Keeps a copy of the loaded vertex shader constants:

DWORD g_TransformConstant[4*192];

// Current loading index:

DWORD g_TransformConstantLoad;

// The next jump is a sub-routine call, ignore it for now:

BOOL g_IgnorePushBufferJump;

//------------------------------------------------------------------------------
// ParseMethodWrite

VOID ParseMethodWrite(
    DWORD Method,   // With sub-channel encoded, but not count of dwords
    PPUSH pPush,   // Write-combined memory, so expensive to read
    DWORD Count)
{
    DWORD i;

    // All valid offsets are a multiple of 4:

    ASSERT((Method & 3) == 0);

    // Get the last value written to this register:

    DWORD lastValue = *(pPush + Count - 1);

    // Mark that the Method has been written to:

    g_Modified[Method >> 2] = TRUE;

    // Record the last value written to the register:

    g_ParseState[Method >> 2] = lastValue;

    // Catch a particular write to register 0x1b70:

    // if ((Method == PUSHER_METHOD(SUBCH_3D, 0x1b70, 0)) && 
    //     ((lastValue & 0xffff0000) == 0xdead0000)) 
    //     _asm int 3; 

    // Watch for special cases:

    if (Method == PUSHER_METHOD(SUBCH_3D, NV097_NO_OPERATION, 0))
    {
        // For now, we don't parse called push-buffers.

        if ((*pPush == NVX_PUSH_BUFFER_RUN) || (*pPush == NVX_PUSH_BUFFER_FIXUP))
            g_IgnorePushBufferJump = TRUE;
    }
    else if (Method == PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0))
    {
        // The previous load should have done a multiple of 4:

        ASSERT((g_TransformProgramLoad & 3) == 0);

        g_TransformProgramLoad = 4*lastValue;
    }
    else if (Method == PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_CONSTANT_LOAD, 0))
    {
        // The previous load should have done a multiple of 4:

        ASSERT((g_TransformConstantLoad & 3) == 0);

        g_TransformConstantLoad = 4*lastValue;
    }
    else if ((Method >= PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_PROGRAM(0), 0)) &&
             (Method < PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_PROGRAM(32), 0)))
    {
        for (i = 0; i < Count; i++)
        {
            g_TransformProgram[g_TransformProgramLoad++] = *pPush++;

            ASSERT(g_TransformProgramLoad <= sizeof(g_TransformProgram) / 4);
        }
    }
    else if ((Method >= PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_CONSTANT(0), 0)) &&
             (Method < PUSHER_METHOD(SUBCH_3D, NV097_SET_TRANSFORM_CONSTANT(32), 0)))
    {
        for (i = 0; i < Count; i++)
        {
            g_TransformConstant[g_TransformConstantLoad++] = *pPush++;

            ASSERT(g_TransformConstantLoad <= sizeof(g_TransformConstant) / 4);
        }
    }
}

//------------------------------------------------------------------------------
// PARSE_PUSH_BUFFER()
//
// Here we parse the command stream to build our global structure that tracks
// the hardware's current state.

VOID PARSE_PUSH_BUFFER()
{
    CDevice* pDevice = g_pDevice;

    // Reading from write-combined memory is painful, so the dumper is not
    // enabled by default:

    if (!D3D__Parser)
        return;

    // The parser doesn't understand about NULL hardware:

    if (D3D__NullHardware)
        return;

    // When we're recording push-buffer data, we don't parse the push-buffer
    // (the parsing will automatically happen when they do RunPushBuffer):

    if (pDevice->m_StateFlags & STATE_RECORDPUSHBUFFER)
        return;

    PPUSH pPush = g_ParsePut;

    // Catch up from our old parse point to our new:

    while (pPush != pDevice->m_Pusher.m_pPut)
    {
        DWORD push = *pPush;
        DWORD instruction = push >> 29;
        DWORD count = (push & 0x1ffc0000) >> 18;
        DWORD method = push & 0xffff;   // Note we include sub-channel
        DWORD jumpOffset = push & 0x1ffffffc;

        // Advance:

        pPush++;

        // Long jump or call

        if (((push & 3) == 1) || ((push & 3) == 2))
        {
            if (g_IgnorePushBufferJump)
            {
                // Skip over pushbuffer fixup info

                pPush += sizeof(CMiniport::PUSHBUFFERFIXUPINFO) / sizeof(DWORD);

                g_IgnorePushBufferJump = FALSE;
            }
            else
            {
                pPush = (DWORD *)GetWriteCombinedAddress(push & ~3);
            }
        }

        // Parse the instruction:

        else if (instruction == 0) // Immediate increment
        {
            while (count-- != 0)
            {
                ParseMethodWrite(method, pPush, 1);

                pPush++;
                method += 4;
            }
        }
        else if (instruction == 1) // Jump short instruction
        {
            pPush = pDevice->m_pPushBase + (jumpOffset >> 2);
        }
        else if (instruction == 2) // Immediate nonincrement
        {
            ParseMethodWrite(method, pPush, count);

            pPush += count;
        }
        else
        {
            DXGRIP("Unexpected push-buffer instruction: 0x%lx  at: 0x%lx", push, pPush - 1);
        }
    }

    // Remember the new push-buffer location:

    g_ParsePut = pDevice->m_Pusher.m_pPut;
}

//------------------------------------------------------------------------------
// BitField
//
// Given a dword and a bitfield range like 5:2, returns the numeric value.

DWORD BitField(
    DWORD Value,
    DWORD HighBit,
    DWORD LowBit)
{
    if (HighBit != 31)
    {
        Value &= ((1 << (HighBit + 1)) - 1);
    }

    Value >>= LowBit;

    return Value;
}

//------------------------------------------------------------------------------
// PrintFloat
//
// Hack-around to remove our sprintf CRT depedency, since libcntpr.lib 
// (currently) does not support floats with printf.

VOID PrintFloat(
    float* f)
{
    DWORD scaled = (DWORD)FloatToLong(*f * 1000.0f);

    DbgPrint("%li.%03li", scaled / 1000, scaled % 1000);
}

//------------------------------------------------------------------------------
// ShowDump()
//
// Show a dump of the current register state.

VOID ShowDump(
    DWORD subch)
{
    DWORD method;
    DWORD fieldCount;
    DWORD i;
    DWORD j;
    DWORD* pState;
    BOOL* pModified;
    FieldDescription* pField;
    FieldDescription* pFieldLimit;
    FieldDescription* pThis;
    BOOL showIt;

    if (subch == SUBCH_3D)
    {
        pField = (FieldDescription*)g_Kelvin;
        pFieldLimit = pField + ((sizeof(g_Kelvin) / sizeof(g_Kelvin[0])) - 1);

        pState = &g_ParseState[(subch << 13) >> 2];
        pModified = &g_Modified[(subch << 13) >> 2];
    }
    else
    {
        DbgPrint("No field description for subchannel %li", subch);
        return;
    }

    DbgPrint("-----------------------------------------------------------\n");

    // Loop through the description array in order:

    while (pField < pFieldLimit)
    {
        ASSERT((pField->ReplicationCount[0] != 0) &&
               (pField->ReplicationCount[1] != 0));

        showIt = FALSE;
        for (i = 0; i < pField->ReplicationCount[0]; i++)
        {
            for (j = 0; j < pField->ReplicationCount[1]; j++)
            {
                method = pField->Method
                       + i * pField->ReplicationStride[0]
                       + j * pField->ReplicationStride[1];

                if (pState[method >> 2] != 0)
                    showIt = TRUE;
            }
        }

        // Don't show fields that have zero values, or any fields with
        // capital letter 'types' (which denote read-only fields):

        if ((showIt) &&
            !((pField->FieldType >= 'A') && (pField->FieldType <= 'Z')))
        {
            // Handle replications specially:

            if (pField->ReplicationCount[0] > 1)
            {
                // Handle 1-d replications of dword values specially:

                if ((pField->ReplicationCount[1] <= 1) &&
                    (pField->ReplicationCount[0] > 1) &&
                    (pField->FieldHighBit == 31) &&
                    (pField->FieldLowBit == 0))
                {
                    // 0x400 - SET_MODEL_VIEW_MATRIX0
                    //     Zero:    0.0    1.0   2.0   3.0

                    DbgPrint("0x%lx - %s\n    %-20s:\t",
                             pField->Method,
                             pField->MethodName,
                             pField->FieldName);
                                            
                    for (i = 0; i < pField->ReplicationCount[0]; i++)
                    {
                        // Insert a return to make matrices appear 4x4:

                        if ((i > 0) && 
                            ((i & 3) == 0) && 
                            (pField->ReplicationCount[0] == 16))
                        {
                            DbgPrint("\n\t\t\t\t");
                        }

                        method = pField->Method 
                               + i * pField->ReplicationStride[0];

                        if (pField->FieldType == 'f')
                        {
                            PrintFloat((float*) &pState[method >> 2]);
                            DbgPrint(" \t");
                        }
                        else
                        {
                            DbgPrint("0x%lx\t", pState[method >> 2]);
                        }
                    }

                    DbgPrint("\n");
                }
                else
                {
                    for (i = 0; i < pField->ReplicationCount[0]; i++)
                    {
                        // Handle all 2-d replications specially.
                        // We assume there's no bitfields stuff in 2-d 
                        // replications.
    
                        if (pField->ReplicationCount[1] > 1)
                        {
                            ASSERT((pField->FieldHighBit == 31) &&
                                   (pField->FieldLowBit == 0));

                            // 0x800 - SET_LIGHT_AMBIENT_COLOR(3)
                            //     Zero:   0.0    0.0    0.0   0.0
        
                            DbgPrint("0x%lx - %s(%li)\n    %-20s:\t",
                                     pField->Method,
                                     pField->MethodName,
                                     i,
                                     pField->FieldName);
            
                            for (j = 0; j < pField->ReplicationCount[1]; j++)
                            {
                                // Insert a return to make matrices appear 4x4:
        
                                if ((j > 0) && 
                                    ((j & 3) == 0) && 
                                    (pField->ReplicationCount[1] == 16))
                                {
                                    DbgPrint("\n\t\t\t\t");
                                }
        
                                method = pField->Method 
                                       + i * pField->ReplicationStride[0]
                                       + j * pField->ReplicationStride[1];
        
                                if (pField->FieldType == 'f')
                                {
                                    PrintFloat((float*) &pState[method >> 2]);
                                    DbgPrint("\t ");
                                }
                                else
                                {
                                    DbgPrint("0x%lx\t", pState[method >> 2]);
                                }
                            }
        
                            DbgPrint("\n");
                        }
                        else
                        {
                            // 0x218 - SET_TEXTURE_FORMAT(2)
                            //     CONTEXT_DMA:     0x0
                            //     ENABLE:          0x0

                            method = pField->Method 
                                   + i * pField->ReplicationStride[0];

                            DbgPrint("0x%lx - %s(%li)\t\t\t0x%lx\n",
                                     pField->Method,
                                     pField->MethodName,
                                     i,
                                     pState[method >> 2]);

                            for (pThis = pField; 
                                 pThis->Method == pField->Method;
                                 pThis++)
                            {
                                DbgPrint("    %-20s:\t0x%lx\n",
                                         pThis->FieldName,
                                         BitField(pState[method >> 2],
                                                  pThis->FieldHighBit,
                                                  pThis->FieldLowBit));
                            }
                        }
                    }
                }
            }
            else
            {
                // 0x200 - SET_SURFACE_CLIP_HORIZONTAL 
                //     X:   0x0
                //     WIDTH:   0x1e0

                method = pField->Method;

                DbgPrint("0x%lx - %s\t\t\t0x%lx\n",
                         pField->Method,
                         pField->MethodName,
                         pState[method >> 2]);

                if (pField->FieldType == 'f')
                {
                    DbgPrint("    %-20s:\t",
                             pField->FieldName);
                    PrintFloat((float*) &pState[method >> 2]);
                    DbgPrint("\n");
                }
                else
                {
                    for (pThis = pField; 
                         pThis->Method == pField->Method;
                         pThis++)
                    {
                        DbgPrint("    %-20s:\t0x%lx\n",
                                 pThis->FieldName,
                                 BitField(pState[method >> 2],
                                          pThis->FieldHighBit,
                                          pThis->FieldLowBit));
                    }
                }
            }
        }

        // Advance and skip over any multiple bitfields for this method 
        // (since we already handled them above):

        pThis = pField;
        do {
            pField++;
        } while (pField->Method == pThis->Method);
    }
}

//------------------------------------------------------------------------------
// Like DRF_VAL, but without the dorky separate parameter requirement:

#define HW_VAL(Reg, Field, Val) (((Val)>>DRF_SHIFT(NV097##_##Reg##_## Field)) & \
    DRF_MASK(NV097##_##Reg##_##Field))

// Read back from the register state:

#define HW(Reg, Field) \
    HW_VAL(Reg, Field, g_ParseState[(NV097##_##Reg >> 2) + SUBCH_3D*2048])
    
#define HWx(x, Reg, Field) \
    HW_VAL(Reg, Field, g_ParseState[(NV097##_##Reg##(##x##) >> 2) + SUBCH_3D*2048])

//------------------------------------------------------------------------------
// PRIMITIVE_LAUNCH_STATE_TEST()
//
// Performs a test identical to that which the hardware does when it launches
// a primitive.  By doing this in software here, we can easily figure out
// what the exact problem is...

VOID PRIMITIVE_LAUNCH_STATE_TEST()
{
    // If the dumper is not enabled, we can't check the state:

    if (!D3D__Parser)
        return;

    // Make sure we've parsed everything added so far so that g_ParseState
    // is current:

    PARSE_PUSH_BUFFER();

    if ((((HW(SET_BLEND_ENABLE, V))) == NV097_SET_BLEND_ENABLE_V_TRUE) &&
        ((((HW(SET_BLEND_EQUATION, V))) == NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT_SIGNED) ||
         (((HW(SET_BLEND_EQUATION, V))) == NV097_SET_BLEND_EQUATION_V_FUNC_ADD_SIGNED)) &&
        ((((HW(SET_SURFACE_FORMAT, COLOR))) == NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5) ||
         (((HW(SET_SURFACE_FORMAT, COLOR))) == NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5) ||
         (((HW(SET_SURFACE_FORMAT, COLOR))) == NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5) ||
         (((HW(SET_SURFACE_FORMAT, COLOR))) == NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8) ||
         (((HW(SET_SURFACE_FORMAT, COLOR))) == NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8)))
    {
        DXGRIP("Combination of signed SetBlendEquation and certain color formats is not allowed");
    }

    if ((((HWx(0, SET_TEXTURE_CONTROL0, LOG_MAX_ANISO)) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
        ((((HWx(0, SET_TEXTURE_FORMAT, BASE_SIZE_P))) > NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P_1) ||
         (((HWx(0, SET_TEXTURE_FORMAT, DIMENSIONALITY))) == NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_THREE))) &&
        (((HWx(0, SET_TEXTURE_CONTROL0, ENABLE)))))
    {
        DXGRIP("No aniso support for 3D textures");
    }

    if ((((HWx(1, SET_TEXTURE_CONTROL0, LOG_MAX_ANISO)) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
        ((((HWx(1, SET_TEXTURE_FORMAT, BASE_SIZE_P))) > NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P_1) ||
         (((HWx(1, SET_TEXTURE_FORMAT, DIMENSIONALITY))) == NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_THREE))) &&
        (((HWx(1, SET_TEXTURE_CONTROL0, ENABLE)))))
    {
        DXGRIP("No aniso support for 3D textures");
    }

    if ((((HWx(2, SET_TEXTURE_CONTROL0, LOG_MAX_ANISO)) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
        ((((HWx(2, SET_TEXTURE_FORMAT, BASE_SIZE_P))) > NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P_1) ||
         (((HWx(2, SET_TEXTURE_FORMAT, DIMENSIONALITY))) == NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_THREE))) &&
        (((HWx(2, SET_TEXTURE_CONTROL0, ENABLE)))))
    {
        DXGRIP("No aniso support for 3D textures");
    }

    if ((((HWx(3, SET_TEXTURE_CONTROL0, LOG_MAX_ANISO)) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
        ((((HWx(3, SET_TEXTURE_FORMAT, BASE_SIZE_P))) > NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P_1) ||
         (((HWx(3, SET_TEXTURE_FORMAT, DIMENSIONALITY))) == NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_THREE))) &&
        (((HWx(3, SET_TEXTURE_CONTROL0, ENABLE)))))
    {
        DXGRIP("No aniso support for 3D textures");
    }

    if ((((HWx(0, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
        (((HWx(0, SET_TEXTURE_IMAGE_RECT, HEIGHT))) & 0x1) &&
        (((HWx(0, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
    {
        DXGRIP("Invalid height with field enable");
    }

    if ((((HWx(1, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
        (((HWx(1, SET_TEXTURE_IMAGE_RECT, HEIGHT))) & 0x1) &&
        (((HWx(1, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
    {
        DXGRIP("Invalid height with field enable");
    }

    if ((((HWx(2, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
        (((HWx(2, SET_TEXTURE_IMAGE_RECT, HEIGHT))) & 0x1) &&
        (((HWx(2, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
    {
        DXGRIP("Invalid height with field enable");
    }

    if ((((HWx(3, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
        (((HWx(3, SET_TEXTURE_IMAGE_RECT, HEIGHT))) & 0x1) &&
        (((HWx(3, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
    {
        DXGRIP("Invalid height with field enable");
    }

    if ((((HWx(0, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
        (((HWx(0, SET_TEXTURE_FORMAT, BORDER_SOURCE))) != NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR) &&
        (((HWx(0, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
    {
        DXGRIP("Can't use ImageFieldEnable with border data in the texture");
    }

    if ((((HWx(1, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
        (((HWx(1, SET_TEXTURE_FORMAT, BORDER_SOURCE))) != NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR) &&
        (((HWx(1, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
    {
        DXGRIP("Can't use ImageFieldEnable with border data in the texture");
    }

    if ((((HWx(2, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
        (((HWx(2, SET_TEXTURE_FORMAT, BORDER_SOURCE))) != NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR) &&
        (((HWx(2, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
    {
        DXGRIP("Can't use ImageFieldEnable with border data in the texture");
    }

    if ((((HWx(3, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
        (((HWx(3, SET_TEXTURE_FORMAT, BORDER_SOURCE))) != NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR) &&
        (((HWx(3, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
    {
        DXGRIP("Can't use ImageFieldEnable with border data in the texture");
    }

    if ((((HWx(0, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE)))) &&
        (((HWx(0, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        (! ((((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8))))
    {
        DXGRIP("Can't use ImageFieldEnable unless texture is a linear format");
    }

    if ((((HWx(0, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE)))) &&
        (((HWx(0, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        (! ((((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
          (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8))))
    {
        DXGRIP("Can't use ImageFieldEnable unless texture is a linear format");
    }

    if ((((HWx(1, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE)))) &&
        (((HWx(1, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        (! ((((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
          (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8))))
    {
        DXGRIP("Can't use ImageFieldEnable unless texture is a linear format");
    }

    if ((((HWx(2, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE)))) &&
        (((HWx(2, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        (! ((((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
          (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8))))
    {
        DXGRIP("Can't use ImageFieldEnable unless texture is a linear format");
    }

    if ((((HWx(3, SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE)))) &&
        (((HWx(3, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        (! ((((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
          (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8))))
    {
        DXGRIP("Can't use ImageFieldEnable unless texture is a linear format");
    }

    if ((((HWx(0, SET_TEXTURE_CONTROL0, LOG_MAX_ANISO))) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
        ((((HWx(0, SET_TEXTURE_FILTER, MIN))) == NV097_SET_TEXTURE_FILTER_MIN_CONVOLUTION_2D_LOD0) ||
         (((HWx(0, SET_TEXTURE_FILTER, MAG))) == NV097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0)) &&
        (((HWx(0, SET_TEXTURE_CONTROL0, ENABLE)))))
    {
        DXGRIP("No aniso support for convolution filter");
    }

    if ((((HWx(1, SET_TEXTURE_CONTROL0, LOG_MAX_ANISO))) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
        ((((HWx(1, SET_TEXTURE_FILTER, MIN))) == NV097_SET_TEXTURE_FILTER_MIN_CONVOLUTION_2D_LOD0) ||
         (((HWx(1, SET_TEXTURE_FILTER, MAG))) == NV097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0)) &&
        (((HWx(1, SET_TEXTURE_CONTROL0, ENABLE)))))
    {
        DXGRIP("No aniso support for convolution filter");
    }

    if ((((HWx(2, SET_TEXTURE_CONTROL0, LOG_MAX_ANISO))) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
        ((((HWx(2, SET_TEXTURE_FILTER, MIN))) == NV097_SET_TEXTURE_FILTER_MIN_CONVOLUTION_2D_LOD0) ||
         (((HWx(2, SET_TEXTURE_FILTER, MAG))) == NV097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0)) &&
        (((HWx(2, SET_TEXTURE_CONTROL0, ENABLE)))))
    {
        DXGRIP("No aniso support for convolution filter");
    }

    if ((((HWx(3, SET_TEXTURE_CONTROL0, LOG_MAX_ANISO))) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
        ((((HWx(3, SET_TEXTURE_FILTER, MIN))) == NV097_SET_TEXTURE_FILTER_MIN_CONVOLUTION_2D_LOD0) ||
         (((HWx(3, SET_TEXTURE_FILTER, MAG))) == NV097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0)) &&
        (((HWx(3, SET_TEXTURE_CONTROL0, ENABLE)))))
    {
        DXGRIP("No aniso support for convolution filter");
    }

    if (((((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT1_A1R5G5B5) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT23_A8R8G8B8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT45_A8R8G8B8)) &&
        (((HWx(0, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        ((((HWx(0, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_ALPHA) ||
         (((HWx(0, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_RGBA) ||
         (((HWx(0, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_KILL)))
    {
        DXGRIP("No color-key support for compressed texture formats");
    }

    if (((((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT1_A1R5G5B5) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT23_A8R8G8B8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT45_A8R8G8B8)) &&
        (((HWx(1, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        ((((HWx(1, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_ALPHA) ||
         (((HWx(1, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_RGBA) ||
         (((HWx(1, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_KILL)))
    {
        DXGRIP("No color-key support for compressed texture formats");
    }

    if (((((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT1_A1R5G5B5) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT23_A8R8G8B8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT45_A8R8G8B8)) &&
        (((HWx(2, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        ((((HWx(2, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_ALPHA) ||
         (((HWx(2, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_RGBA) ||
         (((HWx(2, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_KILL)))
    {
        DXGRIP("No color-key support for compressed texture formats");
    }

    if (((((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT1_A1R5G5B5) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT23_A8R8G8B8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT45_A8R8G8B8)) &&
        (((HWx(3, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        ((((HWx(3, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_ALPHA) ||
         (((HWx(3, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_RGBA) ||
         (((HWx(3, SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_KILL)))
    {
        DXGRIP("No color-key support for compressed texture formats");
    }

    if (((((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
         (((HWx(0, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8)) &&
        (((HWx(0, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        ((((HWx(0, SET_TEXTURE_ADDRESS, U))) == NV097_SET_TEXTURE_ADDRESS_U_WRAP) || 
         (((HWx(0, SET_TEXTURE_ADDRESS, U))) == NV097_SET_TEXTURE_ADDRESS_U_MIRROR) ||
         (((HWx(0, SET_TEXTURE_ADDRESS, V))) == NV097_SET_TEXTURE_ADDRESS_V_WRAP) || 
         (((HWx(0, SET_TEXTURE_ADDRESS, V))) == NV097_SET_TEXTURE_ADDRESS_V_MIRROR)))
    {
        DXGRIP("No wrap or mirror if texture is a linear format");
    }

    if (((((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
         (((HWx(1, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8)) &&
        (((HWx(1, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        ((((HWx(1, SET_TEXTURE_ADDRESS, U))) == NV097_SET_TEXTURE_ADDRESS_U_WRAP) || 
         (((HWx(1, SET_TEXTURE_ADDRESS, U))) == NV097_SET_TEXTURE_ADDRESS_U_MIRROR) ||
         (((HWx(1, SET_TEXTURE_ADDRESS, V))) == NV097_SET_TEXTURE_ADDRESS_V_WRAP) || 
         (((HWx(1, SET_TEXTURE_ADDRESS, V))) == NV097_SET_TEXTURE_ADDRESS_V_MIRROR)))
    {
        DXGRIP("No wrap or mirror if texture is a linear format");
    }

    if (((((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
         (((HWx(2, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8)) &&
        (((HWx(2, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        ((((HWx(2, SET_TEXTURE_ADDRESS, U))) == NV097_SET_TEXTURE_ADDRESS_U_WRAP) || 
         (((HWx(2, SET_TEXTURE_ADDRESS, U))) == NV097_SET_TEXTURE_ADDRESS_U_MIRROR) ||
         (((HWx(2, SET_TEXTURE_ADDRESS, V))) == NV097_SET_TEXTURE_ADDRESS_V_WRAP) || 
         (((HWx(2, SET_TEXTURE_ADDRESS, V))) == NV097_SET_TEXTURE_ADDRESS_V_MIRROR)))
    {
        DXGRIP("No wrap or mirror if texture is a linear format");
    }

    if (((((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
         (((HWx(3, SET_TEXTURE_FORMAT, COLOR))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8)) &&
        (((HWx(3, SET_TEXTURE_CONTROL0, ENABLE)))) &&
        ((((HWx(3, SET_TEXTURE_ADDRESS, U))) == NV097_SET_TEXTURE_ADDRESS_U_WRAP) || 
         (((HWx(3, SET_TEXTURE_ADDRESS, U))) == NV097_SET_TEXTURE_ADDRESS_U_MIRROR) ||
         (((HWx(3, SET_TEXTURE_ADDRESS, V))) == NV097_SET_TEXTURE_ADDRESS_V_WRAP) || 
         (((HWx(3, SET_TEXTURE_ADDRESS, V))) == NV097_SET_TEXTURE_ADDRESS_V_MIRROR)))
    {
        DXGRIP("No wrap or mirror if texture is a linear format");
    }

    if (((((HWx(0, SET_COMBINER_COLOR_OCW, BLUETOALPHA_AB))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE) &&
         (((HWx(0, SET_COMBINER_ALPHA_OCW, AB_DST))) > NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0) &&
         (((HWx(0, SET_COMBINER_ALPHA_OCW, AB_DST))) == ((HWx(0, SET_COMBINER_COLOR_OCW, AB_DST))))) ||
        ((((HWx(0, SET_COMBINER_COLOR_OCW, BLUETOALPHA_CD))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE) &&
         (((HWx(0, SET_COMBINER_ALPHA_OCW, CD_DST))) > NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0) &&
         (((HWx(0, SET_COMBINER_ALPHA_OCW, CD_DST))) == ((HWx(0, SET_COMBINER_COLOR_OCW, CD_DST))))))
    {
        DXGRIP("Can't have Color OCW combiner and Alpha OCW combiner going to same destination");
    }

    if (((((HWx(1, SET_COMBINER_COLOR_OCW, BLUETOALPHA_AB))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE) &&
         (((HWx(1, SET_COMBINER_ALPHA_OCW, AB_DST))) > NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0) &&
         (((HWx(1, SET_COMBINER_ALPHA_OCW, AB_DST))) == ((HWx(1, SET_COMBINER_COLOR_OCW, AB_DST))))) ||
        ((((HWx(1, SET_COMBINER_COLOR_OCW, BLUETOALPHA_CD))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE) &&
         (((HWx(1, SET_COMBINER_ALPHA_OCW, CD_DST))) > NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0) &&
         (((HWx(1, SET_COMBINER_ALPHA_OCW, CD_DST))) == ((HWx(1, SET_COMBINER_COLOR_OCW, CD_DST))))))
    {
        DXGRIP("Can't have Color OCW combiner and Alpha OCW combiner going to same destination");
    }

    if (((((HWx(2, SET_COMBINER_COLOR_OCW, BLUETOALPHA_AB))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE) &&
         (((HWx(2, SET_COMBINER_ALPHA_OCW, AB_DST))) > NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0) &&
         (((HWx(2, SET_COMBINER_ALPHA_OCW, AB_DST))) == ((HWx(2, SET_COMBINER_COLOR_OCW, AB_DST))))) ||
        ((((HWx(2, SET_COMBINER_COLOR_OCW, BLUETOALPHA_CD))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE) &&
         (((HWx(2, SET_COMBINER_ALPHA_OCW, CD_DST))) > NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0) &&
         (((HWx(2, SET_COMBINER_ALPHA_OCW, CD_DST))) == ((HWx(2, SET_COMBINER_COLOR_OCW, CD_DST))))))
    {
        DXGRIP("Can't have Color OCW combiner and Alpha OCW combiner going to same destination");
    }

    if (((((HWx(3, SET_COMBINER_COLOR_OCW, BLUETOALPHA_AB))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE) &&
         (((HWx(3, SET_COMBINER_ALPHA_OCW, AB_DST))) > NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0) &&
         (((HWx(3, SET_COMBINER_ALPHA_OCW, AB_DST))) == ((HWx(3, SET_COMBINER_COLOR_OCW, AB_DST))))) ||
        ((((HWx(3, SET_COMBINER_COLOR_OCW, BLUETOALPHA_CD))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE) &&
         (((HWx(3, SET_COMBINER_ALPHA_OCW, CD_DST))) > NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0) &&
         (((HWx(3, SET_COMBINER_ALPHA_OCW, CD_DST))) == ((HWx(3, SET_COMBINER_COLOR_OCW, CD_DST))))))
    {
        DXGRIP("Can't have Color OCW combiner and Alpha OCW combiner going to same destination");
    }

    if (((((HWx(4, SET_COMBINER_COLOR_OCW, BLUETOALPHA_AB))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE) &&
         (((HWx(4, SET_COMBINER_ALPHA_OCW, AB_DST))) > NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0) &&
         (((HWx(4, SET_COMBINER_ALPHA_OCW, AB_DST))) == ((HWx(4, SET_COMBINER_COLOR_OCW, AB_DST))))) ||
        ((((HWx(4, SET_COMBINER_COLOR_OCW, BLUETOALPHA_CD))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE) &&
         (((HWx(4, SET_COMBINER_ALPHA_OCW, CD_DST))) > NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0) &&
         (((HWx(4, SET_COMBINER_ALPHA_OCW, CD_DST))) == ((HWx(4, SET_COMBINER_COLOR_OCW, CD_DST))))))
    {
        DXGRIP("Can't have Color OCW combiner and Alpha OCW combiner going to same destination");
    }

    if (((((HWx(5, SET_COMBINER_COLOR_OCW, BLUETOALPHA_AB))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE) &&
         (((HWx(5, SET_COMBINER_ALPHA_OCW, AB_DST))) > NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0) &&
         (((HWx(5, SET_COMBINER_ALPHA_OCW, AB_DST))) == ((HWx(5, SET_COMBINER_COLOR_OCW, AB_DST))))) ||
        ((((HWx(5, SET_COMBINER_COLOR_OCW, BLUETOALPHA_CD))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE) &&
         (((HWx(5, SET_COMBINER_ALPHA_OCW, CD_DST))) > NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0) &&
         (((HWx(5, SET_COMBINER_ALPHA_OCW, CD_DST))) == ((HWx(5, SET_COMBINER_COLOR_OCW, CD_DST))))))
    {
        DXGRIP("Can't have Color OCW combiner and Alpha OCW combiner going to same destination");
    }

    if (((((HWx(6, SET_COMBINER_COLOR_OCW, BLUETOALPHA_AB))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE) &&
         (((HWx(6, SET_COMBINER_ALPHA_OCW, AB_DST))) > NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0) &&
         (((HWx(6, SET_COMBINER_ALPHA_OCW, AB_DST))) == ((HWx(6, SET_COMBINER_COLOR_OCW, AB_DST))))) ||
        ((((HWx(6, SET_COMBINER_COLOR_OCW, BLUETOALPHA_CD))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE) &&
         (((HWx(6, SET_COMBINER_ALPHA_OCW, CD_DST))) > NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0) &&
         (((HWx(6, SET_COMBINER_ALPHA_OCW, CD_DST))) == ((HWx(6, SET_COMBINER_COLOR_OCW, CD_DST))))))
    {
        DXGRIP("Can't have Color OCW combiner and Alpha OCW combiner going to same destination");
    }

    if (((((HWx(7, SET_COMBINER_COLOR_OCW, BLUETOALPHA_AB))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE) &&
         (((HWx(7, SET_COMBINER_ALPHA_OCW, AB_DST))) > NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0) &&
         (((HWx(7, SET_COMBINER_ALPHA_OCW, AB_DST))) == ((HWx(7, SET_COMBINER_COLOR_OCW, AB_DST))))) ||
        ((((HWx(7, SET_COMBINER_COLOR_OCW, BLUETOALPHA_CD))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE) &&
         (((HWx(7, SET_COMBINER_ALPHA_OCW, CD_DST))) > NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0) &&
         (((HWx(7, SET_COMBINER_ALPHA_OCW, CD_DST))) == ((HWx(7, SET_COMBINER_COLOR_OCW, CD_DST))))))
    {
        DXGRIP("Can't have Color OCW combiner and Alpha OCW combiner going to same destination");
    }

    if (((((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ST) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ZW) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_REFLECT_DIFFUSE) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_BUMPENVMAP) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_BUMPENVMAP_LUMINANCE) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DEPENDENT_AR) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DEPENDENT_GB) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_PRODUCT)) &&
        (((((HW(SET_SHADER_OTHER_STAGE_INPUT, STAGE2))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2_INSTAGE_1) &&
          ((((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_CLIP_PLANE) ||
           (((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT) ||
           (((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE))) ||
         ((((HW(SET_SHADER_OTHER_STAGE_INPUT, STAGE2))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2_INSTAGE_0) &&
          ((((HW(SET_SHADER_STAGE_PROGRAM, STAGE0))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CLIP_PLANE) ||
           (((HW(SET_SHADER_STAGE_PROGRAM, STAGE0))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE)))))
    {
        DXGRIP("Need a color in this stage");
    }

    if (((((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ST) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ZW) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR_CONST) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_3D) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_CUBE) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP_LUMINANCE) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_AR) ||
         (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_GB)) &&
        (((((HW(SET_SHADER_OTHER_STAGE_INPUT, STAGE3))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_2) &&
          ((((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_CLIP_PLANE) ||
           (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_PRODUCT) ||
           (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PROGRAM_NONE))) ||
         ((((HW(SET_SHADER_OTHER_STAGE_INPUT, STAGE3))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_1) &&
          ((((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_CLIP_PLANE) ||
           (((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT) ||
           (((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE))) ||
         ((((HW(SET_SHADER_OTHER_STAGE_INPUT, STAGE3))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_0) &&
          ((((HW(SET_SHADER_STAGE_PROGRAM, STAGE0))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CLIP_PLANE) ||
           (((HW(SET_SHADER_STAGE_PROGRAM, STAGE0))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE)))))
    {
        DXGRIP("Need a color in this stage");
    }

    if ((((HW(SET_SHADER_STAGE_PROGRAM, STAGE0))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CLIP_PLANE) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE0))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PASS_THROUGH) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE0))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE) &&
        (((HWx(0, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE))
    {
        DXGRIP("Stage requires a texture but none enabled");
    }

    if ((((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_CLIP_PLANE) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PASS_THROUGH) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE1))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE) &&
        (((HWx(1, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE))
    {
        DXGRIP("Stage requires a texture but none enabled");
    }

    if ((((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_CLIP_PLANE) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PASS_THROUGH) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_PRODUCT) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ZW) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PROGRAM_NONE) &&
        (((HWx(2, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE))
    {
        DXGRIP("Stage requires a texture but none enabled");
    }

    if ((((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CLIP_PLANE) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PASS_THROUGH) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE2))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ZW) &&
        (((HW(SET_SHADER_STAGE_PROGRAM, STAGE3))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PROGRAM_NONE) &&
        (((HWx(3, SET_TEXTURE_CONTROL0, ENABLE))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE))
    {
        DXGRIP("Stage requires a texture but none enabled");
    }
}

//------------------------------------------------------------------------------
// GetTextureAddress

DWORD GetTextureAddress(DWORD Stage)
{
    DWORD* pTextureStates = D3D__TextureState[Stage];

    DWORD texCoordIndex = pTextureStates[D3DTSS_TEXCOORDINDEX] & 0xffff;

    DWORD address
        = DRF_NUMFAST(097, _SET_TEXTURE_ADDRESS, _U,
                      pTextureStates[D3DTSS_ADDRESSU])
        | DRF_NUMFAST(097, _SET_TEXTURE_ADDRESS, _V,
                      pTextureStates[D3DTSS_ADDRESSV])
        | DRF_NUMFAST(097, _SET_TEXTURE_ADDRESS, _P,
                      pTextureStates[D3DTSS_ADDRESSW])
        | D3D__RenderState[D3DRS_WRAP0 + texCoordIndex];

    return address;
}

//------------------------------------------------------------------------------
// GetTextureControl0

DWORD GetTextureControl0(DWORD Stage)
{
    CDevice* pDevice = g_pDevice;

    if (pDevice->m_Textures[Stage] == NULL)
    {
        return 0;
    }
    else
    {
        return pDevice->m_TextureControl0Enabled[Stage];
    }
}

//------------------------------------------------------------------------------
// GetTextureFormat

DWORD GetTextureFormat(DWORD Stage)
{
    CDevice* pDevice = g_pDevice;

    if (pDevice->m_Textures[Stage])
    {
        return pDevice->m_Textures[Stage]->Format;
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
// GetTextureImageRect

DWORD GetTextureImageRect(DWORD Stage)
{
    CDevice* pDevice = g_pDevice;
    D3DBaseTexture* pTexture = pDevice->m_Textures[Stage];

    if (pTexture == NULL)
    {
        return 0;
    }

    DWORD width = PixelJar::GetWidth(pTexture);
    DWORD height = PixelJar::GetHeight(pTexture);

    return (DRF_NUMFAST(097, _SET_TEXTURE_IMAGE_RECT, _WIDTH, width) |
            DRF_NUMFAST(097, _SET_TEXTURE_IMAGE_RECT, _HEIGHT, height));
}

//------------------------------------------------------------------------------
// GetTextureFilter

DWORD GetTextureFilter(DWORD Stage)
{
    DWORD* pTextureStates = D3D__TextureState[Stage];

    if (pTextureStates == NULL)
    {
        return 0;
    }

    CDevice* pDevice = g_pDevice;
    DWORD minFilter = pTextureStates[D3DTSS_MINFILTER];
    DWORD magFilter = pTextureStates[D3DTSS_MAGFILTER];

    DWORD convolutionKernel = DRF_DEF(097, _SET_TEXTURE_FILTER, _CONVOLUTION_KERNEL, _QUINCUNX);

    if ((minFilter >= D3DTEXF_ANISOTROPIC) || (magFilter >= D3DTEXF_ANISOTROPIC))
    {
        if ((minFilter > D3DTEXF_ANISOTROPIC) || (magFilter > D3DTEXF_ANISOTROPIC))
        {
            if ((minFilter == D3DTEXF_GAUSSIANCUBIC) ||
                (magFilter == D3DTEXF_GAUSSIANCUBIC))
            {
                convolutionKernel = DRF_DEF(097, _SET_TEXTURE_FILTER, 
                                            _CONVOLUTION_KERNEL, _GAUSSIAN_3);
            }

            convolutionKernel |= DRF_DEF(097, _SET_TEXTURE_FILTER, 
                                         _MIN, _CONVOLUTION_2D_LOD0);

            minFilter = D3DTEXF_LINEAR;
            magFilter = NV097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0;
        }
        else
        {
            DWORD maxAnisotropy = pTextureStates[D3DTSS_MAXANISOTROPY];
            if (maxAnisotropy == 0)
            {
                minFilter = D3DTEXF_POINT;
                magFilter = D3DTEXF_POINT;
            }
            else
            {
                minFilter = D3DTEXF_LINEAR;
                magFilter = D3DTEXF_LINEAR;
            }
        }
    }

    INT lodBias = Round(256.0f * (Floatify(pTextureStates[D3DTSS_MIPMAPLODBIAS])
                                  + pDevice->m_SuperSampleLODBias));

    if (lodBias < -0x1000)
        lodBias = -0x1000;
    if (lodBias > 0xfff)
        lodBias = 0xfff;

    DWORD colorSign = (pTextureStates[D3DTSS_COLOROP] >= D3DTOP_BUMPENVMAP)
        ? (D3DTSIGN_GSIGNED | D3DTSIGN_BSIGNED)
        : pTextureStates[D3DTSS_COLORSIGN];

    DWORD filter 
        = DRF_NUM(097, _SET_TEXTURE_FILTER, _MIPMAP_LOD_BIAS, lodBias)

        | convolutionKernel

        | MinFilter(minFilter, pTextureStates[D3DTSS_MIPFILTER])

        | DRF_NUMFAST(097, _SET_TEXTURE_FILTER, _MAG, magFilter)

        | colorSign;

    return filter;
}

//------------------------------------------------------------------------------
// GetShaderStageProgram

DWORD GetShaderStageProgram()
{
    CDevice* pDevice = g_pDevice;
    INT stage;
    DWORD shaderStageProgram = 0;

    D3D__DirtyFlags &= ~D3DDIRTYFLAG_SHADER_STAGE_PROGRAM;

    if (pDevice->m_pPixelShader == NULL)
    {
        for (stage = 3; stage >= 0; stage--)
        {
            DWORD shaderMode 
                = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_2D_PROJECTIVE;

            D3DBaseTexture* pTexture = pDevice->m_Textures[stage];
    
            if (pTexture == NULL)
            {
                shaderMode = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE;
            }
            else if (D3D__TextureState[stage][D3DTSS_COLOROP] == D3DTOP_BUMPENVMAP)
            {
                shaderMode = NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP;
            }
            else if (D3D__TextureState[stage][D3DTSS_COLOROP] 
                == D3DTOP_BUMPENVMAPLUMINANCE)
            {
                shaderMode =
                    NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE;
            }
            else if (pTexture->Format &
                 DRF_DEF(097, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, _TRUE))
            {
                shaderMode = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CUBE_MAP;
            }
            else if ((pTexture->Format & DRF_NUM(097, _SET_TEXTURE_FORMAT, 
                                                 _DIMENSIONALITY, ~0)) ==
                  DRF_DEF(097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, _THREE))
            {
                shaderMode = NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_3D_PROJECTIVE;
            }
    
            shaderStageProgram = (shaderStageProgram << 5) | shaderMode;
        }
    }
    else
    {
        DWORD PSShaderStageProgram = pDevice->m_PSShaderStageProgram;

        if(pDevice->m_ShaderAdjustsTexMode == 0)
        {
            // don't adjust shader stage program
            shaderStageProgram = PSShaderStageProgram;
        }
        else
        {
            // adjust shader stage program according to set textures
            for (stage = 3; stage >= 0; stage--)
            {
                // Assume by default we'll keep the specified mode for this stage:
    
                DWORD shaderMode = (PSShaderStageProgram >> (stage * 5)) & 0x1f;
    
                // PERF: Disable texture unit for _NONE case?
    
                D3DBaseTexture* pTexture = pDevice->m_Textures[stage];
    
                if (pTexture == NULL)
                {
                    shaderMode = PS_TEXTUREMODES_NONE;
                }
                else if ((shaderMode >= PS_TEXTUREMODES_PROJECT2D) &&
                         (shaderMode <= PS_TEXTUREMODES_CUBEMAP))
                {
                    DWORD colorFormat = pTexture->Format 
                        & DRF_NUM(097, _SET_TEXTURE_FORMAT, _COLOR, ~0);
    
                    if (pTexture->Format & 
                        DRF_DEF(097, _SET_TEXTURE_FORMAT, _CUBEMAP_ENABLE, _TRUE))
                    {
                        shaderMode = PS_TEXTUREMODES_CUBEMAP;
                    }
                    else if (((pTexture->Format & DRF_NUM(097, _SET_TEXTURE_FORMAT, 
                                                          _DIMENSIONALITY, ~0)) 
                                == DRF_DEF(097, _SET_TEXTURE_FORMAT, _DIMENSIONALITY, 
                                           _THREE)) ||
    
                             ((colorFormat >= DRF_DEF(097, _SET_TEXTURE_FORMAT, 
                                                      _COLOR, _SZ_DEPTH_X8_Y24_FIXED)) &&
                              (colorFormat <= DRF_DEF(097, _SET_TEXTURE_FORMAT, 
                                                      _COLOR, _LU_IMAGE_DEPTH_Y16_FLOAT))))
                    {
                        shaderMode = PS_TEXTUREMODES_PROJECT3D;
                    }
                    else
                    {
                        shaderMode = PS_TEXTUREMODES_PROJECT2D;
                    }
                }
                else if ((shaderMode == PS_TEXTUREMODES_DOT_STR_3D) ||
                         (shaderMode == PS_TEXTUREMODES_DOT_STR_CUBE))
                {
                    if (pTexture->Format & DRF_DEF(097, _SET_TEXTURE_FORMAT, 
                                                  _CUBEMAP_ENABLE, _TRUE))
                    {
                        shaderMode = PS_TEXTUREMODES_DOT_STR_CUBE;
                    }
                    else
                    {
                        shaderMode = PS_TEXTUREMODES_DOT_STR_3D;
                    }
                }
        
                shaderStageProgram = (shaderStageProgram << 5) | shaderMode;
            }
        }
    }

    return shaderStageProgram;
}

//------------------------------------------------------------------------------
// PRIMITIVE_LAUNCH_STATE_TEST2

VOID PRIMITIVE_LAUNCH_STATE_TEST2()
{
    CDevice* pDevice = g_pDevice;
    int i;

    DWORD SFmt = PixelJar::GetSurfaceFormat(pDevice->m_pRenderTarget, pDevice->m_pZBuffer);
    if (D3D__RenderState[D3DRS_ALPHABLENDENABLE] &&
        (D3D__RenderState[D3DRS_BLENDOP] == D3DBLENDOP_REVSUBTRACTSIGNED ||
         D3D__RenderState[D3DRS_BLENDOP] == D3DBLENDOP_ADDSIGNED) &&
        (((HW_VAL(SET_SURFACE_FORMAT, COLOR, SFmt)) == NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5) ||
         ((HW_VAL(SET_SURFACE_FORMAT, COLOR, SFmt)) == NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5) ||
         ((HW_VAL(SET_SURFACE_FORMAT, COLOR, SFmt)) == NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5) ||
         ((HW_VAL(SET_SURFACE_FORMAT, COLOR, SFmt)) == NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8) ||
         ((HW_VAL(SET_SURFACE_FORMAT, COLOR, SFmt)) == NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8)))
    {
        DXGRIP("Combination of signed SetBlendEquation and certain color formats is not allowed");
    }

    for (i = 0; i < 4; i++)
    {
        DWORD TCtl = GetTextureControl0(i);
        DWORD TFmt = GetTextureFormat(i);
        
        if ((((HW_VAL(SET_TEXTURE_CONTROL0, LOG_MAX_ANISO, TCtl)) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
            ((((HW_VAL(SET_TEXTURE_FORMAT, BASE_SIZE_P, TFmt))) > NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P_1) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, DIMENSIONALITY, TFmt))) == NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY_THREE))) &&
            (((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl)))))
        {
            DXGRIP("No aniso support for 3D textures");
        }
    }

    for (i = 0; i < 4; i++)
    {
        DWORD TCtl = GetTextureControl0(i);
        DWORD IRect = GetTextureImageRect(i);

        if ((((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
            (((HW_VAL(SET_TEXTURE_IMAGE_RECT, HEIGHT, IRect))) & 0x1) &&
            (((HW_VAL(SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE, TCtl))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
        {
            DXGRIP("Invalid height with field enable");
        }
    }

    for (i = 0; i < 4; i++)
    {
        DWORD TCtl = GetTextureControl0(i);
        DWORD TFmt = GetTextureFormat(i);

        if ((((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_TRUE) &&
            (((HW_VAL(SET_TEXTURE_FORMAT, BORDER_SOURCE, TFmt))) != NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR) &&
            (((HW_VAL(SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE, TCtl))) == NV097_SET_TEXTURE_CONTROL0_IMAGE_FIELD_ENABLE_TRUE))
        {
            DXGRIP("Can't use ImageFieldEnable with border data in the texture");
        }
    }

    for (i = 0; i < 4; i++)
    {
        DWORD TCtl = GetTextureControl0(i);
        DWORD TFmt = GetTextureFormat(i);

        if ((((HW_VAL(SET_TEXTURE_CONTROL0, IMAGE_FIELD_ENABLE, TCtl)))) &&
            (((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl)))) &&
            (! ((((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
              (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8))))
        {
            DXGRIP("Can't use ImageFieldEnable unless texture is a linear format");
        }
    }

    for (i = 0; i < 4; i++)
    {
        DWORD TCtl = GetTextureControl0(i);
        DWORD Fltr = GetTextureFilter(i);

        if ((((HW_VAL(SET_TEXTURE_CONTROL0, LOG_MAX_ANISO, TCtl))) > NV097_SET_TEXTURE_CONTROL0_LOG_MAX_ANISO_0) &&
            ((((HW_VAL(SET_TEXTURE_FILTER, MIN, Fltr))) == NV097_SET_TEXTURE_FILTER_MIN_CONVOLUTION_2D_LOD0) ||
             (((HW_VAL(SET_TEXTURE_FILTER, MAG, Fltr))) == NV097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0)) &&
            (((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl)))))
        {
            DXGRIP("No aniso support for convolution filter");
        }
    }

    for (i = 0; i < 4; i++)
    {
        DWORD TFmt = GetTextureFormat(i);
        DWORD TCtl = GetTextureControl0(i);

        if (((((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT1_A1R5G5B5) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT23_A8R8G8B8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT45_A8R8G8B8)) &&
            (((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl)))) &&
            ((((HW_VAL(SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION, TCtl))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_ALPHA) ||
             (((HW_VAL(SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION, TCtl))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_RGBA) ||
             (((HW_VAL(SET_TEXTURE_CONTROL0, COLOR_KEY_OPERATION, TCtl))) == NV097_SET_TEXTURE_CONTROL0_COLOR_KEY_OPERATION_KILL)))
        {
            DXGRIP("No color-key support for compressed texture formats");
        }
    }

    for (i = 0; i < 4; i++)
    {
        DWORD TFmt = GetTextureFormat(i);
        DWORD TAdr = GetTextureAddress(i);
        DWORD TCtl = GetTextureControl0(i);

        if (((((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FLOAT) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5) ||
             (((HW_VAL(SET_TEXTURE_FORMAT, COLOR, TFmt))) == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8)) &&
            (((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl)))) &&
            ((((HW_VAL(SET_TEXTURE_ADDRESS, U, TAdr))) == NV097_SET_TEXTURE_ADDRESS_U_WRAP) || 
             (((HW_VAL(SET_TEXTURE_ADDRESS, U, TAdr))) == NV097_SET_TEXTURE_ADDRESS_U_MIRROR) ||
             (((HW_VAL(SET_TEXTURE_ADDRESS, V, TAdr))) == NV097_SET_TEXTURE_ADDRESS_V_WRAP) || 
             (((HW_VAL(SET_TEXTURE_ADDRESS, V, TAdr))) == NV097_SET_TEXTURE_ADDRESS_V_MIRROR)))
        {
            DXGRIP("No wrap or mirror if texture is a linear format");
        }
    }

    for (i = 0; i < 7; i++)
    {
#if 0
        DWORD OCW = pDevice->m_ColorOCW[i];

        if (((((HW_VAL(SET_COMBINER_COLOR_OCW, BLUETOALPHA_AB, OCW))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE) &&
             (((HW_VAL(SET_COMBINER_ALPHA_OCW, AB_DST, OCW))) > NV097_SET_COMBINER_ALPHA_OCW_AB_DST_REG_0) &&
             (((HW_VAL(SET_COMBINER_ALPHA_OCW, AB_DST, OCW))) == ((HWx(0, SET_COMBINER_COLOR_OCW, AB_DST))))) ||
            ((((HW_VAL(SET_COMBINER_COLOR_OCW, BLUETOALPHA_CD, OCW))) == NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE) &&
             (((HW_VAL(SET_COMBINER_ALPHA_OCW, CD_DST, OCW))) > NV097_SET_COMBINER_ALPHA_OCW_CD_DST_REG_0) &&
             (((HW_VAL(SET_COMBINER_ALPHA_OCW, CD_DST, OCW))) == ((HWx(0, SET_COMBINER_COLOR_OCW, CD_DST))))))
        {
            DXGRIP("Can't have Color OCW combiner and Alpha OCW combiner going to same destination");
        }
#endif
    }

    if (pDevice->m_pPixelShader == NULL)
    {
        return;
    }

    DWORD SOSI = D3D__RenderState[D3DRS_PSINPUTTEXTURE];
    DWORD SSP = GetShaderStageProgram();

    if (((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ST) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ZW) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_REFLECT_DIFFUSE) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_BUMPENVMAP) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_BUMPENVMAP_LUMINANCE) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DEPENDENT_AR) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DEPENDENT_GB) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_PRODUCT)) &&
        (((((HW_VAL(SET_SHADER_OTHER_STAGE_INPUT, STAGE2, SOSI))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2_INSTAGE_1) &&
          ((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_CLIP_PLANE) ||
           (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT) ||
           (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE))) ||
         ((((HW_VAL(SET_SHADER_OTHER_STAGE_INPUT, STAGE2, SOSI))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2_INSTAGE_0) &&
          ((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE0, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CLIP_PLANE) ||
           (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE0, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE)))))
    {
        DXGRIP("Need a color in this stage");
    }

    if (((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ST) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ZW) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR_CONST) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_3D) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_CUBE) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP_LUMINANCE) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_AR) ||
         (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_GB)) &&
        (((((HW_VAL(SET_SHADER_OTHER_STAGE_INPUT, STAGE3, SOSI))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_2) &&
          ((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_CLIP_PLANE) ||
           (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_PRODUCT) ||
           (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PROGRAM_NONE))) ||
         ((((HW_VAL(SET_SHADER_OTHER_STAGE_INPUT, STAGE3, SOSI))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_1) &&
          ((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_CLIP_PLANE) ||
           (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT) ||
           (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE))) ||
         ((((HW_VAL(SET_SHADER_OTHER_STAGE_INPUT, STAGE3, SOSI))) == NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_0) &&
          ((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE0, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CLIP_PLANE) ||
           (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE0, SSP))) == NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE)))))
    {
        DXGRIP("Need a color in this stage");
    }

    DWORD TCtl = GetTextureControl0(0);
    if ((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE0, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CLIP_PLANE) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE0, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PASS_THROUGH) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE0, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE) &&
        (((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE))
    {
        DXGRIP("Stage requires a texture but none enabled");
    }

    TCtl = GetTextureControl0(1);
    if ((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_CLIP_PLANE) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PASS_THROUGH) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE1, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE) &&
        (((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE))
    {
        DXGRIP("Stage requires a texture but none enabled");
    }

    TCtl = GetTextureControl0(2);
    if ((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_CLIP_PLANE) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PASS_THROUGH) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_PRODUCT) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ZW) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PROGRAM_NONE) &&
        (((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE))
    {
        DXGRIP("Stage requires a texture but none enabled");
    }

    TCtl = GetTextureControl0(3);
    if ((((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CLIP_PLANE) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PASS_THROUGH) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE2, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ZW) &&
        (((HW_VAL(SET_SHADER_STAGE_PROGRAM, STAGE3, SSP))) != NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PROGRAM_NONE) &&
        (((HW_VAL(SET_TEXTURE_CONTROL0, ENABLE, TCtl))) == NV097_SET_TEXTURE_CONTROL0_ENABLE_FALSE))
    {
        DXGRIP("Stage requires a texture but none enabled");
    }
}

#endif // DBG

} // end of namespace

