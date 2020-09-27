#pragma once
/*************************************************************************************************\
CMsgDialog.h		: Interface for the CMsgDialog component.  This is the standard OK/YES/No dlg 
                        for the dash.  There are easy to use global functions to make these 
                        objects in Globals.h
Creation Date		: 1/8/2002 11:08:29 AM
Copyright Notice	: (C) 2000 Microsoft
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/

#include "PrimitiveScene.h"
#include "tgl.h"
#include "XBInput.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
CMsgDialog:
**************************************************************************************************/
// progress functions need to pass back the float  a value between 0 and 1 to display progress
// the returned HRESULT determines wether the called function succeeded or failed
typedef HRESULT (*ProgressFunction)(float*);
typedef HRESULT (*WaitFunction)();

class TG_Shape;







struct MSGPARAMETERS
{
    eSceneId        m_SceneID;
    const TCHAR*    m_pStrMessageID; // text in the panel
    const TCHAR*    m_pStrYesID; // set these ID's to NULL if you want to hide the button
    const TCHAR*    m_pStrNoID;
    const TCHAR*    m_pStrCenterButtonID;
    bool            m_bUseBigButtons;
    ProgressFunction    m_pFunction;
    WaitFunction    m_pWaitFunction;    // version with no known time
};
class CMsgDialog: public CPrimitiveScene // must derive from this to get into navigation map, but don't call any CScene functions
{
	public:

        CMsgDialog(){}
        virtual ~CMsgDialog(){}

        // set all the necessary info
        virtual HRESULT SetParameters( const MSGPARAMETERS& params ); // builds a standard OK dlg
        

        HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );


        HRESULT SetMessage( const TCHAR* strIDMessage ); // in case the message needs to change

    protected:
        void SetButtonText( eButtonId ID, const TCHAR* pText );



	private:
      	
        CMsgDialog& operator=( const CMsgDialog& MsgDialog );
		CMsgDialog( const CMsgDialog& src );
		
};

class CWaitCursor : public CPrimitiveScene
{
public:
    // set all the necessary info
    virtual HRESULT SetParameters( const MSGPARAMETERS& params ); 
    HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );

private:

    WaitFunction        m_pWaitFunction;
    float               m_fTime;


};


/**************************************************************************************************
CLASS DESCRIPTION
CProgressDialog:  Progress Bar!
**************************************************************************************************/


class CProgressDialog: public CMsgDialog
{
	public:

        virtual HRESULT SetParameters( const MSGPARAMETERS& params );

       	virtual HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );

        CProgressDialog();

        float   SetProgress( float newProgress );
        


	private:
      	
        CProgressDialog& operator=( const CMsgDialog& MsgDialog );
		CProgressDialog( const CMsgDialog& src );

        ProgressFunction    m_pCallback;
        TG_Shape*           m_pSliderBar;
		
};



//*************************************************************************************************

