//
// INTEL CONFIDENTIAL.  Copyright (c) 1997-1999 Intel Corp. All rights reserved.
//
#pragma once
class stress
{
public:
	stress(SIZE_T block_size, CXModuleMemory * hCXMem);
	~stress();
	LPBYTE StressRead      (LPBYTE pSrcBuf,LPBYTE pDestBuf);
	void   StressWrite     (LPBYTE pSrcBuf,LPBYTE pDestBuf);
	void   StressWriteDword(LPBYTE pSrcBuf,LPBYTE pDestBuf);
	LPBYTE StressReadDword (LPBYTE pSrcBuf,LPBYTE pDestBuf);
	void   StressStrideWrite(DWORD index,DWORD stride,LPBYTE pSrcBuf,LPBYTE pDestBuf);
	LPBYTE StressStrideRead (DWORD index,DWORD stride,LPBYTE pSrcBuf,LPBYTE pDestBuf);
	void   build_read_log();
	void   build_write_log();
	SIZE_T get_log_size() const {return m_nLogSize;}
	SIZE_T get_test_size() const {return m_nTestSize;}

	struct log 
	{
		SIZE_T address;
		SIZE_T size;
	};




	const log* get_read_log() const {return m_pReadLog;}
	const log* get_write_log() const {return m_pWriteLog;}

private:
	CXModuleMemory * hCXMem;
	void build_log(log* pLog);
	bool get_location(SIZE_T* pAddr,SIZE_T* pSize);

	// The m_pdwUsed array is initialized by InitStress.

	// Checks if an address is marked as used in the bitmapped array.
	inline bool is_used(SIZE_T addr)
	{
		return (m_pdwUsed[addr / 32] & (1 << (addr % 32))) != 0;
	}

	// Marks an address as used in the bitmapped array.
	inline void mark_used(SIZE_T addr)
	{
		m_pdwUsed[addr / 32] |= 1 << (addr % 32);
	}

	SIZE_T  m_nTestSize;
	SIZE_T  m_nMaxTransferSize;
	SIZE_T  m_nLogSize;
	LPDWORD m_pdwUsed;

	log* m_pReadLog;
	log* m_pWriteLog;

	// These are used by the get_location algorithm to speed it up.
	// They remember the highest and lowest addresses outside of which
	// all the addresses are used.
	/*__declspec(thread) static*/ SIZE_T m_curbot,m_curtop;
};



