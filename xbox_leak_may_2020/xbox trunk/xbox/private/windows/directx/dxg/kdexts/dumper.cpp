#include "precomp.hpp"

using namespace D3D;

#if DBG

#define STATE_SIZE (65536 / 4)

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
    DWORD scaled = (DWORD) (*f * 1000.0f);

    dprintf("%li.%03li", scaled / 1000, scaled % 1000);
}

//------------------------------------------------------------------------------
// ShowDump()
//
// Show a dump of the current register state.

DECLARE_API(dumper)
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
    DWORD ParseState[STATE_SIZE];
    BOOL Modified[STATE_SIZE];
    DWORD subch = 0;
    ULONG addr;

    addr = GetExpression("D3D__g_ParseState");
    if (addr == 0)
    {
        dprintf("Error: can't get g_ParseState address\n");
        return;
    }

    if (ReadMemory(addr, (PVOID)ParseState, sizeof(ParseState), NULL) == 0)
    {
        dprintf("Error: can't read memory for g_ParseState\n");
        return;
    }

    addr = GetExpression("D3D__g_Modified");
    if (addr == 0)
    {
        dprintf("Error: can't get g_Modified address\n");
        return;
    }

    if (ReadMemory(addr, (PVOID)Modified, sizeof(Modified), NULL) == 0)
    {
        dprintf("Error: can't read memory for g_Modified\n");
        return;
    }

    pField = (FieldDescription*)g_Kelvin;
    pFieldLimit = pField + ((sizeof(g_Kelvin) / sizeof(g_Kelvin[0])) - 1);
    pState = &ParseState[(subch << 13) >> 2];
    pModified = &Modified[(subch << 13) >> 2];

    dprintf("-----------------------------------------------------------\n");

    // Loop through the description array in order:

    while (pField < pFieldLimit)
    {
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
                    // SET_MODEL_VIEW_MATRIX0 0x400
                    //     Zero:    0.0    1.0   2.0   3.0

                    dprintf("0x%lx - %s\n    %-20s:\t",
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
                            dprintf("\n\t\t\t\t");
                        }

                        method = pField->Method 
                               + i * pField->ReplicationStride[0];

                        if (pField->FieldType == 'f')
                        {
                            PrintFloat((float*) &pState[method >> 2]);
                            dprintf(" \t");
                        }
                        else
                        {
                            dprintf("0x%lx\t", pState[method >> 2]);
                        }
                    }

                    dprintf("\n");
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
                            // 0x800 - SET_LIGHT_AMBIENT_COLOR(3)
                            //     Zero:   0.0    0.0    0.0   0.0
        
                            dprintf("0x%lx - %s(%li)\n    %-20s:\t",
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
                                    dprintf("\n\t\t\t\t");
                                }
        
                                method = pField->Method 
                                       + i * pField->ReplicationStride[0]
                                       + j * pField->ReplicationStride[1];
        
                                if (pField->FieldType == 'f')
                                {
                                    PrintFloat((float*) &pState[method >> 2]);
                                    dprintf("\t ");
                                }
                                else
                                {
                                    dprintf("0x%lx\t", pState[method >> 2]);
                                }
                            }
        
                            dprintf("\n");
                        }
                        else
                        {
                            // 0x218 - SET_TEXTURE_FORMAT(2)
                            //     CONTEXT_DMA:     0x0
                            //     ENABLE:          0x0

                            method = pField->Method 
                                   + i * pField->ReplicationStride[0];

                            dprintf("0x%lx - %s(%li)\t\t\t0x%lx\n",
                                       pField->Method,
                                       pField->MethodName,
                                       i,
                                       pState[method >> 2]);

                            for (pThis = pField; 
                                 pThis->Method == pField->Method;
                                 pThis++)
                            {
                                dprintf("    %-20s:\t0x%lx\n",
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
                // SET_SURFACE_CLIP_HORIZONTAL 0x200
                //     X:   0x0
                //     WIDTH:   0x1e0

                method = pField->Method;

                dprintf("0x%lx - %s\t\t\t0x%lx\n",
                           pField->Method,
                           pField->MethodName,
                           pState[method >> 2]);

                if (pField->FieldType == 'f')
                {
                    dprintf("    %-20s:\t",
                               pField->FieldName);
                    PrintFloat((float*) &pState[method >> 2]);
                    dprintf("\n");
                }
                else
                {
                    for (pThis = pField; 
                         pThis->Method == pField->Method;
                         pThis++)
                    {
                        dprintf("    %-20s:\t0x%lx\n",
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

#else // DBG

DECLARE_API(dumper)
{
    dprintf("Dumper extention works for checked builds only\n");
}

#endif // DBG

