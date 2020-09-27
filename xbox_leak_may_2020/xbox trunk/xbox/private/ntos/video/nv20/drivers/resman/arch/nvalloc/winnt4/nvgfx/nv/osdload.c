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
|* Module: osdload.c                                                         *|
|*    These are the OS-specific bits of implementing dynamic loading of      *|
|* driver code.  Be that from file, ROM/BIOS, etc.                           *|
|*                                                                           *|
|*****************************************************************************|
|*                                                                           *|
|* History:                                                                  *|
|*       Ken Adams (kadams)     02/2001 - wrote it based upon investigatory  *|
|*                                        code from Todd Poynter.            *|
|*                                                                           *|
\*****************************************************************************/

#include <ntddk.h>
#include <nvrm.h>
#include <os.h>
#include <nvos.h>
#include <nvhw.h>
#include <Nvcm.h>
#include <dload.h>    


// At the moment, only supported for Win2k+
#if (_WIN32_WINNT >= 0x0500)


#define BAIL(r,s,p)  {     \
   ret = (r);              \
   DBG_PRINT_STRING(p ,s); \
   DBG_BREAKPOINT();       \
   goto cleanup;           \
}

static RM_STATUS setupMemOffsets(PDLD_MODULE pImg);
static RM_STATUS setupRelocations(PDLD_MODULE pImg);

//------------------------------------------------------------------
// osDldLinkImg
//
//    As we come into this routine, pImg is pointing into the raw
// image as given to the DldLoadImg interface.  We need to copy
// everything we need out of that struct (and what it points to) 
// and replace it memory malloc'd for what we need, here.  
// Oh yeah, we also have to do any relocations, get the memory 
// offsets setup and the like.  This is the real meat of the
// linking process.  For WindowsNT anyway this is pretty easy.
// We can just simply assign memory offsets in serial fashion
// to the various sections.  Addresses have to be assigned to 
// all the symbols.  Then any relocations have to be patched in.
//
//------------------------------------------------------------------
RM_STATUS osDldLinkImg ( VOID *_pMod )
{

	PDLD_MODULE             pMod = (PDLD_MODULE) _pMod;
	PNV_ImageHeader         pHeader;
	PSectionInfoEntry	pSections;
	PRelocationInfoEntry	pRelocs;
	PSymbolTableEntry	pSymbols;
	NvU8		       *pCode;
	NvU8                   *pData;
	NvU8                   *pBss;
	int                     ret = 0;
	unsigned int            checkSize;

	pMod->footer = 0;  // we no longer care about the footer

	// pMod->header should ALWAYS be valid.  ??? The rest may or may not 
	// be needed later ???  For kicks fuse data,code,bss because we know
	// we won't ever need to remove them individually.  We can probably
	// remove the sections,relocs,symbols at some point though...

	ret |= osAllocMem(&pHeader,   sizeof(NV_ImageHeader)      * 1);
	ret |= osAllocMem(&pSections, sizeof(SectionInfoEntry)    * pMod->header->nSections);
	ret |= osAllocMem(&pRelocs,   sizeof(RelocationInfoEntry) * pMod->header->nRelocs);
	ret |= osAllocMem(&pSymbols,  sizeof(SymbolTableEntry)    * pMod->header->nSymbols);
	ret |= osAllocMem(&pData,     sizeof(char) * (  pMod->header->dataSize 
						      + pMod->header->codeSize 
						      + pMod->header->bssSize  ) );
	pCode = pData + pMod->header->dataSize;
	pBss  = pCode + pMod->header->codeSize;
			  
	if ( ret != RM_OK ) 
		BAIL(RM_ERROR,"NVRM: oom section headers",DEBUGLEVEL_ERRORS);

	osMemCopy((NvU8*)pHeader,  (NvU8*)pMod->header,     sizeof(NV_ImageHeader)      * 1);
	osMemCopy((NvU8*)pSections,(NvU8*)pMod->secTable,   sizeof(SectionInfoEntry)    * pMod->header->nSections);
	osMemCopy((NvU8*)pRelocs,  (NvU8*)pMod->relocTable, sizeof(RelocationInfoEntry) * pMod->header->nRelocs);
	osMemCopy((NvU8*)pSymbols, (NvU8*)pMod->symTable,   sizeof(SymbolTableEntry)    * pMod->header->nSymbols);
	osMemCopy((NvU8*)pData,    (NvU8*)pMod->dataBuffer, sizeof(NvU8)                * pMod->header->dataSize);
	osMemCopy((NvU8*)pCode,    (NvU8*)pMod->codeBuffer, sizeof(NvU8)                * pMod->header->codeSize);


	pMod->header     = pHeader;    	pHeader   = 0;
	pMod->secTable   = pSections;   pSections = 0;
	pMod->relocTable = pRelocs;     pRelocs   = 0;
	pMod->symTable   = pSymbols;    pSymbols  = 0;
	pMod->dataBuffer = pData;       pData     = 0;
	pMod->codeBuffer = pCode;       pCode     = 0;
	pMod->bssBuffer  = pBss;        pBss      = 0;


	if ( RM_OK != (ret = setupMemOffsets(pMod) ) ) {
		goto cleanup;
	}

	// This step performs the actual "linking".  
	ret = setupRelocations(pMod);
		

 cleanup:
	if ( ret != RM_OK ) {
		if ( pHeader )   osFreeMem(pHeader); 
		if ( pSections ) osFreeMem(pSections);
		if ( pRelocs )   osFreeMem(pRelocs);
		if ( pSymbols )  osFreeMem(pSymbols);
		if ( pData )     osFreeMem(pData);
	}

	return ret;
}


//------------------------------------------------------------------
// osDldUnlinkImg
//
// Jettison the image.
//
//------------------------------------------------------------------
RM_STATUS osDldUnlinkImg ( VOID *_pMod )
{
	PDLD_MODULE pMod = (PDLD_MODULE)_pMod;
	int ret = RM_OK;

	if ( pMod->header )     { osFreeMem(pMod->header);     pMod->header     = 0; }
	if ( pMod->secTable )   { osFreeMem(pMod->secTable);   pMod->secTable   = 0; }
	if ( pMod->relocTable ) { osFreeMem(pMod->relocTable); pMod->relocTable = 0; }
	if ( pMod->symTable )   { osFreeMem(pMod->symTable);   pMod->symTable   = 0; }
	if ( pMod->dataBuffer ) { 
		osFreeMem(pMod->dataBuffer); 
		pMod->dataBuffer = pMod->codeBuffer = pMod->bssBuffer = 0; 
	}

	return ret;
}


#ifndef _WIN64

//------------------------------------------------------------------
//  setupMemOffsets
//
//  Cruise through the sections, assigning an offset within the
//  proper buffer for each.  In otherwords, place the sections
//  in memory...
//------------------------------------------------------------------
RM_STATUS setupMemOffsets(PDLD_MODULE pMod)
{
	unsigned int i = 0;
	NvU16       nSections = pMod->header->nSections;
	NvU8        *dataPtr  = pMod->dataBuffer;
	NvU8        *codePtr  = pMod->codeBuffer;
        NvU8        *bssPtr   = pMod->bssBuffer;
	int          ret = RM_OK;
	
	for (i = 0; i < nSections; i++)
	{
		switch ( pMod->secTable[i].type ) {

			case NV_DLOAD_SECTION_TYPE_DATA:
				pMod->secTable[i].memOffset = (NvU32)(NV_UINTPTR_T)dataPtr; // XXX - ain't right 32 vs 64b
				dataPtr += pMod->secTable[i].dataSize;
				break;
			case NV_DLOAD_SECTION_TYPE_CODE:
				pMod->secTable[i].memOffset = (NvU32)(NV_UINTPTR_T)codePtr; // XXX - ain't right 32 vs 64b
				codePtr += pMod->secTable[i].dataSize;
				break;
			case NV_DLOAD_SECTION_TYPE_BSS:
				pMod->secTable[i].memOffset = (NvU32)(NV_UINTPTR_T)bssPtr;  // XXX - ain't right 32 vs 64b
				bssPtr += pMod->secTable[i].dataSize;
				break;
			default:
				BAIL(RM_ERROR,"NVRM:  setupMemOffsets(...) unknown type",DEBUGLEVEL_ERRORS);
		}
	}

 cleanup:

	return ret;
}



// Defined in winnt.h.  But we don't need that whole mess.  These aren't likely to change, ever.
#ifndef IMAGE_REL_I386_ABSOLUTE

#define IMAGE_REL_I386_ABSOLUTE         0x0000  // Reference is absolute, no relocation is necessary
#define IMAGE_REL_I386_DIR16            0x0001  // Direct 16-bit reference to the symbols virtual address
#define IMAGE_REL_I386_REL16            0x0002  // PC-relative 16-bit reference to the symbols virtual address
#define IMAGE_REL_I386_DIR32            0x0006  // Direct 32-bit reference to the symbols virtual address
#define IMAGE_REL_I386_DIR32NB          0x0007  // Direct 32-bit reference to the symbols virtual address, base not included
#define IMAGE_REL_I386_SEG12            0x0009  // Direct 16-bit reference to the segment-selector bits of a 32-bit virtual address
#define IMAGE_REL_I386_SECTION          0x000A
#define IMAGE_REL_I386_SECREL           0x000B
#define IMAGE_REL_I386_REL32            0x0014  // PC-relative 32-bit reference to the symbols virtual address

#endif


//------------------------------------------------------------------
//  setupRelocations
//
//  This routine walks all of the registered relocations in the
//  NVImage and fixes them up as necessary.  Note that it calls
//  back into the image-format-independent code (Dld*) to get
//  symbol offsets and the like.
//------------------------------------------------------------------
RM_STATUS setupRelocations( PDLD_MODULE pMod )
{
	int         ret = RM_OK;
	unsigned int i, j, relocIndex;
	NV_UINTPTR_T relocAddr;
	NvU16        nSections  = pMod->header->nSections;
	NvU32        nRelocs    = pMod->header->nRelocs;


	for (i = 0; i < nSections; i++)	{

		if ( !pMod->secTable[i].numRelocs ) continue;

		// find the first relocation table index with the same sec number 
		// as the section we are currently working on.

		for ( relocIndex = 0; 
		      ( relocIndex < pMod->header->nRelocs )
			      && (pMod->relocTable[relocIndex].secNumber != pMod->secTable[i].secNumber ) ; 
		      relocIndex++ ) {;;;;}

		if ( relocIndex == pMod->header->nRelocs ) 
			BAIL(RM_ERROR,"NVRM: invalid relocIndex",DEBUGLEVEL_ERRORS);

		for ( j = 0; j < pMod->secTable[i].numRelocs; j++ ) {

			unsigned symIndex;

			// get the target address
			relocAddr = (unsigned) (pMod->secTable[i].memOffset + pMod->relocTable[relocIndex+j].address);
				
			symIndex = pMod->relocTable[relocIndex + j].symIndex;

			switch ( pMod->relocTable[relocIndex + j].type ) {
				// XXX Note: the (NvU32) casts after *Get*Offset aren't necessary
				// for x86.  I've added them for ia64 compilation checking, even though
				// we won't be running throught this code on that platform.
				case IMAGE_REL_I386_DIR32:
					*((NvU32*)relocAddr) = DldGetMemOffset(pMod,symIndex);
					break;

				case IMAGE_REL_I386_DIR32NB:
					*((NvU32*)relocAddr) = DldGetImageOffset(pMod,symIndex);
					break;

				case IMAGE_REL_I386_SECTION:
					*((NvU16*)relocAddr) = DldGetSymbolSection(pMod,symIndex);
					break;

				case IMAGE_REL_I386_SECREL:
					*((NvU32*)relocAddr) = 
						(NvU32) (DldGetMemOffset(pMod,symIndex) 
							 - DldGetSectionMemOffset(pMod, DldGetSymbolSection(pMod,symIndex)));
					break;

				case IMAGE_REL_I386_REL32:
					if ( pMod->secTable[i].type == NV_DLOAD_SECTION_TYPE_DATA ) 
						BAIL(RM_ERROR,"NVRM: unsupported relative data section reloc",DEBUGLEVEL_ERRORS);

					*((NvU32*)relocAddr) = (NvU32) ( DldGetMemOffset(pMod,symIndex) 
								 - (relocAddr + 4) ); // +4 since rel is from _next_ instr.
					break;

				    case IMAGE_REL_I386_ABSOLUTE:
				    case IMAGE_REL_I386_DIR16:
				    case IMAGE_REL_I386_REL16:
				    case IMAGE_REL_I386_SEG12:
				    default:
					    BAIL(RM_ERROR,"NVRM: unsupported reloc",DEBUGLEVEL_ERRORS);
			}
#ifdef NV_DBG_RELOC
			DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO," f ",(unsigned*)relocAddr);
			DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO,"  a",(unsigned*)*(unsigned*)relocAddr);
#endif
			
		}
	}

 cleanup:
	
	return ret;
	
}


#else
// Win64 
RM_STATUS setupMemOffsets(PDLD_MODULE pImg)
{
	return RM_ERROR;
}

RM_STATUS setupRelocations(PDLD_MODULE pImg)
{
	return RM_ERROR;
}


#endif



#endif










