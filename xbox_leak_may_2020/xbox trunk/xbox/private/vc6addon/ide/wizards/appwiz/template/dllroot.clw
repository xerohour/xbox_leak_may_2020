; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
$$IF(EXTDLL)
ClassCount=0
$$ELSE //!EXTDLL
$$IF(AUTOMATION)
ODLFile=$$root$$.odl
$$ENDIF //AUTOMATION
ClassCount=1
Class1=$$APP_CLASS$$
LastClass=$$APP_CLASS$$
NewFileInclude2=#include "$$root$$.h"
$$ENDIF //EXTDLL
ResourceCount=0
NewFileInclude1=#include "stdafx.h"

$$IF(!EXTDLL)
[CLS:$$APP_CLASS$$]
Type=0
HeaderFile=$$root$$.h
ImplementationFile=$$root$$.cpp
Filter=N
$$ENDIF //EXTDLL
