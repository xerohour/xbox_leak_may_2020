

#define HOST_NAME_LEN	255

class TSetupDialog
{
	HWND	m_hWnd;

  private:

	static TSetupDialog*	_this;

  public:

  	TCHAR	m_hostName [HOST_NAME_LEN];
	MPT		m_mpt;
	
  public:

	BOOL OnInitDialog ();

	BOOL OnCommand (WPARAM command);
	BOOL OnCommand_OK ();
	BOOL OnCommand_Cancel ();
	BOOL OnCommand_Help ();

	int DoModal (HWND hWndParent);

  private:

  	static BOOL APIENTRY
	DialogProc(
		HWND	hWnd,
		UINT	msg,
		WPARAM	wParam,
		LPARAM	lParam
		);
};

