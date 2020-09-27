#ifndef __PCHANNELGROUP_H__
#define __PCHANNELGROUP_H__

// PChannelGroup.h : header file
//

class CTool;

//////////////////////////////////////////////////////////////////////
//  CPChannelGroup

class CPChannelGroup
{
friend class CDirectMusicGraph;
friend class CGraphDlg;
friend class CTool;

public:
	CPChannelGroup();
	virtual ~CPChannelGroup();

	void FormatPChannelText( CString& strText );
	void InsertTool( CDirectMusicGraph* pGraph, CTool* pTool );

	// Member variables
private:
	DWORD				m_dwNbrPChannels;
	DWORD*				m_pdwPChannel;	// Variable array

	CTypedPtrList<CPtrList, CTool*> m_lstTools;
};

#endif // __PCHANNELGROUP_H__
