/*****************************************************
*** rebootcommand.h
***
*** H file for our XDash CFG Reboot XBox option
*** This option will reboot the XBox when the user
*** selects "A"
*** 
*** by James N. Helm
*** November 3rd, 2000
*** 
*****************************************************/

#ifndef _REBOOTCOMMAND_H_
#define _REBOOTCOMMAND_H_

#include "xdcitem.h"
#include "xboxvideo.h"
#include "usbmanager.h"

class CRebootCommand : public CXDCItem
{
public:
    CRebootCommand();
    CRebootCommand( CXDCItem* pParent, WCHAR* pwszTitle, WCHAR* pwszDescription );
    ~CRebootCommand();

    void Action( CXBoxVideo* Screen );
    void HandleInput( enum BUTTONS buttonPressed, bool bFirstPress );

private:
};

#endif // _REBOOTCOMMAND_H_