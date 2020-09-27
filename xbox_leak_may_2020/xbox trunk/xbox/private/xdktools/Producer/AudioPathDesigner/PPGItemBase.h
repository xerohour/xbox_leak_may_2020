#if !defined(AFX_PPGITEMBASE_H__818EB751_0DC9_4AE7_ABE6_56FCB72464A9__INCLUDED_)
#define AFX_PPGITEMBASE_H__818EB751_0DC9_4AE7_ABE6_56FCB72464A9__INCLUDED_

typedef enum { PPG_NONE = 0, PPG_MIX_GROUP = 1, PPG_BUFFER = 2, PPG_EFFECT = 3 } PPG_INDEX;

class CDirectMusicAudioPath;

struct PPGItemBase
{
// Construction
public:
	PPGItemBase()
	{
		m_ppgIndex = PPG_NONE;
		m_dwChanged = 0;
		m_pAudioPath = NULL;
		m_fValid = false;
	};
	virtual ~PPGItemBase()
	{
	};

// Operations
public:
	// Helper functions
	void Copy( const PPGItemBase &ppgItemBase )
	{
		m_ppgIndex = ppgItemBase.m_ppgIndex;
		m_dwChanged = ppgItemBase.m_dwChanged;
		m_pAudioPath = ppgItemBase.m_pAudioPath;
		m_strAudioPathName = ppgItemBase.m_strAudioPathName;
		m_fValid = ppgItemBase.m_fValid;
	};

	PPG_INDEX	m_ppgIndex;
	DWORD		m_dwChanged;
	bool		m_fValid;
	CDirectMusicAudioPath	*m_pAudioPath;
	CString		m_strAudioPathName;
};

#endif // !defined(AFX_PPGITEMBASE_H__818EB751_0DC9_4AE7_ABE6_56FCB72464A9__INCLUDED_)
