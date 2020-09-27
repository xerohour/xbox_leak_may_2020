#ifndef __TOOLVIEW_H__
#define __TOOLVIEW_H__

#undef FOURBUTTONS

#ifdef FOURBUTTONS
#define N_TOOLS 4
#else
#define N_TOOLS 2
#endif
#define N_DRIVE 26
#define ID_COMBOBOX 101
#define ID_REBAR 102

class TOOLVIEW
{
private:
	HWND hWndCB, hWndTB;
	HWND hToolWnd;
	void CreateToolBar( HWND );
	void CreateComboBox( HWND );
	int InsertDriveNameToComboBox();

	RECT toolRect;
	static TBBUTTON tbb[N_TOOLS];
	const static TBBUTTON tb;
	const static char *drivename[N_DRIVE];

public:
	void InsertToolBar();
	void InsertComboBox();
	TOOLVIEW();
	~TOOLVIEW();
	HWND GethWnd();
	HWND CreateToolView( HWND );
	HWND RedrawToolView( HWND );
    void Refresh();
    void Clear();
};

#endif
