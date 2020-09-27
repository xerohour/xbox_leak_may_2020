///////////////////////////////////////////////////////////////////////////////
//   UIWBMSG.H
//
//  Created by :            Date :
//      DavidGa                 1/21/94
//
//  Description :
//      Declaration of the  UIWBMessageBox class
//

#ifndef __UIWBMSG_H__
#define __UIWBMSG_H__

#include "..\..\umsgbox.h"

#include "shlxprt.h"

#ifndef __UIMSGBOX_H__
    #error include 'umsgbox.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//   UIWBMessageBox class

// BEGIN_CLASS_HELP
// ClassName: UIWBMessageBox
// BaseClass: UIMessageBox
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIWBMessageBox : public  UIMessageBox
{
     UIWND_COPY_CTOR( UIWBMessageBox,  UIMessageBox);

// Utilities
public:
	virtual BOOL IsValid(void) const;
};

#endif //__UIWBMSG_H__
