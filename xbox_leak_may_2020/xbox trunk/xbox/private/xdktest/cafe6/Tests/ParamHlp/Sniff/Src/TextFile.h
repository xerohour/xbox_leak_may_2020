#include "stdafx.h"

class CTextFile : public CFile
{
public:
	int ReadLn(CString& str);
	void WriteLn(CString str);
};
