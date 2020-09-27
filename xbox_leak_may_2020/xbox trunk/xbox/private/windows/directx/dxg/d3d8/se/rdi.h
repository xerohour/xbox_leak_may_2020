/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       rdi.h
 *  Content:    header file for RDI Data access
 *
 ***************************************************************************/

#ifndef _D3DRDI_H_
#define _D3DRDI_H_


#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#ifdef __cplusplus
extern "C" {
#endif

#define RDI_INDEX_XL_PROGRAM     0x00000010 //136x4   // Cheops Program
#define RDI_INDEX_XL_VAB         0x00000015 //17x4    // VAB
#define RDI_INDEX_VTX_CONSTANTS0 0x00000017 //192x4   // cheop context memory0
#define RDI_INDEX_VTX_CONSTANTS1 0x000000cc //192x4   // cheop context memory1
#define RDI_INDEX_IDX_FMT        0x00000026 //16+16+3 // Vertex Array Regs+State
#define RDI_INDEX_VTX_FILE0      0x00000028 //48x4    // Vertex File0
#define RDI_INDEX_VTX_FILE1      0x00000029 //144x4   // Vertex File1
#define RDI_INDEX_CAS0           0x0000002c //6x4 + 5 // Primitive Assembly Vtx0
#define RDI_INDEX_CAS1           0x0000002d //6x4 + 5 // Primitive Assembly Vtx1
#define RDI_INDEX_CAS2           0x0000002e //6x4 + 5 // Primitive Assembly Vtx2
#define RDI_INDEX_ASSM_STATE     0x0000002f //2       // Primitive Assembly State

// vtx file cache entry structure
typedef struct _tagRDIRAMDATA_VTX_FILEENTRY
{
    // RDI_INDEX_VTX_FILE0
    DWORD diff;
    DWORD spec;
    DWORD ptsize;
    DWORD mask;
    DWORD bdiff;
    DWORD bspec;
    DWORD fog;
    DWORD unused;

    // RDI_INDEX_VTX_FILE1
    DWORD pos[4];
    DWORD tex[4][4];

} D3DRDI_VTX_FILEENTRY;

// Primitive Assembly entry (from color assembly RAMS)
typedef struct _tagRDIRAMDATA_PAENTRY
{
    DWORD diff;
    DWORD spec;
    DWORD z, w;
    DWORD bdiff;
    DWORD bspec;
    DWORD fog;
    DWORD ptsize;
    DWORD tex[4][4];
    DWORD pos[4];
    DWORD unknown[4];

} D3DRDI_PAENTRY;

// Get a block of RDI data - !!! waits for gpu to become idle !!!
void WINAPI D3DRDI_GetRamData(DWORD index, DWORD address, void *pData, DWORD SizeOfData);

// Given an index 0-23 return the LRU cache entry data
void WINAPI D3DRDI_GetVTXFileEntry(DWORD index, D3DRDI_VTX_FILEENTRY *pentry);

// Get the next write slot for the 24 entry LRU cache
DWORD WINAPI D3DRDI_GetNextVTXLRUSlot();

// Given an index 0-2 return the PA data
void WINAPI D3DRDI_GetPAEntry(DWORD index, D3DRDI_PAENTRY *pentry);


/*
 * Dumper routines
 */

// Given an index 0-23, dump the LRU cache entry
void WINAPI D3DRDI_DumpVTXFileEntry(DWORD index);

// Given an index 0-135, dump the vertex shader program
void WINAPI D3DRDI_DumpVTXProgram(DWORD index, DWORD length);

// Given an index 0-191, dump the vertex shader constant values
void WINAPI D3DRDI_DumpVTXConstants(DWORD index, DWORD count);

// Dump VAB values
void WINAPI D3DRDI_DumpVAB();

// Dump the PA data
void WINAPI D3DRDI_DumpPA();

#ifdef __cplusplus
}
#endif

} // end namespace

#endif // _D3DRDI_H_

