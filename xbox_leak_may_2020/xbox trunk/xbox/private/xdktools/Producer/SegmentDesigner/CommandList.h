#ifndef __COMMANDLIST_H__
#define __COMMANDLIST_H__ 1
#include <afxtempl.h>
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include <dmusicf.h>

#pragma pack(2)
typedef struct IMATemplateCommand
{
	BYTE		abFill[4];	// Originally, pointer to next command
	LONG		lTime;		// Time, in clocks
	SHORT		nMeasure;	// Which measure
	DWORD		dwCommand;	// Command type
	DWORD		dwSignPost;	// Used by composition engine
} IMATemplateCommandExt;
#pragma pack()

typedef struct IMASectionCommand
{
	long		lTime;		// Time, in clocks
	DWORD		dwCommand;	// Command type
} IMASectionCommandExt;

class CCommandList {
public:
    CCommandList();
	~CCommandList();
	HRESULT IMA_AddSectionCommand( IStream* pIStream, long lRecSize );
	HRESULT IMA_AddTemplateCommand( IStream* pIStream, long lRecSize );
	HRESULT CreateCommandTrack( class CTrack** ppTrack );
	HRESULT CreateSignPostTrack( class CTrack** ppTrack );
	interface IDMUSProdFramework*	m_pIFramework;

private:
	HRESULT DM_SaveCommandList( interface IDMUSProdRIFFStream* pIRIFFStream );
	HRESULT DM_SaveSignPostList( interface IDMUSProdRIFFStream* pIRIFFStream );
//	HRESULT LoadCommandList( interface IDMUSProdRIFFStream* pIRIFFStream );
//	HRESULT SendCommandListToCommandMgr( LPUNKNOWN punkCommandMgr );
	void RemoveAll();

	CTypedPtrList<CPtrList, DMUS_IO_COMMAND*>	m_lstDMCommands;
	CTypedPtrList<CPtrList, DMUS_IO_SIGNPOST*>	m_lstDMSignPosts;
    short							m_nLastImportantMeasure;
	LONG							m_lClocksPerMeasure;
	DMUS_TIMESIGNATURE				m_TimeSig;
};

#endif //__COMMANDLIST_H__
