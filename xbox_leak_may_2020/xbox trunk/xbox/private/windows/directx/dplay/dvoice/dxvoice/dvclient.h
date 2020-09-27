/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dvclient.h
 *  Content:	Defines functions for the DirectXVoiceClient interface
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	02/07/99	rodtoll	Created It
 * 09/01/2000  georgioc started rewrite/port to xbox 
 *
 ***************************************************************************/

#ifndef __DVCLIENT__
#define __DVCLIENT__

#include "dvntos.h"
#include "dvoicep.h"
#include "dvcleng.h"
#include "dvtran.h"
#include "dvshared.h"

volatile struct DIRECTVOICECLIENTOBJECT : public DIRECTVOICEOBJECT
{
	CDirectVoiceClientEngine	*lpDVClientEngine;
};

typedef DIRECTVOICECLIENTOBJECT *LPDIRECTVOICECLIENTOBJECT;


#endif
