/*============================================================================
Microsoft Access

Microsoft Confidential.
Copyright 1992 Microsoft Corporation. All Rights Reserved.

	Component Message Range Definitions

Don't start any lower than 400 because we run into Projects private
messages that they accidently send to other applications
============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#define WM_FORMSTART	(WM_USER + 400)
#define WM_TBSTART		(WM_USER + 490)
#define WM_DOCSTART 	(WM_USER + 500)
#define WM_KTSTART		(WM_USER + 600)
#define WM_ECLSTART 	(WM_USER + 700)
#define WM_DEMSTART		(WM_USER + 800)
#define WM_OESTART		(WM_USER + 900)
#define WM_SUISTART		(WM_USER + 1100)
#define WM_QRYSTART 	(WM_USER + 1200)
#define WM_PRINTSTART	(WM_USER + 1300)
#define WM_DSKSTART 	(WM_USER + 1400)
#define WM_DBCSTART 	(WM_USER + 1500)
#define WM_MAINSTART	(WM_USER + 1600)
#define WM_FEDTSTART	(WM_USER + 1700)
#define WM_GRIDSTART	(WM_USER + 1800)
#define WM_DRAGSTART	(WM_USER + 1900)
#define WM_GRIDXSTART	(WM_USER + 2000)
#define WM_BUILDSTART	(WM_USER + 2050)
#define WM_SRELSTART	(WM_USER + 2100)
#define WM_DDESUPSTART	(WM_USER + 2200)
#define WM_MODSTART		(WM_USER + 2300)

// These two messages are here because they are shared between OLE and DDE
#define WM_PROCESSDDEEXECUTE	WM_DDESUPSTART + 0
#define WM_PROCESSDDEREQUEST	WM_DDESUPSTART + 1


#ifdef __cplusplus
}
#endif
