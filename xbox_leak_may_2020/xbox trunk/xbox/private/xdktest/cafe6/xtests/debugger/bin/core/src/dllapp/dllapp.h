/*DOC***
**
** Suite : step
** Test  : dllapp.h
**
** Purpose : test Codeview stepping model
**
** General : stepping
** Primary : redirect
** Secondy : file comparison
**
** Dependencies : none (no screen dumps)
**
** Products : CV410
**
** Revision History :
**
** Mxxx	dd-Mon-yy	email name
** - description
** M000	28-Jun-92	waltcr
** - created
**
**
**
**DOC***/

#define IDM_EXIT  100
#define IDM_ABOUT 101

BOOL InitApplication(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
BOOL CenterWindow (HWND hwndChild, HWND hwndParent);

LONG APIENTRY MainWndProc(HWND, UINT, UINT, LONG);
BOOL APIENTRY About(HWND, UINT, UINT, LONG);
void BPWithLengthTest(void);
