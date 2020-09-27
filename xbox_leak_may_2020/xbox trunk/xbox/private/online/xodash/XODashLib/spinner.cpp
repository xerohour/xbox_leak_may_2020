#define SPINNER_CPP
/*************************************************************************************************\
spinner.cpp			: Implementation of the spinner component.
Creation Date		: 2/4/2002 12:47:03 PM
Copyright Notice	: (C) 2000 Microsoft
Author				: Victor Blanco
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
CSpinner::CSpinner()
{
    m_nStartValue   = 0;
    m_nEndValue     = 0;
    m_nCurrentValue = 0;
    m_nPrecision    = 2;
    m_nScrollUpChangeDelay = 0;
    m_nScrollDownChangeDelay = 0;
    m_pScrollUpButton   = NULL;
    m_pScrollDownButton = NULL;
}


/*************************************************************************************************/
CSpinner::~CSpinner()
{

}

/*************************************************************************************************/
void CSpinner::SetRange( unsigned int nStartVal, unsigned int nEndVal )
{
    m_nStartValue   = nStartVal;
    m_nEndValue     = nEndVal;
    m_nCurrentValue = nStartVal;
        
    // Reset scroll arrow states
    m_pScrollUpButton->setVisible( 0 );
    m_pScrollDownButton->setVisible( 0 );

    UpdateText();
}

/*************************************************************************************************/
void CSpinner::UpdateRange( unsigned int nStartVal, unsigned int nEndVal )
{
    m_nStartValue   = nStartVal;
    m_nEndValue     = nEndVal;
        
    // Reset scroll arrow states
    m_pScrollUpButton->setVisible( 0 );
    m_pScrollDownButton->setVisible( 0 );

    UpdateText();
}

/*************************************************************************************************/
void CSpinner::SetValue( unsigned int nCurVal )
{
    ASSERT( nCurVal <= m_nEndValue );
    ASSERT( nCurVal >= m_nStartValue );
    
    m_nCurrentValue = nCurVal;
    
    UpdateText();
}

/*************************************************************************************************/
unsigned int CSpinner::GetValue( void )
{
    return m_nCurrentValue;
}

/*************************************************************************************************/
void CSpinner::SetPrecision( unsigned int nPrecision )
{
    ASSERT( nPrecision > 0 );
    m_nPrecision = nPrecision;
}

/*************************************************************************************************/
HRESULT CSpinner::IncrementValue( )
{
    if( (m_nCurrentValue + 1 ) <= m_nEndValue )
    {
        m_nCurrentValue++;
        UpdateText();
        return S_OK;
    }

    return E_FAIL;
}

/*************************************************************************************************/
HRESULT CSpinner::DecrementValue( )
{
    if( (m_nCurrentValue - 1 ) >= m_nStartValue )
    {
        m_nCurrentValue--;
        UpdateText();
        return S_OK;
    }

    return E_FAIL;
}

/*************************************************************************************************/
void CSpinner::Show( bool bDrawThisObject )
{
    if ( m_pAppearance )
    {
        m_pAppearance->setVisible( bDrawThisObject );
    }
}

/*************************************************************************************************/
HRESULT CSpinner::FrameMove( XBGAMEPAD& GamepadInput, float fElapsedTime )
{
    UpdateText();

    // Reset scroll arrow states
    if( !m_nScrollUpChangeDelay )
        m_pScrollUpButton->setVisible( 0 );
    else
        m_nScrollUpChangeDelay--;

    if( !m_nScrollDownChangeDelay )
        m_pScrollDownButton->setVisible( 0 );
    else    
        m_nScrollDownChangeDelay--;
 
    // The spinner must have focus to get render control
    if ( m_pSpinnerButton->GetState() == eButtonHighlighted )
    {
        // Joystick Down, Increment our value
        if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN || GamepadInput.fY1 == -1)
        {
            m_pScrollDownButton->setVisible( 1 );
            m_nScrollDownChangeDelay = 3;
            DecrementValue();
            UpdateText();
        }
        
        
        // Joystick Down, Decrement our value
        else if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP || GamepadInput.fY1 == 1)
        {
            m_pScrollUpButton->setVisible( 1 );
            m_nScrollUpChangeDelay = 3;
            IncrementValue();
            UpdateText();
        }
    }

    return S_OK;
    
}

/*************************************************************************************************/
void CSpinner::UpdateText()
{
    // fill in the buttons with the proper text
    TCHAR buffer[11];

    wsprintf( buffer, L"%010d", m_nCurrentValue );  // Create using maximum 
    
    m_pSpinnerButton->SetText( buffer + ( 10 - m_nPrecision ) );
    m_pSpinnerButton->SetHelpText( L"helptest" );
}

 /*************************************************************************************************/
HRESULT CSpinner::LoadFromXBG( File* pFile, TG_Shape* pRoot, CButton** pButtons, unsigned long count )
{

    BYTE shapeID[64];

    pFile->read( shapeID, 64 );

    m_pAppearance = pRoot->FindObject( (char*)shapeID );
    ASSERT( m_pAppearance );

    unsigned char entryCount = pFile->readByte( );
    
    m_pSpinnerButton = pButtons[entryCount];

    m_pScrollUpButton = m_pAppearance->FindObject( "Spinner_Up" );
    m_pScrollDownButton = m_pAppearance->FindObject( "Spinner_Down" );

    ASSERT( m_pScrollUpButton && m_pScrollDownButton );

    if ( m_pScrollUpButton )
        m_pScrollUpButton->setVisible( 1 );

    if ( m_pScrollDownButton )
        m_pScrollDownButton->setVisible( 1 );

    return S_OK;
     
}

//*************************************************************************************************
// end of file ( listbox.cpp )
