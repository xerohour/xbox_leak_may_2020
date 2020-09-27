#include <stdlib.h>			// for _MAX_PATH
#include <afxmt.h>
#include "path.h"			// for CDir

typedef struct MAPPATH
{
	LPSTR		lszFrom;	// path to map from
	LPSTR		lszTo;		// path to map it to
} MAPPATH;

extern int cmappath;
#define cmappathMax 100		// I can't be bothered to make this variable-sized
extern MAPPATH FAR rgmappath[cmappathMax];

#define LPFNFMAPPATHTEST LPFNFILEFINDVALIDATOR

BOOL FMapPath(
	const CDir& dirProj,
	CHAR AlternatePath[_MAX_PATH],
	LPSTR lszFilename,
	LPDIALOGPARAM lpdlgparam,
	LPFNFMAPPATHTEST lpfnFMapPathTest,
	LONG lParam);

VOID ClearPathMappings(VOID);
VOID AddMapping(LPSTR,LPSTR);

// The following are for Find Symbols

extern class CSemaphore g_evFindSymbols;
typedef struct _SEARCHDEBUGINFOANDRETURN {
	PSEARCHDEBUGINFO	psdi;
	BOOL				fSuccess;
} SEARCHDEBUGINFOANDRETURN, *PSEARCHDEBUGINFOANDRETURN;