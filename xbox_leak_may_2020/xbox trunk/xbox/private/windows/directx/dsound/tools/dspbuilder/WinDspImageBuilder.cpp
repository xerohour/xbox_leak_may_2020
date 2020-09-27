/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WinDspImageBuilder.cpp

Abstract:

	Windowed version of CDspImageBuilder

Author:

	Robert Heitkamp (robheit) 08-Jun-2001

Revision History:

	08-Jun-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"
#include "WinDspImageBuilder.h"
#include "resource.h"

class CDSPDialog : public CDialog 
{
public:

	CDSPDialog(LPCSTR pString=NULL) : CDialog(IDD_DSP_ERROR) 
	{
		unsigned i;
		unsigned length	= strlen(pString);
		unsigned count	= 0;

		// Count the number of carriage returns
		for(i=0; i<length; ++i)
		{
			if(pString[i] == '\n')
				++count;
		}

		m_pString = new char [length + count + 1];

		for(count=0, i=0; i<length; ++i)
		{
			if(pString[i] == '\n')
				m_pString[count++] = 13;
			m_pString[count++] = pString[i];
		}
		m_pString[count] = '\0';
	};

	virtual ~CDSPDialog(void)
	{
		delete [] m_pString;
	}

protected:

	virtual BOOL OnInitDialog(void)
	{
		CDialog::OnInitDialog();
		SetDlgItemText(IDC_DSP_ERROR_EDIT, m_pString);
		return TRUE;
	}

private:

	LPTSTR	m_pString;
};

//------------------------------------------------------------------------------
//	CWinDspImageBuilder::CWinDspImageBuilder
//------------------------------------------------------------------------------
CWinDspImageBuilder::CWinDspImageBuilder(void) : CDspImageBuilder("dspbuilder")
/*++

Routine Description:

	Constructor

Arguments:

	IN pAppName -	Application name

Return Value:

	None

--*/
{
	m_showDialog = FALSE;
}

//------------------------------------------------------------------------------
//	CWinDspImageBuilder::~CWinDspImageBuilder
//------------------------------------------------------------------------------
CWinDspImageBuilder::~CWinDspImageBuilder(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CWinDspImageBuilder::ShowDialog
//------------------------------------------------------------------------------
void
CWinDspImageBuilder::ShowDialog(void)
/*++

Routine Description:

	Shows the dialog if any text exists

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_showDialog)
	{
		m_showDialog = FALSE;
		CDSPDialog	dialog(m_text);
		dialog.DoModal();
	}
}

//------------------------------------------------------------------------------
//	CWinDspImageBuilder::ClearText
//------------------------------------------------------------------------------
void
CWinDspImageBuilder::ClearText(void)
/*++

Routine Description:

	Clears any text

Arguments:

	None

Return Value:

	None

--*/
{
	m_text.Empty();
	m_showDialog = TRUE;
}

//------------------------------------------------------------------------------
//	CWinDspImageBuilder::Print
//------------------------------------------------------------------------------
void
CWinDspImageBuilder::Print(
						   IN PCHAR	pFormat,
						   IN ...
						   )
/*++

Routine Description:

	Virtual print method.

Arguments:

	IN pFormat -	Format string
	IN ... -		Additional arguments

Return Value:

	None

--*/
{
    CHAR    szString[0x400];
    va_list	va;

	va_start(va, pFormat);
	vsprintf(szString, pFormat, va);
	va_end(va);

	m_text			+= szString;
	m_showDialog	= TRUE;
}
