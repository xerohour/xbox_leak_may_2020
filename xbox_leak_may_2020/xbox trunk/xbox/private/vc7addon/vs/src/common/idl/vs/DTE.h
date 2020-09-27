/*****************************************************************************
*  dte.h
*
*  Copyright (C) 1995-2000, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
******************************************************************************/

//Some forward defines so MSO is not needed everywhere.
#ifndef FORWARDDECLCOMMANDBAR
    interface CommandBar;
    interface CommandBars;
    interface CommandBarControl;
#endif

namespace VxDTE
{
    #include "dteinternal.h"
};

#ifndef FORCE_EXPLICIT_DTE_NAMESPACE
using namespace VxDTE;
#endif
