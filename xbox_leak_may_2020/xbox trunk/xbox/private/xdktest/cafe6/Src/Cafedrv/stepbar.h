#ifndef INC_STEPBAR_H
#define INC_STEPBAR_H

#define LPSZPIPENAME   "\\\\.\\pipe\\debugbar.pip"	  // cryptic, but it's \\.\.\debugbar.pip (this computer current dir)
#define PIPEWAITTIME   5000

#define MAXIMUMTEXTWIDTH    (30 * BUTTONWIDTH)
#define BUTTONWIDTH   16

#define MAXTEXTSIZE  200


class CDebugToolBar : public CToolBar
{
protected:
	CStatic *m_pcstaticText;
	HANDLE  m_hPipe;
	CRect   m_crect;  // rectangle delimiting the text area

public:

	CDebugToolBar() : CToolBar() {m_pcstaticText = NULL; m_hPipe = INVALID_HANDLE_VALUE;};
	~CDebugToolBar();

	BOOL Initialize(UINT nID);  // opens the pipe.

	BOOL SetToolBarText();
	BOOL SetStaticText(char *szBuffer);

};

#endif  // INC_STEPBAR_H
