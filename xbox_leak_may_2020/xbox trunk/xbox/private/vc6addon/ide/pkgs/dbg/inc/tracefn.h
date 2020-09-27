// tracefn.h : header file
//
#ifndef __TRACEFN_H__
#define __TRACEFN_H__

/////////////////////////////////////////////////////////////////////////////
// CTraceFunction

class CTraceFunction : public CObject
{
// Construction
public:
	CTraceFunction();
	~CTraceFunction();

// Attributes
public:

// Operations
public:
	BOOL SetupStep (VOID);
	BOOL GetString (CString &);
	BOOL CompareCxt (PCXT pCXT);

// Implementation
protected:
	ADDR m_addrPC;
	ADDR m_addrLine;
	WORD m_wLineStart;
	WORD m_wLineEnd;
	TML	m_tml;
	BOOL m_fTmlInitialized;
};

extern CTraceFunction *g_pTraceFunction;

/////////////////////////////////////////////////////////////////////////////
#endif // __TRACEFN_H__
