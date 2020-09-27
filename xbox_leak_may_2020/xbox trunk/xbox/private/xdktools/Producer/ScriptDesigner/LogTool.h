#ifndef __LOGTOOL_H__
#define __LOGTOOL_H__

// LogTool.h : header file
//

#include <dmusici.h>

class CLogTool : public IDirectMusicTool
{
public:
	CLogTool( IDMUSProdDebugScript* pIDebugScript, bool fLogToStdOut = false );
	~CLogTool();

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID, LPVOID FAR *);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDirectMusicTool
	STDMETHOD(Init)(IDirectMusicGraph* pGraph);
	STDMETHOD(GetMsgDeliveryType)(DWORD* pdwDeliveryType );
	STDMETHOD(GetMediaTypeArraySize)(DWORD* pdwNumElements );
	STDMETHOD(GetMediaTypes)(DWORD** padwMediaTypes, DWORD dwNumElements);
	STDMETHOD(ProcessPMsg)(IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG);
	STDMETHOD(Flush)(IDirectMusicPerformance* pPerf, DMUS_PMSG* pPMSG, REFERENCE_TIME rtTime);

private:
	long m_cRef;
	IDMUSProdDebugScript* m_pIDebugScript;
	bool m_fLogToStdOut;
};

#endif // __SCRIPTCOMPONENT_H__
