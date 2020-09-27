//
// INTEL CONFIDENTIAL.  Copyright (c) 1997-2000 Intel Corp. All rights reserved.
//
// turn off the you may not inited with warning
#pragma warning( disable : 4701 )
#pragma warning( disable : 4800 )

#include "..\stdafx.h"
#include "..\testobj.h"
#include "memutils.h"
#include "xmemory.h"
#include "stress.h"




//__declspec(thread)*/ DWORD_PTR stress::m_curbot,stress::m_curtop;

//******************************************************************************
// Function name	: stress::stress
// Description	    : Allocates the buffers for the read and write logs and
//                    the buffer for the bitmapped accessed list.
// Return type		: void 
// Argument         : SIZE_T BlockSize - the size of the memory for which to
//                                       generate the list.
//******************************************************************************
stress::stress(SIZE_T BlockSize, CXModuleMemory * hCXMemparmam)
{
	// 
	hCXMem = hCXMemparmam; 
	m_nTestSize        = BlockSize;
	m_nMaxTransferSize = m_nTestSize / 0x200;
	m_nLogSize         = (m_nTestSize / m_nMaxTransferSize) * 3 * 6 * sizeof(SIZE_T) / 4;
	
	// The bit mapped allocation array
	m_pdwUsed  = new DWORD[m_nTestSize / 32];
	
	if(m_pdwUsed == NULL)
	{
		hCXMem->errOutOfMemoryQ(sizeof(DWORD) * m_nTestSize / 32);
		// exit
		throw CXmtaException(99,NULL);
	}
	
	//The log of read and write locations and sizes
	m_pWriteLog = new log[m_nLogSize];
	
	if (m_pWriteLog == NULL)
	{
/* fol		errOutOfMemoryQ.Report(sizeof(log) * m_nLogSize);
		wt_done();*/
	}
	
	m_pReadLog = new log[m_nLogSize];
	
	if (m_pReadLog == NULL)
	{
		hCXMem->errOutOfMemoryQ(sizeof(log) * m_nLogSize);
		// exit
		throw CXmtaException(99,NULL);
	}
}


//******************************************************************************
// Function name	: stress::~stress
// Description	    : Deletes the buffers allocated by the constructor.
// Return type		: void
//******************************************************************************
stress::~stress()
{
	delete [] m_pdwUsed;
	delete [] m_pWriteLog;
	delete [] m_pReadLog;
}


//******************************************************************************
// Function name	: stress::build_read_log
// Description	    : Sets up to build the read log.
// Return type		: void 
//******************************************************************************
void stress::build_read_log()
{
	build_log(m_pReadLog);
}


//******************************************************************************
// Function name	: stress::build_write_log
// Description	    : Sets up to build the write log.
// Return type		: void 
//******************************************************************************
void stress::build_write_log()
{
	build_log(m_pWriteLog);
}


//******************************************************************************
// Function name	: stress::build_log
// Description	    : builds a list of addresses and sizes that completely
//                    cover an amount of memory (m_nTestSize).
// Return type		: void 
// Argument         : log* pLog - either m_pReadLog or m_pWriteLog.
//******************************************************************************
void stress::build_log(log* pLog)
{
	DWORD_PTR loc,size,index;

	// init the bit arrays to 0.  Each bit indicates whether a particular byte
	// in the test block has been addressed yet.
	ZeroMemory(m_pdwUsed,m_nTestSize / 8);
	
	// Initialize the static thread-safe variables to mark the window in the
	// m_pdwUsed array where unused addresses can be found.  This speeds up the
	// search for unused addresses when there aren't many left.
	m_curtop = m_nTestSize;
	m_curbot = 0;

	// build the read log of where we are going to read
	for(index = 0; index < m_nLogSize; index++)
	{
		// returns an address and size and marks them as used in the bit array.
		if(get_location(&loc,&size))
		{
			pLog[index].address = loc;
			pLog[index].size    = size;
		}
		else
			break;
	}

	// Keep alive.
	hCXMem->ReportDebug(1,L"test");
	hCXMem->CheckAbort(0);

	// The log varies in size.  An amount of memory much bigger than the
	// average log size was allocated by InitStress, but just in case...
	if(index == m_nLogSize)
	{
		hCXMem->errLogOverflowQ(index);
		// exit
		throw CXmtaException(99,NULL);
	}
	
	// end the log with an entry of size 0
	// The SMP tests could miss this since type don't look at every array entry,
	// NT supports up to 32 processors so we'll put 32 terminators.
	for(DWORD_PTR i = 0; i < 32; i++)
		pLog[index++].size = 0;

#if defined(_DEBUG)
	// This algorithm verifies that the generated log
	// accesses every bit of the m_pUsed array uniquely.
	
	// First, check that all bits of the m_pUsed array are on
	// after log generation, then zero them.
	LPDWORD end = m_pdwUsed + m_nTestSize / 32;
	for(LPDWORD x = m_pdwUsed; x < end; x++)
		ASSERT(*x == 0xFFFFFFFF);
	ZeroMemory(m_pdwUsed,m_nTestSize / 8);

	// Now walk the log.  For each address and size, verify
	// that all bits are off, then set them using a different
	// algorithm that get_location() uses.
	for(i = 0; pLog[i].size; i++)
	{
		DWORD_PTR size = pLog[i].size;
		DWORD_PTR addr = pLog[i].address;
		
		// determine the starting and ending dword and bit in the bit array
		// for the above address and size.
		LPDWORD pmindword = m_pdwUsed + addr / 32;
		DWORD   minbit    = (DWORD)(addr % 32);
		LPDWORD pmaxdword = m_pdwUsed + (addr + size - 1) / 32;
		DWORD   maxbit    = (DWORD)((addr + size - 1) % 32);

		// Compute the bits that must be set in the first dword and set them.
		DWORD bits = size > 32 ? 32 : size;
		DWORD mask = (0xFFFFFFFF >> (32-bits)) << minbit;
		ASSERT((*pmindword & mask) == 0);
		*pmindword |= mask;

		// if the first dword is the last dword, we are done with this address/size.
		if(pmaxdword == pmindword) continue;

		// Compute the bits that must be set in the last dword and set them.
		mask = 0xFFFFFFFF >> (31-maxbit);
		ASSERT((*pmaxdword & mask) == 0);
		*pmaxdword |= mask;

		// Turn on the bits in all the intermediate dwords.
		for(x = pmindword + 1; x < pmaxdword; x++)
		{
			ASSERT(*x == 0);
			*x = 0xFFFFFFFF;
		}
	}
	
	// Finally, verify all bits are on in the m_pUsed array again.
	for(x = m_pdwUsed; x < end; x++)
		ASSERT(*x == 0xFFFFFFFF);
#endif
}


//******************************************************************************
// Function name	: stress::StressWrite
// Description	    : Iterate through the write log and perform a write from the
//                    source to the destination buffer at each address and size
//                    specified.  On IA-32, this algorithm is meant to test the
//                    hardware byte-swapping capability of the processors.  On
//                    IA-32, perform DWORD writes if the size is divisible
//                    by four, WORD if by two, BYTE otherwise.  For IA-64, this
//                    algorithm simply mixes BYTE, WORD, and DWORD accesses, but
//                    makes sure the addresses are data-aligned.
// Return type		: void 
// Argument         : LPBYTE pSrcBuf  - source data buffer
// Argument         : LPBYTE pDestBuf - destination data buffer
//******************************************************************************
void stress::StressWrite(LPBYTE pSrcBuf,LPBYTE pDestBuf)
{
	DWORD_PTR index = 0,loc,size;
	
#ifdef _WIN64
	// The write log addresses must be multiplied by four to get DWORD-aligned
	// addresses.  Size is in DWORDs, so for BYTE and WORD modes the size of
	// the move must be multiplied.
	for(;;)
	{
		loc   = m_pWriteLog[index].address * 4;
		size  = m_pWriteLog[index].size;

		// check for end-of-log
		if(size == 0) 
			break;

		switch(random() % 3)
		{	
		case  0:  AsmDwordMemMov(pDestBuf + loc,pSrcBuf + loc,(DWORD)size);      break;
		case  1:  AsmWordMemMov (pDestBuf + loc,pSrcBuf + loc,(DWORD)size * 2);  break;
		default:  AsmByteMemMov (pDestBuf + loc,pSrcBuf + loc,(DWORD)size * 4);  break;
		} 

		index++;
	}
#else
	// For IA-32, the write log addresses are fine and the size is in bytes, so it must
	// be divided for WORD and DWORD moves.
	for(;;)
	{
		loc   = m_pWriteLog[index].address;
		size  = m_pWriteLog[index].size;

		// check for end-of-log
		if(size == 0) 
			break;

		switch(size % 4)
		{	
		case  0:  AsmDwordMemMov(pDestBuf + loc,pSrcBuf + loc,(DWORD)size / 4);  break;
		case  2:  AsmWordMemMov (pDestBuf + loc,pSrcBuf + loc,(DWORD)size / 2);  break;
		default:  AsmByteMemMov (pDestBuf + loc,pSrcBuf + loc,(DWORD)size);      break;
		} 

		index++;
	}
#endif
}


//******************************************************************************
// Function name	: stress::StressRead
// Description	    : Iterate through the read log reading and comparing the
//                    destination buffer to the source buffer at each address and
//                    size specified.  Perform DWORD writes if the size is
//                    divisible by four, WORD if by two, BYTE otherwise.
// Return type		: LPBYTE          - the virtual address of a failure in the
//                                      destination buffer or NULL if no failure occurred.
// Argument         : LPBYTE pSrcBuf  - the source buffer.
// Argument         : LPBYTE pDestBuf - the destination buffer.
//******************************************************************************
LPBYTE stress::StressRead(LPBYTE pSrcBuf,LPBYTE pDestBuf)
{
	DWORD_PTR  index = 0,loc,size;
	LPVOID pVirt;

#ifdef _WIN64
	// The read log addresses must be multiplied by four to get DWORD-aligned
	// addresses.  Size is in DWORDs, so for BYTE and WORD modes the size of
	// the compare must be multiplied.
	for(;;)
	{
		loc   = m_pReadLog[index].address * 4;
		size  = m_pReadLog[index].size;

		// check for end-of-log
		if(size == 0) 
			return 0;

		switch(random() % 3)
		{
		case  0:  pVirt = AsmDwordMemCmp(pDestBuf + loc,pSrcBuf + loc,(DWORD)size);      break;
		case  1:  pVirt = AsmWordMemCmp (pDestBuf + loc,pSrcBuf + loc,(DWORD)size * 2);  break;	
		default:  pVirt = AsmByteMemCmp (pDestBuf + loc,pSrcBuf + loc,(DWORD)size * 4);  break;
		} 
		
		if(pVirt)
			return (LPBYTE)pVirt;

		index++;
	}
#else
	// For IA-32, the read log addresses are fine and the size is in bytes, so it must
	// be divided for WORD and DWORD compares.
	for(;;)
	{
		loc   = m_pReadLog[index].address;
		size  = m_pReadLog[index].size;

		// check for end-of-log
		if(size == 0) 
			return 0;

		switch(size % 4)
		{
		case  0:  pVirt = AsmDwordMemCmp(pDestBuf + loc,pSrcBuf + loc,(DWORD)size / 4);  break;
		case  2:  pVirt = AsmWordMemCmp (pDestBuf + loc,pSrcBuf + loc,(DWORD)size / 2);  break;	
		default:  pVirt = AsmByteMemCmp (pDestBuf + loc,pSrcBuf + loc,(DWORD)size);      break;
		} 
		
		if(pVirt)
			return (LPBYTE)pVirt;

		index++;
	}
#endif
}


//******************************************************************************
// Function name	: stress::StressWriteDword
// Description	    : Iterate through the write log and perform a write from the
//                    source to the destination buffer at each address and size
//                    specified.  Size is in DWORDs and performs DWORD writes only.
// Return type		: void 
// Argument         : LPBYTE pSrcBuf
// Argument         : LPBYTE pDestBuf
//******************************************************************************
void stress::StressWriteDword(LPBYTE pSrcBuf,LPBYTE pDestBuf)
{
	DWORD_PTR index = 0,loc,size;
	
	for(;;)
	{
		loc  = m_pWriteLog[index].address;
		size = m_pWriteLog[index].size;

		// check for end-of-log
		if(size == 0) 
			break;

		loc *= 4;  // build_log() assumes byte addresses, convert to dword addresses
		AsmDwordMemMov(pDestBuf + loc,pSrcBuf + loc,(DWORD)size);
		index++;
	}
}


//******************************************************************************
// Function name	: stress::StressReadDword
// Description	    : Iterate through the read log reading and comparing the
//                    destination buffer to the source buffer at each address and
//                    size specified.  Size is in DWORDs and performs DWORD reads only.
// Return type		: LPBYTE          - the virtual address of a failure in the
//                                      destination buffer or NULL if no failure occurred.
// Argument         : LPBYTE pSrcBuf  - the source buffer.
// Argument         : LPBYTE pDestBuf - the destination buffer.
//******************************************************************************
LPBYTE stress::StressReadDword(LPBYTE pSrcBuf,LPBYTE pDestBuf)
{
	DWORD_PTR   index = 0,loc,size;
	LPDWORD pdwVirt;

	for(;;)
	{
		loc  = m_pReadLog[index].address;
		size = m_pReadLog[index].size;
		
		// check for end-of-log
		if(size == 0) 
			return 0;
		
		loc *= 4;  // build_log() assumes byte addresses, convert to dword addresses
		
		pdwVirt = AsmDwordMemCmp(pDestBuf + loc,pSrcBuf + loc,(DWORD)size);
	
		if(pdwVirt)
			return (LPBYTE)pdwVirt;

		index++;
	}
}



//******************************************************************************
// Function name	: stress::StressStrideWrite
// Description	    : Iterate through the write log starting at offset and perform
//                    a write from the source to the destination buffer at every
//                    nth address and size specified by stride.  Perform DWORD
//                    writes if the size is divisible by four, WORD if by two,
//                    BYTE otherwise.
// Return type		: void 
// Argument         : DWORD  index    - starting log index
// Argument         : DWORD  stride   - specifies writing every nth address in the log.
// Argument         : LPBYTE pSrcBuf  - the source buffer.
// Argument         : LPBYTE pDestBuf - the destination buffer.
//******************************************************************************
void stress::StressStrideWrite(DWORD index,DWORD stride,LPBYTE pSrcBuf,LPBYTE pDestBuf)
{
	DWORD_PTR loc,size;
	
#ifdef _WIN64
	// The write log addresses must be multiplied by four to get DWORD-aligned
	// addresses.  Size is in DWORDs, so for BYTE and WORD modes the size of
	// the move must be multiplied.
	for(;;)
	{
		loc   = m_pWriteLog[index].address * 4;
		size  = m_pWriteLog[index].size;

		// check for end-of-log.
		if(size == 0)
			break;
 		
		switch(random() % 3)
		{	
		case  0:  AsmDwordMemMov(pDestBuf + loc,pSrcBuf + loc,(DWORD)size);      break;
		case  1:  AsmWordMemMov (pDestBuf + loc,pSrcBuf + loc,(DWORD)size * 2);  break;
		default:  AsmByteMemMov (pDestBuf + loc,pSrcBuf + loc,(DWORD)size * 4);  break;
		} 

		index += stride;
	}
#else
	// For IA-32, the write log addresses are fine and the size is in bytes, so it must
	// be divided for WORD and DWORD moves.
	for(;;)
	{
		loc   = m_pWriteLog[index].address;
		size  = m_pWriteLog[index].size;

		// check for end-of-log.
		if(size == 0)
			break;
 		
		switch(size % 4)
		{	
		case  0:  AsmDwordMemMov(pDestBuf + loc,pSrcBuf + loc,(DWORD)size / 4);  break;
		case  2:  AsmWordMemMov (pDestBuf + loc,pSrcBuf + loc,(DWORD)size / 2);  break;
		default:  AsmByteMemMov (pDestBuf + loc,pSrcBuf + loc,(DWORD)size);      break;
		} 

		index += stride;
	}
#endif
}

//******************************************************************************
// Function name	: stress::StressStrideRead
// Description	    : Iterate through the read log starting at offset reading and
//                    comparing the destination buffer to the source buffer at
//                    every nth address and size specified by stride.  Perform
//                    DWORD writes if the size is divisible by four, WORD if by
//                    two, BYTE otherwise.
// Return type		: LPBYTE 
// Argument         : DWORD  index    - starting log index
// Argument         : DWORD  stride   - specifies reading every nth address in the log.
// Argument         : LPBYTE pSrcBuf  - the source buffer.
// Argument         : LPBYTE pDestBuf - the destination buffer.
//******************************************************************************
LPBYTE stress::StressStrideRead(DWORD index,DWORD stride,LPBYTE pSrcBuf,LPBYTE pDestBuf)
{
	DWORD_PTR loc,size;
	LPVOID    pVirt;

#ifdef _WIN64
	// The read log addresses must be multiplied by four to get DWORD-aligned
	// addresses.  Size is in DWORDs, so for BYTE and WORD modes the size of
	// the compare must be multiplied.
	for(;;)
	{
		loc   = m_pReadLog[index].address * 4;
		size  = m_pReadLog[index].size;
		
		// check for end-of-log.
		if(size == 0)
			return 0;
		
		switch(size % 4)
		{
 		case  0:  pVirt = AsmDwordMemCmp(pDestBuf + loc,pSrcBuf + loc,(DWORD)size);      break;
 		case  2:  pVirt = AsmWordMemCmp (pDestBuf + loc,pSrcBuf + loc,(DWORD)size * 2);  break;
		default:  pVirt = AsmByteMemCmp (pDestBuf + loc,pSrcBuf + loc,(DWORD)size * 4);  break;
		} 

		if(pVirt)
			return (LPBYTE)pVirt;

		index += stride;
	}
#else
	// For IA-32, the read log addresses are fine and the size is in bytes, so it must
	// be divided for WORD and DWORD compares.
	for(;;)
	{
		loc   = m_pReadLog[index].address;
		size  = m_pReadLog[index].size;
		
		// check for end-of-log.
		if(size == 0)
			return 0;
		
		switch(size % 4)
		{
 		case  0:  pVirt = AsmDwordMemCmp(pDestBuf + loc,pSrcBuf + loc,(DWORD)size / 4);  break;
 		case  2:  pVirt = AsmWordMemCmp (pDestBuf + loc,pSrcBuf + loc,(DWORD)size / 2);  break;
		default:  pVirt = AsmByteMemCmp (pDestBuf + loc,pSrcBuf + loc,(DWORD)size);      break;
		} 

		if(pVirt)
			return (LPBYTE)pVirt;

		index += stride;
	}
#endif
}


//******************************************************************************
// Function name	: stress::get_location
// Description	    : Uses the bitmap array to determine randomly-sized contiguous
//                    area and return its address.  The bitmap array is updated
//                    to mark the area as used. 
// Return type		: bool - TRUE if a location was found.  FALSE if no more locations free
// Argument         : SIZE_T* pAddr - pointer to the returned address.
// Argument         : SIZE_T* pSize - pointer to the returned size.
//******************************************************************************
bool stress::get_location(SIZE_T* pAddr,SIZE_T* pSize)
{
	DWORD_PTR addr,save,size,max_size;
	bool       fUpDir,fTop,fBottom;
	static    count = 0;

	ASSERT(pAddr != NULL);
	ASSERT(pSize != NULL);

	// Flags that we have iterated to the top and bottom of the address range.
	fTop = fBottom = FALSE;

	// Pick a starting address.  The current top and bottom are static, thread-safe, and
	// initialized in the build_log() function.
	save = addr = hCXMem->random() % (m_curtop - m_curbot) + m_curbot;
	
	// Pick a search direction.
	fUpDir = (bool)(hCXMem->random() % 2);
	
	// Find an unused address or return FALSE when done.
	while(is_used(addr))
	{
		if(fUpDir)
		{
			// check for walking off the top
			if(++addr == m_curtop)
			{
				fTop = TRUE;

				// if the top and bottom were reached, no more addresses.
				if(fBottom)
					return FALSE;

				// reverse direction and start back at the address.
				fUpDir = FALSE;
				addr   = save;
			}
		}
		else
		{
			// check for walking off the bottom
			if(addr-- == m_curbot)
			{
				fBottom = TRUE;

				// if the top and bottom were reached, no more addresses.
				if(fTop)
					return FALSE;

				// reverse direction and start back at the address.
				fUpDir  = TRUE;
				addr    = save;
			}
		}
	}

	// We have an unused starting address in addr now.

	// If the top was hit, make the starting address the current top.
	// Add one to fix a rare case where the bottom could equal the top
	// and cause divide-by-zero when picking the next random address.
	if(fTop)
		m_curtop = addr + 1;

	// If the bottom was hit, make the starting address the current bottom.
	if(fBottom)
		m_curbot = addr;

	ASSERT(addr < m_nTestSize);
	ASSERT(!is_used(addr));
	save = addr;

	// The maximum size of the unused area we will return,
	// We will return less than this if we find a used location in the area.
	max_size  = hCXMem->random() % m_nMaxTransferSize + 1;

	// Continue in the direction the unused address was found.
	// It is likely more unused locations are that way.
	if(fUpDir == TRUE)
	{
		// mark addresses as used until we reach max_size or a used address.
		for(size = 0; size < max_size && addr < m_curtop && !is_used(addr); size++,addr++)
		{
			mark_used(addr);
		}
	}
	else
	{
		bool fStop = FALSE;

		// mark addresses as used until we reach max_size or a used address.
		for(size = 0; size < max_size && !fStop && !is_used(addr); size++,addr--)
		{
			mark_used(addr);

			// addresses are unsigned so handle zero carefully.
			if(addr == m_curbot)
				fStop = TRUE;
		}

		save = addr + 1;
	}

	ASSERT(save >= 0 && save < m_nTestSize);
	ASSERT(save + size <= m_nTestSize);
	*pAddr = save;
	*pSize = size;
	return TRUE;
}
