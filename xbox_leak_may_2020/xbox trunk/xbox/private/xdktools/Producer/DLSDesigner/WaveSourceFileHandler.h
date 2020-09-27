#ifndef __WAVESOURCEFILEHANDLER_H__
#define __WAVESOURCEFILEHANDLER_H__

#include "WaveFileHandler.h"
#include "RiffStrm.h"

class CWaveSourceFileHandler : public CWaveFileHandler
{

public:
	// Construction
	CWaveSourceFileHandler(CString sFileName, bool bInACollection);	// Opens the file for READ 

	// Method to load chunks from the header
	HRESULT LoadChunk(MMCKINFO* pckFind, BYTE* pbData, DWORD& dwBytesRead); 
	
	// Reset the offsets of the header and data chunks in the file
	HRESULT ResetOffsets();	

	// Called when the source file is being renamed
	HRESULT OnRename(CString sNewName);

	
private:
	HRESULT AllocRIFFStreamFromFile(IStream** ppIStream, IDMUSProdRIFFStream** ppIRiffStream); // Open a READ stream to the source file
	bool m_bInACollection;
	
};
#endif // __WAVESOURCEFILEHANDLER_H__