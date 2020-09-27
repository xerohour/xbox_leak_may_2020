/*===========================================================================*\
 | 
 |  FILE:	SceneImport.h
 |			Skeleton project and code for a Scene Importer 
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 29-3-99
 | 
\*===========================================================================*/

#ifndef __SCIMPSKEL__H
#define __SCIMPSKEL__H

#include "max.h"
#include "resource.h"
#include "iparamm2.h"

#include <commctrl.h>
#include <richedit.h>
#include <commdlg.h>
#include <dlgs.h>

// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	SCIMP_CLASSID		Class_ID(0x32b73314, 0x67114a4b)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonSceneImportDesc();

/*===========================================================================*\
 |	SkeletonExporter class defn
\*===========================================================================*/

class SkeletonImporter : public SceneImport {
public:
	SkeletonImporter();
	~SkeletonImporter();

	// Preferences values
	BOOL simpleParam;

	// Used in DoImport
	FILE *fileStream;
	Interface* ip;

	const TCHAR *filenameTemp;



	// Number of extensions we support
	int ExtCount();
	const TCHAR * Ext(int n);

	// The bookkeeping functions
	const TCHAR * LongDesc();
	const TCHAR * ShortDesc();
	const TCHAR * AuthorName();
	const TCHAR * CopyrightMessage();
	const TCHAR * OtherMessage1();
	const TCHAR * OtherMessage2();

	// Version number of this importer
	unsigned int Version();

	// Show an about box
	void ShowAbout(HWND hWnd);

	// Do the actual import
	int DoImport(const TCHAR *name, ImpInterface *ii,Interface *i, BOOL suppressPrompts=FALSE);

	// Configuration file management
	BOOL LoadImporterConfig();
	void SaveImporterConfig();
	TSTR GetConfigFilename();

};

#endif