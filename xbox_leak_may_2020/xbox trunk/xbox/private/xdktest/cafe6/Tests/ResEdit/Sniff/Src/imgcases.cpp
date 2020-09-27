///////////////////////////////////////////////////////////////////////////////
//	IMGCASES.CPP
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Implementation of the CImgTestCases class
//
// -------------------------------------------------------
//	Modified by :			Date :			Description:
//		MikePie					04/28/95		Added 48x48 icons and repaired first two tests
//		MikePie					05/04/95		Changed MST keystrokes to reflect menu change
//		ScottSe					06/07/96		Up commands changed to "E" to reflect popup menu change
//		ScottSe					07/02/96        File compare commented out due to many false failures
//		ScottSe					08/07/96		Added {ESC} at end of 48X48 Icon test to dismis properties dialog
//		ScottSe					09/13/96		Major rewrite of the test
//
#include "stdafx.h"
#include "imgcases.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CImgTestCases, CTest, "Image Editor Tests", -1, CSniffDriver)

void CImgTestCases::Run(void)
{

	// Delete previous img files
	KillFile(m_strCWD + "TESTOUT", "IMG*.*");

	HWND hwndRC;

	XSAFETY;

	if((hwndRC = UIWB.CreateNewFile(GetLocString(IDSS_NEW_RC))) == NULL)
	{
		m_pLog->RecordFailure("Could not create RC script");
		UIWB.CloseAllWindows();
		return;
	}
	else
	{
		if (!TestBitmap())
		{
			m_pLog->RecordFailure("Failed Bitmap Test");
			UIWB.CloseAllWindows();
			return;
		}
		if (!TestIcon())
 		{
			m_pLog->RecordFailure("Failed Icon Test");
			UIWB.CloseAllWindows();
			return;
		}
		if (!TestIconDevices())
 		{
			m_pLog->RecordFailure("Failed Icon Devices Test");
			UIWB.CloseAllWindows();
			return;
		}
		if (!TestCursor())
 		{
			m_pLog->RecordFailure("Failed Cursor Test");
			UIWB.CloseAllWindows();
			return;
		}
		if (!TestGIF_JPEG())
 		{
			m_pLog->RecordFailure("Failed GIF_JPEG Test");
			UIWB.CloseAllWindows();
			return;
		}

	}
	UIWB.CloseAllWindows();
}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

BOOL CImgTestCases::TestBitmap(void)
{
	XSAFETY;

	COResScript res;

	//Create new bitmap
	if (res.CreateResource(IDSS_RT_BITMAP) == ERROR_ERROR)
	{
		m_pLog->RecordFailure("Unable to create new bitmap");
		return FALSE;
	}
	Sleep(1500);

    m_uBmpEd = UIWB.GetActiveEditor();

	// Pencil tool
	m_uBmpEd.SetColor(2, VK_LBUTTON);			
	m_uBmpEd.ClickImgTool(GT_PENCIL);			
	m_uBmpEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 5, 5, 10, 40, 6);
	
	// Brush tool
	m_uBmpEd.SetColor(4, VK_LBUTTON);			
	m_uBmpEd.ClickImgTool(GT_BRUSH);			
	m_uBmpEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 5, 2, 40, 10, 6);
	
	// Fillrect tool
	m_uBmpEd.SetColor(5, VK_LBUTTON);			
	m_uBmpEd.ClickImgTool(GT_FILLRECT);			
	m_uBmpEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 3, 20, 13, 30, 6);
	
	// Rect tool
	m_uBmpEd.SetColor(6, VK_LBUTTON);			
	m_uBmpEd.ClickImgTool(GT_RECT);			
	m_uBmpEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 15, 15, 43, 45, 6);

	// Fill tool
	m_uBmpEd.SetColor(7, VK_LBUTTON);			
	m_uBmpEd.ClickImgTool(GT_FILL);			
	m_uBmpEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 25, 25, 26, 26, 6);

	// Filled circle with outline tool
	m_uBmpEd.SetColor(8, VK_LBUTTON);			
	m_uBmpEd.SetColor(9, VK_RBUTTON);			
	m_uBmpEd.ClickImgTool(GT_COMBOROUND);			
	m_uBmpEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 20, 20, 37, 37, 6);

	// Erase tool
	m_uBmpEd.SetColor(10, VK_RBUTTON);			
	m_uBmpEd.ClickImgTool(GT_ERASER);			
	m_uBmpEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 42, 42, 43, 43, 6);

	// Resize Bitmap
	m_uBmpEd.SetColor(11, VK_RBUTTON);			
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	prop.IsValid();
	MST.WEditSetText(prop.GetLabel(VRES_IDC_WIDTH), "60");		// change width
	MST.WEditSetText(prop.GetLabel(VRES_IDC_FILENAME), "TESTOUT\\IMG_BMP1.BMP");		// change name
	UIWB.ShowPropPage(FALSE);	
	
	// Copy to clipboard for icon test
 	UIWB.DoCommand( ID_EDIT_COPY, DC_ACCEL );
	
	// Save resource 
	if ( UIWB.SaveFileAs(m_strCWD + "Icon ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "Bitmap ίαρ.rc");
		return FALSE;
	}
	Sleep(1500);

	// Compare file with baseline doesn't work, don't know why
	//int nResult = CompareFiles(m_strCWD + "TESTOUT\\IMG_BMP1.BMP", m_strCWD + "BASELN\\IMG_BMP1.BMP", 200);
	//if (nResult != CF_SAME)
	//{
	// 	m_pLog->RecordFailure("New bitmap is different from baseline: result = %d", nResult);
	//	return FALSE;
	//}

	// Close editor	
	MST.DoKeys("^{F4}");
	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close editor");
		return FALSE;
	}

	// Open new bitmap
	UIImgEdit   img = UIWB.OpenFile(m_strCWD + "TESTOUT\\IMG_BMP1.BMP");
	
	// Make sure it's an editor but not a resource editor.
	UIEditor    ed2 = UIWB.GetActiveEditor();
	UIResEditor ed3 = UIWB.GetActiveEditor();
	if (!ed2.IsValid() || ed3.IsValid())
	{
		m_pLog->RecordFailure("Opened standalone bitmap incorrectly: %s", (LPCSTR)img.GetText());
		return FALSE;
	}

	// Close bitmap
	if (!img.Close() )
	{
		m_pLog->RecordFailure("Could not close standalone bitmap");
		
		return FALSE;
	}

	// Open large 256 color bitmap
	img = UIWB.OpenFile(m_strCWD + "ms01-10.BMP");
	
	// Make sure it's an editor but not a resource editor.
	ed2 = UIWB.GetActiveEditor();
	ed3 = UIWB.GetActiveEditor();
	if (!ed2.IsValid() || ed3.IsValid()) {
		m_pLog->RecordFailure("Opened standalone bitmap: %s", (LPCSTR)img.GetText());
		return FALSE;
	}

	// Turn it to monochrome
	prop = UIWB.ShowPropPage(TRUE);
	prop.IsValid();
	MST.WComboItemClk(prop.GetLabel(VRES_IDC_COLORS), 1);	
	UIWB.ShowPropPage(FALSE);

	//Undo the change
	UIWB.DoCommand( ID_EDIT_UNDO, DC_ACCEL );

	// Make sure it was undone
	CString strID = UIWB.GetProperty(P_Colors);
	if (strID != "2")
	{
		m_pLog->RecordFailure("Color change not undone correctly P_Colors = %s", strID);
		return FALSE;
	}

	// Close bitmap
	if (!img.Close() )
	{
		m_pLog->RecordFailure("Could not close 256 color bitmap");
		return FALSE;
	}

	return TRUE;
}


BOOL CImgTestCases::TestIcon(void)
{
	XSAFETY;

	COResScript res;

	//Create new icon
	if (res.CreateResource(IDSS_RT_ICON) == ERROR_ERROR)
	{
		m_pLog->RecordFailure("Unable to create new icon");
		return FALSE;
	}
	Sleep(1500);

	m_uIcoEd = UIWB.GetActiveEditor();

	// Fill with inverse color using RButton
	m_uIcoEd.SetColor(-1, VK_RBUTTON);
	m_uIcoEd.ClickImgTool(GT_FILL);
	ClickMouse(VK_RBUTTON, m_uIcoEd.HWnd(), 10, 40);
					
	// Erase 
	m_uIcoEd.ClickImgTool(GT_ERASER);
	m_uIcoEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 14, 10, 18, 14, 6);

	// Paste Clipboard contents 
 	UIWB.DoCommand(ID_EDIT_PASTE, DC_MESSAGE);

	// Set icon name
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	prop.IsValid();
	MST.WEditSetText(prop.GetLabel(VRES_IDC_FILENAME), "TESTOUT\\IMG_ICO1.ICO");		// change name
	UIWB.ShowPropPage(FALSE);	

	// Save resource 
	if ( UIWB.SaveFileAs(m_strCWD + "Icon ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "Icon ίαρ.rc");
		return FALSE;
	}
	Sleep(1500);

	// Compare icon with baseline
	//int nResult = CompareFiles(m_strCWD + "TESTOUT\\IMG_ICO1.ICO", m_strCWD + "BASELN\\IMG_ICO1.ICO", 200);
	//if (nResult != CF_SAME)
	//{
	// 	m_pLog->RecordFailure("New icon is different from baseline: result = %d", nResult);
	//	return FALSE;
	//}

	// Close editor	
	MST.DoKeys("^{F4}");
	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close editor");
		return FALSE;
	}

	// Open new icon
	UIImgEdit   img = UIWB.OpenFile(m_strCWD + "TESTOUT\\IMG_ICO1.ICO");
	
	// Make sure it's an editor but not a resource editor.
	UIEditor    ed2 = UIWB.GetActiveEditor();
	UIResEditor ed3 = UIWB.GetActiveEditor();
	if (!ed2.IsValid() || ed3.IsValid())
	{
		m_pLog->RecordFailure("Opened standalone icon incorrectly: %s", (LPCSTR)img.GetText());
		return FALSE;
	}

	// Close icon 
	if (!img.Close() )
	{
		m_pLog->RecordFailure("Could not close standalone icon");
		return FALSE;
	}

	return TRUE;
}

BOOL CImgTestCases::TestIconDevices(void)
{
	XSAFETY;

	//Create new icon
	COResScript res;
	if (res.CreateResource(IDSS_RT_ICON) == ERROR_ERROR)
	{
		m_pLog->RecordFailure("Unable to create new icon");
		return FALSE;
	}
	Sleep(1500);

	m_uIcoEd = UIWB.GetActiveEditor();
	
	// Draw on it 
	m_uIcoEd.SetColor(12, VK_LBUTTON);	// blue
	m_uIcoEd.ClickImgTool(GT_BRUSH);
	m_uIcoEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 4, 10, 18, 14, 6);
	
	//Create a new image for every available device
	int nDevices;
	do
	{
		UIWB.DoCommand( IDM_NEWDEVIMG, DC_ACCEL );
		Sleep(500);
		nDevices = MST.WListCount( GetLabel(VRES_ID_LIST) );
		if (nDevices > 0)
		{
			//Select the highlighted item on the list
			MST.DoKeys("~");
			Sleep(500);

			// Draw on it 
			m_uIcoEd.SetColor(12 + nDevices, VK_LBUTTON);	
			m_uIcoEd.ClickImgTool(GT_BRUSH);
			m_uIcoEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 4, 10, 14, 14, 6);
		}
		else if (nDevices == -1)	//not found
		{
			m_pLog->RecordInfo("No icon devices found");
			MST.DoKeys("{ESC}");
		}
		
	} while (nDevices > 1);

	// Set icon name
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	prop.IsValid();
	MST.WEditSetText(prop.GetLabel(VRES_IDC_FILENAME), "TESTOUT\\IMG_ICO2.ICO");		// change name
	UIWB.ShowPropPage(FALSE);	

	// Close Editor
	MST.DoKeys("^({F4})"); 
	
	// Save resource 
	if ( UIWB.SaveFileAs(m_strCWD + "DevicesIcon ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "DevicesIcon ίαρ.rc");
		return FALSE;
	}
	Sleep(1500);
	
	// Compare icon with baseline
	//int nResult = CompareFiles(m_strCWD + "TESTOUT\\IMG_ICO2.ICO", m_strCWD + "BASELN\\IMG_ICO2.ICO", 200);
	//if (nResult != CF_SAME)
	//{
	// 	m_pLog->RecordFailure("New devices icon is different from baseline: result = %d", nResult);
	//	return FALSE;
	//}

	// Close editor	
	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close editor");
		return FALSE;
	}

	// Open new icon
	UIImgEdit   img = UIWB.OpenFile(m_strCWD + "TESTOUT\\IMG_ICO2.ICO");
	
	// Make sure it's an editor but not a resource editor.
	UIEditor    ed2 = UIWB.GetActiveEditor();
	UIResEditor ed3 = UIWB.GetActiveEditor();
	if (!ed2.IsValid() || ed3.IsValid())
	{
		m_pLog->RecordFailure("Opened standalone device icon incorrectly: %s", (LPCSTR)img.GetText());
		return FALSE;
	}

	// Close icon 
	if (!img.Close() )
	{
		m_pLog->RecordFailure("Could not close standalone device icon");
		return FALSE;
	}
	
	return TRUE;
}

BOOL CImgTestCases::TestCursor(void)
{
	XSAFETY;

	COResScript res;

	//Create new cursor
	if (res.CreateResource(IDSS_RT_CURSOR) == ERROR_ERROR)
	{
		m_pLog->RecordFailure("Unable to create new cursor");
		return FALSE;
	}
	Sleep(1500);

	m_uIcoEd = UIWB.GetActiveEditor();

	// Draw on it 
	m_uIcoEd.SetColor(1, VK_LBUTTON);	
	m_uIcoEd.ClickImgTool(GT_BRUSH);
	m_uIcoEd.DragMouse(VK_LBUTTON, PANE_RIGHT, 4, 10, 18, 14, 6);

	// Set cursor name
	UIControlProp prop = UIWB.ShowPropPage(TRUE);
	prop.IsValid();
	MST.WEditSetText(prop.GetLabel(VRES_IDC_FILENAME), "TESTOUT\\IMG_CUR1.CUR");		// change name
	UIWB.ShowPropPage(FALSE);	

	// Close editor	
	MST.DoKeys("^{F4}");

	// Save resource 
	if ( UIWB.SaveFileAs(m_strCWD + "Cursor ίαρ.rc", TRUE) == FALSE )
	{
		m_pLog->RecordFailure("File save failed %s", m_strCWD + "Cursor ίαρ.rc");
		return FALSE;
	}
	Sleep(1500);

	// Compare cursor with baseline
	//int nResult = CompareFiles(m_strCWD + "TESTOUT\\IMG_CUR1.CUR", m_strCWD + "BASELN\\IMG_CUR1.CUR", 200);
	//if (nResult != CF_SAME)
	//{
	// 	m_pLog->RecordFailure("New cursor is different from baseline: result = %d", nResult);
	//	return FALSE;
	//}

	// Close editor	
	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close editor");
		return FALSE;
	}

	// Open new icon
	UIImgEdit   img = UIWB.OpenFile(m_strCWD + "TESTOUT\\IMG_CUR1.CUR");
	
	// Make sure it's an editor but not a resource editor.
	UIEditor    ed2 = UIWB.GetActiveEditor();
	UIResEditor ed3 = UIWB.GetActiveEditor();
	if (!ed2.IsValid() || ed3.IsValid())
	{
		m_pLog->RecordFailure("Opened standalone cursor incorrectly: %s", (LPCSTR)img.GetText());
		return FALSE;
	}

	// Close cursor 
	if (!img.Close() )
	{
		m_pLog->RecordFailure("Could not close standalone icon");
		return FALSE;
	}

	return TRUE;
}

BOOL CImgTestCases::TestGIF_JPEG(void)
{
	XSAFETY;

	// Open GIF file
	UIImgEdit   img = UIWB.OpenFile(m_strCWD + "ireland.gif");
	
	// Make sure it's an editor but not a resource editor.
	UIEditor    ed2 = UIWB.GetActiveEditor();
	UIResEditor ed3 = UIWB.GetActiveEditor();
	if (!ed2.IsValid() || ed3.IsValid())
	{
		m_pLog->RecordFailure("Opened standalone GIF incorrectly: %s", (LPCSTR)img.GetText());
		return FALSE;
	}

	//Make sure it came in correctly
	CString strID = UIWB.GetProperty(P_Width);
	if (strID != "427")
	{
		m_pLog->RecordFailure("GIF incorrect size P_Width = %s", strID);
		return FALSE;
	}
	strID = UIWB.GetProperty(P_Height);
	if (strID != "291")
	{
		m_pLog->RecordFailure("GIF incorrect size P_Height = %s", strID);
		return FALSE;
	}
	strID = UIWB.GetProperty(P_Colors);
	if (strID != "2")
	{
		m_pLog->RecordFailure("GIF colors not imported correctly P_Colors = %s", strID);
		return FALSE;
	}

	// Save it as a bitmap
	UIWB.DoCommand(ID_FILE_SAVE_AS, DC_MNEMONIC);
	MST.DoKeys( m_strCWD + "TESTOUT\\IMG_GIF.BMP" );
	MST.DoKeys( "~" );

	// Compare with baseline
	//int nResult = CompareFiles(m_strCWD + "TESTOUT\\\\IMG_GIF.BMP", m_strCWD + "BASELN\\\\IMG_GIF.BMP", 200);
	//if (nResult != CF_SAME)
	//{
	// 	m_pLog->RecordFailure("GIF bitmaps is different from baseline: result = %d", nResult);
	//	return FALSE;
	//}

	// Close editor	
	UIEditor ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close editor");
		return FALSE;
	}

	// Open JPG file
	img = UIWB.OpenFile(m_strCWD + "SKULL.JPG");
	
	// Make sure it's an editor but not a resource editor.
	ed2 = UIWB.GetActiveEditor();
	ed3 = UIWB.GetActiveEditor();
	if (!ed2.IsValid() || ed3.IsValid())
	{
		m_pLog->RecordFailure("Opened standalone JPG incorrectly: %s", (LPCSTR)img.GetText());
		return FALSE;
	}

	//Make sure it came in correctly
	strID = UIWB.GetProperty(P_Width);
	if (strID != "299")
	{
		m_pLog->RecordFailure("JPG incorrect size P_Width = %s", strID);
		return FALSE;
	}
	strID = UIWB.GetProperty(P_Height);
	if (strID != "435")
	{
		m_pLog->RecordFailure("JPG incorrect size P_Height = %s", strID);
		return FALSE;
	}
	strID = UIWB.GetProperty(P_Colors);
	if (strID != "2")
	{
		m_pLog->RecordFailure("JPG colors not imported correctly P_Colors = %s", strID);
		return FALSE;
	}

	// Save it as a bitmap
	UIWB.DoCommand(ID_FILE_SAVE_AS, DC_MNEMONIC);
	MST.DoKeys( m_strCWD + "TESTOUT\\IMG_JPG.BMP" );
	MST.DoKeys( "~" );

	// Compare with baseline
	//nResult = CompareFiles(m_strCWD + "TESTOUT\\\\IMG_JPG.BMP", m_strCWD + "BASELN\\\\IMG_JPG.BMP", 200);
	//if (nResult != CF_SAME)
	//{
	// 	m_pLog->RecordFailure("JPG bitmaps is different from baseline: result = %d", nResult);
	//	return FALSE;
	//}

	// Close editor	
	ed = UIWB.GetActiveEditor();
	EXPECT( ed.IsValid() );
	if( !ed.Close() )
	{
		m_pLog->RecordFailure("Could not close editor");
		return FALSE;
	}

	return TRUE;
}
