#include "std.h"
#include "SmartObject.h"
#include "XOConst.h"
#include "globals.h"
#include "tgl.h"


SmartObject::~SmartObject()
{
	Cleanup();
}

void SmartObject::Cleanup()
{
   // do not remove tg_Shape, CScene will release the memory
	m_pAppearance = NULL;
}


void SmartObject::CopyObj(const SmartObject& OtherObj)
{
	if(OtherObj.m_pAppearance)
	{
		m_pAppearance = new TG_Shape(*OtherObj.m_pAppearance);
		ASSERT(m_pAppearance);
	}
}

SmartObject& SmartObject::operator= (const SmartObject& OtherObj)
{
	if(&OtherObj != this)
	{
		Cleanup();
		CopyObj(OtherObj);
	}
	return *this;
}

SmartObject::SmartObject(const SmartObject& OtherObj)
{
	CopyObj(OtherObj);
}

void SmartObject::Show( bool bDrawThisObject )
{
    if ( m_pAppearance )
    {
        m_pAppearance->setVisible( bDrawThisObject );
    }
}

bool    SmartObject::IsShowing()  const
{ 
    if ( m_pAppearance )
    {
        return m_pAppearance->isVisible();
    }

    return false;
}



