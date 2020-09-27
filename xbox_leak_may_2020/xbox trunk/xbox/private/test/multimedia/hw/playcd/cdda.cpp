#include "cdda.h"

////////////////////////////////////////////////////////////////////////////

CCDDAStreamer::CCDDAStreamer(CNtIoctlCdromService* pDrive)
{
	ASSERT(pDrive != NULL);

	m_ibChunk = 0;
	m_pDrive = pDrive;
	m_dwCurFrame = 0;
	m_chunk = NULL;
	m_bError = false;
}

CCDDAStreamer::~CCDDAStreamer()
{
	delete [] m_chunk;
}

int CCDDAStreamer::ReadChunk(void* pvBuffer)
{
	HRESULT hr = m_pDrive->Read(m_dwCurFrame, FRAMES_PER_CHUNK, pvBuffer);
	if (FAILED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER))
		{
//			TRACE(_T("CCDDAStreamer: We probably hit the end of the disc at frame %d...\n"), m_dwCurFrame);
			return 0;
		}

		m_bError = true;
		return -1;
	}

	m_dwCurFrame += FRAMES_PER_CHUNK;

	return BYTES_PER_CHUNK;
}

int CCDDAStreamer::Read(void* pvBuffer, int cbWanted)
{
	// Once there's an error, forget it...

	if (m_bError)
		return -1;

	// Check for simple case where there's no need to buffer...

	if (m_ibChunk == 0 && cbWanted == BYTES_PER_CHUNK)
		return ReadChunk(pvBuffer);


	// Deal with random reading case where we need to handle the buffering...

	if (m_chunk == NULL)
	{
		m_chunk = new BYTE [BYTES_PER_CHUNK];
	}

	if ( m_chunk == NULL )
	{
		m_bError = true;
		return -1;
	}

	int cbTotalRead = 0;
	while (cbWanted > 0)
	{
		if (m_ibChunk == 0)
		{
			int nRead = ReadChunk(m_chunk);
			if (nRead <= 0)
				return nRead;

			ASSERT(nRead == BYTES_PER_CHUNK);
		}

		int cb = min(cbWanted, (BYTES_PER_CHUNK - m_ibChunk));
		
		CopyMemory(pvBuffer, m_chunk + m_ibChunk, cb);

		m_ibChunk += cb;
		if (m_ibChunk == BYTES_PER_CHUNK)
			m_ibChunk = 0;
		ASSERT(m_ibChunk < BYTES_PER_CHUNK);

		cbWanted -= cb;
		cbTotalRead += cb;
		pvBuffer = ((BYTE*)pvBuffer) + cb;
	}

	return cbTotalRead;
}
