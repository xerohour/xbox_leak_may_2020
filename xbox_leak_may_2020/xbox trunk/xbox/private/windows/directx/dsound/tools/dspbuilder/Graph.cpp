/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Graph.cpp

Abstract:

	Graph of effects

Author:

	Robert Heitkamp (robheit) 08-Nov-2001

Revision History:

	08-Nov-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"
#include "Graph.h"
#include "Module.h"
#include "parser.h"
#include "ExportDialog.h"
#include "ChildView.h"
#include "PatchCord.h"
#include "Jack.h"

//------------------------------------------------------------------------------
//	Defines:
//------------------------------------------------------------------------------
#define SECTION_MAX_SIZE		4096
#define INPUT_MIXBIN_MASK		0x0400
#define OUTPUT_MIXBIN_MASK		0x0800
#define TOLERANCE				2

//------------------------------------------------------------------------------
//	Stuff:
//------------------------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define CI(x, y) (((x) * m_gridHeight) + (y))

//------------------------------------------------------------------------------
//	Structures
//------------------------------------------------------------------------------
struct EffectChainBucket
{
	CArray<CModule*, CModule*>	effects;
	CArray<int, int>			inputs;	 // Negative values are temp mixbins
	CArray<int, int>			outputs; // Negative values are temp mixbins
	CStringArray				stateFiles;
	CStringArray				stateFilePaths;

	EffectChainBucket& operator = (const EffectChainBucket& ecb)
	{
		effects.Copy(ecb.effects);
		inputs.Copy(ecb.inputs);
		outputs.Copy(ecb.outputs);
		stateFiles.Copy(ecb.stateFiles);
		stateFilePaths.Copy(ecb.stateFilePaths);
		return *this;
	}
};

//------------------------------------------------------------------------------
//	CGraph::CGraph
//------------------------------------------------------------------------------
CGraph::CGraph(
			   IN CChildView*	pParent
			   )
/*++

Routine Description:

	Constructor

Arguments:

	IN pParent -	Parent window of the graph

Return Value:

	None

--*/
{
	int		i;
	LPCTSTR	mixbins[] = // These MUST remain in the same order, if their
	{					// order or content changes, InsertModule() must be
		_T("I3DL2"),	// updated
		_T("FX Send 0"), 
		_T("FX Send 1"),
		_T("FX Send 2"),
		_T("FX Send 3"),
		_T("FX Send 4"),
		_T("FX Send 5"),
		_T("FX Send 6"),
		_T("FX Send 7"),
		_T("FX Send 8"),
		_T("FX Send 9"),
		_T("FX Send 10"),
		_T("FX Send 11"),
		_T("FX Send 12"),
		_T("FX Send 13"),
		_T("FX Send 14"),
		_T("FX Send 15"),
		_T("FX Send 16"),
		_T("FX Send 17"),
		_T("FX Send 18"),
		_T("FX Send 19"),
	};

	// Intitialize a few things
	m_pXTalk			= NULL;
	m_pI3DL2			= NULL;
	m_pI3DL2_24K		= NULL;
	m_p2x1Mixer			= NULL;
	m_bEffectsLoaded	= FALSE;
	m_bChangesMade		= FALSE;
	m_bInsertModules	= FALSE;
	m_bLeftMouseDown	= FALSE;
	m_bDrawGrid			= FALSE;
	m_bSnapToGrid		= TRUE;
	m_snapSize			= SCALE;
	m_bDeleteConfigMenu	= FALSE;
	m_bUseOffset		= FALSE;
	m_bDrawLine			= FALSE;
	m_bDrawComplexLine	= FALSE;
	m_bStartDrawLine	= FALSE;
	m_bDragPatch		= FALSE;
	m_bImageSaved		= FALSE;
	m_bPatchXtalkToLFE	= FALSE;
	m_bIncludeXtalk		= TRUE;
	m_pGrid				= NULL;
	m_bDead				= FALSE;
	m_imageBuilder		= new CWinDspImageBuilder;
	m_rect.SetRect(0, 0, 0, 0);
	SetParent(pParent);

	// Set the paths to the effects files
	SetPaths();

	// Load all the effects
	if(!LoadEffects())
	{
		AfxMessageBox(_T("Unable to load effects. Please specify a valid "
					     "directory."), MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}

	// Create the mixbins
	CreateMixbins();

	// Create the effects menu
	m_moduleMenu.CreatePopupMenu();
	m_moduleMenu.AppendMenu(MF_STRING, ID_EFFECTS_PROPERTIES, _T("&Properties..."));
	m_moduleMenu.AppendMenu(MF_STRING, ID_EFFECTS_NAME, _T("&Name..."));
	m_moduleMenu.AppendMenu(MF_STRING, ID_EFFECTS_DELETE, _T("&Delete"));
	m_moduleMenu.AppendMenu(MF_STRING, ID_EFFECTS_DISCONNECT_ALL, _T("Disconnect &All"));
	m_moduleMenu.AppendMenu(MF_STRING, ID_EFFECTS_DISCONNECT_INPUTS, _T("Disconnect &Inputs"));
	m_moduleMenu.AppendMenu(MF_STRING, ID_EFFECTS_DISCONNECT_OUTPUTS,_T("Disconnect &Outputs"));

	// Patch cord menu
	m_patchCordMenu.CreatePopupMenu();
	m_patchCordMenu.AppendMenu(MF_STRING, ID_PATCH_DISCONNECT, _T("&Disconnect"));
//	m_patchCordMenu.AppendMenu(MF_STRING, ID_PATCH_OVERWRITE, _T("&Overwrite"));

	// Speaker menus
	m_inputSpeakerMenu.CreatePopupMenu();
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 0 | INPUT_MIXBIN_MASK, _T("All"));
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 1 | INPUT_MIXBIN_MASK, _T("Front + Rear"));
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 2 | INPUT_MIXBIN_MASK, _T("Front"));
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 3 | INPUT_MIXBIN_MASK, _T("Rear"));
	m_inputSpeakerMenu.AppendMenu(MF_SEPARATOR);
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 4 | INPUT_MIXBIN_MASK, _T("Front Left"));
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 5 | INPUT_MIXBIN_MASK, _T("Front Right"));
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 6 | INPUT_MIXBIN_MASK, _T("Center"));
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 7 | INPUT_MIXBIN_MASK, _T("LFE"));
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 8 | INPUT_MIXBIN_MASK, _T("Back Left"));
	m_inputSpeakerMenu.AppendMenu(MF_STRING, 9 | INPUT_MIXBIN_MASK, _T("Back Right"));

	m_outputSpeakerMenu.CreatePopupMenu();
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 0 | OUTPUT_MIXBIN_MASK, _T("All"));
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 1 | OUTPUT_MIXBIN_MASK, _T("Front + Rear"));
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 2 | OUTPUT_MIXBIN_MASK, _T("Front"));
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 3 | OUTPUT_MIXBIN_MASK, _T("Rear"));
	m_outputSpeakerMenu.AppendMenu(MF_SEPARATOR);
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 4 | OUTPUT_MIXBIN_MASK, _T("Front Left"));
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 5 | OUTPUT_MIXBIN_MASK, _T("Front Right"));
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 6 | OUTPUT_MIXBIN_MASK, _T("Center"));
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 7 | OUTPUT_MIXBIN_MASK, _T("LFE"));
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 8 | OUTPUT_MIXBIN_MASK, _T("Back Left"));
	m_outputSpeakerMenu.AppendMenu(MF_STRING, 9 | OUTPUT_MIXBIN_MASK, _T("Back Right"));

	// XTalk Menus
	m_inputXtalkMenu.CreatePopupMenu();
	m_inputXtalkMenu.AppendMenu(MF_STRING, 10 | INPUT_MIXBIN_MASK, _T("All"));
	m_inputXtalkMenu.AppendMenu(MF_SEPARATOR);
	m_inputXtalkMenu.AppendMenu(MF_STRING, 11 | INPUT_MIXBIN_MASK, _T("Front Left"));
	m_inputXtalkMenu.AppendMenu(MF_STRING, 12 | INPUT_MIXBIN_MASK, _T("Front Right"));
	m_inputXtalkMenu.AppendMenu(MF_STRING, 13 | INPUT_MIXBIN_MASK, _T("Back Left"));
	m_inputXtalkMenu.AppendMenu(MF_STRING, 14 | INPUT_MIXBIN_MASK, _T("Back Right"));

	m_outputXtalkMenu.CreatePopupMenu();
	m_outputXtalkMenu.AppendMenu(MF_STRING, 10 | OUTPUT_MIXBIN_MASK, _T("All"));
	m_outputXtalkMenu.AppendMenu(MF_SEPARATOR);
	m_outputXtalkMenu.AppendMenu(MF_STRING, 11 | OUTPUT_MIXBIN_MASK, _T("Front Left"));
	m_outputXtalkMenu.AppendMenu(MF_STRING, 12 | OUTPUT_MIXBIN_MASK, _T("Front Right"));
	m_outputXtalkMenu.AppendMenu(MF_STRING, 13 | OUTPUT_MIXBIN_MASK, _T("Back Left"));
	m_outputXtalkMenu.AppendMenu(MF_STRING, 14 | OUTPUT_MIXBIN_MASK, _T("Back Right"));

	// Insert mixbin menus
	m_inputMixbinMenu.CreatePopupMenu();
	m_outputMixbinMenu.CreatePopupMenu();

	m_inputMixbinMenu.AppendMenu(MF_POPUP,  (UINT)m_inputSpeakerMenu.Detach(), _T("Speakers"));
	m_inputMixbinMenu.AppendMenu(MF_POPUP,  (UINT)m_inputXtalkMenu.Detach(), _T("XTalk"));
	m_outputMixbinMenu.AppendMenu(MF_POPUP,  (UINT)m_outputSpeakerMenu.Detach(), _T("Speakers"));
	m_outputMixbinMenu.AppendMenu(MF_POPUP,  (UINT)m_outputXtalkMenu.Detach(), _T("XTalk"));

	// Add all the mixbins to the insert mixbin menu
	for(i=0; i<(int)(sizeof(mixbins) / sizeof(mixbins[0])); ++i)
	{
		m_inputMixbinMenu.AppendMenu(MF_STRING, (UINT)(i + 15) | INPUT_MIXBIN_MASK, mixbins[i]);
		m_outputMixbinMenu.AppendMenu(MF_STRING, (UINT)(i + 15) | OUTPUT_MIXBIN_MASK, mixbins[i]);
	}

	// Insert effects menu
	m_effectMenu.CreatePopupMenu();

	// Add all the effects to the insert effects menu
	for(i=0; i<m_effects.GetSize(); ++i)
		m_effectMenu.AppendMenu(MF_STRING, (UINT)(i+1), m_effects[i]->GetEffectName());

	// Grid menu (root menu)
	m_rootMenu.CreatePopupMenu();
	m_rootMenu.AppendMenu(MF_POPUP, (UINT)m_inputMixbinMenu.Detach(), _T("Insert &Input Mixbin"));
	m_rootMenu.AppendMenu(MF_POPUP, (UINT)m_outputMixbinMenu.Detach(), _T("Insert &Output Mixbin"));
	m_rootMenu.AppendMenu(MF_POPUP, (UINT)m_effectMenu.Detach(), _T("Insert &Effect"));
	m_rootMenu.AppendMenu(MF_STRING, ID_GRID_DISCONNECT_ALL, _T("Disconnect &All"));
	m_rootMenu.AppendMenu(MF_STRING, ID_GRID_DELETE_ALL, _T("&Delete All"));
	m_rootMenu.AppendMenu(MF_STRING, ID_GRID_SHOW_GRID, _T("&Show Grid"));

	// Create the font
	m_font.CreatePointFont(80, _T("Arial"));

	// Start it up...
	SetTitle();
}

//------------------------------------------------------------------------------
//	CGraph::~CGraph
//------------------------------------------------------------------------------
CGraph::~CGraph(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	int			i;
	CModule*	pModule;
	CPatchCord*	pPatchCord;

	// I am dead
	m_bDead = TRUE;

	// Stop all drawing
	Freeze();

	// Delete the imagebuilder
	delete m_imageBuilder;

	// Free data
	ClearEffects();
	ClearMixbins();

	// Delete modules
	for(i=m_modules.GetSize()-1; i>=0; --i)
	{
		pModule = m_modules[i];
		m_modules.RemoveAt(i);
		delete pModule;
	}

	// Delete any patch cords that are left
	for(i=m_patchCords.GetSize()-1; i>=0; --i)
	{
		pPatchCord = m_patchCords[i];
		m_patchCords.RemoveAt(i);
		delete pPatchCord;
	}
}

//------------------------------------------------------------------------------
//	CGraph::Draw
//------------------------------------------------------------------------------
void
CGraph::Draw(
			 IN const CRect&	rect,
			 IN CDC*			pDC
			 )
/*++

Routine Description:

	Draws the graph and its contents

Arguments:

	None

Return Value:

	None

--*/
{
	int		i;
	BOOL	deleteDC;;
	CFont*	oldFont;
	int		x;
	int		y;
	CRect	bigRect = rect;

	if(m_bDead || IsFrozen())
		return;

	// Create a device context?
	if(pDC == NULL)
	{
		pDC			= new CClientDC(m_pParent);
		deleteDC	= TRUE;
	}
	else
		deleteDC = FALSE;

	// Adjust the origin
	pDC->SetWindowOrg(m_rect.left, m_rect.top);	

	bigRect.InflateRect(5,5,5,5);

	// If no DC is specified, clear the region manually
	if(deleteDC)
		pDC->FillSolidRect(bigRect, RGB(255, 255, 255));
	
	// Select the font
	oldFont = pDC->SelectObject(&m_font);

	// Draw the grid?
	if(m_bDrawGrid)
	{
		for(x=bigRect.left - (bigRect.left % SCALE); x<=bigRect.right; x+=SCALE)
		{
			for(y=bigRect.top - (bigRect.top % SCALE); y<=bigRect.bottom; y+=SCALE)
				pDC->SetPixel(x, y, RGB(0, 128, 128));
		}
	}

	// Draw all modules within the rect
	for(i=0; i<m_modules.GetSize(); ++i)
	{
		if(IntersectRects(bigRect, m_modules[i]->GetRect()))
			m_modules[i]->Draw(pDC);
	}

	// Draw all the patch cords
	for(i=0; i<m_patchCords.GetSize(); ++i)
	{
		if(IntersectRects(bigRect, m_patchCords[i]->GetRect()))
			m_patchCords[i]->Draw(pDC);
	}

	// Draw a moving modules
	if(m_moveModules.GetSize())
	{
		for(i=0; i<m_moveModules.GetSize(); ++i)
		{
			if(IntersectRects(bigRect, m_moveModules[i]->GetRect()))
				m_moveModules[i]->Draw(pDC);
		}
	}

	// Restore
	pDC->SelectObject(oldFont);

	// Delete the dc?
	if(deleteDC)
		delete pDC;
}

//------------------------------------------------------------------------------
//	CGraph::ToggleGrid
//------------------------------------------------------------------------------
void
CGraph::ToggleGrid(void)
/*++

Routine Description:

	Toggles the state of the grid

Arguments:

	None

Return Value:

	None

--*/
{	
	m_bDrawGrid = !m_bDrawGrid;
	Draw(m_rect);
}

//------------------------------------------------------------------------------
//	CGraph::DeletePatchCord
//------------------------------------------------------------------------------
void 
CGraph::DeletePatchCord(
						IN CPatchCord*	pPatchCord
						)
/*++

Routine Description:

	Deletes a patch cord and redraws the exposed region

Arguments:

	IN pPatchCord -	Patch cord to delete

Return Value:

	None

--*/
{
	int						i;
	int						ii;
	CPatchCord*				pJoin[2] = {NULL, NULL};
	CPatchCord::PatchNode*	pNode;
	int						count;
	BOOL					bEnd;

	for(i=0; i<m_patchCords.GetSize(); ++i)
	{
		if(m_patchCords[i] == pPatchCord)
		{
			m_patchCords.RemoveAt(i);
			Draw(pPatchCord->GetRect());
			break;
		}
	}

	// If the patch cord is connected to only 2 other patch cords, those
	// two cords will need to be joined
	for(ii=0; ii<2; ++ii)
	{
		if(ii)
			bEnd = TRUE;
		else
			bEnd = FALSE;

		for(count=0, pNode = pPatchCord->GetPatchNode(); pNode; pNode=pNode->pNext)
		{
			if(pNode->bEnd == bEnd)
			{
				if(count < 2)
					pJoin[count] = pNode->pPatchCord;
				++count;
			}
		}

		if(count == 2)
		{
			// Remove pPatchCord from both
			pJoin[0]->RemovePatchCord(pPatchCord);
			pJoin[1]->RemovePatchCord(pPatchCord);
			pPatchCord->RemovePatchCord(pJoin[0]);
			pPatchCord->RemovePatchCord(pJoin[1]);

			// Remove B from the list
			for(i=0; i<m_patchCords.GetSize(); ++i)
			{
				if(m_patchCords[i] == pJoin[1])
				{
					m_patchCords.RemoveAt(i);
					break;
				}
			}

			// Join B to A (which will delete B)
			pJoin[0]->Join(pJoin[1]);

			Draw(pJoin[0]->GetRect());
		}
	}

	delete pPatchCord;

	ValidateGraph();
}

//------------------------------------------------------------------------------
//	CGraph::SetRect
//------------------------------------------------------------------------------
void 
CGraph::SetRect(
				const CRect&	rect
				)
/*++

Routine Description:

	Sets the visible rect

Arguments:

	IN rect -	Rect

Return Value:

	None

--*/
{
	m_mouse		-= m_rect.TopLeft();
	m_upperLeft	-= m_rect.TopLeft();
	m_rect		= rect;
	m_mouse		+= m_rect.TopLeft();
	m_upperLeft	+= m_rect.TopLeft();
}

//------------------------------------------------------------------------------
//	CGraph::GetBounds
//------------------------------------------------------------------------------
const CRect&
CGraph::GetBounds(void)
/*++

Routine Description:

	Returns the bounds of all the data

Arguments:

	None

Return Value:

	The data bounds

--*/
{
	int	i;

	m_bounds.SetRect(0, 0, 0, 0);

	for(i=0; i<m_modules.GetSize(); ++i)
		m_bounds.UnionRect(m_bounds, m_modules[i]->GetRect());

	for(i=0; i<m_patchCords.GetSize(); ++i)
		m_bounds.UnionRect(m_bounds, m_patchCords[i]->GetRect());

	return m_bounds;
}

//------------------------------------------------------------------------------
//	CGraph::SetPaths
//------------------------------------------------------------------------------
void 
CGraph::SetPaths(
				 IN LPCTSTR	pIniPath,
				 IN LPCTSTR	pDspCodePath
				 )
/*++

Routine Description:

	Saves the ini and dsp paths to the registry

Arguments:

	None

Return Value:

	None

--*/
{
	int		i;
	HKEY	hKey;

	if(m_iniPath != pIniPath)
	{
		if(!IsEmpty())
		{
			if(AfxMessageBox(_T("Changing the INI File Path will invalidate your current image. Do you wish to continue?"), MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDCANCEL)
				return;
			
			// Start fresh
			New(FALSE);
		}

		m_iniPath = pIniPath;

		// Clear the effects menu
		m_rootMenu.DeleteMenu(2, MF_BYPOSITION);

		if(LoadEffects())
		{
			// Insert effects menu
			if(m_effectMenu.CreatePopupMenu())
			{
				// Add all the effects to the insert effects menu
				for(i=0; i<m_effects.GetSize(); ++i)
					m_effectMenu.AppendMenu(MF_STRING, (UINT)(i+1), m_effects[i]->GetEffectName());
				m_rootMenu.InsertMenu(2, MF_BYPOSITION | MF_POPUP, (UINT)m_effectMenu.Detach(), _T("&Insert Effect"));
			}
		}
		else
			AfxMessageBox(_T("Unable to load effects. Please specify a valid directory."), MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}
	m_dspCodePath = pDspCodePath;

	// Update the registry
	if(RegCreateKeyEx(HKEY_CURRENT_USER,  
					  _T("Software\\Microsoft\\dspbuilder"), 
					  0, NULL, REG_OPTION_NON_VOLATILE,
					  KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, _T("iniPath"), NULL, REG_SZ, 
					  (const unsigned char*)(LPCTSTR)m_iniPath, 
					  (unsigned long)m_iniPath.GetLength()+1);
		RegSetValueEx(hKey, _T("dspCodePath"), NULL, REG_SZ,
					  (const unsigned char*)(LPCTSTR)m_dspCodePath, 
					  (unsigned long)m_dspCodePath.GetLength()+1);

		RegCloseKey(hKey);
	}
}

//------------------------------------------------------------------------------
//	CGraph::IsEmpty
//------------------------------------------------------------------------------
BOOL 
CGraph::IsEmpty(void) const
/*++

Routine Description:

	Returns TRUE if the graph is empty

Arguments:

	None

Return Value:

	TRUE if the graph is empty, FALSE otherwise

--*/
{
	return (m_modules.GetSize() == 0);
}

//------------------------------------------------------------------------------
//	CGraph::New
//------------------------------------------------------------------------------
BOOL 
CGraph::New(
			IN BOOL	bAllowCancel
			)
/*++

Routine Description:

	Nukes the graph and restarts fresh

Arguments:

	IN bAllowCancel -	TRUE to allow the user to cancel if changes are made

Return Value:

	Nothing

--*/
{
	int			i;
	UINT		flags;
	CModule*	pModule;
	CPatchCord*	pPatchCord;

	// Prompt to save
	if(!IsEmpty() && m_bChangesMade)
	{	
		if(bAllowCancel)
			flags = MB_YESNOCANCEL | MB_APPLMODAL | MB_ICONQUESTION;
		else
			flags = MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION;
		switch(AfxMessageBox(_T("Save changes?"), flags))
		{
		case IDYES:
			if(!Save(m_filename))
				return FALSE;
			break;
		case IDCANCEL:
			return FALSE;
		default:
			break;
		}
	}

	// Stop all drawing
	PushFreeze();

	// Delete modules
	for(i=m_modules.GetSize()-1; i>=0; --i)
	{
		pModule = m_modules[i];
		m_modules.RemoveAt(i);
		delete pModule;
	}

	// Delete any patch cords that are left
	for(i=m_patchCords.GetSize()-1; i>=0; --i)
	{
		pPatchCord = m_patchCords[i];
		m_patchCords.RemoveAt(i);
		delete pPatchCord;
	}

	// Reset in use flags
	for(i=0; i<m_inputMixbinInUse.GetSize(); ++i)
		m_inputMixbinInUse[i] = FALSE;
	for(i=0; i<m_outputMixbinInUse.GetSize(); ++i)
		m_outputMixbinInUse[i] = FALSE;

	m_pParent->Reset();

	PopFreeze();
	Draw(m_rect);

	m_filename.Empty();
	m_bChangesMade	= FALSE;
	m_bImageSaved	= FALSE;
	SetTitle();

	return TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::Save
//------------------------------------------------------------------------------
BOOL 
CGraph::Save(
			 IN LPCTSTR	pFilename
			 )
/*++

Routine Description:

	Saves the graph 

Arguments:

	IN pFilename -	File to save to

Return Value:

	TRUE if the save succeeded, FALSE otherwise

--*/
{
	CFile	file;
	CString	string;
	BYTE	b;
	WORD	word;
	int		i;

	if(pFilename && strlen(pFilename))
		m_filename = pFilename;

	// Was a filename specified?
	else if(m_filename.IsEmpty())
	{
		CFileDialog	fileDialog(FALSE, _T("fx"), NULL, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
							   _T("DSP Builder Files (.fx)|*.fx|All Files (*.*)|*.*||"));
		if(fileDialog.DoModal() != IDOK)
			return FALSE;
		m_filename		= fileDialog.GetPathName();
		m_bImageSaved	= FALSE;
	}

	SetTitle();

	// Open the file
	if(!file.Open(m_filename, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary))
	{
		string.Format(_T("Unable to open file: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}

	// Write the header
	file.Write("DSPBUILDER", 10);

	// Write the version
	b = cFileVersion;
	file.Write(&b, sizeof(b));

	// Save the build options
	file.Write((LPCTSTR)m_i3dl2Config, ((unsigned)m_i3dl2Config.GetLength() + 1) * sizeof(TCHAR));
	b = (BYTE)(m_bPatchXtalkToLFE ? 1 : 0);
	file.Write(&b, sizeof(b));
	b = (BYTE)(m_bIncludeXtalk ? 1 : 0);
	file.Write(&b, sizeof(b));

	// Write the number of modules
	word = (WORD)m_modules.GetSize();
	file.Write(&word, sizeof(word));

	// Write all the modules
	for(i=0; i<m_modules.GetSize(); ++i)
		m_modules[i]->Write(file);
	
	// Write the number of patch cords
	word = (WORD)m_patchCords.GetSize();
	file.Write(&word, sizeof(word));

	// Write the patch cords
	for(i=0; i<m_patchCords.GetSize(); ++i)
		m_patchCords[i]->Write(file);

	m_bChangesMade = FALSE;
	file.Close();

	return TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::SelectBuildOptions
//------------------------------------------------------------------------------
BOOL
CGraph::SelectBuildOptions(void)
/*++

Routine Description:

	Selects the build options

Arguments:

	None

Return Value:

	TRUE on Ok, FALSE on Cancel

--*/
{
	CStringArray	strings24k;
	int				i;

	// Prompt for export options
	if(m_pI3DL2_24K)
	{
		for(i=0; i<m_pI3DL2_24K->GetNumConfigs(); ++i)
			strings24k.Add(m_pI3DL2_24K->GetConfigs()[i].name);
	}

	CExportDialog	exportDialog(strings24k);	
	exportDialog.SetPatchXTalkToLFE(m_bPatchXtalkToLFE);
	exportDialog.SetI3DL2(m_i3dl2Config);
	exportDialog.SetIncludeXTalk(m_bIncludeXtalk);
	if(exportDialog.DoModal() == IDCANCEL)
		return FALSE;

	m_i3dl2Config		= exportDialog.GetI3DL2();
	m_bPatchXtalkToLFE	= exportDialog.GetPatchXTalkToLFE();
	m_bIncludeXtalk		= exportDialog.GetIncludeXTalk();
	m_bChangesMade		= TRUE;
	SetTitle();
	return TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::Open
//------------------------------------------------------------------------------
BOOL 
CGraph::Open(
			 IN LPCTSTR	pFilename
			 )
/*++

Routine Description:

	Opens a grid file

Arguments:

	IN pFilename -	File to open

Return Value:

	TRUE if the open succeeded, FALSE otherwise

--*/
{
	CFile	file;
	CString	filename;
	CString	string;
	BYTE	b;
	char	header[11];

	if(pFilename && strlen(pFilename))
		filename = pFilename;
	else
		filename = m_filename;

	// Was a filename specified?
	if(filename.IsEmpty())
	{
		CFileDialog	fileDialog(TRUE, _T("fx"), NULL, OFN_PATHMUSTEXIST,
							   _T("DSP Builder Files (.fx)|*.fx|All Files (*.*)|*.*||"));
		if(fileDialog.DoModal() != IDOK)
			return FALSE;
		filename = fileDialog.GetPathName();
	}

	// If this is the same file, turn off changes made to prevent a save
	if(filename == m_filename)
		m_bChangesMade = FALSE;

	// Open the file
	if(!file.Open(filename, CFile::modeRead | CFile::typeBinary))
	{
		string.Format(_T("Unable to open file: %s"), (LPCTSTR)filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}

	// Read the header
	if(file.Read(header, 10) != 10)
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}
	header[10]	= '\0';
	string		= header;
	if(string != "DSPBUILDER")
	{
		string.Format(_T("File %s is not a valid DSP file"), (LPCTSTR)filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}

	// Read the version
	if(file.Read(&b, sizeof(b)) != sizeof(b))
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}

	// Old Version? give warning
	if((b == 1) || (b == 3))
	{
		if(AfxMessageBox(_T("You are attempting to load an older file version.\n"
						    "When you save the file, it will be saved in the new format."),
						 MB_OKCANCEL | MB_ICONINFORMATION | MB_APPLMODAL) == IDCANCEL)
			return FALSE;
	}
	else if(b == 2)
	{
		if(AfxMessageBox(_T("You are attempting to load an older file version.\n"
						    "When you save the file, it will be saved in the new format.\n"
						    "You will also need to rebuild the image before transmitting."), 
						 MB_OKCANCEL | MB_ICONINFORMATION | MB_APPLMODAL) == IDCANCEL)
			return FALSE;
	}

	New(FALSE);
	m_filename = filename;

	switch(b)
	{
	case 1:
		if(!ReadVersion1(file))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			New(FALSE);
			return FALSE;
		}
		break;
	case 2:
		if(!ReadVersion2or3or4(file, b))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			New(FALSE);
			return FALSE;
		}
		m_bChangesMade = TRUE;
		break;
	case 3:
	case 4:
		if(!ReadVersion2or3or4(file, b))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			New(FALSE);
			return FALSE;
		}
		break;
	default:
		string.Format(_T("Unsupported DSP file version: %s"), (LPCTSTR)filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}

	file.Close();
	SetTitle();

	// Update the used mixbin info
	SetInUse();

	// See if the .ini .bin and .h files exist
	m_bImageSaved = FALSE;
	if(file.Open(GetBinFilename(), CFile::modeRead))
	{
		if(file.GetLength() != 0)
		{
			file.Close();
			if(file.Open(GetIniFilename(), CFile::modeRead))
			{
				if(file.GetLength() != 0)
				{
					file.Close();
					if(file.Open(GetHFilename(), CFile::modeRead))
					{
						if(file.GetLength() != 0)
							m_bImageSaved = TRUE;
						file.Close();
					}
				}
				else
					file.Close();
			}
		}
		else
			file.Close();
	}

	m_pParent->UpdateScrollBars();
	ValidateGraph();
	Draw(m_rect);
	return TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::GetDSPCycles
//------------------------------------------------------------------------------
int 
CGraph::GetDSPCycles(void) const
/*++

Routine Description:

	Returns the total DSP cycles

Arguments:

	None

Return Value:

	Total DSP Cycles

--*/
{
	int	i;
	int	total = 0;

	for(i=0; i<m_modules.GetSize(); ++i)
		total += m_modules[i]->GetDSPCycles();

	// Adjust for the addition of an i3dl2 reverb
	if(m_i3dl2Config.GetLength() && m_pI3DL2_24K)
		total += m_pI3DL2_24K->GetDSPCycles();

	// Adjust for the addition of XTalk
	if(m_bIncludeXtalk && m_pXTalk)
		total += m_pXTalk->GetDSPCycles();

	// Adjust for the addition of the Patch XTalk to LFE
	if(m_bPatchXtalkToLFE && m_p2x1Mixer)
		total += m_p2x1Mixer->GetDSPCycles() * 2;

	return total;
}

//------------------------------------------------------------------------------
//	CGraph::GetYMemSize
//------------------------------------------------------------------------------
int 
CGraph::GetYMemSize(void) const
/*++

Routine Description:

	Returns the total Y Mem size

Arguments:

	None

Return Value:

	Total Y Mem size

--*/
{
	int	i;
	int	total = 0;

	for(i=0; i<m_modules.GetSize(); ++i)
		total += m_modules[i]->GetYMemSize();
	return total;
}

//------------------------------------------------------------------------------
//	CGraph::GetScratchLength
//------------------------------------------------------------------------------
int 
CGraph::GetScratchLength(void) const
/*++

Routine Description:

	Returns the total Scratch length

Arguments:

	None

Return Value:

	Total Scratch length

--*/
{
	int	i;
	int	total = 0;

	for(i=0; i<m_modules.GetSize(); ++i)
		total += m_modules[i]->GetScratchLength();
	return total;
}

//------------------------------------------------------------------------------
//	CGraph::GetDSPCyclesPercent
//------------------------------------------------------------------------------
float 
CGraph::GetDSPCyclesPercent(void) const
/*++

Routine Description:

	Returns the dsp cycles as a percent of max

Arguments:

	None

Return Value:

	Percent of dsp cycles used

--*/
{
	return (float(GetDSPCycles()) / float(DSP_MAXCYCLES_AVAILABLE));
}

//------------------------------------------------------------------------------
//	CGraph::GetYMemSizePercent
//------------------------------------------------------------------------------
float 
CGraph::GetYMemSizePercent(void) const
/*++

Routine Description:

	Returns the y mem size as a percent of max

Arguments:

	None

Return Value:

	Percent of y mem size used

--*/
{
	return (float(GetYMemSize()) / float(DSP_YMEMORY_SIZE));
}

#if 0
//------------------------------------------------------------------------------
//	CGraph::GetScratchLengthPercent
//------------------------------------------------------------------------------
float 
CGraph::GetScratchLengthPercent(void) const
/*++

Routine Description:

	Returns the scratch length as a percent of max

Arguments:

	None

Return Value:

	Percent of scratch length used

--*/
{
	return (float(GetScratchLength()) / float(...));
}
#endif

//------------------------------------------------------------------------------
//	CGraph::ReadString
//------------------------------------------------------------------------------
BOOL 
CGraph::ReadString(
				   IN CFile&	file, 
				   OUT CString&	string
				   )
/*++

Routine Description:

	Reads a string from a file

Arguments:

	IN file -		File to read from
	OUT string -	String read

Return Value:

	TRUE on success, FALSE on failure

--*/
{
	int		i;
	BOOL	ret = TRUE;
	TCHAR	buffer[1024];

	for(i=0; ret && i<1024; ++i)
	{
		ret = (file.Read(&buffer[i], sizeof(TCHAR)) == sizeof(TCHAR));
		if(buffer[i] == '\0')
			break;
	}
	if(ret)
		string = buffer;
	return ret;
}
	
//------------------------------------------------------------------------------
//	CGraph::ShortestPath
//------------------------------------------------------------------------------
void 
CGraph::ShortestPath(
					 IN const CJack*						pA, 
					 IN const CJack*						pB, 
					 OUT CArray<CPatchCord*, CPatchCord*>&	path
					 )
/*++

Routine Description:

	Generates the shortest path between pA and pB

Arguments:

	IN pA -		Jack A
	IN pB -		Jack B
	OUT path -	Path between the two (this will include the patch cords of
				pA and pB)

Return Value:

	None

--*/
{
#if 0
	int									i;
	CArray<CPatchCord*, CPatchCord*>	patchCords;

	// Generate the connection array
	pA->GetPatchCord()->GetPatchCords(patchCords);

	// Quick sanity check - make sure pB is in the list
	for(i=0; i<patchCords.GetSize(); ++i)
	{
		if(patchCords[i] == pB->GetPatchCord())
			break;
	}
	if(i == patchCords.GetSize())
		return;

#endif

	// TODO:

#if 0
struct Vert;
struct Node 
{ 
	Vert*	pVert;
	Node*	pNext;
};
struct Vert
{
	char	c;
	Node*	pNode;
	BOOL	bInTree;
};
struct TreeNode
{
	char		c;
	Vert*		pVert;
	TreeNode*	pNext;
	TreeNode*	pChild;
	TreeNode*	pParent;
};
struct QueueNode
{
	TreeNode*	pNode;
	QueueNode*	pNext;
};

static QueueNode*	queue			= NULL;
static QueueNode*	queueIn			= NULL;
static QueueNode*	queueOut		= NULL;
static int index(char c)
{
	return c - 'A';
}
static void queueinit(void)
{
	QueueNode*	pNode;
	QueueNode*	pFirst = NULL;
	int		i;
	for(i=0; i<1024; ++i)
	{
		pNode			= new QueueNode;
		pNode->pNext	= queue;
		if(pFirst == NULL)
			pFirst = pNode;
		queue = pNode;
	}
	pFirst->pNext	= queue;
	queueIn			= queue;
	queueOut		= queue;
}
static BOOL queueempty(void)
{
	return (queueIn == queueOut) ? TRUE : FALSE;
}
static void put(TreeNode* pTreeNode)
{
	queueIn->pNode	= pTreeNode;
	queueIn			= queueIn->pNext;
}
static TreeNode* get(void)
{
	TreeNode* pNode = queueOut->pNode;
	queueOut = queueOut->pNext;
	return pNode;
}

static void BuildTree(void)
{
	int			i;
	int			x;
	int			y;
	Node*		pNode;
	Vert*		pVerts;
	TreeNode*	pTree;
	TreeNode*	pTreeNode;
	TreeNode*	pChildTreeNode;
	char		charEdges[]	= "FAAGABACLMJMJLJKEDFDHIFEGEDHDIEH";
	char		charVerts[]	= "ABCDEFGHIJKLM";
	int			numVerts	= sizeof(charVerts) - 1;//strlen(charVerts);
	int			numEdges	= (sizeof(charEdges)-1) / 2;

	pVerts = new Vert [numVerts];
	for(i=0; i<numVerts; ++i)
	{
		pVerts[i].c			= charVerts[i];
		pVerts[i].pNode		= NULL;
		pVerts[i].bInTree	= FALSE;
	}

	for(i=0; i<numEdges; ++i)
	{
		x	= index(charEdges[i*2]);
		y	= index(charEdges[i*2+1]);

		pNode			= new Node;
		pNode->pVert	= &pVerts[x];
		pNode->pNext	= pVerts[y].pNode;
		pVerts[y].pNode	= pNode;

		pNode			= new Node;
		pNode->pVert	= &pVerts[y];
		pNode->pNext	= pVerts[x].pNode;
		pVerts[x].pNode	= pNode;
	}

	queueinit();

	pTreeNode					= new TreeNode;
	pTreeNode->c				= pVerts[0].c;
	pTreeNode->pVert			= &pVerts[0];
	pTreeNode->pNext			= NULL;
	pTreeNode->pChild			= NULL;
	pTreeNode->pParent			= NULL;
	pTreeNode->pVert->bInTree	= TRUE;
	pTree						= pTreeNode;
	put(pTreeNode);
	while(!queueempty())
	{
		pTreeNode = get();
		for(pNode=pTreeNode->pVert->pNode; pNode; pNode=pNode->pNext)
		{
			if(!pNode->pVert->bInTree)
			{
				pChildTreeNode					= new TreeNode;
				pChildTreeNode->c				= pNode->pVert->c;
				pChildTreeNode->pChild			= NULL;
				pChildTreeNode->pNext			= pTreeNode->pChild;
				pChildTreeNode->pParent			= pTreeNode;
				pChildTreeNode->pVert			= pNode->pVert;
				pChildTreeNode->pVert->bInTree	= TRUE;
				pTreeNode->pChild				= pChildTreeNode;
				put(pChildTreeNode);
			}
		}
	}
}
#endif
}

//------------------------------------------------------------------------------
//	CGraph::GetIniFilename
//------------------------------------------------------------------------------
LPCTSTR 
CGraph::GetIniFilename(void) const
/*++

Routine Description:

	Returns the ini filename

Arguments:

	None

Return Value:

	The ini filename if a dsp file exists, An empty string otherwuise

--*/
{
	static CString	filename;
	int				index;

	filename	= (LPCTSTR)m_filename;
	index		= filename.ReverseFind('.');
	if(index != -1)
		filename.Delete(index, filename.GetLength()-index);
	filename += _T(".ini");

	return (LPCTSTR)filename;
}

//------------------------------------------------------------------------------
//	CGraph::SaveImage
//------------------------------------------------------------------------------
BOOL 
CGraph::SaveImage(void)
/*++

Routine Description:

	Saves the grid as a dsp image.

Arguments:

	Nothing

Return Value:

	TRUE on success, FALSE on failure

--*/
{
	if(IsGraphValid())
	{
		if(!strcmp(GetIniFilename(), _T(".ini")))
		{
			if(!Save(NULL))
				return FALSE;
		}
		return BuildEffectChains(GetIniFilename());
	}

	AfxMessageBox(_T("The grid is invalid and cannot be saved as a DSP Image."), 
				  MB_OK | MB_ICONERROR | MB_APPLMODAL);
	return FALSE;
}

//------------------------------------------------------------------------------
//	CGraph::IsGraphValid
//------------------------------------------------------------------------------
BOOL 
CGraph::IsGraphValid(void)
/*++

Routine Description:

	Checks to see if the graph if valid. A graph is valid if there are no 
	invalid	patch cords

Arguments:

	None

Return Value:

	TRUE if the graph is valid, FALSE otherwise

--*/
{
	int	i;	

	ValidateGraph();
	for(i=0; i<m_patchCords.GetSize(); ++i)
	{
		if(m_patchCords[i]->IsInvalid())
			return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::MarkPatchCords
//------------------------------------------------------------------------------
void 
CGraph::MarkPatchCords(
					   IN BOOL mark
					   )
/*++

Routine Description:

	Marks all the patch cords

Arguments:

	IN mark -	Mark to use

Return Value:

	None

--*/
{
	int	i;

	for(i=0; i<m_patchCords.GetSize(); ++i)
		m_patchCords[i]->Mark(mark);
}

//------------------------------------------------------------------------------
//	CGraph::GetBinFilename
//------------------------------------------------------------------------------
LPCTSTR 
CGraph::GetBinFilename(void) const
/*++

Routine Description:

	Returns the bin filename

Arguments:

	None

Return Value:

	The bin filename if a dsp file exists, An empty string otherwuise

--*/
{
	static CString	filename;
	int				index;

	filename	= (LPCTSTR)m_filename;
	index		= filename.ReverseFind('.');
	if(index != -1)
		filename.Delete(index, filename.GetLength()-index);
	filename += _T(".bin");

	return (LPCTSTR)filename;
}

//------------------------------------------------------------------------------
//	CGraph::GetHFilename
//------------------------------------------------------------------------------
LPCTSTR 
CGraph::GetHFilename(void) const
/*++

Routine Description:

	Returns the header filename

Arguments:

	None

Return Value:

	The header filename if a dsp file exists, An empty string otherwuise

--*/
{
	static CString	filename;
	int				index;

	filename	= (LPCTSTR)m_filename;
	index		= filename.ReverseFind('.');
	if(index != -1)
		filename.Delete(index, filename.GetLength()-index);
	filename += _T(".h");

	return (LPCTSTR)filename;
}

//------------------------------------------------------------------------------
//	CGraph::SetModuleIndex
//------------------------------------------------------------------------------
void 
CGraph::SetModuleIndex(
					   IN LPCTSTR	pIniName, 
					   IN DWORD		index
					   )
/*++

Routine Description:

	Sets the effect index for a module

Arguments:

	IN pIniName -	Ini name of effect (unique)
	IN index -		Index to set

Return Value:

	None

--*/
{
	int	m;

	for(m=0; m<m_modules.GetSize(); ++m)
	{
		if(!strcmp(m_modules[m]->GetIniName(), pIniName))
		{
			m_modules[m]->SetIndex(index);
			return;
		}
	}
}

//------------------------------------------------------------------------------
//	CGraph::TransmitModuleParameters
//------------------------------------------------------------------------------
void 
CGraph::TransmitModuleParameters(
								 IN const CModule*	pModule,
								 IN int				index	// = -1
								 ) const 
/*++

Routine Description:

	Transmits a module's parameters to the Xbox

Arguments:

	IN pModule -	Module whose parameters will be transmitted
	IN index -		Effect index to transmit (-1 for all)

Return Value:

	None

--*/
{
	m_pParent->TransmitModuleParameters(pModule, index); 
}

//------------------------------------------------------------------------------
//	CGraph::GetPatchCordFromId
//------------------------------------------------------------------------------
CPatchCord* 
CGraph::GetPatchCordFromId(
						   IN DWORD id
						   ) const
/*++

Routine Description:

	Returns a pointer to a patch cord from its id

Arguments:

	IN id -	Id of patch cord

Return Value:

	Pointer to the patch cord (or NULL)

--*/
{
	int	i;
	int	ii;

	// Check for normal patches
	for(i=0; i<m_patchCords.GetSize(); ++i)
	{
		if(m_patchCords[i]->GetId() == id)
			return m_patchCords[i];
	}

	// Check jacks on modules
	for(i=0; i<m_modules.GetSize(); ++i)
	{
		for(ii=0; ii<m_modules[i]->GetInputs().GetSize(); ++ii)
		{
			if(id == m_modules[i]->GetInputs()[ii]->GetOwnPatchCord()->GetId())
				return m_modules[i]->GetInputs()[ii]->GetOwnPatchCord();
		}
		for(ii=0; ii<m_modules[i]->GetOutputs().GetSize(); ++ii)
		{
			if(id == m_modules[i]->GetOutputs()[ii]->GetOwnPatchCord()->GetId())
				return m_modules[i]->GetOutputs()[ii]->GetOwnPatchCord();
		}
	}

	return NULL;
}

//------------------------------------------------------------------------------
//	CGraph::GetModuleFromId
//------------------------------------------------------------------------------
CModule* 
CGraph::GetModuleFromId(
						IN DWORD id
						) const
/*++

Routine Description:

	Returns a pointer to a module from its id

Arguments:

	IN id -	Id of module

Return Value:

	Pointer to the module (or NULL)

--*/
{
	int	i;

	for(i=0; i<m_modules.GetSize(); ++i)
	{
		if(m_modules[i]->GetId() == id)
			return m_modules[i];
	}

	return NULL;
}

//------------------------------------------------------------------------------
//	CGraph::GetJackFromId
//------------------------------------------------------------------------------
CJack* 
CGraph::GetJackFromId(
					  IN DWORD id
					  ) const
/*++

Routine Description:

	Returns a pointer to a jack from its id

Arguments:

	IN id -	Id of jack 

Return Value:

	Pointer to the jack (or NULL)

--*/
{
	int	m;
	int	i;

	for(m=0; m<m_modules.GetSize(); ++m)
	{
		for(i=0; i<m_modules[m]->GetInputs().GetSize(); ++i)
		{
			if(m_modules[m]->GetInputs()[i]->GetId() == id)
				return m_modules[m]->GetInputs()[i];
		}
		for(i=0; i<m_modules[m]->GetOutputs().GetSize(); ++i)
		{
			if(m_modules[m]->GetOutputs()[i]->GetId() == id)
				return m_modules[m]->GetOutputs()[i];
		}
	}

	return NULL;
}

//------------------------------------------------------------------------------
//	CGraph::LoadEffects
//------------------------------------------------------------------------------
BOOL
CGraph::LoadEffects(void)
/*++

Routine Description:

	Loads all effects defined by ".ini" files in the ENV_VAR_INI_PATH directory

Arguments:

	None

Return Value:

	TRUE on success, FALSE otherwise

--*/
{
	CString			pathName;
	HANDLE			fileHandle;
	WIN32_FIND_DATA	findData;
	CHAR*			lpResult;
	CString			name;
	CString			string;
	int				i;
	int				j;
	int				dspCycles;
	int				yMemSize;
	int				scratchLength;
	int				numInputs;
	int				numOutputs;
	CString			dspCode;
	CStringArray	inputs;
	CStringArray	outputs;
	CString			config;
	CStringArray	nameList;
	CStringArray	dspCodeList;
	BOOL			bConfig;
	CModule*		pModule;
	CModule*		pTemp;
	
	// Clear the effects list
	ClearEffects();

	// Find all "*.ini" files in the ini directory
	pathName.Format(_T("%s\\*.ini"), (LPCTSTR)m_iniPath);
	fileHandle = FindFirstFile(pathName, &findData);
	if(fileHandle != INVALID_HANDLE_VALUE)
	{
		// Memory for GetPrivateProfileSection() and GetPrivateProfileString
		lpResult = new CHAR [SECTION_MAX_SIZE];

		// Loop through all the files
		do
		{
			// Full filename
			pathName.Format(_T("%s\\%s"), (LPCTSTR)m_iniPath, 
							findData.cFileName);

			// First pass to see if this is a valid file is to check 
			// for the fstate section
			if(GetPrivateProfileSection(FXSTATE_SECTION_NAME, lpResult, 
										SECTION_MAX_SIZE, pathName) == 0)
				continue;

			// Name
			GetPrivateProfileString(FXSTATE_SECTION_NAME, FXSTATE_EFFECT_NAME,
								    _T("<Unknown Effect>"), lpResult, 
									SECTION_MAX_SIZE, pathName);
			name = lpResult;

			// DSP Code
			GetPrivateProfileString(FXSTATE_SECTION_NAME, FXPARAM_DSPCODE_NAME,
								    _T("<dspFilename>"), lpResult, SECTION_MAX_SIZE, 
								    pathName);
			dspCode = lpResult;

			// Has this effect been loaded before?
			for(bConfig=FALSE, i=0; !bConfig && i<nameList.GetSize(); ++i)
			{
				if((nameList[i] == name) && (dspCodeList[i] == dspCode))
					bConfig = TRUE;
			}

			// Configuration
			GetPrivateProfileString(FXSTATE_SECTION_NAME, FX_CONFIG_NAME,
								    _T(""), lpResult, SECTION_MAX_SIZE, 
								    pathName);
			config = lpResult;

			// DSP Cycles
			dspCycles = (int)GetPrivateProfileInt(FXSTATE_SECTION_NAME, 
												  FXPARAM_DSPCYCLES_NAME,
												  0, pathName);

			// Y mem size
			yMemSize = (int)GetPrivateProfileInt(FXSTATE_SECTION_NAME, 
											     FXPARAM_YMEMORY_SIZE,
												 0, pathName);
			
			// Scratch size
			scratchLength = (int)GetPrivateProfileInt(FXSTATE_SECTION_NAME, 
													  FXPARAM_SCRATCH_LENGTH,
													  0, pathName);

			// Num Inputs & Outputs (special case for i3dl2 reverb
			if((name == _T("I3DL2 Reverb")) || (name == _T("I3DL2 24K Reverb")))
			{
				numInputs	= 2;
				numOutputs	= 4;
			}
			else if(name == _T("Simple Reverb"))
			{
				numInputs	= 1;
				numOutputs	= 4;
			}
			else
			{
				numInputs = (int)GetPrivateProfileInt(FXSTATE_SECTION_NAME, 
													  FXPARAM_NUMINPUTS_NAME,
													  0, pathName);
				numOutputs = (int)GetPrivateProfileInt(FXSTATE_SECTION_NAME, 
													   FXPARAM_NUMOUTPUTS_NAME,
													   0, pathName);
			}
			inputs.SetSize(numInputs);
			outputs.SetSize(numOutputs);

			// Inputs
			for(i=0; i<numInputs; ++i)
			{
				string.Format(_T("FX_INPUT%d_NAME"), i);
				GetPrivateProfileString(FXSTATE_SECTION_NAME, string,
										_T("In"), lpResult, SECTION_MAX_SIZE, 
										pathName);
				inputs[i] = lpResult;
			}

			// Outputs
			for(i=0; i<numOutputs; ++i)
			{
				string.Format(_T("FX_OUTPUT%d_NAME"), i);
				GetPrivateProfileString(FXSTATE_SECTION_NAME, string,
										_T("Out"), lpResult, SECTION_MAX_SIZE, 
										pathName);
				outputs[i] = lpResult;
			}

			// If this is just another config, add the config to the effect
			if(bConfig)
			{
				if(config != _T(""))
				{
					// Find the effect
					for(i=0; i<m_effects.GetSize(); ++i)
					{
						if((m_effects[i]->GetEffectName() == name) &&
						   (m_effects[i]->GetDSPCode() == dspCode))
						{
							// Add the configuration
							m_effects[i]->AddConfiguration(config, findData.cFileName);
							break;
						}
					}
				}
			}

			// Otherwise, create a new effect if it has ins and outs
			else if((numInputs > 0) || (numOutputs > 0))
			{
				// Add the effect to the list
				nameList.Add((LPCTSTR)name);
				dspCodeList.Add((LPCTSTR)dspCode);

				// Create the module
				pModule = new CModule(this, CModule::EFFECT, 
									  CModule::COLOR_EFFECT, name, dspCycles, 
									  yMemSize, scratchLength, dspCode, findData.
									  cFileName, &inputs, &outputs);

				// Set the module parameters
				string.Format(_T("%s\\%s"), m_iniPath, findData.cFileName);
				pModule->SetParameters(m_imageBuilder, string);

				// Add it to the list
				m_effects.Add(pModule);

				// Set the configuration
				if(config != _T(""))
					pModule->AddConfiguration(config, findData.cFileName);

				// Is this an I3DL2 Reverb
				if(name == _T("I3DL2 Reverb"))
					m_pI3DL2 = pModule;
				else if(name == _T("I3DL2 24K Reverb"))
					m_pI3DL2_24K = pModule;

				// Or the mixer
				else if(name == _T("2x1 Mixer"))
					m_p2x1Mixer = pModule;

				// Or crosstalk?
				else if(name.CompareNoCase("xtalk") == 0)
					m_pXTalk = pModule;
			}
		}
		while(FindNextFile(fileHandle, &findData));
		FindClose(fileHandle);

		// Free memory
		delete [] lpResult;

		// Sort the effects list
		for(i=m_effects.GetSize()-1; i>=1; --i)
		{
			for(j=1; j<=i; ++j)
			{
				if(Compare(m_effects[j-1]->GetEffectName(), 
						   m_effects[j]->GetEffectName()) > 0)
				{
					pTemp			= m_effects[j-1];
					m_effects[j-1]	= m_effects[j];
					m_effects[j]	= pTemp;
				}
			}
		}

		// Set all configurations to "default"
		for(i=0; i<m_effects.GetSize(); ++i)
		{
			for(j=0; j<m_effects[i]->GetNumConfigs(); ++j)
			{
				if(!m_effects[i]->GetConfigs()[j].name.CompareNoCase(_T("default")))
				{
					m_effects[i]->SetActiveConfig(j);
					break;
				}
			}
		}

		// Were any effects loaded?
		m_bEffectsLoaded = (m_effects.GetSize() > 0) ? TRUE : FALSE;
	}

	return m_bEffectsLoaded;
}

//------------------------------------------------------------------------------
//	CGraph::ClearEffects
//------------------------------------------------------------------------------
void
CGraph::ClearEffects(void)
/*++

Routine Description:

	Clears all effects

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	// Effects
	for(i=0; i<m_effects.GetSize(); ++i)
		delete m_effects[i];
	m_effects.RemoveAll();

	// Set the effects loaded flag
	m_bEffectsLoaded = FALSE;

	// Reset some pointers
	m_pXTalk		= NULL;
	m_pI3DL2		= NULL;
	m_pI3DL2_24K	= NULL;
	m_p2x1Mixer		= NULL;
}

//------------------------------------------------------------------------------
//	CGraph::CreateMixbins
//------------------------------------------------------------------------------
void
CGraph::CreateMixbins(void)
/*++

Routine Description:

	Creates all the mixbins

Arguments:

	None

Return Value:

	None

--*/
{
	int					i;
	CStringArray		strings;
	CModule*			pModule;
	CModule::ColorFlag	colorFlag;
	LPCTSTR				mixbins[] =		// These must remain in the same order
	{									// If their order changes, InsertModule()
		_T("Speaker: Front Left"),		// must be updated
		_T("Speaker: Front Right"),
		_T("Speaker: Center"),
		_T("Speaker: LFE"),
		_T("Speaker: Back Left"),
		_T("Speaker: Back Right"),
		_T("XTalk: Front Left"),
		_T("XTalk: Front Right"),
		_T("XTalk: Back Left"),
		_T("XTalk: Back Right"),
		_T("I3DL2 Send"),
		_T("FX Send 0"),
		_T("FX Send 1"),
		_T("FX Send 2"),
		_T("FX Send 3"),
		_T("FX Send 4"),
		_T("FX Send 5"),
		_T("FX Send 6"),
		_T("FX Send 7"),
		_T("FX Send 8"),
		_T("FX Send 9"),
		_T("FX Send 10"),
		_T("FX Send 11"),
		_T("FX Send 12"),
		_T("FX Send 13"),
		_T("FX Send 14"),
		_T("FX Send 15"),
		_T("FX Send 16"),
		_T("FX Send 17"),
		_T("FX Send 18"),
		_T("FX Send 19")
	};

	// Clear the lists
	ClearMixbins();

	// Create the Input Mixbins
	strings.Add(CString(_T("Out")));
	for(i=0; i<(int)(sizeof(mixbins) / sizeof(mixbins[0])); ++i)
	{
		if(i <= 5)
			colorFlag = CModule::COLOR_SPEAKER;
		else if(i <= 9)
			colorFlag = CModule::COLOR_XTALK;
		else if(i == 10)
			colorFlag = CModule::COLOR_I3DL2;
		else
			colorFlag = CModule::COLOR_FXSEND;
		pModule = new CModule(this, CModule::INPUT_MIXBIN, colorFlag, 
							  mixbins[i], 0, 0, 0, NULL, NULL, NULL, &strings);
		m_inputMixbins.Add(pModule);
		m_inputMixbinInUse.Add(FALSE);
		pModule->SetMixbin(i);
	}

	// Create the Output Mixbins
	strings[0] = _T("In");
	for(i=0; i<(int)(sizeof(mixbins) / sizeof(mixbins[0])); ++i)
	{
		if(i <= 5)
			colorFlag = CModule::COLOR_SPEAKER;
		else if(i <= 9)
			colorFlag = CModule::COLOR_XTALK;
		else if(i == 10)
			colorFlag = CModule::COLOR_I3DL2;
		else
			colorFlag = CModule::COLOR_FXSEND;
		pModule = new CModule(this, CModule::OUTPUT_MIXBIN, colorFlag, 
							  mixbins[i], 0, 0, 0, NULL, NULL, &strings, NULL);
		m_outputMixbins.Add(pModule);
		m_outputMixbinInUse.Add(FALSE);
		pModule->SetMixbin(i);
	}
}

//------------------------------------------------------------------------------
//	CGraph::ClearMixbins
//------------------------------------------------------------------------------
void
CGraph::ClearMixbins(void)
/*++

Routine Description:

	Clears all mixbins

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	// Input mixbins
	for(i=0; i<m_inputMixbins.GetSize(); ++i)
		delete m_inputMixbins[i];
	m_inputMixbins.RemoveAll();

	// Output mixbins
	for(i=0; i<m_outputMixbins.GetSize(); ++i)
		delete m_outputMixbins[i];
	m_outputMixbins.RemoveAll();

	// In Use flags
	m_inputMixbinInUse.RemoveAll();
	m_outputMixbinInUse.RemoveAll();
}

//------------------------------------------------------------------------------
//	CGraph::SetPaths
//------------------------------------------------------------------------------
void
CGraph::SetPaths(void)
/*++

Routine Description:

	Sets the paths for the effects state files

Arguments:

	None

Return Value:

	None

--*/
{
	TCHAR	buffer[1024];
	HKEY	hKey;
	DWORD	type;
	DWORD	size;

	// First try the registry
	if(RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\Microsoft\\dspbuilder"), 
					  0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		// INI Path
		size = sizeof(buffer);
		if(RegQueryValueEx(hKey, _T("iniPath"), NULL, &type,
						   (unsigned char*)buffer, &size) == ERROR_SUCCESS)
		{
			m_iniPath = buffer;
		}

		// DSP Code Path
		size = sizeof(buffer);
		if(RegQueryValueEx(hKey, _T("dspCodePath"), NULL, &type,
						   (unsigned char*)buffer, &size) == ERROR_SUCCESS)
		{
			m_dspCodePath = buffer;
		}
		RegCloseKey(hKey);
	}
			
	// Then try environment variables
	if(m_iniPath.IsEmpty() && 
	   (GetEnvironmentVariable("_XGPIMAGE_INI_PATH", buffer, 
							   sizeof(buffer) / sizeof(buffer[0])) != 0))
	{
		m_iniPath = buffer;
	}
	if(m_dspCodePath.IsEmpty() && 
	   (GetEnvironmentVariable("_XGPIMAGE_DSP_CODE_PATH", buffer, 
							   sizeof(buffer) / sizeof(buffer[0])) != 0))
	{
		m_dspCodePath = buffer;
	}

	// Then try XDK install path
	if(m_iniPath.IsEmpty())
	{
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\XboxSDK\\", 
						0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		{
			size = sizeof(buffer);
			if(RegQueryValueEx(hKey, _T("InstallPath"), NULL, &type,
							   (unsigned char*)buffer, 
							   &size) == ERROR_SUCCESS)
			{
				m_iniPath	= buffer;
				m_iniPath	+= "\\source\\dsound\\dsp\\ini";
			}
			RegCloseKey(hKey);
		}
	}
	if(m_dspCodePath.IsEmpty())
	{
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"Software\\Microsoft\\XboxSDK\\", 
						0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		{
			size = sizeof(buffer);
			if(RegQueryValueEx(hKey, _T("InstallPath"), NULL, &type,
							   (unsigned char*)buffer, 
							   &size) == ERROR_SUCCESS)
			{
				m_dspCodePath	= buffer;
				m_dspCodePath	+= "\\source\\dsound\\dsp\\bin";
			}
			RegCloseKey(hKey);
		}
	}

	// Now just use the current directory
	if(m_iniPath.IsEmpty() && 
	   (GetCurrentDirectory(sizeof(buffer) / sizeof(buffer[0]), buffer) != 0))
	{
		m_iniPath = buffer;
	}
	if(m_dspCodePath.IsEmpty() && 
	   (GetCurrentDirectory(sizeof(buffer) / sizeof(buffer[0]), buffer) != 0))
	{
		m_dspCodePath = buffer;
	}
}

//------------------------------------------------------------------------------
//	CGraph::Compare
//------------------------------------------------------------------------------
int
CGraph::Compare(
			   IN LPCTSTR	pString1,
			   IN LPCTSTR	pString2
			   ) const
/*++

Routine Description:

	Does a case insenstive compare of two strings

Arguments:

	IN pString1 -	String 1
	IN pString2 -	String 2

Return Value:

	< 0	if pString1 < pString2
	0	if pString1 > pString2
	> 0	if pString1 < pString2


--*/
{
	CString	string1 = pString1;
	return string1.CompareNoCase(pString2);
}

//------------------------------------------------------------------------------
//	CGraph::InsertModule
//------------------------------------------------------------------------------
void
CGraph::InsertModule(
					 IN const CPoint&	point,
					 IN int				index
					 )
/*++

Routine Description:

	Inserts a module into the grid

Arguments:

	IN index -	Index of effect

Return Value:

	None

--*/
{
	CModule*					pModule;
	int							i;
	CPoint						upperLeft;
	CArray<CModule*, CModule*&>	insert;
	CModule::State				state	= CModule::OK;
	int							dy		= m_bSnapToGrid ? SCALE : 3;

	// Make sure the move modules list is clear
	m_moveModules.RemoveAll();

	// Flag to begin insert mode
	m_bInsertModules	= TRUE;
	m_bUseOffset		= FALSE;

	// Save the mouse location
	SetMouse(point);

	// Input mixbin?
	if(index & INPUT_MIXBIN_MASK)
	{
		index ^= INPUT_MIXBIN_MASK;

		switch(index)
		{
		case 0:		// All speakers
			for(i=0; i<=5; ++i)
				insert.Add(m_inputMixbins[i]);
			break;
		case 1:	// Front & Rear speakers
			insert.Add(m_inputMixbins[0]);
			insert.Add(m_inputMixbins[1]);
			insert.Add(m_inputMixbins[4]);
			insert.Add(m_inputMixbins[5]);
			break;

		case 2:		// Front speakers
			insert.Add(m_inputMixbins[0]);
			insert.Add(m_inputMixbins[1]);
			break;
		case 3:		// Rear speakers
			insert.Add(m_inputMixbins[4]);
			insert.Add(m_inputMixbins[5]);
			break;
		case 4:		// Front Left
		case 5:		// Front Right
		case 6:		// Center
		case 7:		// LFE
		case 8:		// Back Left
		case 9:		// Back Right
			insert.Add(m_inputMixbins[index-4]);
			break;
		case 10:	// All Xtalk
			for(i=6; i<=9; ++i)
				insert.Add(m_inputMixbins[i]);
			break;
		case 11:	// XTalk Front Left
		case 12:	// XTalk Front Right
		case 13:	// XTalk Back Left
		case 14:	// XTalk Back Right
			insert.Add(m_inputMixbins[index-5]);
			break;
		case 15:	// I3DL2
			insert.Add(m_inputMixbins[10]);
			break;
		default:	// FX Sends
			insert.Add(m_inputMixbins[index-5]);
			break;
		}
	}

	// Output mixbin?
	else if(index & OUTPUT_MIXBIN_MASK)
	{
		index ^= OUTPUT_MIXBIN_MASK;

		switch(index)
		{
		case 0:		// All speakers
			for(i=0; i<=5; ++i)
				insert.Add(m_outputMixbins[i]);
			break;
		case 1:		// Front & Rear speakers
			insert.Add(m_outputMixbins[0]);
			insert.Add(m_outputMixbins[1]);
			insert.Add(m_outputMixbins[4]);
			insert.Add(m_outputMixbins[5]);
			break;
		case 2:		// Front speakers
			insert.Add(m_outputMixbins[0]);
			insert.Add(m_outputMixbins[1]);
			break;
		case 3:		// Rear speakers
			insert.Add(m_outputMixbins[4]);
			insert.Add(m_outputMixbins[5]);
			break;
		case 4:		// Front Left
		case 5:		// Front Right
		case 6:		// Center
		case 7:		// LFE
		case 8:		// Back Left
		case 9:		// Back Right
			insert.Add(m_outputMixbins[index-4]);
			break;
		case 10:		// All Xtalk
			for(i=6; i<=9; ++i)
				insert.Add(m_outputMixbins[i]);
			break;
		case 11:	// XTalk Front Left
		case 12:	// XTalk Front Right
		case 13:	// XTalk Back Left
		case 14:	// XTalk Back Right
			insert.Add(m_outputMixbins[index-5]);
			break;
		case 15:	// I3DL2
			insert.Add(m_outputMixbins[10]);
			break;
		default:	// FX Sends
			insert.Add(m_outputMixbins[index-5]);
			break;
		}
	}

	// Or just an effect?
	else
	{
		if((index > 0) && (index <= m_effects.GetSize()))
			insert.Add(m_effects[index-1]);
	}

	// Any modules to insert?
	if(insert.GetSize() == 0)
		return;

	// First module to add
	pModule = new CModule(*insert[0]);

	// Upper left will be based on the first module
	CalcUpperLeft(pModule);
	upperLeft = m_upperLeft;

	// Set it's position and state
	pModule->Move(m_upperLeft);
	state = IsPlacementValid(pModule) ? state : CModule::INVALID;

	// Put the module in the move list
	m_moveModules.Add(pModule);

	// Now add all the speaker mixbins
	for(i=1; i<insert.GetSize(); ++i)
	{
		// Adjust the position
		upperLeft.y += pModule->GetRect().Height() + dy;

		// First module will be front left speaker
		pModule = new CModule(*insert[i]);

		// Set it's position and state
		pModule->Move(upperLeft);
		state = IsPlacementValid(pModule) ? state : CModule::INVALID;

		// Put the module in the move list
		m_moveModules.Add(pModule);
	}

	// Set the state of all modules
	for(i=0; i<m_moveModules.GetSize(); ++i)
		m_moveModules[i]->SetState(state);

	m_bChangesMade = TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::DeleteModule
//------------------------------------------------------------------------------
void 
CGraph::DeleteModule(
					 IN CModule* pModule
					 )
/*++

Routine Description:

	Deletes a module from the modules list and redraws the exposed region

Arguments:

	IN pModule -	Module to delete

ReturnValue:

	None

--*/
{
	int		i;
	
	// Remove the module from the list
	for(i=0; i<m_modules.GetSize(); ++i)
	{
		if(m_modules[i] == pModule)
		{
			m_modules.RemoveAt(i);
			Draw(pModule->GetRect());
			break;
		}
	}

	// Is this an input mixbin?
	if(pModule->GetType() == CModule::INPUT_MIXBIN)
		m_inputMixbinInUse[pModule->GetMixbin()] = FALSE;

	// Or an output mixbin
	else if(pModule->GetType() == CModule::OUTPUT_MIXBIN)
		m_outputMixbinInUse[pModule->GetMixbin()] = FALSE;

	// This delete will force redraws of the removed patch cords
	delete pModule;

	m_bChangesMade = TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::SetMouse
//------------------------------------------------------------------------------
void 
CGraph::SetMouse(
				 IN const CPoint&	point
				 )
/*++

Routine Description:

	Sets a valid mouse location based on a scrolled display

Arguments:

	IN point -	Window coordinate

ReturnValue:

	None

--*/
{
	m_mouse = point + m_rect.TopLeft();
}

//------------------------------------------------------------------------------
//	CGraph::GetPatchCord
//------------------------------------------------------------------------------
CPatchCord* 
CGraph::GetPatchCord(
					 IN const CPoint&	point
					 ) const
/*++

Routine Description:

	Returns a patch cord at the given point if one exists

Arguments:

	IN point -	Window coordinate

ReturnValue:

	patch cord or NULL

--*/
{
	int	i;

	// First check for an exact hit
	for(i=0; i<m_patchCords.GetSize(); ++i)
	{
		if(m_patchCords[i]->IsPointOnLine(point, 0))
			return m_patchCords[i];
	}

	// Then check for a close hit (within TOLERANCE pixels)
	for(i=0; i<m_patchCords.GetSize(); ++i)
	{
		if(m_patchCords[i]->IsPointOnLine(point, TOLERANCE))
			return m_patchCords[i];
	}

	return NULL;
}

//------------------------------------------------------------------------------
//	CGraph::DrawPatchLine
//------------------------------------------------------------------------------
void
CGraph::DrawPatchLine(void)
/*++

Routine Description:

	Draws a rubberband line representing a new patch cord

Arguments:

	None

Return Value:

	None

--*/
{
	CClientDC	dc(m_pParent);
	CPen*		oldPen;
	int			oldROP;

	// Set the window origin
	dc.SetWindowOrg(m_rect.left, m_rect.top);	

	// Setup
	oldPen	= (CPen*)dc.SelectStockObject(WHITE_PEN);
	oldROP	= dc.SetROP2(R2_XORPEN);

	// Draw
	dc.Polyline(m_patchLine.points.GetData(), m_patchLine.points.GetSize());

	// Restore
	dc.SetROP2(oldROP);
	dc.SelectObject(oldPen);
}


//------------------------------------------------------------------------------
//	CGraph::IsPlacementValid
//------------------------------------------------------------------------------
BOOL 
CGraph::IsPlacementValid(
						 IN const CModule*	pModule,
						 IN const CModule*	pOriginalModule
						 ) const
/*++

Routine Description:

	Determines if the placement of a module is valid. A module that
	will be moved cannot be placed within 3 pixels of any other modules or
	patch cords.

Arguments:

	IN pModule -		Module to test
	IN pOriginalModule -	Original module

ReturnValue:

	TRUE if the module can be placed at the given location, FALSE otherwise

--*/
{
	int		i;
	CRect	rect = pModule->GetRect();
	
	// Generate a bounding rect 3 pixels bigger than its current rect
	rect.InflateRect(3, 3, 3, 3);

	// Is the module within 3 pixels of another module that is not being moved
	for(i=0; i<m_modules.GetSize(); ++i)
	{
		if((m_modules[i]->GetState() != CModule::MOVING) && 
		   IntersectRects(rect, m_modules[i]->GetRect()))
			return FALSE;
	}

	// Is it within 3 pixels of any patch cords that is not connected to it
	for(i=0; i<m_patchCords.GetSize(); ++i)
	{
		// This test is not a bounding box check
		if(m_patchCords[i]->IntersectRect(rect) && 
		   ((pOriginalModule == NULL) || (!m_patchCords[i]->IsPatchedToModule(pOriginalModule))))
		{
			return FALSE;
		}
	}

	// Will the module fit within graph?
	if((pModule->GetRect().left < 0) || (pModule->GetRect().top < 0) ||
	   (pModule->GetRect().right > MAX_SIZE) || 
	   (pModule->GetRect().bottom > MAX_SIZE))
	{
	   return FALSE;
	}

	// Otherwise it's good
	return TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::IntersectRects
//------------------------------------------------------------------------------
BOOL 
CGraph::IntersectRects(
					   IN const CRect&	rect0,
					   IN const CRect&	rect1
					   ) const
/*++

Routine Description:

	Returns TRUE if the two rectangles intersect

Arguments:

	IN rect0 -	Rect0
	IN rect1 -	Rect1

ReturnValue:

	TRUE if the rectangle intersect, FALSE otherwise

--*/
{
	CRect	rect;

	return rect.IntersectRect(rect0, rect1);
}

//------------------------------------------------------------------------------
//	CGraph::CalcUpperLeft
//------------------------------------------------------------------------------
void
CGraph::CalcUpperLeft(
					  IN const CModule*	pModule
					  )
/*++

Routine Description:

	Given a module and this method calculates the upper left corner of the 
	module using the current mouse position as the center of the module

Arguments:

	IN pModule -	Module

ReturnValue:

	None

--*/
{
	if(m_bUseOffset)
		m_upperLeft = m_mouse - m_offset;
	else
	{
		m_upperLeft.x	= m_mouse.x - (pModule->GetRect().Width() / 2),
		m_upperLeft.y	= m_mouse.y - (pModule->GetRect().Height() / 2);
	}

	if(m_bSnapToGrid)
	{
		m_upperLeft.x = m_upperLeft.x / m_snapSize * m_snapSize;
		m_upperLeft.y = m_upperLeft.y / m_snapSize * m_snapSize;
	}
}

//------------------------------------------------------------------------------
//	CGraph::GetModule
//------------------------------------------------------------------------------
CModule*
CGraph::GetModule(
				  IN const CPoint&	point
				  ) const
/*++

Routine Description:

	Returns the module under the point.

Arguments:

	IN point -	Point

ReturnValue:

	NULL if no module is under the point, otherwise the module

--*/
{
	int		i;
	
	for(i=0; i<m_modules.GetSize(); ++i)
	{
		if(IsPointInRect(point.x, point.y, m_modules[i]->GetRect()))
			return m_modules[i];
	}
	return NULL;
}

//------------------------------------------------------------------------------
//	CGraph::NewPatchCord
//------------------------------------------------------------------------------
CPatchCord*
CGraph::NewPatchCord(void)
/*++

Routine Description:

	Creates a new patch cord from the existing patch line

Arguments:

	None

ReturnValue:

	The new patch cord

--*/
{
	CPatchCord*	pPatchCord;
	CPatchCord*	pSplitPatchCord	= NULL;
	CRect		rect;

	// If a cord is dangling, abort
	if((m_patchLine.firstPatchEnd == CGraph::PATCHEND_NONE) ||
	   (m_patchLine.lastPatchEnd == CGraph::PATCHEND_NONE))
	{
	   return NULL;
	}

	// If the patch cord will be patched at both ends to the same jack, abort
	else if(m_patchLine.pFirstJack && 
		    (m_patchLine.pFirstJack == m_patchLine.pLastJack))
	{
		return NULL;
	}

	// If either end of the patch line is being connected to a jack that
	// is already patched, just abort
	else if(((m_patchLine.pFirstJack) && (m_patchLine.pFirstJack->IsPatched())) ||
			((m_patchLine.pLastJack) && (m_patchLine.pLastJack->IsPatched())))
	{
		AfxMessageBox(_T("You cannot connect a patch cord to a jack that has already been patched. "
					     "Try connecting to a patch cord instead."),
					  MB_OK | MB_ICONWARNING | MB_APPLMODAL);
		return NULL;
	}

	// If this is a direct connect between two mixbins, abort
	else if((m_patchLine.pFirstJack && m_patchLine.pLastJack) &&
		    (((m_patchLine.pFirstJack->GetModule()->GetType() == CModule::INPUT_MIXBIN) ||
			  (m_patchLine.pFirstJack->GetModule()->GetType() == CModule::OUTPUT_MIXBIN)) &&
			 ((m_patchLine.pLastJack->GetModule()->GetType() == CModule::INPUT_MIXBIN) ||
			  (m_patchLine.pLastJack->GetModule()->GetType() == CModule::OUTPUT_MIXBIN))))
	{
		AfxMessageBox(_T("You cannot connect two mixbins."),
					  MB_OK | MB_ICONWARNING | MB_APPLMODAL);
		return NULL;
	}

	// If the patch cord will be patched at both ends to the same patch cord, abort
	else if(m_patchLine.pFirstPatchCord && 
		    (m_patchLine.pFirstPatchCord == m_patchLine.pLastPatchCord))
	{
		AfxMessageBox(_T("You cannot connect both ends of the patch cord to the same patch cord."),
					  MB_OK | MB_ICONWARNING | MB_APPLMODAL);
		return NULL;
	}

	// If the patch cord is connected to the input and output of the same module, abort
	else if((m_patchLine.pFirstJack && m_patchLine.pLastJack) &&
			(m_patchLine.pFirstJack->GetModule() == m_patchLine.pLastJack->GetModule()) &&
			(m_patchLine.pFirstJack->GetInput() != m_patchLine.pLastJack->GetInput()))
	{
		AfxMessageBox(_T("You cannot connect output of an effect to its own input."),
					  MB_OK | MB_ICONWARNING | MB_APPLMODAL);
		return NULL;
	}

	// Create the new patch cord
	pPatchCord = new CPatchCord;
	pPatchCord->SetPoints(m_patchLine.points);
	rect = pPatchCord->GetRect();

	// Connect the first end
	switch(m_patchLine.firstPatchEnd)
	{
	case CGraph::PATCHEND_INPUT:
	case CGraph::PATCHEND_OUTPUT:
		m_patchLine.pFirstJack->Connect(pPatchCord, FALSE);
		break;

	case CGraph::PATCHEND_HORIZONTAL:
	case CGraph::PATCHEND_VERTICAL:
	case CGraph::PATCHEND_POINT:
		rect.UnionRect(rect, m_patchLine.pFirstPatchCord->GetRect());
		pSplitPatchCord = m_patchLine.pFirstPatchCord->Connect(pPatchCord, m_patchLine.points[0], FALSE);
		if(pSplitPatchCord)
			m_patchCords.Add(pSplitPatchCord);
		break;
	default:
		break;
	}
	
	// Connect the last end
	switch(m_patchLine.lastPatchEnd)
	{
	case CGraph::PATCHEND_INPUT:
	case CGraph::PATCHEND_OUTPUT:
		m_patchLine.pLastJack->Connect(pPatchCord, TRUE);
		break;

	case CGraph::PATCHEND_HORIZONTAL:
	case CGraph::PATCHEND_VERTICAL:
	case CGraph::PATCHEND_POINT:
		rect.UnionRect(rect, m_patchLine.pLastPatchCord->GetRect());
		pSplitPatchCord = m_patchLine.pLastPatchCord->Connect(pPatchCord, m_patchLine.points[m_patchLine.points.GetSize()-1], TRUE);
		if(pSplitPatchCord)
			m_patchCords.Add(pSplitPatchCord);
		break;
	default:
		break;
	}

	// Add the patch cord to the list
	m_patchCords.Add(pPatchCord);
	m_bChangesMade = TRUE;

	// Enable overwrite?
//	pPatchCord->SetOverwriteEnable();

	// Validate the new layout
	ValidateGraph();

	// Redraw
	Draw(rect);

	return pPatchCord;
}

//------------------------------------------------------------------------------
//	CGraph::UpdatePatchLine
//------------------------------------------------------------------------------
void
CGraph::UpdatePatchLine(void)
/*++

Routine Description:

	Updates the points on the patch line based on the drawline state

Arguments:

	None

ReturnValue:

	None

--*/
{
	BOOL	bVertical;
	int		last = m_patchLine.points.GetSize() - 1;
	
	// Last point will almost always be where the mouse is
	switch(m_patchLine.lastPatchEnd)
	{
	case CGraph::PATCHEND_INPUT:
		m_patchLine.points[last].x = m_patchLine.pLastJack->GetModule()->GetRect().left;
		m_patchLine.points[last].y = m_patchLine.pLastJack->GetY();
		break;
	case CGraph::PATCHEND_OUTPUT:
		m_patchLine.points[last].x = m_patchLine.pLastJack->GetModule()->GetRect().right - 1;
		m_patchLine.points[last].y = m_patchLine.pLastJack->GetY();
		break;
	case CGraph::PATCHEND_HORIZONTAL:
		if(m_patchLine.points[last].x == m_patchLine.points[last-1].x)
		{
			m_patchLine.points[last].x = m_mouse.x;
			m_patchLine.points[last].y = m_patchLine.pLastPatchCord->GetLastPoint().y;
		}
		else
			m_patchLine.points[last] = m_mouse;
		break;
	case CGraph::PATCHEND_VERTICAL:
		if(m_patchLine.points[last].y == m_patchLine.points[last-1].y)
		{
			m_patchLine.points[last].x = m_patchLine.pLastPatchCord->GetLastPoint().x;
			m_patchLine.points[last].y = m_mouse.y;
		}
		else
			m_patchLine.points[last] = m_mouse;
		break;
	case CGraph::PATCHEND_POINT:
		m_patchLine.points[last] = m_patchLine.pLastPatchCord->GetLastPoint();
		break;
	default:
		m_patchLine.points[last] = m_mouse;
	}

	// Complex line?
	if(m_bDrawComplexLine)
	{
		// Are there more than 3 points?
		if(m_patchLine.points.GetSize() > 3)
		{
			if(m_patchLine.points[last-2].x == m_patchLine.points[last-3].x)
				bVertical = TRUE;
			else
				bVertical = FALSE;
		}

		// Just use the first point info
		else
		{
			switch(m_patchLine.firstPatchEnd)
			{
				case CGraph::PATCHEND_VERTICAL:
				case CGraph::PATCHEND_INPUT:
				case CGraph::PATCHEND_OUTPUT:
					bVertical = TRUE;
					break;
				default: // Graph::HORIZONTAL:
					bVertical = FALSE;
					break;
			}
		}

		if(bVertical)
		{
			m_patchLine.points[last-1].x	= m_patchLine.points[last].x;
			m_patchLine.points[last-1].y	= m_patchLine.points[last-2].y;
		}
		else
		{
			m_patchLine.points[last-1].x	= m_patchLine.points[last-2].x;
			m_patchLine.points[last-1].y	= m_patchLine.points[last].y;
		}
	}

	// Simple line
	else
	{
		// Make sure there are 4 points in the line
		if(m_patchLine.points.GetSize() == 3)
			m_patchLine.points.Add(m_patchLine.points[2]);
		
		// Set the other 2 points based on the state of the line
		switch(m_patchLine.firstPatchEnd)
		{
		case CGraph::PATCHEND_HORIZONTAL:
		case CGraph::PATCHEND_POINT:
			switch(m_patchLine.lastPatchEnd)
			{
				case CGraph::PATCHEND_NONE:
				case CGraph::PATCHEND_VERTICAL:
				case CGraph::PATCHEND_INPUT:
				case CGraph::PATCHEND_OUTPUT:
					m_patchLine.points[1]	= m_patchLine.points[0];
					m_patchLine.points[2].x	= m_patchLine.points[0].x;
					m_patchLine.points[2].y	= m_patchLine.points[3].y;
					break;
				case CGraph::PATCHEND_HORIZONTAL:
				case CGraph::PATCHEND_POINT:
					m_patchLine.points[1].x	= m_patchLine.points[0].x;
					m_patchLine.points[1].y	= MEDIAN(m_patchLine.points[0].y, m_patchLine.points[3].y);
					m_patchLine.points[2].x	= m_patchLine.points[3].x;
					m_patchLine.points[2].y	= m_patchLine.points[1].y;
					break;
				default:
					break;
			}
			break;
		case CGraph::PATCHEND_VERTICAL:
		case CGraph::PATCHEND_INPUT:
		case CGraph::PATCHEND_OUTPUT:
			switch(m_patchLine.lastPatchEnd)
			{
				case CGraph::PATCHEND_NONE:
				case CGraph::PATCHEND_VERTICAL:
				case CGraph::PATCHEND_INPUT:
				case CGraph::PATCHEND_OUTPUT:
					m_patchLine.points[1].x	= MEDIAN(m_patchLine.points[0].x, m_patchLine.points[3].x);
					m_patchLine.points[1].y	= m_patchLine.points[0].y;
					m_patchLine.points[2].x	= m_patchLine.points[1].x;
					m_patchLine.points[2].y	= m_patchLine.points[3].y;
					break;
				case CGraph::PATCHEND_HORIZONTAL:
				case CGraph::PATCHEND_POINT:
					m_patchLine.points[1]	= m_patchLine.points[0];
					m_patchLine.points[2].y	= m_patchLine.points[0].y;
					m_patchLine.points[2].x	= m_patchLine.points[3].x;
					break;
				default: break;
			}
			break;
		default:
			break;
		}
	}
}

//------------------------------------------------------------------------------
//	CGraph::ValidateGraph
//------------------------------------------------------------------------------
void
CGraph::ValidateGraph(void)
/*++

Routine Description:

	Examines the content of the graph to determine if the layout is valid.
	Any patch cords that are in question, will be marked invalid.

Arguments:

	None

Return Value:

	None

--*/
{
	int						m;
	int						i;
	int						c;
	CArray<CJack*, CJack*>	list;
	CJack*					pJack;
	CModule*				pModule;

	if(m_bDead)
		return;

	// First mark all patch cords as valid
	for(i=0; i<m_patchCords.GetSize(); ++i)
		m_patchCords[i]->SetInvalid(FALSE);

	// Check every module
	for(m=0; m<m_modules.GetSize(); ++m)
	{
		pModule = m_modules[m];

		// Check all inputs on the module
		for(i=0; i<pModule->GetInputs().GetSize(); ++i)
		{
			pJack = pModule->GetInputs()[i];

			// Get all the connections
			pJack->GetConnections(list);

			// Loop through all the connections
			for(c=0; c<list.GetSize(); ++c)
			{
				// A mixbin cannot be connected to another mixbin
				// An effect input cannot be connected to own output (yet)
				// An effect input cannot be connected to an output mixbin
				if(((pModule->GetType() == CModule::OUTPUT_MIXBIN) &&
					((list[c]->GetModule()->GetType() == CModule::INPUT_MIXBIN) ||
					 (list[c]->GetModule()->GetType() == CModule::OUTPUT_MIXBIN))) ||
				   ((pModule->GetType() == CModule::EFFECT) &&
					(((list[c]->GetModule() == pModule) && !list[c]->GetInput()) ||
					 (list[c]->GetModule()->GetType() == CModule::OUTPUT_MIXBIN))))
				{
					pJack->Invalidate();
					break;
				}
			}
		}

		// Check all outputs on the module
		for(i=0; i<pModule->GetOutputs().GetSize(); ++i)
		{
			pJack = pModule->GetOutputs()[i];

			// Get all the connections
			pJack->GetConnections(list);

			// Loop through all the connections
			for(c=0; c<list.GetSize(); ++c)
			{
				// A mixbin cannot be connected to another mixbin
				// An effect output cannot be connected to own input (yet)
				// An effect output cannot be connected to a input mixbin
				if(((pModule->GetType() == CModule::INPUT_MIXBIN) &&
					((list[c]->GetModule()->GetType() == CModule::INPUT_MIXBIN) ||
					 (list[c]->GetModule()->GetType() == CModule::OUTPUT_MIXBIN))) ||
				   ((pModule->GetType() == CModule::EFFECT) &&
					(((list[c]->GetModule() == pModule) && list[c]->GetInput()) ||
					 (list[c]->GetModule()->GetType() == CModule::INPUT_MIXBIN))))
				{
					pJack->Invalidate();
					break;
				}
			}
		}
	}

	// For now, just redraw everything
	Draw(m_rect);
}

//------------------------------------------------------------------------------
//	CGraph::SetTitle
//------------------------------------------------------------------------------
void
CGraph::SetTitle(void)
{
	CString	title;
	int		index;
	CString	buildOptions;

	buildOptions.Format(_T("  [Reverb: %s, LFE: %s, XTalk: %s]"),
						m_i3dl2Config.GetLength() ? m_i3dl2Config : _T("<None>"), 
						m_bPatchXtalkToLFE ? _T("Yes") : _T("No"),
						m_bIncludeXtalk ? _T("Yes") : _T("No"));

	if(m_filename.IsEmpty())
		title = _T("dspbuilder");
	else
	{
		title.Format(_T("dspbuilder: %s"), m_filename);
		index = title.ReverseFind('.');
		if(index != -1)
			title.Delete(index, title.GetLength()-index);
	}
	title += buildOptions;
	m_pParent->GetParent()->SetWindowText((LPCTSTR)title);
}

//------------------------------------------------------------------------------
//	CGraph::EnableMixbinsOnMen
//------------------------------------------------------------------------------
void
CGraph::EnableMixbinsOnMenu(void)
/*++

Routine Description:

	Enables and disables individual items on the mixbins menu

Arguments:

	None

Return Value:

	None

--*/
{
	int		i;
	int		m;
	UINT	mask	= INPUT_MIXBIN_MASK;
	BOOL*	mixbin	= m_inputMixbinInUse.GetData();

	// First enable them all
	for(i=0; i<35; ++i)
	{
		m_rootMenu.EnableMenuItem(i | INPUT_MIXBIN_MASK, MF_ENABLED);
		m_rootMenu.EnableMenuItem(i | OUTPUT_MIXBIN_MASK, MF_ENABLED);
	}

	for(i=0; i<2; ++i)
	{
		if(mixbin[0])	// Front Left
		{
			m_rootMenu.EnableMenuItem(0 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(1 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(2 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(4 | mask, MF_GRAYED);
		}
		if(mixbin[1])	// Front Right
		{
			m_rootMenu.EnableMenuItem(0 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(1 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(2 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(5 | mask, MF_GRAYED);
		}
		if(mixbin[2])	// Center
		{
			m_rootMenu.EnableMenuItem(0 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(6 | mask, MF_GRAYED);
		}
		if(mixbin[3])	// LFE
		{
			m_rootMenu.EnableMenuItem(0 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(7 | mask, MF_GRAYED);
		}
		if(mixbin[4])	// Back Left
		{
			m_rootMenu.EnableMenuItem(0 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(1 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(3 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(8 | mask, MF_GRAYED);
		}
		if(mixbin[5])	// Back Right
		{
			m_rootMenu.EnableMenuItem(0 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(1 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(3 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(9 | mask, MF_GRAYED);
		}
		if(mixbin[6])	// XTalk Front Left
		{
			m_rootMenu.EnableMenuItem(10 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(11 | mask, MF_GRAYED);
		}
		if(mixbin[7])	// XTalk Front Right
		{
			m_rootMenu.EnableMenuItem(10 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(12 | mask, MF_GRAYED);
		}
		if(mixbin[8])	// XTalk Back Left
		{
			m_rootMenu.EnableMenuItem(10 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(13 | mask, MF_GRAYED);
		}
		if(mixbin[9])	// XTalk Back Right
		{
			m_rootMenu.EnableMenuItem(10 | mask, MF_GRAYED);
			m_rootMenu.EnableMenuItem(14 | mask, MF_GRAYED);
		}
		if(mixbin[10])	// I3DL2
			m_rootMenu.EnableMenuItem(15 | mask, MF_GRAYED);
		
		for(m=11; m<31; ++m)
		{
			if(mixbin[m])
				m_rootMenu.EnableMenuItem(((unsigned)m+5) | mask, MF_GRAYED);
		}

		// Next pass is output
		mixbin	= m_outputMixbinInUse.GetData();
		mask	= OUTPUT_MIXBIN_MASK;
	}
}

//------------------------------------------------------------------------------
//	CGraph::ReadVersion1
//------------------------------------------------------------------------------
BOOL
CGraph::ReadVersion1(
					 IN OUT CFile&	file
					 )
/*++

Routine Description:

	Reads and converts a version 1 file to a version 2 file

Arguments:

	IN file -	File to read

Return Value:

	None

--*/
{
	CString					filename;
	BYTE					b;
	WORD					word;
	int						width;
	int						height;
	BYTE*					pByte;
	int						numCells;
	int						numEffects;
	CString					string;
	CString					effectName;
	CString					config;
	char*					buffer;
	int						left;
	int						top;
	int						numInputs;
	int						numOutputs;
	int						i;
	CModule*				pModule;
	int						e;
	int						ii;
	BOOL					missingEffects	= FALSE;
	BOOL					missingConfigs	= FALSE;
	int						x;
	int						y;
	CPatchCord*				pPatchCord;
	CArray<CPoint, CPoint&>	points;

	filename = file.GetFilePath();

	// Read the width
	if(file.Read(&word, sizeof(word)) != sizeof(word))
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}
	width = word;

	// Read the height
	if(file.Read(&word, sizeof(word)) != sizeof(word))
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}
	height		= word;
	numCells	= width * height;

	// Allocate the cells
	if(numCells <= 0)
	{
		string.Format(_T("Invalid width / height: %s"), (LPCTSTR)filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		return FALSE;
	}
	
	pByte = new BYTE [(unsigned)numCells];

	// Read the cells
	if(file.Read(pByte, (UINT)numCells) != (UINT)numCells)
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		delete [] pByte;
		return FALSE;
	}

	// Process the Cells and generate the patch cords
	m_gridWidth		= width;
	m_gridHeight	= height;
	m_pGrid			= new CGraph::Grid [(unsigned)(m_gridWidth * m_gridHeight)];

	// Fill the grid
	for(i=0, x=0; x<m_gridWidth; ++x)
	{
		for(y=0; y<m_gridHeight; ++y, ++i)
		{
			ii = 0;
			if(pByte[i] & (1 << 3))
			{
				m_pGrid[i].bLeft = TRUE;
				++ii;
			}
			if(pByte[i] & (1 << 2))
			{
				m_pGrid[i].bRight = TRUE;
				++ii;
			}
			if(pByte[i] & (1 << 1))
			{
				m_pGrid[i].bTop = TRUE;
				++ii;
			}
			if(pByte[i] & 1)
			{
				m_pGrid[i].bBottom = TRUE;
				++ii;
			}
			if((pByte[i] & (1 << 4)) && (ii >= 3))
				m_pGrid[i].bJoined = TRUE;
		}
	}
	delete [] pByte;

	// Read all the effects
	if(file.Read(&word, sizeof(word)) != sizeof(word))
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
		delete [] m_pGrid;
		m_pGrid = NULL;
		return FALSE;
	}
	numEffects = word;

	for(i=0; i<numEffects; ++i)
	{
		// Effect Name (word, string)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		buffer = new char [word+1];
		if(file.Read(buffer, word + 1) != (UINT)(word+1))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			delete [] buffer;
			return FALSE;
		}
		effectName = buffer;
		delete [] buffer;

		// Configuration
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		if (word != 0)
		{
			buffer = new char [word+1];
			if(file.Read(buffer, word + 1) != (UINT)(word+1))
			{
				string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
				AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
				delete [] m_pGrid;
				m_pGrid = NULL;
				delete [] buffer;
				return FALSE;
			}
			config = buffer;
			delete [] buffer;
		}
		else
			config = _T("");

		// Grid Location (word, word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		left = word;
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		top = word;

		// Num Inputs (word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		numInputs = word;

		// Num Outputs (word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		numOutputs = word;

		// Width (word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		width = word;

		// Height (word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		height = word;

		// Try to find the effect in the effects list
		for(e=0; e<m_effects.GetSize(); ++e)
		{
			string = m_effects[e]->GetEffectName();

			// The following need to match: name, numInputs & numOutputs
			if((effectName == m_effects[e]->GetEffectName()) &&
			   (m_effects[e]->GetInputs().GetSize() == numInputs) &&
			   (m_effects[e]->GetOutputs().GetSize() == numOutputs))
			   break;
		}

		// If the effect was found add it
		if(e != m_effects.GetSize())
		{
			pModule = new CModule(*m_effects[e]);
			m_modules.Add(pModule);
			pModule->Move(CPoint((left+1)*SCALE, (top+1)*SCALE));

			// Look up the configuration
			if(config != _T(""))
			{
				for(ii=0; ii<pModule->GetConfigs().GetSize(); ++ii)
				{
					if(pModule->GetConfigs()[ii].name == config)
					{
						pModule->SetActiveConfig(ii);
						break;
					}
				}
				if(ii == pModule->GetConfigs().GetSize())
					missingConfigs = TRUE;
			}

			// Add the effect to the grid
			for(x=left; x<left+width; ++x)
			{
				for(y=top; y<top+height; ++y)
					m_pGrid[CI(x,y)].pModule = pModule;
			}

			// Setup the inputs/outputs
			for(y=0; y<pModule->GetInputs().GetSize(); ++y)
			{
				m_pGrid[CI(left, top+y+1)].bLeft	= TRUE;
				m_pGrid[CI(left, top+y+1)].index	= y;
			}
			for(y=0; y<pModule->GetOutputs().GetSize(); ++y)
			{
				m_pGrid[CI(left+width-1, top+y+1)].bRight	= TRUE;
				m_pGrid[CI(left+width-1, top+y+1)].index	= y;
			}
		}

		// Otherwise drop it and set a flag to display an error
		else
			missingEffects = TRUE;
	}

	// Read the Input Mixbin locations
	for(i=0; i<26; ++i)
	{
		if(file.Read(&b, sizeof(b)) != sizeof(b))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		if(b >= 26)
		{
			string.Format(_T("Invalid input mixbin index: %s"), (LPCTSTR)filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}

		// Speakers
		if(b <= 5)	
			ii = b;

		// FX Sends
		else	
			ii = (int)b + 5;
		
		pModule = new CModule(*m_inputMixbins[ii]);
		m_modules.Add(pModule);
		pModule->Move(CPoint(SCALE, (i*4+1)*SCALE));

		for(x=0; x<9; ++x)
		{
			for(y=0; y<3; ++y)
				m_pGrid[CI(x, i*4+y)].pModule = pModule;
		}

		// Setup the output
		m_pGrid[CI(8, i*4+1)].bRight	= TRUE;
		m_pGrid[CI(8, i*4+1)].index		= 0;
	}
	
	// Read the Output Mixbin locations
	for(i=0; i<26; ++i)
	{
		if(file.Read(&b, sizeof(b)) != sizeof(b))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}
		if(b >= 26)
		{
			string.Format(_T("Invalid output mixbin index: %s"), (LPCTSTR)filename);
			AfxMessageBox(string, MB_OK | MB_ICONERROR | MB_APPLMODAL);
			delete [] m_pGrid;
			m_pGrid = NULL;
			return FALSE;
		}

		// Speakers
		if(b <= 5)	
			ii = b;

		// FX Sends
		else	
			ii = (int)b + 5;
		
		pModule = new CModule(*m_outputMixbins[ii]);
		m_modules.Add(pModule);
		pModule->Move(CPoint((m_gridWidth-8)*SCALE, (i*4+1)*SCALE));

		for(x=m_gridWidth-9; x<m_gridWidth; ++x)
		{
			for(y=0; y<3; ++y)
				m_pGrid[CI(x, i*4+y)].pModule = pModule;
		}

		// Setup the inputs
		m_pGrid[CI(m_gridWidth-9, i*4+1)].bLeft	= TRUE;
		m_pGrid[CI(m_gridWidth-9, i*4+1)].index	= 0;
	}

	// Generate the patch cords from the grid
	for(y=0; y<m_gridHeight; ++y)
	{
		for(x=0; x<m_gridWidth; ++x)
		{
			ii = CI(x, y);
			if((m_pGrid[ii].pModule || m_pGrid[ii].bJoined) && 
			   (m_pGrid[ii].bLeft || m_pGrid[ii].bRight || m_pGrid[ii].bTop || 
			    m_pGrid[ii].bBottom))
			{
				TraceAndBuildPatchCord(x, y);
			}
		}
	}

	// Connect joined cell patch cords
	for(i=m_gridWidth*m_gridHeight-1; i>=0; --i)
	{
		if(m_pGrid[i].bJoined)
		{
			if(m_pGrid[i].pPatchCordLeft)
			{
				if(m_pGrid[i].pPatchCordTop)
					m_pGrid[i].pPatchCordLeft->AddPatchCord(m_pGrid[i].pPatchCordTop, m_pGrid[i].bEndLeft);
				if(m_pGrid[i].pPatchCordRight)
					m_pGrid[i].pPatchCordLeft->AddPatchCord(m_pGrid[i].pPatchCordRight, m_pGrid[i].bEndLeft);
				if(m_pGrid[i].pPatchCordBottom)
					m_pGrid[i].pPatchCordLeft->AddPatchCord(m_pGrid[i].pPatchCordBottom, m_pGrid[i].bEndLeft);
			}
			if(m_pGrid[i].pPatchCordRight)
			{
				if(m_pGrid[i].pPatchCordTop)
					m_pGrid[i].pPatchCordRight->AddPatchCord(m_pGrid[i].pPatchCordTop, m_pGrid[i].bEndRight);
				if(m_pGrid[i].pPatchCordLeft)
					m_pGrid[i].pPatchCordRight->AddPatchCord(m_pGrid[i].pPatchCordLeft, m_pGrid[i].bEndRight);
				if(m_pGrid[i].pPatchCordBottom)
					m_pGrid[i].pPatchCordRight->AddPatchCord(m_pGrid[i].pPatchCordBottom, m_pGrid[i].bEndRight);
			}
			if(m_pGrid[i].pPatchCordTop)
			{
				if(m_pGrid[i].pPatchCordLeft)
					m_pGrid[i].pPatchCordTop->AddPatchCord(m_pGrid[i].pPatchCordLeft, m_pGrid[i].bEndTop);
				if(m_pGrid[i].pPatchCordRight)
					m_pGrid[i].pPatchCordTop->AddPatchCord(m_pGrid[i].pPatchCordRight, m_pGrid[i].bEndTop);
				if(m_pGrid[i].pPatchCordBottom)
					m_pGrid[i].pPatchCordTop->AddPatchCord(m_pGrid[i].pPatchCordBottom, m_pGrid[i].bEndTop);
			}
			if(m_pGrid[i].pPatchCordBottom)
			{
				if(m_pGrid[i].pPatchCordTop)
					m_pGrid[i].pPatchCordBottom->AddPatchCord(m_pGrid[i].pPatchCordTop, m_pGrid[i].bEndBottom);
				if(m_pGrid[i].pPatchCordRight)
					m_pGrid[i].pPatchCordBottom->AddPatchCord(m_pGrid[i].pPatchCordRight, m_pGrid[i].bEndBottom);
				if(m_pGrid[i].pPatchCordLeft)
					m_pGrid[i].pPatchCordBottom->AddPatchCord(m_pGrid[i].pPatchCordLeft, m_pGrid[i].bEndBottom);
			}
		}
	}

	// Add the patch cords to list
	for(i=m_gridWidth*m_gridHeight-1; i>=0; --i)
	{
		if(m_pGrid[i].pPatchCordLeft && (m_pGrid[i].pModule == NULL))
		{
			for(ii=0; ii<m_patchCords.GetSize(); ++ii)
			{
				if(m_patchCords[ii] == m_pGrid[i].pPatchCordLeft)
					break;
			}
			if(ii == m_patchCords.GetSize())
				m_patchCords.Add(m_pGrid[i].pPatchCordLeft);
		}
		if(m_pGrid[i].pPatchCordRight && (m_pGrid[i].pModule == NULL))
		{
			for(ii=0; ii<m_patchCords.GetSize(); ++ii)
			{
				if(m_patchCords[ii] == m_pGrid[i].pPatchCordRight)
					break;
			}
			if(ii == m_patchCords.GetSize())
				m_patchCords.Add(m_pGrid[i].pPatchCordRight);
		}
		if(m_pGrid[i].pPatchCordTop&& (m_pGrid[i].pModule == NULL))
		{
			for(ii=0; ii<m_patchCords.GetSize(); ++ii)
			{
				if(m_patchCords[ii] == m_pGrid[i].pPatchCordTop)
					break;
			}
			if(ii == m_patchCords.GetSize())
				m_patchCords.Add(m_pGrid[i].pPatchCordTop);
		}
		if(m_pGrid[i].pPatchCordBottom && (m_pGrid[i].pModule == NULL))
		{
			for(ii=0; ii<m_patchCords.GetSize(); ++ii)
			{
				if(m_patchCords[ii] == m_pGrid[i].pPatchCordBottom)
					break;
			}
			if(ii == m_patchCords.GetSize())
				m_patchCords.Add(m_pGrid[i].pPatchCordBottom);
		}
	}

	// Remove all the disconnected patch cords
	for(i=m_patchCords.GetSize()-1; i>=0; --i)
	{
		if(!m_patchCords[i]->IsPatchedAtEnds())
		{
			pPatchCord = m_patchCords[i];
			m_patchCords.RemoveAt(i);
			delete pPatchCord;
		}
	}

	// Free resources
	delete [] m_pGrid;
	m_pGrid = NULL;
	
	// Remove unpatched mixbins
	for(i=m_modules.GetSize()-1; i>=0; --i)
	{
		if(((m_modules[i]->GetType() == CModule::INPUT_MIXBIN) ||
			(m_modules[i]->GetType() == CModule::OUTPUT_MIXBIN)) &&
			!m_modules[i]->IsPatched())
		{
			pModule = m_modules[i];
			m_modules.RemoveAt(i);
			delete pModule;
		}
	}

	// Display errors
	if(missingEffects)
	{
		AfxMessageBox(_T("One or more of the effects in the file are "
						 "not currently available and will not be shown."),
					  MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}
	if(missingConfigs)
	{
		AfxMessageBox(_T("One or more of the effects in the file are missing "
						 "configurations and will be set to their default."),
					  MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
	}
	return TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::ReadVersion2or3or4
//------------------------------------------------------------------------------
BOOL
CGraph::ReadVersion2or3or4(
						   IN OUT CFile&	file,
						   IN BYTE			version
						   )
/*++

Routine Description:

	Reads a version 2 file

Arguments:

	IN file -		File to read
	IN version -	File version

Return Value:

	TRUE on success, FALSE on failure

--*/
{
	WORD		word;
	BYTE		byte;
	int			i;	
	CModule*	pModule;
	CPatchCord*	pPatchCord;
	CString		filePath;

	// Read the build options
	if(version >= 4)
	{
		if(!CGraph::ReadString(file, m_i3dl2Config))
			return FALSE;

		if(file.Read(&byte, sizeof(byte)) != sizeof(byte))
			return FALSE;
		m_bPatchXtalkToLFE = byte ? TRUE : FALSE;

		if(file.Read(&byte, sizeof(byte)) != sizeof(byte))
			return FALSE;
		m_bIncludeXtalk = byte ? TRUE : FALSE;
	}

	// Read the number of modules
	if(file.Read(&word, sizeof(word)) != sizeof(word))
		return FALSE;

	// Read all the modules
	for(i=0; i<word; ++i)
	{
		pModule = new CModule();
		if(!pModule->Read(file, version))
		{
			for(i=m_modules.GetSize()-1; i>=0; --i)
			{
				pModule = m_modules[i];
				m_modules.RemoveAt(i);
				delete pModule;
			}
			return FALSE;
		}
		m_modules.Add(pModule);
	}
	
	// Read the number of patch cords
	if(file.Read(&word, sizeof(word)) != sizeof(word))
		return FALSE;

	// Write the patch cords
	for(i=0; i<word; ++i)
	{
		pPatchCord = new CPatchCord();
		if(!pPatchCord->Read(file, version))
		{
			for(i=m_modules.GetSize()-1; i>=0; --i)
			{
				pModule = m_modules[i];
				m_modules.RemoveAt(i);
				delete pModule;
			}
			for(i=m_patchCords.GetSize()-1; i>=0; --i)
			{
				pPatchCord = m_patchCords[i];
				m_patchCords.RemoveAt(i);
				delete pPatchCord;
			}
			return FALSE;
		}
		m_patchCords.Add(pPatchCord);
	}

	// Adjust all the pointers
	for(i=0; i<m_patchCords.GetSize(); ++i)
		m_patchCords[i]->FixPointers(this);
	for(i=0; i<m_modules.GetSize(); ++i)
		m_modules[i]->FixPointers(this);
	
	// Read module parameters
	for(i=0; i<m_modules.GetSize(); ++i)
	{
		filePath.Format(_T("%s\\%s"), m_iniPath, m_modules[i]->GetStateFilename());
		m_modules[i]->SetParameters(m_imageBuilder, filePath);
	}

	// Older versions need the overwrite enable flag generated
//	if(version < 4)
//	{
//		for(i=0; i<m_patchCords.GetSize(); ++i)
//			m_patchCords[i]->SetOverwriteEnable();
//	}

	return TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::BuildEffectChains
//------------------------------------------------------------------------------
BOOL
CGraph::BuildEffectChains(
						  LPCTSTR	pFilename
						  )
/*++

Routine Description:

	Builds a list of effect chains by examining the grid.
	This method assumes there are no invalid cells in the grid.

Arguments:

	IN pFilename -	File to save to

Return Value:

	TRUE on success, FALSE on failure

--*/
{
	int						e;
	int						i;
	int						o;
	int						m;
	int						b;
	int						ii;
	int						oo;
	int						bb;
	int						c;
	int						oldTempMixbin;
	CModule*				pEffect;
	int						itm;
	EffectChainBucket		bucket;
	BOOL					found;
	int*					bins;
	int						numBins;
    int						err;
	char*					argv[4];
	CString					string;
	CStdioFile				file;
	int						totalTempMixbins;
	BOOL					bI3DL2;
	BOOL					flag;
	BOOL					b24k = TRUE;
	int						graph;
	int						extraTempMixbins;
	int						nullInput			= -1;	// Initialized to shut the compiler up
	int						nullOutput			= -1;	// Initialized to shut the compiler up
	int						nullMixbins			= 0;
	int						rvbTempMixbins		= 0;
	int						tempMixbin			= -1;
	BOOL					bNullInput			= FALSE;
	BOOL					bNullOutput			= FALSE;
	DWORD					dwEffectIndex		= 0;
	BOOL*					pbCleared			= NULL;
	BOOL					bInternalI3DL2		= FALSE;
	BOOL					bSimpleReverb		= FALSE;
	BOOL					bExtraMixbins		= FALSE;
	BOOL					bCustomStateFiles	= FALSE;
	int						extra;
	int						mixbin;
	CString					statefile;
	CArray<CJack*, CJack*>	connections;
	CString					statePath;
	CArray<EffectChainBucket, EffectChainBucket&> buckets;

	m_bImageSaved = FALSE;

	// Build options
	if(m_i3dl2Config != _T(""))
		bI3DL2 = TRUE;
	else
		bI3DL2 = FALSE;

	if(!file.Open(pFilename, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
	{
		AfxMessageBox(CString(_T("Unable to open file: ")) + file.GetFilePath(), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// First initialize all connections
	for(m=0; m<m_modules.GetSize(); ++m)
	{
		// Only process effects
		if(m_modules[m]->GetType() == CModule::EFFECT)
		{
			for(i=0; i<m_modules[m]->GetInputs().GetSize(); ++i)
				m_modules[m]->GetInputs()[i]->SetMixbin(INT_MAX);
			for(o=0; o<m_modules[m]->GetOutputs().GetSize(); ++o)
				m_modules[m]->GetOutputs()[o]->SetMixbin(INT_MAX);
		}
		else
		{
			for(i=0; i<m_modules[m]->GetInputs().GetSize(); ++i)
				m_modules[m]->GetInputs()[i]->SetMixbin(m_modules[m]->GetMixbin());
			for(o=0; o<m_modules[m]->GetOutputs().GetSize(); ++o)
				m_modules[m]->GetOutputs()[o]->SetMixbin(m_modules[m]->GetMixbin());
		}
	}

	// Build the connection graph
	for(m=0; m<m_modules.GetSize(); ++m)
	{
		// Only process effects
		if(m_modules[m]->GetType() != CModule::EFFECT)
			continue;

		// Only process connected effects
		if(!m_modules[m]->IsConnected())
			continue;

		// Check all the effect's inputs
		for(i=0; i<m_modules[m]->GetInputs().GetSize(); ++i)
		{
			// Only process unconnected inputs
			if(m_modules[m]->GetInputs()[i]->GetMixbin() != INT_MAX)
				continue;

			// Get the list of connections for the input
			m_modules[m]->GetInputs()[i]->GetConnections(connections);

			// Any connections?
			if(connections.GetSize())
			{
				for(itm=INT_MAX, c=0; c<connections.GetSize(); ++c)
				{
					if(connections[c]->GetMixbin() != INT_MAX)
					{
						ASSERT(itm == INT_MAX);
						itm = connections[c]->GetMixbin();
					}
				}

				// If no mixbins were assigned, create a new tempmixbin
				if(itm == INT_MAX)
					itm = tempMixbin--;

				// Aassign all mixbins
				m_modules[m]->GetInputs()[i]->SetMixbin(itm);
				for(c=0; c<connections.GetSize(); ++c)
					connections[c]->SetMixbin(itm);
			}

			// If no connections were found flag for a null input
			else
				bNullInput = TRUE;
		}

		// Check all the effect's outputs
		for(o=0; o<m_modules[m]->GetOutputs().GetSize(); ++o)
		{
			// Only process unconnected outputs
			if(m_modules[m]->GetOutputs()[o]->GetMixbin() != INT_MAX)
				continue;

			// Get the list of connections for the output
			m_modules[m]->GetOutputs()[o]->GetConnections(connections);

			// Any connections?
			if(connections.GetSize())
			{
				for(itm=INT_MAX, c=0; c<connections.GetSize(); ++c)
				{
					if(connections[c]->GetMixbin() != INT_MAX)
					{
						ASSERT(itm == INT_MAX);
						itm = connections[c]->GetMixbin();
					}
				}

				// If no mixbins were assigned, create a new tempmixbin
				if(itm == INT_MAX)
					itm = tempMixbin--;

				// Aassign all mixbins
				m_modules[m]->GetOutputs()[o]->SetMixbin(itm);
				for(c=0; c<connections.GetSize(); ++c)
					connections[c]->SetMixbin(itm);
			}

			// If no connections were found flag for a null output
			else
				bNullOutput = TRUE;
		}
	}

	// Build a list of used tempmixbins
	numBins	= -(tempMixbin + 1);
	bins	= new int [(unsigned)numBins];
	ASSERT(bins != NULL);
	for(b=0; b<numBins; ++b)
		bins[b] = 0;
	
	// Sort the effects into buckets to build the chains
	for(m=0; m<m_modules.GetSize(); ++m)
	{
		// Only process effects
		if(m_modules[m]->GetType() != CModule::EFFECT)
			continue;

		// Only process connected effects
		if(!m_modules[m]->IsConnected())
			continue;

		// Find the right bucket to insert the effect into
		for(found=FALSE, b=0; !found && b<buckets.GetSize(); ++b)
		{
			// The first bucket that this effect does not have any 
			// dependencies and insert it
			for(found=TRUE, i=0; found && i<buckets[b].effects.GetSize(); ++i)
			{
				if(DoesModuleDependOnModule(m_modules[m], buckets[b].effects[i]))
					found = FALSE;
			}

			// Was a bucket found?
			if(found)
			{
				// Search for reverse dependencies
				for(i=0; found && i<buckets[b].effects.GetSize(); ++i)
				{
					if(DoesModuleDependOnModule(buckets[b].effects[i], m_modules[m]))
						found = FALSE;
				}

				// If no reverse dependencies were found, insert the effect into this bucket
				if(found)
					buckets[b].effects.InsertAt(buckets[b].effects.GetSize(), m_modules[m]);

				// Otherwise, insert a new bucket and place the effect into it
				else
				{
					bucket.effects.RemoveAll();
					bucket.inputs.RemoveAll();
					bucket.outputs.RemoveAll();
					bucket.effects.InsertAt(0, m_modules[m]);
					buckets.InsertAt(b, bucket);
					found = TRUE;
				}
			}
		}

		// Was the effect inserted?
		if(!found)
		{
			bucket.effects.RemoveAll();
			bucket.inputs.RemoveAll();
			bucket.outputs.RemoveAll();
			bucket.effects.InsertAt(0, m_modules[m]);
			buckets.InsertAt(b, bucket);
		}
	}

	// Step through all the buckets and update the input and output lists
	for(b=0; b<buckets.GetSize(); ++b)
	{
		// Step through the effects in the bucket
		for(e=0; e<buckets[b].effects.GetSize(); ++e)
		{
			// Look through the inputs of the effect
			for(i=0; i<buckets[b].effects[e]->GetInputs().GetSize(); ++i)
			{
				// Is the input in the inputs list?
				for(ii=0; ii<buckets[b].inputs.GetSize(); ++ii)
				{
					if(buckets[b].effects[e]->GetInputs()[i]->GetMixbin() == buckets[b].inputs[ii])
						break;
				}

				// Not in the list? Add it
				if(ii == buckets[b].inputs.GetSize())
				{
					buckets[b].inputs.InsertAt(buckets[b].inputs.GetSize(), 
											   buckets[b].effects[e]->GetInputs()[i]->GetMixbin());
				}
			}

			// Look through the outputs of the effect
			for(o=0; o<buckets[b].effects[e]->GetOutputs().GetSize(); ++o)
			{
				// Is the input in the inputs list?
				for(oo=0; oo<buckets[b].outputs.GetSize(); ++oo)
				{
					if(buckets[b].effects[e]->GetOutputs()[o]->GetMixbin() == buckets[b].outputs[oo])
						break;
				}

				// Not in the list? Add it
				if(oo == buckets[b].outputs.GetSize())
				{
					buckets[b].outputs.InsertAt(buckets[b].outputs.GetSize(), 
											    buckets[b].effects[e]->GetOutputs()[o]->GetMixbin());
				}
			}
		}
	}

	// Remove extra temp mixbins
	for(b=0; b<buckets.GetSize(); ++b)
	{
		// Examine all the outputs in this bucket
		for(o=0; o<buckets[b].outputs.GetSize(); ++o)
		{
			// Only examine temp mixbins (negative values)
			if(buckets[b].outputs[o] < 0)
			{
				// Step through all previous buckets, looking for a bucket
				// with a duplicate output. If a duplicate is found, this temp mixbin
				// must be kept. If one is not found, try to find an available temp mixbin
				// that is no longer in use
				for(found=FALSE, bb=0; !found && bb<b; ++bb)
				{
					// Examine all the outputs of the bucket
					for(oo=0; oo<buckets[bb].outputs.GetSize(); ++oo)
					{
						if(buckets[bb].outputs[oo] == buckets[b].outputs[o])
							found = TRUE;
					}
				}

				// If not found, try to replace the temp mixbin
				if(!found)
				{
					// Start with the lowest numbered mixbin
					tempMixbin = -1;

					// If the search makes it's way to this mixbin, just bail
					found = TRUE;
					while(found && (buckets[b].outputs[o] != tempMixbin))
					{
						// Otherwise, look at this and all subsequent mixbins
						// and see if any rely on tempMixbin as an input
						// If none do, tempMixbin is a good choice
						for(bb=b; bb<buckets.GetSize(); ++bb)
						{
							// Examine all the inputs in the bucket
							for(i=0; i<buckets[bb].inputs.GetSize(); ++i)
							{
								if(buckets[bb].inputs[i] == tempMixbin)
									break;
							}
							if(i != buckets[bb].inputs.GetSize())
								break;
						}
						if(bb == buckets.GetSize())
							found = FALSE;
						else
							--tempMixbin;
					}

					// Is tempMixbin a good new choice?
					if(!found)
					{
						// Replace all occurences of the mixbin with
						// tempMixbin in this and all subsequent buckets
						oldTempMixbin = buckets[b].outputs[o];
						for(bb=b; bb<buckets.GetSize(); ++bb)
						{
							// Replace all inputs
							for(ii=0; ii<buckets[bb].inputs.GetSize(); ++ii)
							{
								if(buckets[bb].inputs[ii] == oldTempMixbin)
									buckets[bb].inputs[ii] = tempMixbin;
							}

							// Replace all outputs
							for(oo=0; oo<buckets[bb].outputs.GetSize(); ++oo)
							{
								if(buckets[bb].outputs[oo] == oldTempMixbin)
									buckets[bb].outputs[oo] = tempMixbin;
							}

							// Do the same for the real effects in the bucket
							for(e=0; e<buckets[bb].effects.GetSize(); ++e)
							{
								for(ii=0; ii<buckets[bb].effects[e]->GetInputs().GetSize(); ++ii)
								{
									if(buckets[bb].effects[e]->GetInputs()[ii]->GetMixbin() == oldTempMixbin)
										buckets[bb].effects[e]->GetInputs()[ii]->SetMixbin(tempMixbin);
								}
								for(oo=0; oo<buckets[bb].effects[e]->GetOutputs().GetSize(); ++oo)
								{
									if(buckets[bb].effects[e]->GetOutputs()[oo]->GetMixbin() == oldTempMixbin)
										buckets[bb].effects[e]->GetOutputs()[oo]->SetMixbin(tempMixbin);
								}
							}
						}
					}
				}
			}
		}
	}

	// Figure out which temp mixbins are being used
	for(b=0; b<buckets.GetSize(); ++b)
	{
		// Step through all the effects in the bucket
		for(e=0; e<buckets[b].effects.GetSize(); ++e)
		{
			// Step through all the inputs
			for(i=0; i<buckets[b].effects[e]->GetInputs().GetSize(); ++i)
			{
				// Is this a temp mixbin?
				if(buckets[b].effects[e]->GetInputs()[i]->GetMixbin() < 0)
					++bins[-(buckets[b].effects[e]->GetInputs()[i]->GetMixbin() + 1)];
			}

			// Step through all the outputs
			for(o=0; o<buckets[b].effects[e]->GetOutputs().GetSize(); ++o)
			{
				// Is this a temp mixbin?
				if(buckets[b].effects[e]->GetOutputs()[o]->GetMixbin() < 0)
					++bins[-(buckets[b].effects[e]->GetOutputs()[o]->GetMixbin() + 1)];
			}
		}
	}

	// Remove unused tempmixbins to compress the list of bins
	for(itm=0, bb=0; bb<numBins; ++bb)
	{
		// If this mixbin is not used, decrement all other tempmixbins larger than this
		if(bins[bb] == 0)
		{
			tempMixbin = -(bb + 1);
			for(b=0; b<buckets.GetSize(); ++b)
			{
				// Step through all the effects in the bucket
				for(e=0; e<buckets[b].effects.GetSize(); ++e)
				{
					// Step through all the inputs
					for(i=0; i<buckets[b].effects[e]->GetInputs().GetSize(); ++i)
					{
						// Is this a temp mixbin, and is does it need adjusting?
						mixbin = buckets[b].effects[e]->GetInputs()[i]->GetMixbin();
						if(mixbin < tempMixbin)
							buckets[b].effects[e]->GetInputs()[i]->SetMixbin(mixbin+1);
					}

					// Step through all the outputs
					for(o=0; o<buckets[b].effects[e]->GetOutputs().GetSize(); ++o)
					{
						// Is this a temp mixbin, and is does it need adjusting?
						mixbin = buckets[b].effects[e]->GetOutputs()[o]->GetMixbin();
						if(mixbin < tempMixbin)
							buckets[b].effects[e]->GetOutputs()[o]->SetMixbin(mixbin+1);
					}
				}
			}

		}
		else
			++itm;
	}
	delete [] bins;

	// Write the file
	try
	{
		// header
		file.WriteString(_T("[MAIN]\n"));

		// Name it the same as the ini file
		string.Format(_T("IMAGE_FRIENDLY_NAME=%s\n"), file.GetFileTitle());

		// CString::GetFileTitle() is not supposed to return a file extension but is
		// The following lines remove the extension if it exists
		i = string.Find(_T(".ini\n"));
		if(i == (string.GetLength() - 5))
			string.Delete(i, 4);

		file.WriteString(string);

		// Look for an I3DL2 Reverb in the scene
		for(b=0; b<buckets.GetSize(); ++b)
		{
			for(e=0; e<buckets[b].effects.GetSize(); ++e)
			{
//				if(!strcmp(buckets[b].effects[e]->GetEffectName(), _T("I3DL2 Reverb")) ||
//				   !strcmp(buckets[b].effects[e]->GetEffectName(), _T("I3DL2 24K Reverb")))
				if(buckets[b].effects[e]->IsI3DL2())
				{
					bInternalI3DL2	= TRUE;
					bExtraMixbins	= TRUE;
					numBins			= 0;

					// Count the number of tempbins already attached to the reverb
					for(i=0; i<buckets[b].effects[e]->GetInputs().GetSize(); ++i)
					{
						if(buckets[b].effects[e]->GetInputs()[i]->GetMixbin() < 0)
							++numBins;
					}
					for(i=0; i<buckets[b].effects[e]->GetOutputs().GetSize(); ++i)
					{
						if(buckets[b].effects[e]->GetOutputs()[i]->GetMixbin() < 0)
							++numBins;
					}
					if(numBins > rvbTempMixbins)
						rvbTempMixbins = numBins;
				}
			}
		}
		
		if(bI3DL2)
			bExtraMixbins = TRUE;

		// Look for a Simple Reverb in the scene
		for(b=0; b<buckets.GetSize(); ++b)
		{
			for(e=0; e<buckets[b].effects.GetSize(); ++e)
			{
				if(!strcmp(buckets[b].effects[e]->GetEffectName(), _T("Simple Reverb")))
				{
					bSimpleReverb	= TRUE;
					bExtraMixbins	= TRUE;
					numBins			= 0;

					// Count the number of tempbins already attached to the reverb
					for(i=0; i<buckets[b].effects[e]->GetInputs().GetSize(); ++i)
					{
						if(buckets[b].effects[e]->GetInputs()[i]->GetMixbin() < 0)
							++numBins;
					}
					for(i=0; i<buckets[b].effects[e]->GetOutputs().GetSize(); ++i)
					{
						if(buckets[b].effects[e]->GetOutputs()[i]->GetMixbin() < 0)
							++numBins;
					}
					if(numBins > rvbTempMixbins)
						rvbTempMixbins = numBins;
				}
			}
		}
		
		// Number of temp mixbins
		totalTempMixbins = itm;
		if(bNullInput)
		{
			nullInput = totalTempMixbins++;
			++nullMixbins;
		}
		if(bNullOutput)
		{
			nullOutput = totalTempMixbins++;
			++nullMixbins;
		}

		// Extra tempmixbins
		if(bExtraMixbins)
		{
			if(bInternalI3DL2 || bI3DL2)
				extraTempMixbins = (totalTempMixbins < 31) ? (31 - (totalTempMixbins - rvbTempMixbins - nullMixbins)) : 0;
			else if(bSimpleReverb)
				extraTempMixbins = (totalTempMixbins < 8) ? (8 - (totalTempMixbins - rvbTempMixbins - nullMixbins)) : 0;
			else
				extraTempMixbins = 0;
		}
		else
			extraTempMixbins = 0;

		string.Format(_T("FX_NUMTEMPBINS=%d\n"), totalTempMixbins + extraTempMixbins);
		file.WriteString(string);

		// Allocate the buffer 
		pbCleared = new BOOL [(unsigned)totalTempMixbins];
		ASSERT(pbCleared != NULL);
		for(i=0; i<totalTempMixbins; ++i)
			pbCleared[i] = FALSE;

		// Write the graph names
		for(graph=0, b=0; b<buckets.GetSize(); ++b)
		{
			for(e=0; e<buckets[b].effects.GetSize(); ++e, ++graph)
			{
				string.Format(_T("GRAPH%d=Graph%d\n"), graph, graph);
				file.WriteString(string);
			}
		}
		
		// I3DL2 Reverb or XTalk?
		if(bI3DL2 || m_bIncludeXtalk || m_bPatchXtalkToLFE)
		{
			string.Format(_T("GRAPH%d=Graph%d\n"), graph, graph);
			file.WriteString(string);
		}

		file.WriteString(_T("\n"));

		// Write the graphs
		for(graph=0, b=0; b<buckets.GetSize(); ++b)
		{
			for(e=0; e<buckets[b].effects.GetSize(); ++e, ++graph)
			{
				string.Format(_T("[Graph%d]\n"), graph);
				file.WriteString(string);
				string.Format(_T("FX0=%s\n\n"), buckets[b].effects[e]->GetName());
				string.Remove(' ');
				file.WriteString(string);
			}
		}

		// I3DL2 Reverb, XTalk, or XTalk2LFE?
		if(bI3DL2 || m_bIncludeXtalk || m_bPatchXtalkToLFE)
		{
			string.Format(_T("[Graph%d]\n"), graph);
			file.WriteString(string);
			b = 0;
			if(bI3DL2)
			{
				if(b24k)
					string.Format(_T("FX%d=I3DL2Reverb24K\n"), b++);
				else
					string.Format(_T("FX%d=I3DL2Reverb\n"), b++);
				file.WriteString(string);
			}
			if(m_bIncludeXtalk)
			{
				string.Format(_T("FX%d=XTalk\n"), b++);
				file.WriteString(string);
			}
			if(m_bPatchXtalkToLFE)
			{
				string.Format(_T("FX%d=XTalk2LFE_A\n"), b++);
				file.WriteString(string);
				string.Format(_T("FX%d=XTalk2LFE_B\n"), b++);
				file.WriteString(string);
			}
			file.WriteString(_T("\n"));
		}

		// Determine the path to the state files
		statePath	= file.GetFilePath();
		ii			= statePath.ReverseFind('\\');
		if(ii)
			statePath.Delete(ii, statePath.GetLength()-ii);

		// If any of the effect paramaters are not their default, custom state files will
		// need to be used
		for(bCustomStateFiles=FALSE, graph=0, b=0; !bCustomStateFiles && b<buckets.GetSize(); ++b)
		{
			for(e=0; !bCustomStateFiles && e<buckets[b].effects.GetSize(); ++e, ++graph)
			{
				pEffect = buckets[b].effects[e];
				if(pEffect->AreParametersModified())
					bCustomStateFiles = TRUE;
			}
		}

		// Write the state files
		if(bCustomStateFiles)
		{
			for(graph=0, b=0; bCustomStateFiles && b<buckets.GetSize(); ++b)
			{
				for(e=0; bCustomStateFiles && e<buckets[b].effects.GetSize(); ++e, ++graph)
				{
					pEffect = buckets[b].effects[e];
			
					// Generate the state ini file
					string	= file.GetFileTitle();
					ii		= string.ReverseFind('.');
					if(ii)
						string.Delete(ii, string.GetLength()-ii);
					statefile.Format(_T("%s%d%sstate.ini"), string, graph, pEffect->GetName());
					statefile.Remove(' ');
					buckets[b].stateFiles.Add(statefile);

					string.Format(_T("%s\\%s"), statePath, statefile);
					buckets[b].stateFilePaths.Add(string);

					if(!pEffect->WriteStateFile(string))
						bCustomStateFiles = FALSE;
				}
			}
		}
			
		if(bCustomStateFiles)
		{
			if(bI3DL2)
			{
				if(b24k)
					pEffect = m_pI3DL2_24K;
				else
					pEffect = m_pI3DL2;
				for(i=0; i<pEffect->GetNumConfigs(); ++i)
				{
					if(pEffect->GetConfigs()[i].name == m_i3dl2Config)
						break;
				}
				if(i == pEffect->GetNumConfigs())
					i = 0;
				string.Format(_T("%s\\%s"), GetIniFilePath(), pEffect->GetConfigs()[i].stateFilename);
				statefile.Format(_T("%s\\%s"), statePath, pEffect->GetConfigs()[i].stateFilename);
				if(!CopyFile(string, statefile, FALSE))
					bCustomStateFiles = FALSE;
				else
					SetFileAttributes(statefile, FILE_ATTRIBUTE_NORMAL);
			}

			// XTalk?
			if(m_bIncludeXtalk)
			{
				string.Format(_T("%s\\%s"), GetIniFilePath(), m_pXTalk->GetStateFilename());
				statefile.Format(_T("%s\\%s"), statePath, m_pXTalk->GetStateFilename());
				if(!CopyFile(string, statefile, FALSE))
					bCustomStateFiles = FALSE;
				else
					SetFileAttributes(statefile, FILE_ATTRIBUTE_NORMAL);
			}

			// XTalk2LFE
			if(m_bPatchXtalkToLFE)
			{
				string.Format(_T("%s\\%s"), GetIniFilePath(), m_p2x1Mixer->GetStateFilename());
				statefile.Format(_T("%s\\%s"), statePath, m_p2x1Mixer->GetStateFilename());
				CopyFile(string, statefile, FALSE);
				if(!CopyFile(string, statefile, FALSE))
					bCustomStateFiles = FALSE;
				else
					SetFileAttributes(statefile, FILE_ATTRIBUTE_NORMAL);
			}
		}

		// Was there an error generating the state ini files?
		if(!bCustomStateFiles)
		{
			// Delete them all
			for(graph=0, b=0; b<buckets.GetSize(); ++b)
			{
				for(e=0; e<buckets[b].stateFilePaths.GetSize(); ++e, ++graph)
					CFile::Remove(buckets[b].stateFilePaths[e]);
				buckets[b].stateFilePaths.RemoveAll();
				buckets[b].stateFiles.RemoveAll();
			}
		}

		for(graph=0, b=0; b<buckets.GetSize(); ++b)
		{
			for(e=0; e<buckets[b].effects.GetSize(); ++e, ++graph)
			{
				pEffect = buckets[b].effects[e];

				// I3DL2 Reverb special case
//				if(!strcmp(pEffect->GetEffectName(), _T("I3DL2 Reverb")) ||
//				   !strcmp(pEffect->GetEffectName(), _T("I3DL2 24K Reverb")))
				if(pEffect->IsI3DL2())
				{
					flag	= TRUE;
					extra	= 31;
				}
				else if(!strcmp(pEffect->GetEffectName(), _T("Simple Reverb")))
				{
					flag	= TRUE;
					extra	= 8;
				}
				else
				{
					flag	= FALSE;
					extra	= 0;
				}

				string.Format(_T("[Graph%d_FX0_%s]\n"), graph, pEffect->GetName());
				string.Remove(' ');
				file.WriteString(string);

				// Generate and save the ini name (used in parameter transmission)
				string.Format(_T("Graph%d_%s"), graph, pEffect->GetName());
				string.Remove(' ');
				pEffect->SetIniName(string);

				// If any of the outputs are to tempmixbins that have not been cleared, set mixoutput
				// to 0, otherwise set it to 0
				for(o=0; o<pEffect->GetOutputs().GetSize(); ++o)
				{
					if(pEffect->GetOutputs()[o]->GetMixbin() == INT_MAX)
					{
						if(!pbCleared[nullOutput])
						{
							file.WriteString(_T("FX_MIXOUTPUT=0\n"));
							pbCleared[nullOutput] = TRUE;
							break;
						}
					}
					else if(pEffect->GetOutputs()[o]->GetMixbin() < 0)
					{
						if(!pbCleared[-(pEffect->GetOutputs()[o]->GetMixbin() + 1)])
						{
							file.WriteString(_T("FX_MIXOUTPUT=0\n"));
							pbCleared[-(pEffect->GetOutputs()[o]->GetMixbin() + 1)] = TRUE;
							break;
						}
					}
				}
				if(o == pEffect->GetOutputs().GetSize())
					file.WriteString(_T("FX_MIXOUTPUT=1\n"));
				string.Format(_T("FX_DSPCODE=%s\n"), pEffect->GetDSPCode());
				file.WriteString(string);
				if(bCustomStateFiles)
					string.Format(_T("FX_DSPSTATE=%s\n"), buckets[b].stateFiles[e]);
				else
					string.Format(_T("FX_DSPSTATE=%s\n"), pEffect->GetStateFilename());
				file.WriteString(string);
				string.Format(_T("FX_NUMINPUTS=%d\n"), pEffect->GetInputs().GetSize());
				file.WriteString(string);
				if(flag)
					string.Format(_T("FX_NUMOUTPUTS=%d\n"), pEffect->GetOutputs().GetSize() + extra);
				else
					string.Format(_T("FX_NUMOUTPUTS=%d\n"), pEffect->GetOutputs().GetSize());
				file.WriteString(string);
				for(i=0; i<pEffect->GetInputs().GetSize(); ++i)
				{
					if(pEffect->GetInputs()[i]->GetMixbin() == INT_MAX)
						string.Format("FX_INPUT%d=GPTEMPBIN%d\n", i, nullInput);
					else if(pEffect->GetInputs()[i]->GetMixbin() >= 0)
						string.Format("FX_INPUT%d=VPMIXBIN_%s\n", i, GetMixbinName(pEffect->GetInputs()[i]->GetMixbin()));
					else
						string.Format("FX_INPUT%d=GPTEMPBIN%d\n", i, -(pEffect->GetInputs()[i]->GetMixbin() + 1));
					file.WriteString(string);
				}
				for(o=0; o<pEffect->GetOutputs().GetSize(); ++o)
				{
					if(pEffect->GetOutputs()[o]->GetMixbin() == INT_MAX)
						string.Format("FX_OUTPUT%d=GPTEMPBIN%d\n", o, nullOutput);
					else if(pEffect->GetOutputs()[o]->GetMixbin() >= 0)
						string.Format("FX_OUTPUT%d=GPMIXBIN_%s\n", o, GetMixbinName(pEffect->GetOutputs()[o]->GetMixbin()));
					else
						string.Format("FX_OUTPUT%d=GPTEMPBIN%d\n", o, -(pEffect->GetOutputs()[o]->GetMixbin() + 1));
					file.WriteString(string);
				}

				if(flag)
				{
					for(o=0; o<extra; ++o)
					{
						string.Format("FX_OUTPUT%d=GPTEMPBIN%d\n", o+pEffect->GetOutputs().GetSize(), o+totalTempMixbins);
						file.WriteString(string);
					}
				}

				file.WriteString(_T("\n"));
				++dwEffectIndex;
			}
		}

		// FX index for special additions below
		e = 0;

		// Special I3DL2 Reverb
		if(bI3DL2)
		{
			if(b24k)
			{
				string.Format(_T("[Graph%d_FX%d_I3DL2Reverb24K]\n"), graph, e++);
				pEffect = m_pI3DL2_24K;
			}
			else
			{
				string.Format(_T("[Graph%d_FX%d_I3DL2Reverb]\n"), graph, e++);
				pEffect = m_pI3DL2;
			}
			file.WriteString(string);
			file.WriteString(_T("FX_MIXOUTPUT=1\n"));
			string.Format(_T("FX_DSPCODE=%s\n"), pEffect->GetDSPCode());
			file.WriteString(string);
			for(i=0; i<pEffect->GetNumConfigs(); ++i)
			{
				if(pEffect->GetConfigs()[i].name == m_i3dl2Config)
					break;
			}
			if(i == pEffect->GetNumConfigs())
				i = 0;
			string.Format(_T("FX_DSPSTATE=%s\n"), pEffect->GetConfigs()[i].stateFilename);
			file.WriteString(string);
			file.WriteString(_T("FX_NUMINPUTS=2\n"));
			file.WriteString(_T("FX_NUMOUTPUTS=35\n"));
			file.WriteString("FX_INPUT0=VPMIXBIN_I3DL2SEND\n");
			file.WriteString("FX_INPUT1=VPMIXBIN_I3DL2SEND\n");
			file.WriteString("FX_OUTPUT0=GPMIXBIN_XTLK_FL\n");
			file.WriteString("FX_OUTPUT1=GPMIXBIN_XTLK_FR\n");
			file.WriteString("FX_OUTPUT2=GPMIXBIN_XTLK_BL\n");
			file.WriteString("FX_OUTPUT3=GPMIXBIN_XTLK_BR\n");
			for(o=0; o<31; ++o)
			{
				string.Format("FX_OUTPUT%d=GPTEMPBIN%d\n", o+4, o);
				file.WriteString(string);
			}
			file.WriteString(_T("\n"));
			m_dwI3DL2Index = dwEffectIndex++;
		}

		// XTalk?
		if(m_bIncludeXtalk)
		{
			string.Format(_T("[Graph%d_FX%d_XTalk]\n"), graph, e++);
			file.WriteString(string);
			file.WriteString(_T("FX_MIXOUTPUT=1\n"));
			string.Format(_T("FX_DSPCODE=%s\n"), m_pXTalk->GetDSPCode());
			file.WriteString(string);
			string.Format(_T("FX_DSPSTATE=%s\n"), m_pXTalk->GetStateFilename());
			file.WriteString(string);
			file.WriteString(_T("FX_NUMINPUTS=4\n"));
			file.WriteString(_T("FX_NUMOUTPUTS=4\n"));
			file.WriteString(_T("FX_INPUT0=VPMIXBIN_XTLK_FL\n"));
			file.WriteString(_T("FX_INPUT1=VPMIXBIN_XTLK_FR\n"));
			file.WriteString(_T("FX_INPUT2=VPMIXBIN_XTLK_BL\n"));
			file.WriteString(_T("FX_INPUT3=VPMIXBIN_XTLK_BR\n"));
			file.WriteString(_T("FX_OUTPUT0=GPMIXBIN_FRONTLEFT\n"));
			file.WriteString(_T("FX_OUTPUT1=GPMIXBIN_FRONTRIGHT\n"));
			file.WriteString(_T("FX_OUTPUT2=GPMIXBIN_BACKLEFT\n"));
			file.WriteString(_T("FX_OUTPUT3=GPMIXBIN_BACKRIGHT\n"));
			file.WriteString(_T("\n"));
			m_dwXTalkIndex = dwEffectIndex++;
		}

		// XTalk2LFE
		if(m_bPatchXtalkToLFE)
		{
			string.Format(_T("[Graph%d_FX%d_XTalk2LFE_A]\n"), graph, e++);
			pEffect = m_p2x1Mixer;
			file.WriteString(string);
			file.WriteString(_T("FX_MIXOUTPUT=1\n"));
			string.Format(_T("FX_DSPCODE=%s\n"), pEffect->GetDSPCode());
			file.WriteString(string);
			string.Format(_T("FX_DSPSTATE=%s\n"), pEffect->GetStateFilename());
			file.WriteString(string);
			file.WriteString(_T("FX_NUMINPUTS=2\n"));
			file.WriteString(_T("FX_NUMOUTPUTS=1\n"));
			file.WriteString("FX_INPUT0=GPMIXBIN_XTLK_FL\n");
			file.WriteString("FX_INPUT1=GPMIXBIN_XTLK_FR\n");
			file.WriteString("FX_OUTPUT0=GPMIXBIN_LFE\n");
			file.WriteString(_T("\n"));

			string.Format(_T("[Graph%d_FX%d_XTalk2LFE_B]\n"), graph, e++);
			pEffect = m_p2x1Mixer;
			file.WriteString(string);
			file.WriteString(_T("FX_MIXOUTPUT=1\n"));
			string.Format(_T("FX_DSPCODE=%s\n"), pEffect->GetDSPCode());
			file.WriteString(string);
			string.Format(_T("FX_DSPSTATE=%s\n"), pEffect->GetStateFilename());
			file.WriteString(string);
			file.WriteString(_T("FX_NUMINPUTS=2\n"));
			file.WriteString(_T("FX_NUMOUTPUTS=1\n"));
			file.WriteString("FX_INPUT0=GPMIXBIN_XTLK_BL\n");
			file.WriteString("FX_INPUT1=GPMIXBIN_XTLK_BR\n");
			file.WriteString("FX_OUTPUT0=GPMIXBIN_LFE\n");
			file.WriteString(_T("\n"));
		}
	}
	catch (CFileException ex)
	{
		AfxMessageBox(CString(_T("Error writing DSP Image File: ")) + file.GetFilePath(), MB_OK | MB_ICONERROR);
		if(pbCleared)
			delete [] pbCleared;
		return FALSE;
	}

	// Free resources
	if(pbCleared)
		delete [] pbCleared;

	// Get the filename and close the file
	string = file.GetFilePath();
	file.Close();

	// Now that the .ini file has been written, compile it...

	// Set the paths
	if(bCustomStateFiles)
		m_imageBuilder->SetIniPath(statePath);
	else
		m_imageBuilder->SetIniPath(m_iniPath);
	m_imageBuilder->SetDspCodePath(m_dspCodePath);

	// Build the argument list for CDspImageBuilder
	argv[0] = new char [11];
	strcpy(argv[0], "dspbuilder");

	argv[1] = new char [(unsigned)string.GetLength()+1];
	strcpy(argv[1], (LPCTSTR)string);

	string.Replace(_T(".ini"), _T(".bin"));
	argv[2] = new char [(unsigned)string.GetLength()+1];
	strcpy(argv[2], (LPCTSTR)string);
		
	string.Replace(_T(".bin"), _T(".h"));
	argv[3] = new char [(unsigned)string.GetLength()+1];
	strcpy(argv[3], (LPCTSTR)string);

	m_imageBuilder->ClearText();
    err = m_imageBuilder->ParseCommandLine(4, argv);

	// Free the argument list
	delete [] argv[0];
	delete [] argv[1];
	delete [] argv[2];
	delete [] argv[3];

    if (err != ERROR_SUCCESS) 
        return FALSE;
    
    // Parse the actual file and assemble the dsp image
    err = m_imageBuilder->ParseInputFile();
    if (err != ERROR_SUCCESS) 
	{
		m_imageBuilder->ShowDialog();
		AfxMessageBox(_T("Failed parsing input file"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    // Parsing complete.
	// at this point we should have Graphs created with an array of FX in each
	// the next step is to validate the FX chain and claculate the resource reqs
	err = m_imageBuilder->ValidateFxGraphs();
    if (err != ERROR_SUCCESS) 
	{
		m_imageBuilder->ShowDialog();
		//AfxMessageBox(_T("Validation of FX chains returned error"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    err = m_imageBuilder->BuildDspImage();
    if (err != ERROR_SUCCESS) 
	{
		m_imageBuilder->ShowDialog();
		//AfxMessageBox(_T("Failed to created DSP image"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

    err = m_imageBuilder->CreateEnumHeader();
    if (err != ERROR_SUCCESS) 
	{
		m_imageBuilder->ShowDialog();
		//AfxMessageBox(_T("Failed to created C Language enumeration header"), MB_OK | MB_ICONERROR);
        return FALSE;
    }

	// Display any messages
	m_imageBuilder->ShowDialog();

	// Workaround a CDspImageBuilder bug:
	delete m_imageBuilder;
	m_imageBuilder = new CWinDspImageBuilder;

	m_bImageSaved	= TRUE;
	m_bChangesMade	= FALSE;

	// Resave the file to update ini names and indices
	Save(NULL);

	return TRUE;
}

//------------------------------------------------------------------------------
//	CGraph::DoesModuleDependOnModule
//------------------------------------------------------------------------------
BOOL 
CGraph::DoesModuleDependOnModule(
								 IN const CModule*	pM0,
								 IN const CModule*	pM1
								 )
/*++

Routine Description:

	Tests to see if module pE0 depends on pE1. A dependency exists if any input
	of pE0 can be traced to an output (possibly through other modules) of pE1.

Arguments:

	IN pM0 -	Module to lookup
	IN pM1 -	Module to attempt to trace to

ReturnValue:

	TRUE if pE0 depends on pE1, FALSE otherwise

--*/
{
	int	i;
	int	o;

	for(i=0; i<pM0->GetInputs().GetSize(); ++i)
	{
		for(o=0; o<pM1->GetOutputs().GetSize(); ++o)
		{
			if(CanJackBeRoutedToJack(pM0->GetInputs()[i], pM1->GetOutputs()[o]))
				return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CGraph::CanJackBeRoutedToJack
//------------------------------------------------------------------------------
BOOL
CGraph::CanJackBeRoutedToJack(
							  IN CJack*			pJack0,
							  IN const CJack*	pJack1
							  )
/*++

Routine Description:

	Determines if one jack is connected to another even through other effects

Arguments:

	IN pJack0 -	First jack
	IN pJack1 -	Second jack

Return Value:

	TRUE if a route can be traced between the, FALSE otherwise

--*/
{
	int						i;
	CArray<CJack*, CJack*>	connections;

	// Get the connections list
	pJack0->GetAllConnections(connections);

	// Are they connected?
	for(i=0; i<connections.GetSize(); ++i)
	{
		if(connections[i] == pJack1)
			return TRUE;
	}

	// 
	return FALSE;
}

//------------------------------------------------------------------------------
//	CGraph::GetMixbinName
//------------------------------------------------------------------------------
LPCTSTR 
CGraph::GetMixbinName(
					  IN int mixbin
					  ) const
/*++

Routine Description:

	Returns an xgpimage friendly mixbin name

Arguments:

	IN mixbin -	Mixbin index

Return Value:

	mixbin name

--*/
{
	switch(mixbin)
	{
	case 0:
		return _T("FRONTLEFT");
	case 1:
		return _T("FRONTRIGHT");
	case 2:
		return _T("CENTER");
	case 3:
		return _T("LFE");
	case 4:
		return _T("BACKLEFT");
	case 5:
		return _T("BACKRIGHT");
	case 6:
		return _T("XTLK_FL");
	case 7:
		return _T("XTLK_FR");
	case 8:
		return _T("XTLK_BL");
	case 9:
		return _T("XTLK_BR");
	case 10:
		return _T("I3DL2SEND");
	case 11:
		return _T("FXSEND0");
	case 12:
		return _T("FXSEND1");
	case 13:
		return _T("FXSEND2");
	case 14:
		return _T("FXSEND3");
	case 15:
		return _T("FXSEND4");
	case 16:
		return _T("FXSEND5");
	case 17:
		return _T("FXSEND6");
	case 18:
		return _T("FXSEND7");
	case 19:
		return _T("FXSEND8");
	case 20:
		return _T("FXSEND9");
	case 21:
		return _T("FXSEND10");
	case 22:
		return _T("FXSEND11");
	case 23:
		return _T("FXSEND12");
	case 24:
		return _T("FXSEND13");
	case 25:
		return _T("FXSEND14");
	case 26:
		return _T("FXSEND15");
	case 27:
		return _T("FXSEND16");
	case 28:
		return _T("FXSEND17");
	case 29:
		return _T("FXSEND18");
	case 30:
		return _T("FXSEND19");
	default:
		break;
	}
	return _T("Unknown");
}

//------------------------------------------------------------------------------
//	CGraph::SetInUse
//------------------------------------------------------------------------------
void 
CGraph::SetInUse(void)
/*++

Routine Description:

	Adjusts the list of in use mixbins

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	// Clear the inuse array
	for(i=0; i<m_inputMixbinInUse.GetSize(); ++i)
		m_inputMixbinInUse[i] = FALSE;
	for(i=0; i<m_outputMixbinInUse.GetSize(); ++i)
		m_outputMixbinInUse[i] = FALSE;

	// Set the in use flags
	for(i=0; i<m_modules.GetSize(); ++i)
	{
		if(m_modules[i]->GetType() == CModule::INPUT_MIXBIN)
			m_inputMixbinInUse[m_modules[i]->GetMixbin()] = TRUE;
		else if(m_modules[i]->GetType() == CModule::OUTPUT_MIXBIN)
			m_outputMixbinInUse[m_modules[i]->GetMixbin()] = TRUE;
	}
}

//------------------------------------------------------------------------------
//	CGraph::TraceAndBuildPatchCord
//------------------------------------------------------------------------------
void
CGraph::TraceAndBuildPatchCord(
							   IN int	x,
							   IN int	y
							   )
/*++

Routine Description:

	Traces a cell based patch cord and builds a real patch cord

Arguments:

	IN x -			X coordinate on grid
	IN y -			Y coordinate on grid

Return Value:

	None

--*/
{
	CPatchCord*				pPatchCord;
	CArray<CPoint, CPoint&>	points;
	CPoint					point;

	if(!m_pGrid)
		return;

	// Is it part of a module
	if(m_pGrid[CI(x,y)].pModule)
	{
		// Is this an output?
		if(m_pGrid[CI(x,y)].bRight)
		{
			// Is the cell to the right valid?
			// Is the cell on the right patched to this output?
			// Has the patchcord for the cell to the right not been generated?
			if((x < (m_gridWidth-1)) &&  (m_pGrid[CI(x+1, y)].bLeft) &&  (!m_pGrid[CI(x+1, y)].pPatchCordLeft))
			{
				// Create a patch cord for the right cell
				pPatchCord = new CPatchCord;

				// Add a patchord to the beginning
				m_pGrid[CI(x,y)].pModule->GetOutputs()[m_pGrid[CI(x,y)].index]->Connect(pPatchCord, FALSE);

				// Add an endpoint
				point.x = (x+2) * SCALE;
				point.y	= ((y+1) * SCALE) + (SCALE/2);
				points.Add(point);

				// Trace and mark the remainder of this patch cord
				TraceAndMarkPatchCord(x+1, y, pPatchCord, CGraph::LEFT, points);
				pPatchCord->SetPoints(points);
				points.RemoveAll();
			}
		}

		// Input
		else if(m_pGrid[CI(x,y)].bLeft)
		{
			// Is the left cell valid?
			// Is the left cell patched to this?
			// Has a patch cord not been generated for the left cell?
			if((x > 0) && (m_pGrid[CI(x-1, y)].bRight) &&  (!m_pGrid[CI(x-1, y)].pPatchCordRight))
			{
				// Create a patch cord for the right cell
				pPatchCord = new CPatchCord;

				// Add a patchord to the beginning
				m_pGrid[CI(x,y)].pModule->GetInputs()[m_pGrid[CI(x,y)].index]->Connect(pPatchCord, FALSE);

				// Add an endpoint
				point.x	= ((x+1) * SCALE) - 1;
				point.y	= ((y+1) * SCALE) + (SCALE/2);
				points.Add(point);

				// Trace and mark the remainder of this patch cord
				TraceAndMarkPatchCord(x-1, y, pPatchCord, CGraph::RIGHT, points);
				pPatchCord->SetPoints(points);
				points.RemoveAll();
			}
		}
	}

	// Or a junction
	else if(m_pGrid[CI(x,y)].bJoined)
	{
		point.x	= ((x+1) * SCALE) + (SCALE/2);
		point.y	= ((y+1) * SCALE) + (SCALE/2);

		// Patch Left?
		// Not patched yet?
		// Valid patch to left?
		if(m_pGrid[CI(x,y)].bLeft && !m_pGrid[CI(x,y)].pPatchCordLeft && (x > 0) && m_pGrid[CI(x-1, y)].bRight)
		{
			m_pGrid[CI(x,y)].bEndLeft = FALSE;

			// Create a patch cord for the right cell
			pPatchCord = new CPatchCord;
			m_pGrid[CI(x,y)].pPatchCordLeft = pPatchCord;

			// Add an endpoint
			points.Add(point);

			// Trace and mark the remainder of this patch cord
			TraceAndMarkPatchCord(x-1, y, pPatchCord, CGraph::RIGHT, points);
			pPatchCord->SetPoints(points);
			points.RemoveAll();
		}

		// Patch Right?
		// Not patched yet?
		// Valid patch to right?
		if(m_pGrid[CI(x,y)].bRight && !m_pGrid[CI(x,y)].pPatchCordRight && (x+1 < m_gridWidth) && m_pGrid[CI(x+1, y)].bLeft)
		{
			m_pGrid[CI(x,y)].bEndRight = FALSE;

			// Create a patch cord for the right cell
			pPatchCord = new CPatchCord;
			m_pGrid[CI(x,y)].pPatchCordRight = pPatchCord;

			// Add an endpoint
			points.Add(point);

			// Trace and mark the remainder of this patch cord
			TraceAndMarkPatchCord(x+1, y, pPatchCord, CGraph::LEFT, points);
			pPatchCord->SetPoints(points);
			points.RemoveAll();
		}

		// Patch Top?
		// Not patched yet?
		// Valid patch to top?
		if(m_pGrid[CI(x,y)].bTop && !m_pGrid[CI(x,y)].pPatchCordTop && (y > 0) && m_pGrid[CI(x, y-1)].bBottom)
		{
			m_pGrid[CI(x,y)].bEndTop = FALSE;

			// Create a patch cord for the right cell
			pPatchCord = new CPatchCord;
			m_pGrid[CI(x,y)].pPatchCordTop = pPatchCord;

			// Add an endpoint
			points.Add(point);

			// Trace and mark the remainder of this patch cord
			TraceAndMarkPatchCord(x, y-1, pPatchCord, CGraph::BOTTOM, points);
			pPatchCord->SetPoints(points);
			points.RemoveAll();
		}

		// Patch Bottom?
		// Not patched yet?
		// Valid patch to bottom?
		if(m_pGrid[CI(x,y)].bBottom && !m_pGrid[CI(x,y)].pPatchCordBottom && (y+1 < m_gridHeight) && m_pGrid[CI(x, y+1)].bTop)
		{
			m_pGrid[CI(x,y)].bEndBottom = FALSE;

			// Create a patch cord for the right cell
			pPatchCord = new CPatchCord;
			m_pGrid[CI(x,y)].pPatchCordBottom = pPatchCord;

			// Add an endpoint
			points.Add(point);

			// Trace and mark the remainder of this patch cord
			TraceAndMarkPatchCord(x, y+1, pPatchCord, CGraph::TOP, points);
			pPatchCord->SetPoints(points);
			points.RemoveAll();
		}
	}
}

//------------------------------------------------------------------------------
//	CGraph::TraceAndMarkPatchCord
//------------------------------------------------------------------------------
void
CGraph::TraceAndMarkPatchCord(
							  IN int							x,
							  IN int							y,
							  IN CPatchCord*					pPatchCord,
							  IN CGraph::Direction				from,
							  IN OUT CArray<CPoint, CPoint&>&	points
							  )
/*++

Routine Description:

	Traces a cell-based patch cord and continues to build the patchcord

Arguments:

	IN x -			X location in grid
	IN y -			Y location in grid
	IN pPatchCord -	Patch cord to assign
	IN from	-		Direction travelling from
	IN points -		Points array

Return Value:

	None

--*/
{
	CPoint	center(((x+1) * SCALE) + (SCALE / 2), ((y+1) * SCALE) + (SCALE / 2));

	if(!m_pGrid)
		return;

	// Is this a module?
	if(m_pGrid[CI(x,y)].pModule)
	{
		// Can only come from the left or right
		switch(from)
		{
		case CGraph::LEFT:
			// This is not the center
			center.x	= (x+1)*SCALE;
			center.y	= ((y+1)*SCALE) + (SCALE/2);
			points.Add(center);
			m_pGrid[CI(x,y)].pModule->GetInputs()[m_pGrid[CI(x,y)].index]->Connect(pPatchCord, TRUE);
			break;

		case CGraph::RIGHT:
			// This is not the center
			center.x	= (x+2)*SCALE-1;
			center.y	= ((y+1)*SCALE) + (SCALE/2);
			points.Add(center);
			m_pGrid[CI(x,y)].pModule->GetOutputs()[m_pGrid[CI(x,y)].index]->Connect(pPatchCord, TRUE);
			break;
		case CGraph::TOP:
		case CGraph::BOTTOM:
		default:
			break;
		}
		return;
	}

	switch(from)
	{
	case CGraph::LEFT:
		if(!m_pGrid[CI(x,y)].bLeft)
			return;

		m_pGrid[CI(x,y)].pPatchCordLeft = pPatchCord;

		if(m_pGrid[CI(x,y)].bJoined)
		{
			m_pGrid[CI(x,y)].bEndLeft = TRUE;
			points.Add(center);
			return;
		}
		else if(m_pGrid[CI(x,y)].bRight)
		{
			m_pGrid[CI(x,y)].pPatchCordRight = pPatchCord;
			if((x+1 < m_gridWidth) && (m_pGrid[CI(x+1, y)].bLeft))
				TraceAndMarkPatchCord(x+1, y, pPatchCord, LEFT, points);
			else
				points.Add(center);
		}
		else if(m_pGrid[CI(x,y)].bTop)
		{
			m_pGrid[CI(x,y)].pPatchCordTop = pPatchCord;
			points.Add(center);
			if((y-1 >= 0) && (m_pGrid[CI(x, y-1)].bBottom))
				TraceAndMarkPatchCord(x, y-1, pPatchCord, BOTTOM, points);
		}
		else if(m_pGrid[CI(x,y)].bBottom)
		{
			m_pGrid[CI(x,y)].pPatchCordBottom = pPatchCord;
			points.Add(center);
			if((y+1 < m_gridHeight) && (m_pGrid[CI(x, y+1)].bTop))
				TraceAndMarkPatchCord(x, y+1, pPatchCord, TOP, points);
		}
		else
			points.Add(center);
		break;

	case CGraph::RIGHT:
		if(!m_pGrid[CI(x,y)].bRight)
			return;

		m_pGrid[CI(x,y)].pPatchCordRight = pPatchCord;

		if(m_pGrid[CI(x,y)].bJoined)
		{
			m_pGrid[CI(x,y)].bEndRight = TRUE;
			points.Add(center);
			return;
		}
		else if(m_pGrid[CI(x,y)].bLeft)
		{
			m_pGrid[CI(x,y)].pPatchCordLeft = pPatchCord;
			if((x-1 >= 0) && (m_pGrid[CI(x-1, y)].bRight))
				TraceAndMarkPatchCord(x-1, y, pPatchCord, RIGHT, points);
			else
				points.Add(center);
		}
		else if(m_pGrid[CI(x,y)].bTop)
		{
			m_pGrid[CI(x,y)].pPatchCordTop = pPatchCord;
			points.Add(center);
			if((y-1 >= 0) && (m_pGrid[CI(x, y-1)].bBottom))
				TraceAndMarkPatchCord(x, y-1, pPatchCord, BOTTOM, points);
		}
		else if(m_pGrid[CI(x,y)].bBottom)
		{
			m_pGrid[CI(x,y)].pPatchCordBottom = pPatchCord;
			points.Add(center);
			if((y+1 < m_gridHeight) && (m_pGrid[CI(x, y+1)].bTop))
				TraceAndMarkPatchCord(x, y+1, pPatchCord, TOP, points);
		}
		else
			points.Add(center);
		break;

	case CGraph::TOP:
		if(!m_pGrid[CI(x,y)].bTop)
			return;

		m_pGrid[CI(x,y)].pPatchCordTop = pPatchCord;

		if(m_pGrid[CI(x,y)].bJoined)
		{
			m_pGrid[CI(x,y)].bEndTop = TRUE;
			points.Add(center);
			return;
		}
		else if(m_pGrid[CI(x,y)].bBottom)
		{
			m_pGrid[CI(x,y)].pPatchCordBottom = pPatchCord;
			if((y+1 < m_gridHeight) && (m_pGrid[CI(x, y+1)].bTop))
				TraceAndMarkPatchCord(x, y+1, pPatchCord, TOP, points);
			else
				points.Add(center);
		}
		else if(m_pGrid[CI(x,y)].bLeft)
		{
			m_pGrid[CI(x,y)].pPatchCordLeft = pPatchCord;
			points.Add(center);
			if((x-1 >= 0) && (m_pGrid[CI(x-1, y)].bRight))
				TraceAndMarkPatchCord(x-1, y, pPatchCord, RIGHT, points);
		}
		else if(m_pGrid[CI(x,y)].bRight)
		{
			m_pGrid[CI(x,y)].pPatchCordRight = pPatchCord;
			points.Add(center);
			if((x+1 < m_gridWidth) && (m_pGrid[CI(x+1, y)].bLeft))
				TraceAndMarkPatchCord(x+1, y, pPatchCord, LEFT, points);
		}
		else
			points.Add(center);
		break;

	case CGraph::BOTTOM:
		if(!m_pGrid[CI(x,y)].bBottom)
			return;

		m_pGrid[CI(x,y)].pPatchCordBottom = pPatchCord;

		if(m_pGrid[CI(x,y)].bJoined)
		{
			m_pGrid[CI(x,y)].bEndBottom = TRUE;
			points.Add(center);
			return;
		}
		else if(m_pGrid[CI(x,y)].bTop)
		{
			m_pGrid[CI(x,y)].pPatchCordTop = pPatchCord;
			if((y-1 >= 0) && (m_pGrid[CI(x, y-1)].bBottom))
				TraceAndMarkPatchCord(x, y-1, pPatchCord, BOTTOM, points);
			else
				points.Add(center);
		}
		else if(m_pGrid[CI(x,y)].bLeft)
		{
			m_pGrid[CI(x,y)].pPatchCordLeft = pPatchCord;
			points.Add(center);
			if((x-1 >= 0) && (m_pGrid[CI(x-1, y)].bRight))
				TraceAndMarkPatchCord(x-1, y, pPatchCord, RIGHT, points);
		}
		else if(m_pGrid[CI(x,y)].bRight)
		{
			m_pGrid[CI(x,y)].pPatchCordRight = pPatchCord;
			points.Add(center);
			if((x+1 < m_gridWidth) && (m_pGrid[CI(x+1, y)].bLeft))
				TraceAndMarkPatchCord(x+1, y, pPatchCord, LEFT, points);
		}
		else
			points.Add(center);
		break;
	default:
		break;
	}
}

//------------------------------------------------------------------------------
//	CGraph::OnLButtonDown
//------------------------------------------------------------------------------
void
CGraph::OnLButtonDown(
					  IN UINT			nFlags,
					  IN const CPoint&	point
					  )
/*++

Routine Description:

	Left mouse down:
		Select Module
		Drag Module
		Draw Patch Cord
		Connect / Disconnect Patch Cord
		Drop Module

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	CModule*	pModule;
	CPatchCord*	pPatchCord;
	CJack*		pJack;
	CPoint		patchPoint;

	m_bLeftMouseDown = TRUE;

	// If any other button is down besides the left mouse, ignore
	if((nFlags & MK_MBUTTON) || (nFlags & MK_RBUTTON))
		return;

	// Set the mouse position
	SetMouse(point);

	// See what was hit
	pModule		= GetModule(m_mouse);
	pPatchCord	= GetPatchCord(m_mouse);
	if(pModule)
		pJack = pModule->GetJack(m_mouse, TRUE);
	else
		pJack = NULL;

	// Drawing a complex line?
	if(m_bDrawComplexLine)
	{
		DrawPatchLine();

		// If the last two points are within 2 pixels, merge them
		if((abs(m_patchLine.points[m_patchLine.points.GetSize()-1].x -
				m_patchLine.points[m_patchLine.points.GetSize()-2].x) <= 2) &&
		   (abs(m_patchLine.points[m_patchLine.points.GetSize()-1].y -
				m_patchLine.points[m_patchLine.points.GetSize()-2].y) <= 2))
		{
			m_patchLine.points.RemoveAt(m_patchLine.points.GetSize()-1);
		}

		// Was the click over a module?
		if(pModule)
		{
			m_bDrawComplexLine	= FALSE;
			m_bStartDrawLine	= FALSE;

			// Was a jack hit?
			if(pJack)
				NewPatchCord();
		}

		// Or was it over a patch cord?
		else if(pPatchCord)
		{
			m_bDrawComplexLine	= FALSE;
			m_bStartDrawLine	= FALSE;

			NewPatchCord();
		}

		// Otherwise, just keep adding points
		else
		{
			// Add one more points
			m_patchLine.points.Add(m_mouse);
			DrawPatchLine();
		}
	}

	// Insert an effect?
	else if(m_bInsertModules)
		DropModules();

	// Move a module?
	else if(pModule)
	{
		// Was a patch on the module selected?
		if(pJack)
		{
			// Clear the line
			m_patchLine.points.RemoveAll();

			if(pJack->GetInput())
			{
				m_patchLine.firstPatchEnd	= CGraph::PATCHEND_INPUT;
				patchPoint.x				= pModule->GetRect().left;
				patchPoint.y				= pJack->GetY();
			}
			else
			{
				m_patchLine.firstPatchEnd	= CGraph::PATCHEND_OUTPUT;
				patchPoint.x				= pModule->GetRect().right;
				patchPoint.y				= pJack->GetY();
			}

			m_patchLine.lastPatchEnd	= m_patchLine.firstPatchEnd;
			m_patchLine.pFirstJack		= pJack;
			m_patchLine.pLastJack		= pJack;
			m_patchLine.pFirstPatchCord	= NULL;
			m_patchLine.pLastPatchCord	= NULL;
			m_patchLine.points.Add(patchPoint);
			m_patchLine.points.Add(patchPoint);
			m_patchLine.points.Add(patchPoint);

			m_bDrawLine			= TRUE;
			m_bDrawComplexLine	= TRUE;
			m_bStartDrawLine	= TRUE;
			DrawPatchLine();
		}

		// Otherwise, just pick up and move the module
		else
		{
			// Make sure the list is empty
			m_moveModules.RemoveAll();
			m_originalModules.RemoveAll();

			// Set the state
			pModule->SetState(CModule::MOVING);

			// Save the original link
			m_originalModules.Add(pModule);

			// Copy the module
			pModule = new CModule(*pModule);

			// Set the state
			pModule->SetState(CModule::MOVING);

			// Add the module to the move list
			m_moveModules.Add(pModule);
			
			// Use offsets for positioning module
			m_bUseOffset	= TRUE;
			m_offset		= m_mouse - pModule->GetRect().TopLeft();

			CalcUpperLeft(pModule);
		}
	}

	else if(pPatchCord)
	{
		// Clear the line
		m_patchLine.points.RemoveAll();

		switch(pPatchCord->GetLastPointOn())
		{
		case CPatchCord::POINTON_HORIZONTAL:
			m_patchLine.firstPatchEnd = CGraph::PATCHEND_HORIZONTAL;
			break;
		case CPatchCord::POINTON_VERTICAL:
			m_patchLine.firstPatchEnd = CGraph::PATCHEND_VERTICAL;
			break;
		case CPatchCord::POINTON_POINT:
			m_patchLine.firstPatchEnd = CGraph::PATCHEND_POINT;
			break;
		case CPatchCord::POINTON_NONE:
		default:
			break;
		}
		patchPoint = pPatchCord->GetLastPoint();

		m_patchLine.lastPatchEnd	= m_patchLine.firstPatchEnd;
		m_patchLine.pFirstJack		= NULL;
		m_patchLine.pLastJack		= NULL;
		m_patchLine.pFirstPatchCord	= pPatchCord;
		m_patchLine.pLastPatchCord	= pPatchCord;
		m_patchLine.points.Add(patchPoint);
		m_patchLine.points.Add(patchPoint);
		m_patchLine.points.Add(patchPoint);

		m_bDrawLine			= TRUE;
		m_bDrawComplexLine	= TRUE;
		m_bStartDrawLine	= TRUE;
		DrawPatchLine();
	}
}

//------------------------------------------------------------------------------
//	CGraph::OnLButtonUp
//------------------------------------------------------------------------------
void
CGraph::OnLButtonUp(
				   IN UINT,
				   IN const CPoint&	point
				   )
/*++

Routine Description:

	Left mouse up:
		Drop module

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	int		i;
	CRect	rect(0,0,0,0);

	m_bLeftMouseDown = FALSE;
	SetMouse(point);

	// Beginning of line?
	if(m_bStartDrawLine)
	{
		m_bDrawLine			= FALSE;
		m_bStartDrawLine	= FALSE;
	}

	// Dragging a patch cord?
	else if(m_bDragPatch)
	{
		CClientDC	dc(m_pParent);
		m_pDragPatch->DrawXOR(&dc);
		m_bDragPatch = FALSE;
	}

	// Drawing a complex line?
	else if(m_bDrawComplexLine)
		return;

	// Drawing a patch cord?
	else if(m_bDrawLine)
	{
		// Erase the line
		DrawPatchLine();
		m_bDrawLine			= FALSE;
		m_bStartDrawLine	= FALSE;

		NewPatchCord();
	}

	// Inserting a module? (this is new)
	else if(m_bInsertModules)
		return;

	// Was a module being moved?
	else if(m_moveModules.GetSize())
	{
		// If the mouse is out of the window, dump the modules
		if(!IsPointInRect(m_mouse.x, m_mouse.y, m_rect))
		{
			for(i=0; i<m_moveModules.GetSize(); ++i)
			{
				rect.UnionRect(rect, m_moveModules[i]->GetRect());
				delete m_moveModules[i];
			}
			m_moveModules.RemoveAll();
			Draw(rect);
			return;
		}

		// If any of the states are invalid, just dump the modules
		for(i=0; i<m_moveModules.GetSize(); ++i)
		{
			if(m_moveModules[i]->GetState() == CModule::INVALID)
				break;
		}
		
		// Were there any invalid modules?
		if((i != m_moveModules.GetSize()) || (m_originalModules.GetSize() != m_moveModules.GetSize()))
		{
			for(i=0; i<m_moveModules.GetSize(); ++i)
			{
				rect.UnionRect(rect, m_moveModules[i]->GetRect());
				delete m_moveModules[i];
			}
			m_moveModules.RemoveAll();
			Draw(rect);

			// Reset all moving modules to OK
			for(i=0; i<m_modules.GetSize(); ++i)
			{
				if(m_modules[i]->GetState() == CModule::MOVING)
					m_modules[i]->SetState(CModule::OK);
			}
			return;
		}

		// Did the modules really move?
		for(i=0; i<m_moveModules.GetSize(); ++i)
		{
			if(m_moveModules[i]->GetRect() != m_originalModules[i]->GetRect())
				break;
		}

		// If they did not move, don't do anything
		if(i == m_moveModules.GetSize())
		{
			// Suspend updates
			PushFreeze();

			for(i=0; i<m_moveModules.GetSize(); ++i)
			{
				rect.UnionRect(rect, m_moveModules[i]->GetRect());
				rect.UnionRect(rect, m_originalModules[i]->GetRect());
				m_originalModules[i]->SetState(CModule::OK);
				delete m_moveModules[i];
			}
			m_moveModules.RemoveAll();
			m_originalModules.RemoveAll();

			// Resume updates
			PopFreeze();

			// Redraw
			Draw(rect);
			return;
		}

		for(i=0; i<m_originalModules.GetSize(); ++i)
			m_originalModules[i]->Disconnect();

		// Suspend updates
		PushFreeze();

		// Update the draw rect
		for(i=0; i<m_moveModules.GetSize(); ++i)
		{
			rect.UnionRect(rect, m_moveModules[i]->GetRect());
			rect.UnionRect(rect, m_originalModules[i]->GetRect());
		}

		// Delete all move modules
		for(i=0; i<m_moveModules.GetSize(); ++i)
		{
			m_originalModules[i]->SetState(CModule::OK);
			m_originalModules[i]->SetRect(m_moveModules[i]->GetRect());
			delete m_moveModules[i];
		}

		// Clear the originals list
		m_originalModules.RemoveAll();
		m_moveModules.RemoveAll();

		// Resume updates
		PopFreeze();

		// Redraw
		Draw(rect);

		m_bChangesMade		= TRUE;

		m_pParent->UpdateScrollBars();
	}
}

//------------------------------------------------------------------------------
//	CGraph::OnRButtonDown
//------------------------------------------------------------------------------
void
CGraph::OnRButtonDown(
					 IN UINT			nFlags,
					 IN const CPoint&	point
					 )
/*++

Routine Description:

	Right Mouse down: Popup menus

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	int						i;
	int						nCmd;
	CPoint					menuPoint	= point;
	CRect					rect(0, 0, 0, 0);
	CModule*				pModule;
	CPatchCord*				pPatchCord;
	CArray<CJack*, CJack*>	jackList;

	// If any other button is down besides left mouse or any key is down, ignore
	if((nFlags & MK_MBUTTON) || (nFlags & MK_LBUTTON) || (nFlags & MK_CONTROL) ||
	   (nFlags & MK_SHIFT))
		return;

	// If a complex line was being drawn cancel it
	CancelDrawComplexLine();

	// Convert the point
	m_pParent->ClientToScreen(&menuPoint);

	// Set the mouse position
	SetMouse(point);

	// See what was hit
	pModule		= GetModule(m_mouse);
	pPatchCord	= GetPatchCord(m_mouse);

#if 1
	if(m_bInsertModules)
		DropModules();
	else
#else	// The old way
	// If modules were being moved, cancel it
	if(m_moveModules.GetSize())
	{
		for(i=0; i<m_originalModules.GetSize(); ++i)
		{
			rect.UnionRect(rect, m_originalModules[i]->GetRect());
			m_originalModules[i]->SetState(CModule::OK);
		}
		for(i=0; i<m_moveModules.GetSize(); ++i)
		{
			rect.UnionRect(rect, m_moveModules[i]->GetRect());
			delete m_moveModules[i];
		}
		m_moveModules.RemoveAll();
		Draw(rect);
	}
#endif

	// Was a module hit?
	if(pModule)
	{
		if(m_bDeleteConfigMenu)
		{
			m_moduleMenu.DeleteMenu(2, MF_BYPOSITION);
			m_bDeleteConfigMenu = FALSE;
		}

		// Setup configurations
		if(pModule->GetNumConfigs() != 0)
		{
			m_configMenu.CreatePopupMenu();
			for(i=0; i<pModule->GetNumConfigs(); ++i)
				m_configMenu.AppendMenu(MF_STRING, (UINT)(-(i+1)), pModule->GetConfigs()[i].name);
			m_moduleMenu.InsertMenu(2, MF_BYPOSITION | MF_POPUP, (UINT)m_configMenu.Detach(), _T("&Configuration"));
			m_moduleMenu.EnableMenuItem(ID_EFFECTS_CONFIGS, MF_ENABLED);
			m_bDeleteConfigMenu = TRUE;
		}

		m_moduleMenu.EnableMenuItem(ID_EFFECTS_PROPERTIES, pModule->HasProperties() ? MF_ENABLED : MF_GRAYED);
		m_moduleMenu.EnableMenuItem(ID_EFFECTS_NAME, (pModule->GetMixbin() == -1) ? MF_ENABLED : MF_GRAYED);
		m_moduleMenu.EnableMenuItem(ID_EFFECTS_DISCONNECT_ALL, pModule->IsPatched() ? MF_ENABLED : MF_GRAYED);
		m_moduleMenu.EnableMenuItem(ID_EFFECTS_DISCONNECT_INPUTS, pModule->AreInputsPatched() ? MF_ENABLED : MF_GRAYED);
		m_moduleMenu.EnableMenuItem(ID_EFFECTS_DISCONNECT_OUTPUTS, pModule->AreOutputsPatched() ? MF_ENABLED : MF_GRAYED);
		nCmd = m_moduleMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | 
											TPM_RIGHTBUTTON | TPM_RETURNCMD |
											TPM_NONOTIFY,
											menuPoint.x, menuPoint.y, m_pParent);

		if(nCmd < 0)
		{
			if(pModule->GetActiveConfig() != -(nCmd+1))
			{
				m_bChangesMade		= TRUE;
				pModule->SetActiveConfig(-(nCmd+1));
				Draw(pModule->GetRect());
			}
		}
		else
		{
			switch(nCmd)
			{
			case ID_EFFECTS_PROPERTIES:
				pModule->ShowProperties();
				break;
			case ID_EFFECTS_NAME:
				pModule->SetEffectName();
				break;
			case ID_EFFECTS_DELETE:
				DeleteModule(pModule);
				break;
			case ID_EFFECTS_DISCONNECT_ALL:
				pModule->Disconnect();
				m_bChangesMade = TRUE;
				break;
			case ID_EFFECTS_DISCONNECT_INPUTS:
				pModule->DisconnectInputs();
				m_bChangesMade = TRUE;
				break;
			case ID_EFFECTS_DISCONNECT_OUTPUTS:
				pModule->DisconnectOutputs();
				m_bChangesMade = TRUE;
				break;
			default:
				break;
			}
		}
	}

	// Or was a patch cord hit?
	else if(pPatchCord)
	{
		m_patchCordMenu.EnableMenuItem(ID_PATCH_DISCONNECT, MF_ENABLED);
//		m_patchCordMenu.CheckMenuItem(ID_PATCH_OVERWRITE, pPatchCord->GetOverwrite() ? MF_CHECKED : MF_UNCHECKED);
//		m_patchCordMenu.EnableMenuItem(ID_PATCH_OVERWRITE, !pPatchCord->IsInvalid() && pPatchCord->GetOverwriteEnable() ? MF_ENABLED : MF_GRAYED);
		nCmd = m_patchCordMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | 
											  TPM_RIGHTBUTTON | TPM_RETURNCMD |
											  TPM_NONOTIFY,
											  menuPoint.x, menuPoint.y, m_pParent);
		if(nCmd == ID_PATCH_DISCONNECT)
		{
			DeletePatchCord(pPatchCord);
			m_bChangesMade = TRUE;
		}
//		else if(nCmd == ID_PATCH_OVERWRITE)
//		{
//			Draw(pPatchCord->SetOverwrite(!pPatchCord->GetOverwrite()));
//			m_bChangesMade = TRUE;
//		}
	}

	// Otherwise, popup the root menu
	else
	{
		// Setup the menu
		m_rootMenu.CheckMenuItem(ID_GRID_SHOW_GRID, 
								 m_bDrawGrid ? MF_CHECKED : MF_UNCHECKED);	
		m_rootMenu.EnableMenuItem(ID_GRID_DELETE_ALL, 
								  m_modules.GetSize() > 0 ? MF_ENABLED : MF_GRAYED);
		m_rootMenu.EnableMenuItem(ID_GRID_DISCONNECT_ALL, 
								  IsPatched() ? MF_ENABLED : MF_GRAYED);
		m_rootMenu.EnableMenuItem(ID_GRID_INSERT_EFFECT, MF_ENABLED);

		// Enable/Disable mixbin items
		EnableMixbinsOnMenu();

		// Popup the menu
		nCmd = m_rootMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | 
										 TPM_RIGHTBUTTON | TPM_RETURNCMD |
										 TPM_NONOTIFY,
										 menuPoint.x, menuPoint.y, m_pParent);

		// Show grid?
		if(nCmd == ID_GRID_SHOW_GRID)
			ToggleGrid();

		// Disconnect all patch cords?
		else if(nCmd == ID_GRID_DISCONNECT_ALL)
		{
			if(m_patchCords.GetSize())
			{
				for(i=m_patchCords.GetSize()-1; i>=0; --i)
				{
					rect.UnionRect(rect, m_patchCords[i]->GetRect());
					pPatchCord = m_patchCords[i];
					m_patchCords.RemoveAt(i);
					delete pPatchCord;
				}
				Draw(rect);
				m_bChangesMade = TRUE;
				Draw(m_rect);
			}
		}

		else if(nCmd == ID_GRID_DELETE_ALL)
		{
			if(m_modules.GetSize())
			{
				for(i=m_modules.GetSize()-1; i>=0; --i)
				{
					rect.UnionRect(rect, m_modules[i]->GetRect());
					pModule = m_modules[i];
					m_modules.RemoveAt(i);
					delete pModule;
				}
				SetInUse();
				m_bChangesMade = TRUE;
				Draw(m_rect);
			}
		}

		else
			InsertModule(point, nCmd);
	}
}

//------------------------------------------------------------------------------
//	CGraph::OnMouseMove
//------------------------------------------------------------------------------
void 
CGraph::OnMouseMove(
				   IN UINT,
				   IN const CPoint&	point
				   )
/*++

Routine Description:

	Moves a module, or draws a patch cord

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

Return Value:

	None

--*/
{
	int				i;
	CPoint			upperLeft;
	CModule*		pModule;
	CPatchCord*		pPatchCord;
	CJack*			pJack;
	CRect			rect(0, 0, 0, 0);
	CModule::State	state	= CModule::MOVING;
	int				dy		= m_bSnapToGrid ? SCALE : 3;

	// Set the mouse position
	SetMouse(point);
	
	// Has line drawing just began?
	if(m_bStartDrawLine)
	{
		if((abs(m_mouse.x - m_patchLine.points[0].x) > 2) ||
		   (abs(m_mouse.y - m_patchLine.points[0].y) > 2))
		{
			m_bStartDrawLine = FALSE;

			if(m_bLeftMouseDown)
				m_bDrawComplexLine = FALSE;
		}
	}

	// Drawing a line?
	if(m_bDrawLine || m_bDrawComplexLine)
	{
		// Clear the current line
		DrawPatchLine();

		// Where is the mouse?
		pModule		= GetModule(m_mouse);
		pPatchCord	= GetPatchCord(m_mouse);

		// Over a module?
		if(pModule)
		{
			// Over a jack?
			pJack = pModule->GetJack(m_mouse, FALSE);
			if(pJack)
				m_patchLine.lastPatchEnd = pJack->GetInput() ? CGraph::PATCHEND_INPUT : CGraph::PATCHEND_OUTPUT;
			else
				m_patchLine.lastPatchEnd = CGraph::PATCHEND_NONE;
			m_patchLine.pLastJack		= pJack;
			m_patchLine.pLastPatchCord	= NULL;
		}

		// Over a patch cord?
		else if(pPatchCord)
		{
			switch(pPatchCord->GetLastPointOn())
			{
			case CPatchCord::POINTON_HORIZONTAL:
				m_patchLine.lastPatchEnd = CGraph::PATCHEND_HORIZONTAL;
				break;
			case CPatchCord::POINTON_VERTICAL:
				m_patchLine.lastPatchEnd = CGraph::PATCHEND_VERTICAL;
				break;
			case CPatchCord::POINTON_POINT:
				m_patchLine.lastPatchEnd = CGraph::PATCHEND_POINT;
				break;
			case CPatchCord::POINTON_NONE:
			default:
				break;
			}
			m_patchLine.pLastJack		= NULL;
			m_patchLine.pLastPatchCord	= pPatchCord;
		}

		// Over nothing
		else
		{
			m_patchLine.lastPatchEnd	= CGraph::PATCHEND_NONE;
			m_patchLine.pLastJack		= NULL;
			m_patchLine.pLastPatchCord	= NULL;
		}

		UpdatePatchLine();
		DrawPatchLine();
	}

	// Drag a patch cord
	else if(m_bDragPatch)
	{
		// TODO:
	}

	// Moving modules?
	else if(m_moveModules.GetSize())
	{
		// Some calculations		
		CalcUpperLeft(m_moveModules[0]);
		upperLeft = m_upperLeft;
		
		for(i=0; i<m_moveModules.GetSize(); ++i)
		{
			// Generate the current invalidation rect
			rect.UnionRect(rect, m_moveModules[i]->GetRect());

			// Move the modules
			m_moveModules[i]->Move(upperLeft);

			// Generate the new invalidation rect
			rect.UnionRect(rect, m_moveModules[i]->GetRect());

			// Check the state - any invalid state will set all to invalid
			if(m_originalModules.GetSize())
				state = IsPlacementValid(m_moveModules[i], m_originalModules[i]) ? state : CModule::INVALID;
			else
				state = IsPlacementValid(m_moveModules[i]) ? state : CModule::INVALID;

			// Adjust the position
			upperLeft.y += m_moveModules[i]->GetRect().Height() + dy;
		}

		// Update the states
		for(i=0; i<m_moveModules.GetSize(); ++i)
			m_moveModules[i]->SetState(state);

		// Draw
		Draw(rect);
	}
}

//------------------------------------------------------------------------------
//	CGraph::OnLButtonDblClk
//------------------------------------------------------------------------------
void
CGraph::OnLButtonDblClk(
					    IN UINT,
					    IN const CPoint&
					    )
/*++

Routine Description:

	Left mouse double click:
		Terminate complex patch cord drawing

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	CancelDrawComplexLine();
}

//------------------------------------------------------------------------------
//	CGraph::DropModules
//------------------------------------------------------------------------------
void
CGraph::DropModules(void)
/*++

Routine Description:

	Drops all moving modules into the graph

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	if(m_moveModules.GetSize())
	{
		// Check for an invalid module
		for(i=0; i<m_moveModules.GetSize(); ++i)
		{
			if(m_moveModules[i]->GetState() == CModule::INVALID)
			{
				SetInUse();
				return;
			}
		}

		// Add the modules to the modules list
		for(i=0; i<m_moveModules.GetSize(); ++i)
		{
			m_moveModules[i]->SetState(CModule::OK);
			m_modules.Add(m_moveModules[i]);
		}
		SetInUse();

		// Clear the move list
		m_moveModules.RemoveAll();

		m_bChangesMade = TRUE;
	}

	// Set some flags
	m_bInsertModules = FALSE;
}

//------------------------------------------------------------------------------
//	CGraph::CancelDrawComplexLine
//------------------------------------------------------------------------------
void 
CGraph::CancelDrawComplexLine(void)
/*++

Routine Description:

	Cancels the drawing of a complex line

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_bDrawComplexLine)
	{
		DrawPatchLine();
		m_patchLine.points.RemoveAll();
		m_bDrawComplexLine	= FALSE;
		m_bStartDrawLine	= FALSE;
	}
}

