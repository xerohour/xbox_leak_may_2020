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
|* Module: dload.c                                                           *|
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

#include <nv_ref.h>
#include <nvrm.h>
#include <os.h>

#include <dload.h>
#include <nvdload.h>


#define MakePtr(cast, ptr, addValue) (cast)((unsigned)(ptr) + (addValue))

#define BAIL(r,s,p)  {     \
   ret = (r);              \
   DBG_PRINT_STRING(p ,s); \
   DBG_BREAKPOINT();       \
   goto cleanup;           \
}


RM_STATUS initDload(PHWINFO pDev)
{
#if 0
	PRMINFO pRmInfo = (PRMINFO) pDev->pRmInfo;
	// The assumption is that the RMINFO global will be
	// filled to zero when we start.  When and if we ever
	// need to do anything more than zero'ing out structs
	// add a flag and test for it... or some such thing.
	pRmInfo->Dload.modules    = 0;
	pRmInfo->Dload.modulesSz  = 0; // How many NV_Image pointers we have allocated.
#endif

    return RM_OK;
}


//-----------------------------------------------------------------------------
// DldLoadModule
// 
// Given a pointer to the binary image as it reside(s,d) in BIOS/ROM or a file,
// perform the necessary steps to bring it in, make it executable, etc. and
// then wire it up.  The original image pointer given will not be held 
// so _that_ memory can be freed as necessary by the caller.
//-----------------------------------------------------------------------------

RM_STATUS DldLoadModule(PHWINFO pDev, NvU8 *pImage)
{
	
	PRMINFO     pRmInfo = (PRMINFO) pDev->pRmInfo;
	RM_STATUS   ret     = RM_OK;
	PDLD_MODULE pMod;
	unsigned    int checkSize, modIdx;
	NvU8        *pRawImg = 0;

	
	for ( modIdx = 0; modIdx < pRmInfo->Dload.modulesSz && pRmInfo->Dload.modules[modIdx] ; modIdx++ ) {;;;;}
	
	if ( RM_OK != osAllocMem((VOID**)&pMod, sizeof(DLD_MODULE) ) ) 
		BAIL(RM_ERROR,"NVRM: oom in DldLoadModule\n",DEBUGLEVEL_ERRORS);

	pMod->refCnt = 0;
	pMod->osHook = 0;

	// The set of all loaded images is managed as a dynamic array of pointers.
	// Zero pointers are by definition unused, and we just attempt to realloc
	// and copy the existing contents if the array needs to grow.  Simple simple.
	// XXX Some sort of locking necessary here

	if ( modIdx == pRmInfo->Dload.modulesSz )  {
		unsigned int j;
		void *p;
		
		// Attempt to grow the number of NV_Image pointers we're holding onto
		// by 4, and then copy the existing ones over...

		if ( RM_OK != osAllocMem( &p, sizeof( PDLD_MODULE ) * 4 ) ) 
			BAIL(RM_ERROR,"NVRM: can't grow PDLD_MODULE array\n",DEBUGLEVEL_ERRORS);
		
		((PDLD_MODULE*)p)[pRmInfo->Dload.modulesSz] = pMod;
		
		for ( j = 1; j < 4 ; j++ )  ((PDLD_MODULE*)p)[j] = 0;
		
		for ( j = 0; j < pRmInfo->Dload.modulesSz ; j++ ) {
			((PDLD_MODULE*)p)[j] =  pRmInfo->Dload.modules[j];
		}
		
		if ( pRmInfo->Dload.modules ) 
			osFreeMem( pRmInfo->Dload.modules );

		pRmInfo->Dload.modules = (PDLD_MODULE*)p;
		pRmInfo->Dload.modulesSz += 4;
		
	} 
	else {
		pRmInfo->Dload.modules[modIdx] = pMod;
	}
	
	// Wire up the pMod with the raw image offsets, etc.
	// The OS-layer must copy out anything it needs such that
	// after linking there will not be any references to this
	// raw image.  Again, the caller can free everything about
	// pImage... pMod however will stay 'round.  So the OS-layer
	// linking code can attach anything it needs via that extra
	// pointer at the end.
	
	pMod->header     = (PNV_ImageHeader)       pImage;

        // See if this is a compressed image.  If so, 'inflate' it here.
        
        if ( pMod->header->compressedSize ) {

                
                if ( RM_OK != osAllocMem((VOID **)&pRawImg, pMod->header->imageSize - sizeof(NV_ImageHeader) ) ) {
                        BAIL(RM_ERROR,"NVRM: oom in DldLoadModule\n",DEBUGLEVEL_ERRORS);
                }

                if ( RM_OK != DldInflate((NvU8*)(pMod->header + 1),pRawImg) ) {
                        BAIL(RM_ERROR,"NVRM: error in DldInflate\n",DEBUGLEVEL_ERRORS);
                }

                pMod->secTable   = (PSectionInfoEntry)    (pRawImg);

        }
        else {
                pMod->secTable   = (PSectionInfoEntry)    (pMod->header     + 1);
        }

        // Refer to 'secTable' above for start of image addressing...

        pMod->relocTable = (PRelocationInfoEntry) (pMod->secTable   + pMod->header->nSections);
        pMod->symTable   = (PSymbolTableEntry)    (pMod->relocTable + pMod->header->nRelocs);
        pMod->dataBuffer = (NvU8*)                (pMod->symTable   + pMod->header->nSymbols);
        pMod->codeBuffer = (NvU8*)                ((NvU8*)pMod->dataBuffer + pMod->header->dataSize);
        pMod->bssBuffer  = 0;
        pMod->footer     = (PNV_ImageFooter)      (pMod->codeBuffer + pMod->header->codeSize);


	checkSize = (unsigned int) ((NvU8*) pMod->footer - (NvU8*) pMod->secTable) + sizeof(NV_ImageFooter);


	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: header.magicNumber = ", pMod->header->magicNumber);
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: footer.magicNumber = ", pMod->footer->magicNumber); 
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: header.imageSize   = ", pMod->header->imageSize);
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: header.nSections   = ", pMod->header->nSections);
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: header.nRelocs     = ", pMod->header->nRelocs);
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: header.nSymbols    = ", pMod->header->nSymbols);
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: header.dataSize    = ", pMod->header->dataSize);
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: header.codeSize    = ", pMod->header->codeSize);
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: header.bssSize     = ", pMod->header->bssSize);
	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"NVRM: checkSize          = ", checkSize);	


	// Reject egregiously wrong images here.

	if ( (pMod->header->magicNumber != NV_DLOAD_HEADER_MAGIC) 
	     || (pMod->footer->magicNumber != NV_DLOAD_FOOTER_MAGIC) 
	     || (pMod->header->imageSize != (checkSize + sizeof(NV_ImageHeader)) ) ) 
		BAIL(RM_ERROR,"NVRM: bad image",DEBUGLEVEL_ERRORS);

	

	// Now ask the OS-layer to make the image executable by putting it
	// into executable pages, wiring up anything gnarly necessary, etc.

	if ( RM_OK != osDldLinkImg((VOID*)pMod) ) 
		BAIL(RM_ERROR,"NVRM: link error",DEBUGLEVEL_ERRORS);

	pMod->refCnt = 1;

#if 0
	// This is a placeholder for something other than just a "plug-in-the-fn-table" link step.
	{
#define NV_DLOAD_QUERY_IMAGE_LAYER

		int layer;
		void ** nvImgIface;
		typedef RM_STATUS (PDLD_MODULEQueryFn*)(int, void*);

		nvImgIface = (void**)osDloadFindImgSymbol(pMod, NV_DLOAD_IMAGE_ENTRY);
		if ( !nvImgIface ) BAIL(RM_ERROR,"NVRM: can't find image entry point table",DEBUGLEVEL_ERRORS);

		pQueryFn = (PDLD_MODULEQueryFn)nvImgIface[0];
		if ( !pQueryFn ) BAIL(RM_ERROR,"NVRM: can't find image query fn",DEBUGLEVEL_ERRORS);

		// Here we go.  Not much we can do now except assume this is going to work...
		ret = pQueryFn(NV_DLOAD_QUERY_IMAGE_LAYER,(void*)&layer);
		if ( ret != RM_OK ) BAIL(RM_ERROR,"NVRM: can't get image layer",DEBUGLEVEL_ERRORS);
		
		switch (layer) {
			case NV_CFGEX_DLOAD_HAL_LAYER:
			break;

			// Some point later we may want to extend this beyond the HAL?
			case NV_CFGEX_DLOAD_RM_LAYER:
			default:
				BAIL(RM_ERROR,"NVRM: bad image layer",DEBUGLEVEL_ERRORS);
		}
		
	}
#endif
    // Try to find the interface table.
    // Spew what we find.  Wire it in.
    {
	    VOID **iface;

	    iface = (VOID *)DldFindMemOffset(pMod,"_halimg"); // Hack name (see e.g. resman/module/nv20/halimg_nv20.c)

	    if ( ! iface ) {
		    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: can't find iface block");
	    } 
	    else {
		    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO,"NVRM: hal iface block :",iface);
		    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO,"NVRM:   fntable   :",(unsigned*)(iface[0]));
		    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO,"NVRM:   #clasdesc :",(unsigned*)(iface[1]));
		    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO,"NVRM:    clasdesc :",(unsigned*)(iface[2]));
		    DBG_PRINT_STRING_PTR(DEBUGLEVEL_TRACEINFO,"NVRM:    engdesc  :",(unsigned*)(iface[3]));
	    }

	    // Finish the deed.
	    pDev->pHalFuncs = (PHALFUNCS)iface[0];
    }



    cleanup:

        if ( pRawImg ) {

                osFreeMem((void*)pRawImg);

                pRawImg = 0;
        }

	if ( ret != RM_OK ) {

		if ( pMod ) {
		    
			if ( pMod->refCnt ) {
				(void)osDldUnlinkImg((VOID*)pMod);
			}
			
			if ( pRmInfo->Dload.modules[modIdx] == pMod ) 
				pRmInfo->Dload.modules[modIdx] = 0;
			
			osFreeMem(pMod);
		}
		
	}
    
	return ret;
}





// Given a symbol name, find it's address after linking.

NV_UINTPTR_T DldFindMemOffset(PDLD_MODULE pMod, char *symname) 
{
	unsigned i = 0;
	NvU32 nSymbols = pMod->header->nSymbols;

	// find the symbol with this index
	for (i = 0; i < nSymbols; i++)
	{
		if ( !osStringCompare((const char *)pMod->symTable[i].name,symname) ) {
			return DldGetMemOffset(pMod,pMod->symTable[i].symIndex);
		}
	}

	return ~(NV_UINTPTR_T)0;
}


NV_UINTPTR_T DldGetImageOffset(PDLD_MODULE pMod, unsigned symIndex)
{
	return DldGetMemOffset(pMod,symIndex) - (NV_UINTPTR_T)pMod->dataBuffer;
}


NvU16 DldGetSymbolSection(PDLD_MODULE pMod, unsigned symIndex)
{
	NvU16 j, nSections = pMod->header->nSections;
	NvU32 i, nSymbols  = pMod->header->nSymbols;
	
	// find the symbol with this index
	for (i = 0; i < nSymbols; i++)
	{
		if (pMod->symTable[i].symIndex == symIndex)
		{
			// find the section with this section number and return the memOffset
			// printf("searching for %s's memoffset\n",nv_img.symTable[i].name);
			for (j = 0; j < nSections; j++)
			{
				if(pMod->secTable[j].secNumber == pMod->symTable[i].secNumber)
				{
					return j;
				}
			}
		}
	}

	return ~(NvU16)0;
}


NV_UINTPTR_T DldGetSectionMemOffset(PDLD_MODULE pMod, unsigned secNumber)
{
	NvU16 i, nSections = pMod->header->nSections;

	for ( i=0; i < nSections; i++ ) {
		if ( pMod->secTable[i].secNumber == secNumber ) {
			return (unsigned) pMod->secTable[i].memOffset;
		}
	}
	
	return ~(NV_UINTPTR_T)0;
}


NV_UINTPTR_T DldGetMemOffset(PDLD_MODULE pMod, unsigned symIndex)
{
	NvU16 j, nSections = pMod->header->nSections;
	NvU32 i, nSymbols = pMod->header->nSymbols;
	
	// find the symbol with this index
	for (i = 0; i < nSymbols; i++)
	{
		if (pMod->symTable[i].symIndex == symIndex)
		{
			// find the section with this section number and return the memOffset
			for (j = 0; j < nSections; j++)
			{
				if(pMod->secTable[j].secNumber == pMod->symTable[i].secNumber)
				{
					return((NV_UINTPTR_T) pMod->secTable[j].memOffset + pMod->symTable[i].value);
				}
			}
		}
	}
	
	// The image should never have made it this far...
	// Our images are fully self contained and make no
	// external references...

	DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS,"NVRM: Error: getMemOffset(...) symIndex was not found! ", symIndex);
	
	DBG_BREAKPOINT();
	return ~(NV_UINTPTR_T)0;
}


//------------------------------------------------------------------
// void printImgInfo()
//------------------------------------------------------------------
void DldPrintImgInfo(PDLD_MODULE pMod)
{
	DldPrintSectionInfo(pMod);
	DldPrintRelocationInfo(pMod);
	DldPrintSymbolTableInfo(pMod);
}


//------------------------------------------------------------------
// void printSectionInfo(unsigned nSections)
//------------------------------------------------------------------
void DldPrintSectionInfo(PDLD_MODULE pMod)
{
	unsigned int i = 0;

	DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: PRINTING pMod->secTable ----------------------------");
	for (i = 0; i < pMod->header->nSections; i++)
	{
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"secNumber:      ", pMod->secTable[i].secNumber);
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"type:           ", pMod->secTable[i].type);
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"dataSize:       ", pMod->secTable[i].dataSize);
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"numRelocs:      ", pMod->secTable[i].numRelocs);
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"memOffset:      ", pMod->secTable[i].memOffset);

	}
}


//------------------------------------------------------------------
// void printRelocationInfo(unsigned nRelocs)
//------------------------------------------------------------------
void DldPrintRelocationInfo(PDLD_MODULE pMod)
{
	unsigned int i = 0;

	DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: PRINTING pMod->relocTable --------------------------");
	for (i = 0; i < pMod->header->nRelocs; i++)
	{
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"address:        ", pMod->relocTable[i].address);
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"symIndex:       ", pMod->relocTable[i].symIndex);
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"secNumber:      ", pMod->relocTable[i].secNumber);
	}
}


//------------------------------------------------------------------
// void printSymbolTableInfo(unsigned nSymbols)
//------------------------------------------------------------------
void DldPrintSymbolTableInfo(PDLD_MODULE pMod)
{
	unsigned int i = 0;

	DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"NVRM: PRINTING pMod->symTable ----------------------------");
	for (i = 0; i < pMod->header->nSymbols; i++)
	{
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"symIndex:       ", pMod->symTable[i].symIndex);
		DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,"name:           ");
		DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO,pMod->symTable[i].name);
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO,"secNumber:      ", pMod->symTable[i].secNumber);

	}
}









