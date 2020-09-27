/*****************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2001 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2001  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
|*****************************************************************************|
|*                                                                           *|
|* File: nvdload.h                                                           *|
|*                                                                           *|
|*   This file describes the image layout for dynamically loadable driver    *|
|* code.                                                                     *|
|*****************************************************************************|
|*                                                                           *|
|* History:                                                                  *|
|*       Ken Adams (kadams)     02/2001 - wrote it based upon investigatory  *|
|*                                        code from Todd Poynter.            *|
|*                                                                           *|
\*****************************************************************************/

#ifndef _NVDLOAD_H_
#define _NVDLOAD_H_


// defines


#define NV_DLOAD_SECTION_TYPE_DATA  0
#define NV_DLOAD_SECTION_TYPE_CODE  1
#define NV_DLOAD_SECTION_TYPE_BSS   2

#define NV_DLOAD_IMAGE_ENTRY  "nvimg"

#define NV_DLOAD_MAX_SYMWIDTH      40
#define NV_DLOAD_HEADER_MAGIC      0xdead
#define NV_DLOAD_FOOTER_MAGIC      0xbeef


// This is just a swag.  Don't know what if any 
// other than X86 __stdcall will ever make sense.
// These are just arbitrarily made-up numbers.

#define NV_DLOAD_IMAGE_ARCH_X86_STDCALL     0xbe01
#define NV_DLOAD_IMAGE_ARCH_IA64            0xbe02
#define NV_DLOAD_IMAGE_ARCH_PPC             0xbe03


//XXX These entry definitions should probably be 32b vs 64b ABI specific.
// For instance, suggesting NvU32 for memOffset below doesn't make sense for IA64.


typedef struct section_info_entry
{
	NvU16    type;			// data or code
	NvU16    secNumber;		// the section number
	NvU32    dataSize;		// size of the data within this section
	NvU32    numRelocs;		// number of relocations for this section
	NvU32    memOffset;	        // offset within our malloced buffer to this section
} SectionInfoEntry, *PSectionInfoEntry;

typedef struct relocation_info_entry
{
	NvU16    type;                  // relo type (arch specific)
	NvU16    secNumber;		// the section number
	NvU32    symIndex;		// symbol table index
	NvU32    address;		// offset from the memOffset where we do our relocation
} RelocationInfoEntry, *PRelocationInfoEntry;

typedef struct symbol_table_entry
{
	NvU32           value;          // offset (section?)
	NvU32           symIndex;       // symbol table index
	NvU16           secNumber;      // symbols section number
	NvU8	        name[NV_DLOAD_MAX_SYMWIDTH]; // symbol name
} SymbolTableEntry, *PSymbolTableEntry;

typedef struct nv_image_header
{
	NvU16 magicNumber;
	NvU16 arch;
	NvU32 imageSize;       // size of the entire image including this header (after any decompression).
        NvU32 compressedSize;  // non-zero if the image behind the header should be 'inflated'.
	NvU16 nSections;       
	NvU32 nRelocs;
	NvU32 nSymbols;
	NvU32 dataSize;
	NvU32 codeSize;
	NvU32 bssSize;
} NV_ImageHeader, *PNV_ImageHeader;

typedef struct nv_image_footer
{
	unsigned magicNumber;
} NV_ImageFooter, *PNV_ImageFooter;

typedef struct nv_image
{
	NV_ImageHeader			*header;
	SectionInfoEntry		*secTable;
	RelocationInfoEntry		*relocTable;
	SymbolTableEntry		*symTable;
	NvU8				*dataBuffer;
	NvU8				*codeBuffer;
	NvU8                            *bssBuffer;
	NV_ImageFooter			*footer;
} NV_Image, *PNV_Image;


//-----------------------------------------------------------------------------
// Raw Image Layout
//                    NV_ImageHeader
//                    SectionInfoEntry[...]
//                    RelocationInfoEntry[...]
//                    SymbolTableEntry[...]
//                    DataBuffer[...]
//                    CodeBuffer[...]
//                    NV_ImageFooter
//-----------------------------------------------------------------------------

#endif 

