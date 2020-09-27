// StylePersonalities.cpp : implementation file
//

#include "stdafx.h"

#include "StyleDesignerDLL.h"
#include "Style.h"
#include "Personality.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStylePersonalities constructor/destructor

CStylePersonalities::CStylePersonalities()
{
	m_pStyle = NULL;
}

CStylePersonalities::~CStylePersonalities()
{
	CPersonality* pPersonality;

	while( !m_lstPersonalities.IsEmpty() )
	{
		pPersonality = static_cast<CPersonality*>( m_lstPersonalities.RemoveHead() );
		delete pPersonality;
	}
}


