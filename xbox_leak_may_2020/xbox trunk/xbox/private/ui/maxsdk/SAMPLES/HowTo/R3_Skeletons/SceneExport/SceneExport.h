/*===========================================================================*\
 | 
 |  FILE:	SceneExport.h
 |			Skeleton project and code for a Scene Exporter 
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 26-3-99
 | 
\*===========================================================================*/

#ifndef __SCEXPSKEL__H
#define __SCEXPSKEL__H

#include "max.h"
#include "resource.h"
#include "iparamm2.h"

// IMPORTANT:
// The ClassID must be changed whenever a new project
// is created using this skeleton
#define	SCEXP_CLASSID		Class_ID(0x1e211639, 0x1eac4cd1)


TCHAR *GetString(int id);
extern ClassDesc* GetSkeletonSceneExportDesc();

/*===========================================================================*\
 |	SkeletonExporter class defn
\*===========================================================================*/

class SkeletonExporter : public SceneExport {
public:
	SkeletonExporter();
	~SkeletonExporter();

	// Preferences values
	BOOL simpleParam;

	// Used in DoExport
	BOOL exportSelected;
	FILE *fileStream;
	Interface* ip;



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

	// Version number of this exporter
	unsigned int Version();

	// Show an about box
	void ShowAbout(HWND hWnd);

	// Do the actual export
	int DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

	// Returns whether we support the extended exporter options
	BOOL SupportsOptions(int ext, DWORD options);

	// Scene enumeration
	BOOL nodeEnum(INode* node,Interface *ip);


	// Configuration file management
	BOOL LoadExporterConfig();
	void SaveExporterConfig();
	TSTR GetConfigFilename();

};

#endif