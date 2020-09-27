#ifndef cmduiena_h
#define cmduiena_h

// This class is designed to trap the enable status of a particular command
// It is used by the custom keyboard map and an OLE automation handler
// to check whether custom commands are available,

class CCmdUIEnable : public CCmdUI
{
public:
	CCmdUIEnable()
	{
		m_bEnabled = FALSE;
	}

	void Enable(BOOL bOn)
	{
		m_bEnabled = bOn;
		m_bEnableChanged = TRUE;
	}

	void SetCheck(int nCheck)
	{
	}

	void SetRadio(BOOL bOn)
	{
	}

	void SetText(LPCSTR lpszText)
	{
	}

	BOOL m_bEnabled;
};

#endif
