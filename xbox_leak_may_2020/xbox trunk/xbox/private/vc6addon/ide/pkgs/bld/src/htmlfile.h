//
// CHTMLPageFile
//   CHTMLPageFile represents an HTML file. It's based on CFile.
//   Use this class to perform operations on HTML files.
//				   

#ifndef __HTMLFILE_H__
#define __HTMLFILE_H__

//----------------------------------------------------------------
// CHTMLPageFile
//----------------------------------------------------------------
class CHTMLPageFile: public CFile
{
// ctor/dtor
public:
	CHTMLPageFile();
	~CHTMLPageFile();

// applet block operations
public:
	UINT ReadAppletBlock(LPCSTR pszClassFileName, CString& strAppletBlock);

// param block operations
public:
	UINT ReadParamBlock(LPCSTR pszClassFileName, CString& strParamBlock);

// internal functions
private:
	UINT ReadNextAppletBlock(CString& strHTML, CString& strAppletBlock);
	UINT ReadCodeFileName(CString& strAppletBlock, CString& strCodeFileName);
};

#endif // __HTMLFILE_H__
