//
// CHTMLPageFile
//   CHTMLPageFile represents an HTML file. It's based on CFile.
//   Use this class to perform operations on HTML files.
//				   

#include "stdafx.h"

#include "htmlfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// REVIEW(briancr): some known limitations of this html parsing
// - escape character '\' (?) is not honored (i.e. \>): is this an issue?
// - comments are not honored, except when parsing for <applet>

//----------------------------------------------------------------
// CHTMLPageFile
//----------------------------------------------------------------

CHTMLPageFile::CHTMLPageFile()
{
}

CHTMLPageFile::~CHTMLPageFile()
{
}

UINT CHTMLPageFile::ReadAppletBlock(LPCSTR pszClassFileName, CString& strAppletBlock)
{
	ASSERT_VALID(this);
	// the file must be open in order to read from it
	ASSERT(m_hFile != (UINT)hFileNull);

	strAppletBlock.Empty();

	// read through the file, looking for applet tags
	// when an applet tag is found, check the code= tag for the
	// specified class file

	// read the HTML file into memory

	// get the length of the file
	DWORD dwLen = GetLength();

	// start at the beginning of the file
	SeekToBegin();

	// read the file
	CString strHTMLFile;
	Read(strHTMLFile.GetBuffer(dwLen), dwLen);
	strHTMLFile.ReleaseBuffer();

	// find each applet block and check it for the class file we want
	while (strAppletBlock.IsEmpty() && !strHTMLFile.IsEmpty()) {

		// get the next applet block
		CString strNextAppletBlock;
		ReadNextAppletBlock(strHTMLFile, strNextAppletBlock);

		// get the code file name
		CString strCodeFileName;
		ReadCodeFileName(strNextAppletBlock, strCodeFileName);
		strCodeFileName.MakeLower();

		// get the class file name we're looking for
		CString strClassFileName = pszClassFileName;
		strClassFileName.MakeLower();

		// is the class file name in the code file name?
		if (strCodeFileName.Find(strClassFileName) != -1)
			// copy the applet block
			strAppletBlock = strNextAppletBlock;
	}

	return strAppletBlock.GetLength();
}

UINT CHTMLPageFile::ReadParamBlock(LPCSTR pszClassFileName, CString& strParamBlock)
{
	ASSERT_VALID(this);
	// the file must be open in order to read from it
	ASSERT(m_hFile != (UINT)hFileNull);

	strParamBlock.Empty();

	// get the applet block for the specified class
	CString strAppletBlock;
	ReadAppletBlock(pszClassFileName, strAppletBlock);

	// find each param tag and enter it in the param block
	while (!strAppletBlock.IsEmpty()) {

		// find the next tag
		int nTagBeg = strAppletBlock.Find(_T('<'));
		if (nTagBeg == -1)
			nTagBeg = strAppletBlock.GetLength() - 1;

		// discard HTML up to tag
		strAppletBlock = strAppletBlock.Mid(nTagBeg);

		int nTagEnd = 0;

		// is this a param tag?
		CString strParam = strAppletBlock.Left(6);
		strParam.MakeLower();
		if (strParam == _T("<param")) {

			// find the end of the tag
			int nEndTag = strAppletBlock.Find(_T('>'));
			if (nEndTag == -1)
				nEndTag = strAppletBlock.GetLength() - 1;

			// copy the param block
			strParamBlock += strAppletBlock.Left(nEndTag + 1);
			// separate param tags by newlines
			strParamBlock += _T("\r\n");
		}
		// some other tag--skip it
		else {

			// find the end of the tag
			int nTagEnd = strAppletBlock.Find(_T('>'));
			if (nTagEnd == -1)
				nTagEnd = strAppletBlock.GetLength() - 1;
		}
		// discard HTML through the end of the tag
		strAppletBlock = strAppletBlock.Mid(nTagEnd + 1);
	}

	return strParamBlock.GetLength();
}

UINT CHTMLPageFile::ReadNextAppletBlock(CString& strHTML, CString& strAppletBlock)
{
	strAppletBlock.Empty();

	BOOL bAppletBlockFound = FALSE;
	// read until no more tags or applet block found
	while (!strHTML.IsEmpty() && !bAppletBlockFound) {

		// find the next tag
		int nTagBeg = strHTML.Find(_T('<'));
		if (nTagBeg == -1)
			nTagBeg = strHTML.GetLength() - 1;

		// discard HTML up to tag
		strHTML = strHTML.Mid(nTagBeg);

		// is this a comment tag?
		CString strComment = strHTML.Left(2);
		strComment.MakeLower();
		if (strComment == _T("<!--")) {

			// find the end of the comment
			int nTagEnd = strHTML.Find(_T("-->"));
			if (nTagEnd == -1)
				nTagEnd = strHTML.GetLength() - 1;
			else
				// skip "-->"
				nTagEnd += 3;

			// discard HTML through the end of the comment
			strHTML = strHTML.Mid(nTagEnd + 1);
		}
		// is this an applet tag?
		else {
			CString strApplet = strHTML.Left(7);
			strApplet.MakeLower();
			if (strApplet == _T("<applet")) {

				// find the end of the applet block
				int nTagEnd = 0;

				while (nTagEnd < strHTML.GetLength() - 1 && !bAppletBlockFound) {

					// try to find the end of the applet block
					int nNextTagEnd = strHTML.Mid(nTagEnd).Find(_T("</"));
					if (nNextTagEnd == -1)
						nTagEnd = strHTML.GetLength() - 1;
					else
						// skip "</"
						nTagEnd = nTagEnd + nNextTagEnd + 2;

					// skip white space
					while (nTagEnd < strHTML.GetLength() &&
						(strHTML[nTagEnd] == _T(' ') ||
						strHTML[nTagEnd] == _T('\t') ||
						strHTML[nTagEnd] == _T('\r') ||
						strHTML[nTagEnd] == _T('\n'))) {
						nTagEnd++;
					}

					// ensure we didn't read to the end of the string
					if (nTagEnd < strHTML.GetLength()) {

						// is this the </applet> tag?
						CString strAppletEnd = strHTML.Mid(nTagEnd, 6);
						strAppletEnd.MakeLower();
						if (strAppletEnd == _T("applet")) {

							// skip "applet"
							nTagEnd += 6;

							// find the true end of the applet block (the ">")
							int nTrueEnd = strHTML.Mid(nTagEnd).Find(_T('>'));
							if (nTrueEnd == -1)
								nTrueEnd = strHTML.Mid(nTagEnd).GetLength() - 1;

							// advance the end of the tag to the end
							nTagEnd += nTrueEnd;

							bAppletBlockFound = TRUE;
						}
					}
				}

				// copy the applet block
				strAppletBlock = strHTML.Left(nTagEnd + 1);

				// discard HTML through the end of the applet block
				strHTML = strHTML.Mid(nTagEnd + 1);

				bAppletBlockFound = TRUE;
			}
			// some other tag--skip it
			else {

				// find the end of the tag
				int nTagEnd = strHTML.Find(_T('>'));
				if (nTagEnd == -1)
					nTagEnd = strHTML.GetLength() - 1;
				
				// discard HTML through the end of the tag
				strHTML = strHTML.Mid(nTagEnd + 1);
			}
		}
	}

	return strAppletBlock.GetLength();
}

UINT CHTMLPageFile::ReadCodeFileName(CString& strAppletBlock, CString& strCodeFileName)
{
	strCodeFileName.Empty();

	// find the end of the applet tag ">"
	int nEndTag = strAppletBlock.Find(_T('>'));
	if (nEndTag == -1)
		nEndTag = strAppletBlock.GetLength() - 1;

	// copy the applet tag
	CString strTag = strAppletBlock.Left(nEndTag + 1);

	// lower case the applet tag
	strTag.MakeLower();

	// look for the "code" tag
	int nCodeTag = -1;
	BOOL bDone = FALSE;
	while (!bDone) {

		// look for the "code" tag
		nCodeTag = strTag.Find(_T("code"));

		// did we find the "code" tag?
		if (nCodeTag >= 0) {

			// skip "code"
			nCodeTag += 4;

			// discard applet tag through code tag
			strTag = strTag.Mid(nCodeTag);

			// trim white space
			strTag.TrimLeft();

			// expect '='
			if (strTag[0] == _T('=')) {

				// skip '='
				strTag = strTag.Mid(1);

				// trim white space
				strTag.TrimLeft();

				// we're done
				bDone = TRUE;
			}
		}
		else
			// we're done (and didn't find the code tag)
			bDone = TRUE;
	}

	// did we find a "code" tag?
	if (nCodeTag >= 0) {

		int nEndName;

		// is the name quoted?
		if (strTag[0] == _T('\"')) {

			// skip '"'
			strTag = strTag.Mid(1);

			// find ending quote
			nEndName = strTag.Find(_T('\"'));
			if (nEndName == -1)
				nEndName = strTag.GetLength();
		}
		else {
			// name is not quoted, find ending white space
			nEndName = strTag.FindOneOf(_T(" \r\n\t"));
			if (nEndName == -1)
				nEndName = strTag.GetLength();
		}

		strCodeFileName = strTag.Left(nEndName);
	}

	return strCodeFileName.GetLength();
}
