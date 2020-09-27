[!if LIB_APP]
========================================================================
    STATIC LIBRARY : [!output PROJECT_NAME] Project Overview
========================================================================

AppWizard has created this [!output PROJECT_NAME] library project for you. 
[!endif]
[!if XBOX_APP]
========================================================================
    Xbox APPLICATION : [!output PROJECT_NAME] Project Overview
========================================================================

AppWizard has created this [!output PROJECT_NAME] application for you.  
[!endif]

[!if !LIB_APP]
This file contains a summary of what you will find in each of the files that
make up your [!output PROJECT_NAME] application.
[!else]
[!if PRE_COMPILED_HEADER]
This file contains a summary of what you will find in each of the files that
make up your [!output PROJECT_NAME] application.
[!else]
No source files were created as part of your project.
[!endif]
[!endif]


[!output PROJECT_NAME].vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

[!if !EMPTY_PROJECT && !LIB_APP]
[!output PROJECT_NAME].cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named [!output PROJECT_NAME].pch and a precompiled types file named StdAfx.obj.

[!endif]
[!if LIB_APP]
[!if PRE_COMPILED_HEADER]

/////////////////////////////////////////////////////////////////////////////

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named [!output PROJECT_NAME].pch and a precompiled types file named StdAfx.obj.

[!endif]
[!endif]
/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
