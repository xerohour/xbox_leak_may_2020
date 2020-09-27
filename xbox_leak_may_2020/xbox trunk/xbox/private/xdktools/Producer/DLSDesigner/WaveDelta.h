#ifndef __WAVEDELTA_H__
#define __WAVEDELTA_H__

class CWaveDelta
{
	// Construction
public:
	CWaveDelta();
	CWaveDelta(DWORD dwActualPosition, DWORD dwStartSample, DWORD dwLength, DWORD dwFileOffset, BOOL bEditDelta = FALSE);
	
	// Methods
public:

	BOOL IsSampleInDelta(DWORD dwSample);

	BOOL IsEditDelta();

	DWORD GetActualPosition();
	void SetActualPosition(DWORD dwPosition);
	
	DWORD GetFileOffset();
	void SetFileOffset(DWORD dwFileOffset);

	DWORD GetStartSample();
	void SetStartSample(DWORD dwSample);

	DWORD GetLength();
	void SetLength(DWORD dwLength);

	// Attributes
private:
	DWORD m_dwActualPosition;
	DWORD m_dwStartSample;
	DWORD m_dwLength;
	DWORD m_dwFileOffset;
	BOOL  m_bEditDelta; 
};

#endif __WAVEDELTA_H__