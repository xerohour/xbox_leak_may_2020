#ifndef __WAVEFILEHANDLER_H__
#define __WAVEFILEHANDLER_H__


class CWaveFileHandler : public CFile
{

public:
	// Construction
	CWaveFileHandler(CString sFileName, UINT nOpenFlags);

public:
	// Methods to seek to marked position in the file
	void SeekToBegin();					// Seeks to the m_dwStartOffset into the file
	void SeekToData();					// Seeks to the m_dwDataOffset into the file

	// Accessors/Mutators for the offsets marks into the file
	virtual DWORD GetStartOffset();
	virtual DWORD GetDataOffset();

	virtual void SetStartOffset(DWORD dwOffset = 0);
	virtual void SetDataOffset(DWORD dwOffset);

	// Methods to deal with the data
	HRESULT ReadData(DWORD dwOffset, DWORD dwLength, BYTE* pbData, DWORD& dwBytesRead);
	
	// Attributes
	CString	m_sFileName;	// Name of the file opened

private:
	// The offsets are from the beginning of the file
	DWORD m_dwStartOffset;	// Offset to the start of WAVE chunk in the file; it's the position just before the WAVE RIFF header
	DWORD m_dwDataOffset;	// Offset to the start of DATA chunk in the file; it's the position right after the DATA chunk header i.e. sample 0 position
};


#endif // __WAVEFILEHANDLER_H__