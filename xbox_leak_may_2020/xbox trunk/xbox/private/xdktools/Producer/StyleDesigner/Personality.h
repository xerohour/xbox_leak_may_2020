#ifndef __PERSONALITY_H__
#define __PERSONALITY_H__

interface IDMUSProdRIFFStream;

// Personality.h : header file
//

class CDirectMusicStyle;

class CPersonality
{
friend class CDirectMusicStyle;

public:
    CPersonality();
	~CPersonality();

    HRESULT DM_SavePersonalityRefChunk( IDMUSProdRIFFStream* pIRiffStream );
    HRESULT DM_LoadPersonalityRef( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain );
    HRESULT IMA25_LoadPersonalityRef( IStream* pIStream, MMCKINFO* pckMain );

private:
// Personality Reference data for Music Engine
    CString			   m_strName;
	CString			   m_strFileName;
    GUID			   m_guid;
    BYTE			   m_fDefault;           // 1=Default personality
};

#endif // __PERSONALITY_H__
