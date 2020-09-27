#define N_COLUMN 4
#define CHAR_DIR "<DIR>"
#define ITEM_NONE -1

enum {
	MENU_FILE,
    MENU_EDIT,
	MENU_VIEW,
	MENU_HELP
};
enum {
	COLUMN_NAME,
	COLUMN_SIZE,
	COLUMN_TYPE,
	COLUMN_MODIFIED
};
extern char g_szCopyDir[MAX_PATH];

class CFileContainer;
class LISTVIEW
{
private:
	HWND hListWnd;
	TIME_ZONE_INFORMATION g_tzi;
	BOOL fHaveTimeZone;
	BOOL sortreverse;
	DWORD viewmode;
	int columnSize[N_COLUMN];
	char cszColumn[N_COLUMN][100];

    bool PopulateWithSelected(CFileContainer *pfc);
	BOOL InsertFiles( int = COLUMN_NAME, BOOL = FALSE );

    CDropTarget *m_pdroptarget;
    int m_nDragHighlightItem;
    bool m_fCut;
    
public:
    bool BeginDrag();
	LISTVIEW();
	~LISTVIEW();
	BOOL FileTime2LocalTime( LPFILETIME, LPSYSTEMTIME );
	HWND GetHwnd();
	void Focus();
	HWND CreateListView( HWND, LPRECT = NULL );
	HWND RedrawListView( HWND, LPRECT = NULL );
	bool SortListView( int, BOOL );
	void ChangeListView( DWORD );
	void CheckFiles( int*, int*, int* );
	BOOL CheckSelectedFiles( int*, int*, int* );
	BOOL IsParentFolder( int );
	BOOL IsFolder( int );
	BOOL IsSelected( int );
	int IsExistence( char* );
	void ChangeView( DWORD = 0 );
	DWORD GetViewMode();
    int GetItemAtPoint(POINT *ppt);
    bool SetDragHighlight(bool fEnable, POINT *ppt);
    bool GetDragItemName(char szName[MAX_PATH]);
    void CutCopy(bool fCut);
    void Paste();
    void Delete(HWND hwndOwner);
    int GetNumSelected();
    void Clear();
    void ClearSelection();
    int m_nLastSort;
};


int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);