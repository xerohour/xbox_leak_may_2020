//
//	SCHMZTL.H
//
//		Definition of classes for tracking buidld tools.

#ifndef _SCHMZTL_H_
#define _SCHMZTL_H_

class  CSchmoozeTool : public CBuildTool
{
	DECLARE_DYNAMIC (CSchmoozeTool)

public:
	CSchmoozeTool()
	{
		m_strName = _TEXT("schmooze.exe");
	}
	~CSchmoozeTool() {}
};

#endif // _SCHMZTL_H_
