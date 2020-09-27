/*===========================================================================*\
 | 
 |  FILE:	DoExport.cpp
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

#include "SceneExport.h"


/*===========================================================================*\
 |  Determine what options we support
\*===========================================================================*/

BOOL SkeletonExporter::SupportsOptions(int ext, DWORD options)
{
	switch(ext) {
		case 0:
			if(options & SCENE_EXPORT_SELECTED) return TRUE;
			else return FALSE;
			break;
		}
	return FALSE;
}



/*===========================================================================*\
 |  Preferences dialog handler
\*===========================================================================*/

static BOOL CALLBACK PrefsDlgProc(
		HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
	SkeletonExporter *se = (SkeletonExporter*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!se && msg!=WM_INITDIALOG) return FALSE;

	switch (msg) {
		case WM_INITDIALOG:	
			// Update class pointer
			se = (SkeletonExporter*)lParam;
			SetWindowLong(hWnd,GWL_USERDATA,lParam);

			// Setup preferences initial state
			SetCheckBox(hWnd,IDC_SIMPLE,se->simpleParam);
			break;

		case WM_DESTROY:
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDC_CANCEL:
					EndDialog(hWnd,1);
					break;

				case IDC_OK:
					// Retrieve preferences
					se->simpleParam = GetCheckBox(hWnd,IDC_SIMPLE);

					EndDialog(hWnd,0);
				break;
			}
			break;

		default:
			return FALSE;
		}	
	return TRUE;
} 



/*===========================================================================*\
 |  For every node we get, just print out its name and object ref's classID
\*===========================================================================*/

BOOL SkeletonExporter::nodeEnum(INode* node,Interface *ip) 
{
	if(exportSelected && node->Selected() == FALSE)
		return TREE_CONTINUE;

	// Check for user cancel
	if (ip->GetCancel())
		return FALSE;


	if(!exportSelected || node->Selected()) 
	{
		Class_ID cid = node->GetObjectRef()->ClassID();
		fprintf(fileStream, "%s : Class_ID(%X,%X)\n",node->GetName(),cid.PartA(),cid.PartB());
	}


	// Recurse through this node's children, if any
	for (int c = 0; c < node->NumberOfChildren(); c++) {
		if (!nodeEnum(node->GetChildNode(c), ip))
			return FALSE;
	}


	return TRUE;
}



/*===========================================================================*\
 |  Do the export to the file
\*===========================================================================*/

int	SkeletonExporter::DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	// Set local interface pointer
	ip = i;

	// load the configuration from disk
	// so that it can be used in our dialog box
	if(!LoadExporterConfig()) return 0;

	if(!suppressPrompts)
	{

		// Show preferences setup dialog
		int res = DialogBoxParam(
			hInstance,
			MAKEINTRESOURCE(IDD_SKELETON_SCEXP),
			i->GetMAXHWnd(),
			PrefsDlgProc,
			(LPARAM)this);

		// User clicked 'Cancel'
		if(res!=0) return 0;

	}

	exportSelected = (options & SCENE_EXPORT_SELECTED) ? TRUE : FALSE;


	// Open a filestream for writing out to
	fileStream = _tfopen(name,_T("wt"));
	if (!fileStream) {
		return 0;
	}


	// Print out a title for the file header
	fprintf(fileStream, "Skeleton File Format\nFrom 3D Studio MAX R3 Skeleton SDK Samples\n\n");

	// Print out the state of the simple parameter
	fprintf(fileStream, "Simple Parameter State: %s\n\n", simpleParam?"TRUE":"FALSE");


	// Simple root node -> children enumeration
	// This will get the root node, and then cycle through its children (ie, the basic scene nodes)
	// It will then recurse to search their children, and then their children, etc
	int numChildren = i->GetRootNode()->NumberOfChildren();

	for (int idx=0; idx<numChildren; idx++) {
		if (i->GetCancel())
			break;
		nodeEnum(i->GetRootNode()->GetChildNode(idx), i);
	}

	fclose(fileStream);


	// Save the current configuration back out to disk
	// for use next time the exporter is run
	SaveExporterConfig();


	return 1;
}
