/*===========================================================================*\
 | 
 |  FILE:	Dialog.cpp
 |			Skeleton project and code for a plugin ColorPicker
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 27-1-99
 | 
\*===========================================================================*/


#include "ColorPicker.h"


#define RANDOM( a )		( ( (float)rand()/(float)RAND_MAX)*(a) )



// Redraw the color display frame on my UI
// This is purely skeleton-specific
void RedrawColorBox(HWND hWnd)
{
	HWND item = GetDlgItem(hWnd,IDC_COLBOX);
	Rect rect;
	GetClientRect(item,&rect);
	InvalidateRect (hWnd, &(RECT)rect, TRUE);
	UpdateWindow(hWnd);
}



/*===========================================================================*\
 |	The main dialog handler for our color picker
\*===========================================================================*/

BOOL CALLBACK SkelDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	SkeletonCPDialog *cp = (SkeletonCPDialog*)GetWindowLong(hWnd,GWL_USERDATA);
	if (!cp && msg!=WM_INITDIALOG) return FALSE;

    switch (msg)
	    {

		case WM_INITDIALOG:
			{
				// Update the class pointer
				cp = (SkeletonCPDialog*)lParam;
				SetWindowLong(hWnd,GWL_USERDATA,lParam);

				// Remove the black frame, and replace it with an ownerdrawn style
				LONG style = GetWindowLong(GetDlgItem(hWnd,IDC_COLBOX),GWL_STYLE);
				style &= ~(SS_BLACKFRAME);
				style |= SS_OWNERDRAW;
				SetWindowLong(GetDlgItem(hWnd,IDC_COLBOX),GWL_STYLE,style);

				// Reposition and rename the dialog
				SetWindowPos(hWnd,NULL,cp->initPos.x,cp->initPos.y,0,0,SWP_NOSIZE|SWP_NOZORDER);
				SetWindowText(hWnd,cp->colName);

				// Set default arrow cursor
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			
				// Show the window
				ShowWindow(GetDlgItem(hWnd,IDC_ADDCOLOR),cp->objColor);

				RedrawColorBox(hWnd);
				cp->hwPanel = hWnd;
			break;}



		// Repaint the dialog
		case WM_PAINT:  cp->DoPaint(hWnd);	break;



		case WM_COMMAND:
			switch (LOWORD(wParam)) {

				// Randomize the current color
				// I wanted the skeleton to do 'something' with it :)
				case IDC_RANDOM:
					cp->currentRGB = RGB(RANDOM(255),RANDOM(255),RANDOM(255));
					if (cp->callback)
						cp->callback->ColorChanged(cp->currentRGB,1);
					RedrawColorBox(hWnd);
					break;

				// If we are in 'Add Custom Color' mode, send the current
				// color back as the chosen color to add to the list
				case IDC_ADDCOLOR:
					SendMessage(GetParent(hWnd), WM_ADD_COLOR, (WPARAM)cp->currentRGB, 0);
					RedrawColorBox(hWnd);
				break;

				// Reset the current color to the original
				case IDC_RESET:
					cp->currentRGB = cp->origRGB;
					if (cp->callback)
						cp->callback->ColorChanged(cp->currentRGB,1);
					RedrawColorBox(hWnd);
					break;

				// Close this colorpicker
				case IDC_CLOSE:
					SendMessage(hWnd,WM_CLOSE,0,0);
					break;
			}
			break;


		// On a move operation, save the location
		case WM_SIZE:
		case WM_MOVE:
			{
				RECT r;
				GetWindowRect(hWnd,&r);
				cp->curPos.x = cp->initPos.x = r.left; 
				cp->curPos.y = cp->initPos.y = r.top; 
			break;}
		

		// Shutdown the color picker, return final values
		case WM_CLOSE:
		case WM_DESTROY:
			{
				RECT r;
				GetWindowRect(hWnd,&r);
				cp->curPos.x = r.left; 
				cp->curPos.y = r.top; 

				if (cp->callback) 
					cp->callback->BeingDestroyed(cp->curPos);

				SetWindowLong(hWnd, GWL_USERDATA, LONG(0));

				if(cp->Modeless) 
				{
					delete cp;
					cp=NULL;
					DestroyWindow(hWnd);
				}
				else EndDialog(hWnd,0);

			break;}

	    default:
	        return FALSE;
	    }
    return TRUE;
}



/*===========================================================================*\
 |	Paint a color swatch onto the dialog surface
\*===========================================================================*/

void SkeletonCPDialog::DoPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HPEN Pen; HBRUSH Brush; HWND item;

	BeginPaint (hWnd,  &ps);

	item = GetDlgItem(hWnd,IDC_COLBOX);
	Rect rect;
	GetClientRect(item,&rect);
	HDC hdc = GetDC(item);

	Rect tR = rect;
	tR.right--;
	tR.bottom--;
	Rect3D(hdc,tR,FALSE);


	// Original Color
	Pen = CreatePen( PS_SOLID , 1 , (COLORREF)origRGB );
	Brush = CreateSolidBrush((COLORREF)origRGB);
	SelectObject(hdc,Pen);
	SelectObject(hdc,Brush);

	Rectangle( hdc, 1,1,(rect.w()-2)/2,rect.h()-2);

	DeleteObject(Pen);
	DeleteObject(Brush);

	// Current Color
	Pen = CreatePen( PS_SOLID , 1 , (COLORREF)currentRGB );
	Brush = CreateSolidBrush((COLORREF)currentRGB);
	SelectObject(hdc,Pen);
	SelectObject(hdc,Brush);

	Rectangle( hdc, (rect.w()-2)/2,1,rect.w()-2,rect.h()-2);

	DeleteObject(Pen);
	DeleteObject(Brush);



	ReleaseDC(item,hdc);
	EndPaint (hWnd, &ps);
}



/*===========================================================================*\
 |	Different ways to run the dialog
\*===========================================================================*/

int SkeletonCPDialog::StartModal()
{
	Modeless = FALSE;
	return (int)DialogBoxParam(
		hInstance, 
		MAKEINTRESOURCE(IDD_COLPICK_SKEL), 
		hwOwner, 
		SkelDlgProc,
		(LPARAM)this);
}

HWND SkeletonCPDialog::StartModeless()
{
	Modeless = TRUE;
	return (HWND)CreateDialogParam(
		hInstance, 
		MAKEINTRESOURCE(IDD_COLPICK_SKEL), 
		hwOwner, 
		SkelDlgProc,
		(LPARAM)this);
}


/*===========================================================================*\
 |	Dialog class handler constructor/destructor
\*===========================================================================*/

SkeletonCPDialog::SkeletonCPDialog(HWND hOwner, DWORD col,  IPoint2* pos,
								HSVCallback *cb, TCHAR *name, int objClr, SkeletonModeless *smp)
{
	hwOwner = hOwner;
	origRGB = currentRGB = col;
	if(pos) curPos = initPos = *pos;
	callback = cb;
	colName = name;
	Modeless = FALSE;
	hwPanel = NULL;
	objColor = objClr?TRUE:FALSE;
	sm = smp;
}

SkeletonCPDialog::~SkeletonCPDialog()
{
	if(hwPanel) hwPanel = NULL;
}



/*===========================================================================*\
 |	Various information callbacks
\*===========================================================================*/

void SkeletonCPDialog::SetNewColor (DWORD color, TCHAR *name)
{
	colName = name;
	origRGB = currentRGB = color;
	SetWindowText(hwPanel,colName);
	InvalidateRect(hwPanel,NULL,FALSE);
}

void SkeletonCPDialog::ModifyColor (DWORD color)
{
	currentRGB=color;
	InvalidateRect(hwPanel,NULL,FALSE);
}

DWORD SkeletonCPDialog::GetColor()
{
	return currentRGB;
}

IPoint2 SkeletonCPDialog::GetPosition()
{
	return curPos;
}

void SkeletonCPDialog::InstallNewCB(DWORD col, HSVCallback *pcb, TCHAR *name)
{
	if (callback) 
		callback->ColorChanged(currentRGB,1);
	callback = pcb;
    currentRGB = origRGB = col;
	colName = name;
	SetWindowText(hwPanel,colName);
	InvalidateRect(hwPanel,NULL,FALSE);
}




