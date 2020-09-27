#ifndef __STYLEPERSONALITIES_H__
#define __STYLEPERSONALITIES_H__

// StylePersonalities.h : header file
//

#include <afxtempl.h>

class CDirectMusicStyle;
class CPersonality;

class CStylePersonalities
{
friend class CDirectMusicStyle;

public:
    CStylePersonalities();
	~CStylePersonalities();

private:
	CTypedPtrList<CPtrList, CPersonality*> m_lstPersonalities;
	CDirectMusicStyle*	m_pStyle;
};

#endif // __STYLEPERSONALITIES_H__
