#define LISTBOX_CPP
/*************************************************************************************************\
listbox.cpp			: Implementation of the listbox component.
Creation Date		: 1/16/2002 12:47:03 PM
Copyright Notice	: (C) 2000 Microsoft
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/
#include "std.h"
#include "smartobject.h"
#include "tgl.h"
#include "xbInput.h"
#include "XOConst.h"
#include "File.h"

/*************************************************************************************************/
CListBox::CListBox()
{
    m_nCurTextIndex = 0;
    m_pScrollUpButton = NULL;
    m_pScrollDownButton = NULL;
    
}


/*************************************************************************************************/
CListBox::~CListBox()
{
   RemoveAllItems();
    // don't delete these here, their memory is controlled by Primitive Scene
   m_pListButtons.clear();
}

/*************************************************************************************************/
void CListBox::Show( bool bDrawThisObject )
{
    if ( m_pAppearance )
    {
        m_pAppearance->setVisible( bDrawThisObject );
    }

    for ( unsigned long i = 0; i < m_pListButtons.size(); i++ )
    {
        m_pListButtons[i]->Show( bDrawThisObject );
    }        
}

/*************************************************************************************************/
HRESULT CListBox::FrameMove( XBGAMEPAD& GamepadInput, float fElapsedTime )
{
    
 
   	if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
			GamepadInput.fY1 == -1)
		{
	        // figure out who's currently highlighted.
            int lastButtonIndex = m_pListButtons.size() - 1;
            if ( lastButtonIndex > -1 )
            {
                if ( m_pListButtons[lastButtonIndex]->GetState() == eButtonHighlighted )
                {
                    // is i at the bottom of the list, and we 
                    // need to scroll
                    if ( m_ItemList.size() - m_nCurTextIndex > m_pListButtons.size() )
                    {
                        m_nCurTextIndex++;

                        UpdateText();
                    }
                       
                }
                
            }
        }


	else if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ||
			GamepadInput.fY1 == 1)
		{
		    if ( m_pListButtons.size() &&
                m_pListButtons[0]->GetState() == eButtonHighlighted
                && m_nCurTextIndex > 0 )
            {
                // is i at the bottom of the list, and we 
                // need to scroll
                m_nCurTextIndex --;
               
                UpdateText();
            }
		}


     // hide and show buttons as appropriate
    if ( m_nCurTextIndex == 0 && m_pScrollUpButton)
    {
        m_pScrollUpButton->setVisible( 0 );
    }
    else if ( m_pScrollUpButton )
    {
        m_pScrollUpButton->setVisible( 1 );
    }

    if ( m_ItemList.size() - m_nCurTextIndex > m_pListButtons.size() )
    {
        if ( m_pScrollDownButton )
            m_pScrollDownButton->setVisible( 1 );
    }
    else
    {
        
        if ( m_pScrollDownButton )
            m_pScrollDownButton->setVisible( 0 );
    }


    return S_OK;
    



    
}

void CListBox::UpdateText()
{
     unsigned long curButton = 0;
    // get the iterator to the right place
    TEXT_LIST::iterator iter = m_ItemList.begin();
    TEXT_LIST::iterator helpIter = m_ItemHelpText.begin();

    for ( int i = 0; i < m_nCurTextIndex; i++ )
    {
        iter++;
        helpIter++;
    }
    
    // fill in the buttons with the proper text
    for ( ; iter != m_ItemList.end(), curButton < m_pListButtons.size(); 
        iter++ )
        {
            m_pListButtons[curButton]->SetText( (*iter) );
             m_pListButtons[curButton]->SetHelpText( (*helpIter) );

            curButton++;
        }

}
/*************************************************************************************************/
HRESULT CListBox::SetItemText( unsigned long index, const TCHAR* pText, const TCHAR* pHelpText )
{

    if ( index < m_ItemList.size() )
    {
        TEXT_LIST::iterator iter = m_ItemList.begin();
        for ( unsigned long i = 0; i < index; i++ )
        {
            iter++;
        }
        if ( (*iter ) )
        {
            delete [] *iter;
            (*iter) = NULL;
        }

        if ( pText )
        {
            (*iter) = new TCHAR[_tcslen( pText )+1];
            _tcscpy( (*iter), pText );
        }

        // set the help text

        iter = m_ItemHelpText.begin();
        for ( unsigned long i = 0; i < index; i++ )
        {
            iter++;
        }
        if ( (*iter ) )
        {
            delete [] *iter;
            (*iter) = NULL;
        }

        if ( pHelpText )
        {
            (*iter) = new TCHAR[_tcslen( pHelpText )+1];
            _tcscpy( (*iter), pHelpText );
        }

        // need to find the appropriate button and set that
        if ( m_nCurTextIndex + index < m_pListButtons.size() )
        {
            m_pListButtons[m_nCurTextIndex + index]->SetText( pText );
            m_pListButtons[m_nCurTextIndex + index]->SetHelpText( pHelpText );
        }


        return S_OK;
    }

    return E_FAIL;
}
/*************************************************************************************************/
HRESULT CListBox::RemoveAllItems()
{
    for ( TEXT_LIST::iterator iter = m_ItemList.begin(); 
        iter != m_ItemList.end(); iter ++ )
    {
        if ( (*iter ) )
            delete (*iter);
    }

    m_ItemList.clear();

    for ( iter = m_ItemHelpText.begin(); 
        iter != m_ItemHelpText.end(); iter ++ )
    {
        if ( (*iter ) )
            delete (*iter);
    }

    m_ItemHelpText.clear();

    for ( unsigned long i = 0; i < m_pListButtons.size(); i++ )
    {
        m_pListButtons[i]->Show( 0 );
    } 

    m_nCurTextIndex = 0;
    
    if ( m_pScrollDownButton ) 
        m_pScrollDownButton->setVisible( 0 );
    if ( m_pScrollUpButton )
        m_pScrollUpButton->setVisible( 0 );
    
    return S_OK;
}
/*************************************************************************************************/
HRESULT CListBox::AppendItem( const TCHAR* pText, const TCHAR* pHelpText )
{
    TCHAR* pTextToAdd = NULL;
    if ( pText )
    {
        pTextToAdd = new TCHAR[_tcslen(pText)+1];
        _tcscpy( pTextToAdd, pText );
    }

    m_ItemList.push_back( pTextToAdd );

    // can be NULL I suppose
    TCHAR* pHelpTextToAdd = NULL;
    if ( pHelpText )
    {
        pHelpTextToAdd = new TCHAR[_tcslen(pHelpText)+1];
        _tcscpy( pHelpTextToAdd, pHelpText );
    }

    m_ItemHelpText.push_back( pHelpTextToAdd );

    for ( unsigned long i = 0; i < m_ItemList.size() && i < m_pListButtons.size(); i++ )
    {
        m_pListButtons[i]->Show( 1 );
    }

    if ( m_ItemList.size() <= m_pListButtons.size() )
    {
        m_pListButtons[m_ItemList.size() -1]->SetText( pText );
        m_pListButtons[m_ItemList.size() -1]->SetHelpText( pHelpText );

        if ( m_pAppearance && m_pAppearance->isVisible() )
        {
            m_pListButtons[m_ItemList.size() -1]->Show(1);
        }
    }

    if ( m_ItemList.size() + m_nCurTextIndex > m_pListButtons.size() )
    {
        if ( m_pScrollDownButton )
            m_pScrollDownButton->setVisible( 1 );
    }

    return S_OK;
}
/*************************************************************************************************/ 
HRESULT CListBox::AppendItems( const TCHAR** ppText, const TCHAR** ppHelpText, int count )
{
    for ( int i = 0; i < count; i++ )
    {
        AppendItem( ppText[i], ppHelpText[i] );
    }

    return S_OK;
}

 /*************************************************************************************************/
HRESULT CListBox::LoadFromXBG( File* pFile, TG_Shape* pRoot, CButton** pButtons, unsigned long count )
{

    BYTE shapeID[64];
    pFile->read( shapeID, 64 );

    m_pAppearance = pRoot->FindObject( (char*)shapeID );
    ASSERT( m_pAppearance );


    unsigned char entryCount = pFile->readByte( );
    m_pListButtons.resize( entryCount );

    for ( int i = 0; i < entryCount; i++ )
    {
        unsigned char curButton = pFile->readByte();
        ASSERT( curButton < count );

        if ( ( curButton < count ) )
        {
            m_pListButtons[i] = pButtons[curButton];
            m_pListButtons[i]->Show( 0 ); // hide until we have text attributed
        }
        else
            return E_FAIL;

        m_pScrollUpButton = m_pAppearance->FindObject( "Spinner_Up" );
        m_pScrollDownButton = m_pAppearance->FindObject( "Spinner_Down" );

        ASSERT( m_pScrollUpButton && m_pScrollDownButton );

        if ( m_pScrollUpButton )
            m_pScrollUpButton->setVisible( 0 );

        if ( m_pScrollDownButton )
            m_pScrollDownButton->setVisible( 0 );
    }

    return S_OK;
     
}




//*************************************************************************************************
// end of file ( listbox.cpp )
