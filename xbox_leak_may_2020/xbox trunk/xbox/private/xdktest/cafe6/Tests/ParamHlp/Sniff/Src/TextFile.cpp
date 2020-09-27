#include "TextFile.h"


int CTextFile::ReadLn(CString& str)
{
	char *ch = new char;
	int nLength = 0;
	str = "";

	while ((Read(ch, 1)) && (*ch != (char)13)) {
		str += *ch;
		nLength++;
	}

	if (*ch == (char)13) {
		// Eat next char ((char)10)
		Read(ch, 1);
	}

	delete ch;
	return nLength;
}

void CTextFile::WriteLn(CString str)
{
	static long lCount = 0;
	CString strWrite;
	
	strWrite.Format("%d\t%s\r\n", ++lCount, str);

	try {
		Write(strWrite, strWrite.GetLength());
		Flush();
	}
	catch(CFileException e) {
		CString strErr;
		strErr.Format("Failed writing to failures file. Cause:%s", e.m_cause);
		MessageBox(AfxGetMainWnd()->m_hWnd, strErr, "WriteLn Error", MB_OK);
	}
}
