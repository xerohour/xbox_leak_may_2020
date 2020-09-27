/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dvserver.h
 *  Content:	Defines functions for the DirectXVoiceServer interface
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	02/07/99	rodtoll	Created It
 *  09/01/2000  georgioc started rewrite/port to xbox
 ***************************************************************************/
#ifndef __DVSERVER__
#define __DVSERVER__

#include "dvntos.h"
#include <dvoicep.h>
#include "dvsereng.h"
#include "dvshared.h"
#include "trnotify.h"
#include "dvdxtran.h"


volatile struct DIRECTVOICESERVEROBJECT : public DIRECTVOICEOBJECT
{
	CDirectVoiceServerEngine	*lpDVServerEngine;
};

typedef DIRECTVOICESERVEROBJECT *LPDIRECTVOICESERVEROBJECT;


#endif
