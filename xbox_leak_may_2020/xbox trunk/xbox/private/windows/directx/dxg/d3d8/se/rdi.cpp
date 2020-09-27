/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       rdi.cpp
 *  Content:    implementation for gpu ram data access
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

// Spew out disassembled vertex shader instruction
bool WINAPI DisasmInstr(void *pv);

//------------------------------------------------------------------------------
// D3DRDI_GetRamData

extern "C"
void WINAPI D3DRDI_GetRamData(
    DWORD index,
    DWORD address,
    void *pData,
    DWORD SizeOfData)
{
    if (DBG_CHECK(TRUE))
    {
        if (SizeOfData & 0x3)
        {
            DPF_ERR("SizeOfData must be multiple of 4");
        }
    }

    CMiniport *pMiniPort = &g_pDevice->m_Miniport;
    BYTE *RegBase = (BYTE *)(pMiniPort->m_RegisterBase);

    // from Shaun Ho:
    //
    // > Hi Mike,
    // > In general, we use RDI to handle context switch and therefore your read
    // > should just work. Please keep in mind that the read port
    // > (single ported) is muxed with other address requests, in
    // > addition to the RDI request. This is the reason why we
    // > always emphasize the importance of wait_for_idle for any
    // > rdi read.

    // kickoff pushbuffer
    D3DDevice_KickPushBuffer();

    // Calling D3DDevice_BlockUntilIdle() here seemed to trash some of the results
    // so I switched to TilingUpdateIdle.
    ULONG dmapush;
    pMiniPort->TilingUpdateIdle(&dmapush);

    REG_WR32(RegBase, NV_PGRAPH_RDI_INDEX, ((index << 16) | (address << 2)));

    DWORD *pdwData = (DWORD *)pData;

    SizeOfData /= sizeof(DWORD);

    while(SizeOfData--)
    {
        *pdwData++ = REG_RD32(RegBase, NV_PGRAPH_RDI_DATA);
    }

    // Resume graphics, now that PFB/PGRAPH are consistent
    REG_WR32(RegBase, NV_PFIFO_CACHE1_DMA_PUSH, dmapush);
}

//------------------------------------------------------------------------------
// D3DRDI_GetNextVTXLRUSlot

extern "C"
DWORD WINAPI D3DRDI_GetNextVTXLRUSlot()
{
    DWORD Data;

    D3DRDI_GetRamData(RDI_INDEX_IDX_FMT, 33, &Data, sizeof(Data));
    return Data >> 24;
}

//------------------------------------------------------------------------------
// D3DRDI_GetVTXFileEntry

extern "C"
void WINAPI D3DRDI_GetVTXFileEntry(
    DWORD index,
    D3DRDI_VTX_FILEENTRY *pentry)
{
    // Read in the first 8 DWORDS
    D3DRDI_GetRamData(RDI_INDEX_VTX_FILE0, index * 8, pentry, 8 * sizeof(DWORD));

    // Read in the next 20 DWORDS
    D3DRDI_GetRamData(RDI_INDEX_VTX_FILE1, index * 20, pentry->pos, 20 * sizeof(DWORD));
}

//------------------------------------------------------------------------------
// D3DRDI_GetPAEntry

extern "C"
void WINAPI D3DRDI_GetPAEntry(
    DWORD index,
    D3DRDI_PAENTRY *pentry)
{
    D3DRDI_GetRamData(RDI_INDEX_CAS0 + index, 0, pentry, sizeof(D3DRDI_PAENTRY));
}

/*
 * Helper dump routines
 */
VOID SpewFloat(
    void *pv)
{
    char szBuf[10];
    float f = *(float *)pv;
    long l = FloatToLong(f);

    f -= l;

    for(int i = 0; i < 6; i++)
    {
        f *= 10;
        DWORD dw = (DWORD)FloatToLong(f);

        szBuf[i] = (char)(dw + '0');

        f -= dw;
    }
    szBuf[i] = 0;

    DbgPrint("%li.%s ", l, szBuf);
}

//------------------------------------------------------------------------------
// D3DRDI_DumpVTXFileEntry

extern "C"
void WINAPI D3DRDI_DumpVTXFileEntry(
    DWORD index)
{
    D3DRDI_VTX_FILEENTRY vtxentry;

    D3DRDI_GetVTXFileEntry(index, &vtxentry);

    DbgPrint("%d\n", index);

    DbgPrint("  xyzw: ");
    for(int ipos = 0; ipos < 4; ipos++)
        SpewFloat(&vtxentry.pos[ipos]);
    DbgPrint("\n");

    DbgPrint("  xyzw: 0x%08lx, 0x%08lx, 0x%08lx, 0x%08lx\n",
        vtxentry.pos[0], vtxentry.pos[1], vtxentry.pos[2], vtxentry.pos[3]);

    DbgPrint("  diff: 0x%08lx  spec: 0x%08lx\n", vtxentry.diff, vtxentry.spec);
    DbgPrint("  bdiff: 0x%08lx  bspec: 0x%08lx\n", vtxentry.bdiff, vtxentry.bspec);

    DbgPrint("  fog: ");
    SpewFloat(&vtxentry.fog);
    DbgPrint("0x%08lx\n", vtxentry.fog);

    DbgPrint("  ptsize: 0x%08lx\n", vtxentry.ptsize);

    for(int tex = 0; tex < 4; tex++)
    {
        DbgPrint("  t%d: ", tex);

        for(int i = 0; i < 4; i++)
            SpewFloat(&vtxentry.tex[tex][i]);

        DbgPrint("\n");
    }

    DbgPrint("  mask: 0x%08lx  unused: 0x%08lx\n",
        vtxentry.mask, vtxentry.unused);
}

//------------------------------------------------------------------------------
// D3DRDI_DumpVTXProgram

extern "C"
void WINAPI D3DRDI_DumpVTXProgram(
    DWORD index,
    DWORD length)
{
    DWORD rgInstr[VSHADER_PROGRAM_SLOTS][4];

    index = min(VSHADER_PROGRAM_SLOTS - 1, index);
    length = min(VSHADER_PROGRAM_SLOTS - index, length);

    D3DRDI_GetRamData(RDI_INDEX_XL_PROGRAM, index * 4, rgInstr, length * 4 * sizeof(DWORD));

    for(DWORD i = 0; i < length; i++)
    {
        DbgPrint("% 3d: ", index + i);

        DisasmInstr(rgInstr[i]);
    }
}

// The user gets to use constant registers -96 through 95 (or 0 through 95
// if only using 96 registers).  We add 96 before giving those register
// values to the hardware:

#define VSHADER_CONSTANT_NUM 192
#define VSHADER_CONSTANT_BIAS 96

//------------------------------------------------------------------------------
// D3DRDI_DumpVTXConstants

extern "C"
void WINAPI D3DRDI_DumpVTXConstants(
    DWORD index,
    DWORD count)
{
    DWORD rgConsts[VSHADER_CONSTANT_NUM][4];

    index = min(VSHADER_CONSTANT_NUM - 1, index);
    count = min(VSHADER_CONSTANT_NUM - index, count);

    D3DRDI_GetRamData(RDI_INDEX_VTX_CONSTANTS0, index * 4, rgConsts, count * 4 * sizeof(DWORD));

    for(DWORD var = 0; var < count; var++)
    {
        DbgPrint("%03d: ", index + var);

        for(int i = 3; i >= 0; i--)
        {
            SpewFloat(&rgConsts[var][i]);
        }

        DbgPrint("\n");
    }
}

//------------------------------------------------------------------------------
// D3DRDI_DumpVAB

extern "C"
void WINAPI D3DRDI_DumpVAB()
{
#define VAB_SLOTS 17
    DWORD rgvab[VAB_SLOTS][4];

    D3DRDI_GetRamData(RDI_INDEX_XL_VAB, 0, rgvab, VAB_SLOTS * 4 * sizeof(DWORD));

    for(DWORD slot = 0; slot < VAB_SLOTS; slot++)
    {
        DbgPrint("%02d: ", slot);

        for(int i = 3; i >= 0; i--)
            DbgPrint("0x%08lx ", rgvab[slot][i]);

        DbgPrint("\n     (");

        for(int i = 3; i >= 0; i--)
            SpewFloat(&rgvab[slot][i]);

        DbgPrint(")\n");
    }
}

//------------------------------------------------------------------------------
// D3DRDI_DumpVAB

void WINAPI D3DRDI_DumpPA()
{
    for(int cas = 0; cas < 3; cas++)
    {
        D3DRDI_PAENTRY casentry = {0};

        D3DRDI_GetPAEntry(cas, &casentry);

        DbgPrint("CAS %d\n", cas);

        DbgPrint("  xyzw: ");
        for(int i = 0; i < 4; i++)
            SpewFloat(&casentry.pos[i]);
        DbgPrint("\n");

        DbgPrint("  xyzw: 0x%08lx, 0x%08lx, 0x%08lx, 0x%08lx\n",
            casentry.pos[0], casentry.pos[1], casentry.pos[2], casentry.pos[3]);

        DbgPrint("  z: 0x%08lx  w: 0x%08lx\n", casentry.z, casentry.w);
        DbgPrint("  diff: 0x%08lx  spec: 0x%08lx\n", casentry.diff, casentry.spec);
        DbgPrint("  bdiff: 0x%08lx  bspec: 0x%08lx\n", casentry.bdiff, casentry.bspec);

        DbgPrint("  fog: ");
        SpewFloat(&casentry.fog);
        DbgPrint("0x%08lx\n", casentry.fog);

        DbgPrint("  ptsize: 0x%08lx\n", casentry.ptsize);

        for(int tex = 0; tex < 4; tex++)
        {
            DbgPrint("  t%d: ", tex);

            for(int i = 0; i < 4; i++)
                SpewFloat(&casentry.tex[tex][i]);

            DbgPrint("\n");
        }

        DbgPrint("  unknown:");
        for(i = 0; i < 4; i++)
            DbgPrint(" 0x%08lx", casentry.unknown[i]);
        DbgPrint("\n");
    }
}

// Turn off warning about zero length array
#pragma warning(push)
#pragma warning(disable: 4200)

#pragma pack(push, 1)

struct VINSTR
{
    DWORD eos:1;     /* 0:00 last instruction */
    DWORD cin:1;     /* 0:01 ctx indexed address */
    DWORD  om:1;     /* 0:02 output mux */
    DWORD  oc:9;     /* 0:03 output write control */
    DWORD owm:4;     /* 0:12 output write mask */
    DWORD swm:4;     /* 0:16 secondary register write mask */
    DWORD  rw:4;     /* 0:20 register write */
    DWORD rwm:4;     /* 0:24 primary register write mask */
    DWORD cmx:2;     /* 0:28 c mux (NA,r1,v,c) */
    DWORD crrlo:2;   /* 0:30 c register read (low part) */

    DWORD crrhi:2;   /* 0:30 of c register read (hi part) */
    DWORD cws:2;     /* 0:30 c register read */
    DWORD czs:2;     /* 1:02 c w swizzle */
    DWORD cys:2;     /* 1:04 c z swizzle */
    DWORD cxs:2;     /* 1:06 c y swizzle */
    DWORD cne:1;     /* 1:08 c x swizzle */
    DWORD bmx:2;     /* 1:10 c negate */
    DWORD brr:4;     /* 1:11 b mux (NA,r1,v,c) */
    DWORD bws:2;     /* 1:13 b register read */
    DWORD bzs:2;     /* 1:17 b w swizzle */
    DWORD bys:2;     /* 1:19 b z swizzle */
    DWORD bxs:2;     /* 1:21 b y swizzle */
    DWORD bne:1;     /* 1:23 b x swizzle */
    DWORD amx:2;     /* 1:25 b negate */
    DWORD arr:4;     /* 1:26 a mux (NA,r0,v,c) */

    DWORD aws:2;     /* 2:00 a w swizzle */
    DWORD azs:2;     /* 2:02 a z swizzle */
    DWORD ays:2;     /* 2:04 a y swizzle */
    DWORD axs:2;     /* 2:06 a x swizzle */
    DWORD ane:1;     /* 2:08 a negate */
    DWORD  va:4;     /* 2:09 ibuffer address */
    DWORD  ca:8;     /* 2:13 ctx address */
    DWORD mac:4;     /* 2:21 MLU/ALU op */
    DWORD ilu:7;     /* 2:25 ILU op */

    DWORD dwPad;
};

#pragma pack(pop)
#pragma warning(pop)

static const bool kMacUsesA[] = {
    false,  // NV_IGRAPH_XF_V_NOP        0x00
    true,   // NV_IGRAPH_XF_V_MOV        0x01
    true,   // NV_IGRAPH_XF_V_MUL        0x02
    true,   // NV_IGRAPH_XF_V_ADD        0x03
    true,   // NV_IGRAPH_XF_V_MAD        0x04
    true,   // NV_IGRAPH_XF_V_DP3        0x05
    true,   // NV_IGRAPH_XF_V_DPH        0x06
    true,   // NV_IGRAPH_XF_V_DP4        0x07
    true,   // NV_IGRAPH_XF_V_DST        0x08
    true,   // NV_IGRAPH_XF_V_MIN        0x09
    true,   // NV_IGRAPH_XF_V_MAX        0x0a
    true,   // NV_IGRAPH_XF_V_SLT        0x0b
    true,   // NV_IGRAPH_XF_V_SGE        0x0c
    true,   // NV_IGRAPH_XF_V_ARL        0x0d
    false,  // ??
};
static const bool kMacUsesB[] = {
    false,  // NV_IGRAPH_XF_V_NOP        0x00
    false,  // NV_IGRAPH_XF_V_MOV        0x01
    true,   // NV_IGRAPH_XF_V_MUL        0x02
    false,  // NV_IGRAPH_XF_V_ADD        0x03
    true,   // NV_IGRAPH_XF_V_MAD        0x04
    true,   // NV_IGRAPH_XF_V_DP3        0x05
    true,   // NV_IGRAPH_XF_V_DPH        0x06
    true,   // NV_IGRAPH_XF_V_DP4        0x07
    true,   // NV_IGRAPH_XF_V_DST        0x08
    true,   // NV_IGRAPH_XF_V_MIN        0x09
    true,   // NV_IGRAPH_XF_V_MAX        0x0a
    true,   // NV_IGRAPH_XF_V_SLT        0x0b
    true,   // NV_IGRAPH_XF_V_SGE        0x0c
    false,  // NV_IGRAPH_XF_V_ARL        0x0d
    false,  // ??
};

static const bool kMacUsesC[] = {
    false,  // NV_IGRAPH_XF_V_NOP        0x00
    false,  // NV_IGRAPH_XF_V_MOV        0x01
    false,  // NV_IGRAPH_XF_V_MUL        0x02
    true,   // NV_IGRAPH_XF_V_ADD        0x03
    true,   // NV_IGRAPH_XF_V_MAD        0x04
    false,  // NV_IGRAPH_XF_V_DP3        0x05
    false,  // NV_IGRAPH_XF_V_DPH        0x06
    false,  // NV_IGRAPH_XF_V_DP4        0x07
    false,  // NV_IGRAPH_XF_V_DST        0x08
    false,  // NV_IGRAPH_XF_V_MIN        0x09
    false,  // NV_IGRAPH_XF_V_MAX        0x0a
    false,  // NV_IGRAPH_XF_V_SLT        0x0b
    false,  // NV_IGRAPH_XF_V_SGE        0x0c
    false,  // NV_IGRAPH_XF_V_ARL        0x0d
    false,  // ??
    false,  // ??
};

// inversion unit operation
static const char *iluOps[] = {
    "nop",  //  0x0
    "mov",  //  0x1
    "rcp",  //  0x2
    "rcc",  //  0x3
    "rsq",  //  0x4
    "exp",  //  0x5
    "log",  //  0x6
    "lit",  //  0x7
    "??8",
    "??9",
    "??a",
    "??b",
    "??c",
    "??d",
    "??e",
    "??f"
};

// multiply / add operation
static const char *macOps[] = {
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
    "arl",  //      0xD
    "??e",
    "??f",
};

//=========================================================================
//
//=========================================================================
static const char *RegisterWriteMask(DWORD m)
{
    static const char *masks[] =
    {
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
        "",         // all
        "error"
    };

    return masks[min(m, ARRAYSIZE(masks) - 1)];
}

//=========================================================================
//
//=========================================================================
void ParseOut(VINSTR *pinstr)
{
    static const char *rgszOut[] =
    {
        "oPos",     // 0
        "?o1",      // 1
        "?o2",      // 2
        "oD0",      // 3
        "oD1",      // 4
        "oFog",     // 5
        "oPts",     // 6
        "oB0",      // 7
        "oB1",      // 8
        "oT0",      // 9
        "oT1",      // 10
        "oT2",      // 11
        "oT3",      // 12
        "?"
    };

    bool oc_output = (pinstr->oc & 0x0100) != 0;
    DWORD oc_index = pinstr->oc & 0xff;

    if(oc_output)
    {
        oc_index = min(oc_index, ARRAYSIZE(rgszOut) - 1);
        DbgPrint("%s%s", rgszOut[oc_index], RegisterWriteMask(pinstr->owm));
    }
    else
    {
        DbgPrint("c[%d]%s", oc_index - 96, RegisterWriteMask(pinstr->owm));
    }
}

//=========================================================================
//
//=========================================================================
void ParseMux(int mx, int rr, int ws, int zs, int ys, int xs, int ne,
    VINSTR *pinstr)
{
    static const char s[] = "xyzw"; // Swizzle
    static const char m[] = "?rvc";

    if(ne)
        DbgPrint("-");

    DbgPrint("%c", m[mx]);

    switch(mx)
    {
    default:
    case 0:
        DbgPrint("error");
        break;
    case 1:
        DbgPrint("%d", rr);
        break;
    case 2:
        DbgPrint("%d", pinstr->va);
        break;
    case 3:
        if(pinstr->cin)
            DbgPrint("a0.x+");

        DbgPrint("[%d]", pinstr->ca - 96);
        break;
    }

    if(xs == 0 && ys == 1 && zs == 2 && ws == 3)
        ;                       // print nothing for .xyzw
    else if(xs == ys && zs == ws && xs == zs)
        DbgPrint(".%c", s[xs]);   // print .x for .xxxx, etc.
    else
        DbgPrint(".%c%c%c%c", s[xs], s[ys], s[zs], s[ws]);

}

//=========================================================================
//
//=========================================================================
bool WINAPI DisasmInstr(void *pv)
{
    VINSTR *pinstr = (VINSTR *)pv;

    if(pinstr->mac || !pinstr->ilu)
    {
        // MAC instruction
        DbgPrint("%s ", macOps[pinstr->mac & 0xf]);

        // check if instruction has two destinations
        bool ftwodest = (pinstr->owm && !pinstr->om) && pinstr->rwm;

        if(ftwodest)
            DbgPrint("{");

        if(pinstr->mac == 0xd)
        {
            // arl is special case
            DbgPrint("a0.x");
        }
        else if(pinstr->owm && !pinstr->om)
        {
            ParseOut(pinstr);
        }

        if(ftwodest)
            DbgPrint(", ");

        if(pinstr->rwm)
        {
            DbgPrint("r%d%s", pinstr->rw, RegisterWriteMask(pinstr->rwm));
        }

        if(ftwodest)
            DbgPrint("}");

        if(kMacUsesA[pinstr->mac & 0xf])
        {
            DbgPrint(", ");
            ParseMux(
                pinstr->amx,        /* 1:26 a mux (NA,r0,v,c) */
                pinstr->arr,        /* 1:28 a register read */
                pinstr->aws,        /* 2:00 a w swizzle */
                pinstr->azs,        /* 2:02 a z swizzle */
                pinstr->ays,        /* 2:04 a y swizzle */
                pinstr->axs,        /* 2:06 a x swizzle */
                pinstr->ane,        /* 2:08 a negate */
                pinstr);
        }

        if(kMacUsesB[pinstr->mac & 0xf])
        {
            DbgPrint(", ");
            ParseMux(
                pinstr->bmx,        /* 1:11 b mux (NA,r1,v,c) */
                pinstr->brr,        /* 1:13 b register read */
                pinstr->bws,        /* 1:17 b w swizzle */
                pinstr->bzs,        /* 1:19 b z swizzle */
                pinstr->bys,        /* 1:21 b y swizzle */
                pinstr->bxs,        /* 1:23 b x swizzle */
                pinstr->bne,        /* 1:25 b negate */
                pinstr);
        }

        if(kMacUsesC[pinstr->mac & 0xf])
        {
            DbgPrint(", ");
            ParseMux(
                pinstr->cmx,        /* 0:28 c mux (NA,r1,v,c) */
                (pinstr->crrhi << 2) | pinstr->crrlo,        /* 0:30 c register read */
                pinstr->cws,        /* 1:02 c w swizzle */
                pinstr->czs,        /* 1:04 c z swizzle */
                pinstr->cys,        /* 1:06 c y swizzle */
                pinstr->cxs,        /* 1:08 c x swizzle */
                pinstr->cne,        /* 1:10 c negate */
                pinstr);
        }
    }

    if(pinstr->ilu)
    {
        // indent paired instruction
        if(pinstr->mac)
            DbgPrint("\n    ");

        DbgPrint("%s ", iluOps[pinstr->ilu & 0xf]);

        if(pinstr->swm)
        {
            // When this is a double opcode, and the ilu is writing to
            // a register, then the register has to be 1.
            DWORD ilu_rw = (pinstr->mac && pinstr->ilu) ? 1 : pinstr->rw;

            DbgPrint("r%d%s", ilu_rw, RegisterWriteMask(pinstr->swm));
        }

        if(pinstr->owm && pinstr->om)
        {
            if(pinstr->swm)
                DbgPrint(", ");

            ParseOut(pinstr);
        }

        DbgPrint(", ");
        ParseMux(
            pinstr->cmx,        /* 0:28 c mux (NA,r1,v,c) */
            (pinstr->crrhi << 2) | pinstr->crrlo,        /* 0:30 c register read */
            pinstr->cws,        /* 1:02 c w swizzle */
            pinstr->czs,        /* 1:04 c z swizzle */
            pinstr->cys,        /* 1:06 c y swizzle */
            pinstr->cxs,        /* 1:08 c x swizzle */
            pinstr->cne,        /* 1:10 c negate */
            pinstr);
    }

    DbgPrint("; %s \n", pinstr->eos ? "* eos *" : "");

    return pinstr->eos;
}

} // end of namespace

