#include "std.h"
#include "SmartObject.h"
#include "XOConst.h"
#include "globals.h"
#include "tgl.h"
#include "File.h"
#include <malloc.h>
#include "StringObj.h"
#include "utilities.h"

/*	eButtonDefault = 0,
	eButtonPressed ,
	eButtonHighlighted,
	eButtonDisabled,
 */

long CButton::MaterialMap[eButtonStateMax][3] =  // standard, egglow, text
{
     eFlatSurfaces2sided3, 0, 14,
    eGameHilite, eEggGlow, 2,
    eGameHilite, eEggGlow, 2,
    eFlatSurfaces, eEggGlow, 2,
    eGameHilite, eEggGlow, 2

};

CButton::CButton( TG_Shape* pAppearance ):
 m_pHelpText(NULL),
 m_eButtonState(eButtonDefault),
 m_eButtonId(eNullButtonId),
 m_eOrientation(eButtonVerticalOrientation),
 m_pfnCallBack(NULL),
 m_pTextObj(NULL),
 m_nEgglowCount(0),
 m_pEgglowShapes(NULL),
 m_nStandardCount(0),
 m_pStandardShapes(NULL),
 m_textObjId(-1),
 m_eType(eOneTextButton),
 SmartObject(pAppearance),
 m_eSound( eStandardSound )
 { 

 }


CButton::~CButton()
{
	Cleanup();
}

void CButton::Cleanup()
{
    if ( m_pEgglowShapes )
    {
        delete[] m_pEgglowShapes;
        m_pEgglowShapes = NULL;
        m_nEgglowCount = 0;
    }
    if ( m_pStandardShapes )
    {
        delete[] m_pStandardShapes;
        m_pStandardShapes = NULL;
        m_nStandardCount = 0;
    }

    delete[] m_pHelpText;
	m_pHelpText = NULL;

	m_pTextObj = NULL;  // no we do not delete CText, Scene deletes all the text objects
	SmartObject::Cleanup();
}


void CButton::SetState(eButtonState newState)
{
    if ( m_eButtonState != newState )
    {
       // need to go ahead and set all the materials appropriately
        long oldMaterialStandard = MaterialMap[m_eButtonState][0];
        long newMaterialStandard = MaterialMap[newState][0];
        
        long oldMaterialEgglow = MaterialMap[m_eButtonState][1];
        long newMaterialEgglow = MaterialMap[newState][1];

        long text = MaterialMap[newState][2];

        if ( m_pTextObj )
            m_pTextObj->SetMaterial( text );

        for ( int i = 0; i < m_nStandardCount; i++ )
        {
            m_pStandardShapes[i]->SwapMaterials(oldMaterialStandard, newMaterialStandard);
        }
        for ( int i = 0; i < m_nEgglowCount; i++ )
        {
            m_pEgglowShapes[i]->SwapMaterials(oldMaterialEgglow, newMaterialEgglow);
        }            
    }

    if(m_eButtonId != eNullButtonId)
  	    DbgPrint("CButton::SetState,button = %d(%s), newstate = %d\n", m_eButtonId, ButtonName[m_eButtonId], newState);
    else 
        DbgPrint("CButton::SetState,button = %d(%s), newstate = %d\n", m_eButtonId, "eNullButton", newState);

    m_eButtonState = newState;
}

bool CButton::IsDisabled () const
{
    if( m_eButtonState == eButtonDisabled || 
        m_eButtonState == eButtonDisabledHighlighted )
        return true;

    return false;
}

void CButton::SetHelpText(const TCHAR* pHelpText)
{
    delete[] m_pHelpText;
    m_pHelpText = NULL;

    if ( pHelpText )
    {
        m_pHelpText = new TCHAR[_tcslen(pHelpText)+1];
        _tcscpy( m_pHelpText, pHelpText );
    }
}





HRESULT CButton::LoadFromXBG( File* pFile, TG_Shape* pRoot )
{
        
    char nodeName[64];
    

    long buttonSize =   pFile->readLong( );
    BYTE* pReadBuffer = (BYTE*)_alloca(buttonSize); // max this could ever be
	if(!pReadBuffer)
	{
		return E_OUTOFMEMORY;
	}

    m_textObjId2 = pFile->readByte();

     pFile->read( (BYTE*)nodeName, 64 * sizeof( char ) );
     m_pAppearance = pRoot->FindObject( nodeName );

    ASSERT( m_pAppearance );

    if ( !m_pAppearance )
    {
        return -1;        
    }

    m_textObjId =       pFile->readByte();
    m_eButtonId =       (eButtonId)pFile->readLong();
    m_eOrientation =    (eButtonOrientation)pFile->readLong();
    m_nStandardCount =  pFile->readByte();
    m_nEgglowCount =    pFile->readByte();
    float tmp =         pFile->readFloat();
    tmp =               pFile->readFloat();
	
	if (m_eButtonId == eNullButtonId)
	{
		m_pHelpText = new TCHAR[_tcslen(_T("HELP_NULL"))+1];
		_tcscpy(m_pHelpText, _T("HELP_NULL"));
	}
	else
	{
		ASSERT(m_eButtonId < countof(ButtonHelpText));
		if(m_eButtonId < countof(ButtonHelpText))
		{
			m_pHelpText = new TCHAR[ _tcslen( ButtonHelpText[m_eButtonId] ) + 1];
			_tcscpy(m_pHelpText, ButtonHelpText[m_eButtonId]);
		}
	}

    long count = 0;
    if ( m_nStandardCount ) // load up standard shapes
    {
        m_pStandardShapes = new TG_Shape*[m_nStandardCount];
		if(!m_pStandardShapes)
		{
			return E_OUTOFMEMORY;
		}

        for ( char i = 0; i < m_nStandardCount; i++ )
        {
            pFile->readString( pReadBuffer );
            TG_Shape* pFound = pRoot->FindObject( (const char*)pReadBuffer );
            ASSERT( pFound );
            if ( pFound )
            {
                m_pStandardShapes[count++] = pFound;
                // temp, until we get real swap info                
                pFound->SetMaterials( MaterialMap[eButtonDefault][0] );

            }


        }
    }

    count = 0;
    if ( m_nEgglowCount ) // now the egglow ones
    {
        m_pEgglowShapes = new TG_Shape*[m_nEgglowCount];
		if(!m_pEgglowShapes)
		{
			return E_OUTOFMEMORY;
		}
        for ( char i = 0; i < m_nEgglowCount; i++ )
        {
            pFile->readString( pReadBuffer );
            TG_Shape* pFound = pRoot->FindObject( (const char*)pReadBuffer );
            ASSERT( pFound );
            if ( pFound )
            {
                m_pEgglowShapes[count++] = pFound;
                // temp, until we get real swap info                
                pFound->SetMaterials( MaterialMap[eButtonDefault][1] );

            }
        }
    }
    
    return S_OK; 
}


const TCHAR* CButton::GetHelpText() const
{	
	return m_pHelpText;
}

void CButton::AutoTest()
{
	if(!m_pTextObj)
	{
		DbgPrint("This is a button without text: %d [%s]\n",m_eButtonId, ButtonName[m_eButtonId]);
		return;
	}
	const TCHAR*pTextId = m_pTextObj->GetTextId();
	char szBuf [256];
	ZeroMemory(szBuf, countof(szBuf));
	if(pTextId)
	{
		Ansi(szBuf, pTextId,min(_tcslen(pTextId),countof(szBuf) - 1) );
	}
	else
	{
		strcpy(szBuf, "NULL");
	}
	
	const TCHAR*pText = m_pTextObj->GetText();
	char szBuf2 [256];
	ZeroMemory(szBuf2, countof(szBuf));
	if(pText)
	{
		Ansi(szBuf2, pText, min(_tcslen(pText),countof(szBuf2) - 1) );
	}
	else
	{
		strcpy(szBuf2, "NULL");
	}
	
	DbgPrint("AUTO:BUTTON=BUTTON_ID[%d], BUTTON_NAME[%s], BUTTON_STATE[%d], TEXT_ID[%s], TEXT[%s]\n",m_eButtonId, ButtonName[m_eButtonId], m_eButtonState, szBuf, szBuf2);
}

void CButton::Dump(bool bAllData)
{

	DbgPrint("Button:[%s] State:[%d]", (m_eButtonId == eNullButtonId) ? "[null]": ButtonName[m_eButtonId], m_eButtonState);	

	if(bAllData)
	{
	    DbgPrint(" Orientation:[%d]",m_eOrientation);
		if(m_pHelpText)
		{
			char szBuf[256];
			Ansi(szBuf, m_pHelpText, countof(szBuf));
			DbgPrint("HelpText:[%s]", szBuf);
		}
		else
		{
			DbgPrint("HelpText:[null]");
		}

		DbgPrint("\n");

		if(m_pTextObj)
		{
			m_pTextObj->Dump(bAllData);
		}
		else
		{
			DbgPrint("TextObj:[null]\n");
		}
	}
}

void CButton::Show( bool bDrawThisObject )
{
    SmartObject::Show( bDrawThisObject );
    if ( m_pTextObj )
    {
        m_pTextObj->Show(bDrawThisObject);
    }
}
void CButton::SetTextId( const TCHAR* pTextTop, const TCHAR* pTextBottom  )
{
    if(m_pTextObj)
	{
		m_pTextObj->SetTextId(pTextTop);
	}
}

void CButton::SetText( const TCHAR* pTextTop, const TCHAR* pTextBottom )
{
    if ( m_pTextObj )
    {
        m_pTextObj->SetText( pTextTop );
    }
}

HRESULT    CButton::HandlePress()
{
    if ( m_pfnCallBack )
        return m_pfnCallBack();

    return S_OK;
}

void CInputButton::Show( bool bDrawThisObject )
{
    CButton::Show( bDrawThisObject );
    if ( m_pInputTextObj )
    {
        m_pInputTextObj->Show(bDrawThisObject);
    }
}

void CInputButton::SetTextId( const TCHAR* pTextTop, const TCHAR* pTextBottom  )
{
    CButton::SetTextId( pTextTop, NULL );
    if ( m_pInputTextObj )
    {
        m_pInputTextObj->SetTextId(pTextBottom); 
    }
}

void CInputButton::SetText( const TCHAR* pTextTop, const TCHAR* pTextBottom)
{
    CButton::SetText( pTextTop );
    if ( m_pInputTextObj )
    {
        m_pInputTextObj->SetText( pTextBottom );
    }
}

void CInputButton::Dump(bool bAllData)
{
	CButton::Dump(bAllData);
	m_pInputTextObj->Dump(bAllData);					  
}


void CInputButton::SetState(eButtonState newState)
{
	if(m_eButtonState == newState)
	{
		return;
	}
	if ( m_pInputTextObj && (newState == eButtonDisabled || m_eButtonState == eButtonDisabled)  )
	{
		m_pInputTextObj->SetMaterial(MaterialMap[newState][2]);
	}
	
	CButton::SetState(newState);
}



