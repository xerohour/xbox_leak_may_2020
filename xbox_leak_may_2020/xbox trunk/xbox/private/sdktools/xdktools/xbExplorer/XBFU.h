//
//
// Xbox Target Box file utility
//
// 2000/12/8	YuichM		Release alpha version
// 2000/12/19-	Shinjich
//
//

#pragma warning(disable:4786)
#include <map>

using namespace std;

typedef map<HANDLE, PDM_WALK_DIR > MAPFINDFILE;

class XBFU
{
public:
	XBFU();
	~XBFU();

	HRESULT GetTarget( LPSTR, UINT );						// ターゲット名の取得
	HRESULT SetTarget( LPCSTR );							// ターゲット名の設定
	bool IsDrive( CHAR );									// ドライブが存在するかのチェック（a...z) YES=TRUE

	HANDLE FindFirstFile( LPCSTR, PDM_FILE_ATTRIBUTES, bool fTargetXbox, LPCSTR = NULL );	// ファイル検索
	bool FindNextFile( HANDLE, PDM_FILE_ATTRIBUTES, bool fTargetXbox );
	bool FindClose( HANDLE, bool fTargetXbox );

    HRESULT CopyMoveToXbox(LPCSTR szSrcPathFile, LPCSTR szDestPathFile, bool *pfQuery,
						   bool fDestXbox, bool fSrcXbox, bool fRecursive, bool fMove, bool *pfSkipped = NULL);

    bool Del(LPCSTR szPathFileName, bool *pfQuery, HWND hWnd);
	bool Md( LPCSTR );										// ディレクトリ作成
	bool Ren( LPCSTR, LPCSTR );								// ファイル名変更

	HRESULT Reboot( bool = FALSE, LPCSTR = NULL );			// ターゲットをリブートする
    HRESULT GetFileAttributes(char *szFileName, DM_FILE_ATTRIBUTES *pdmfa, bool fXbox = true);
    HRESULT SetFileAttributes(char *szFileName, DM_FILE_ATTRIBUTES *pdmfa, bool fXbox = true);
    int GetNumFiles(char *szRoot);
    bool RemoveAttribute(const char *szDestPathFile, DWORD dwAttr, bool fXbox = true);
    HRESULT GetDriveFreeSize(char *szDrive, ULARGE_INTEGER *puliSpace);
    bool SetAttribute(const char *szDestPathFile, DWORD dwAttr, bool fXbox = true);
    bool GetDriveList(LPSTR sz, LPDWORD pcch);
    bool FailedConnect()
    {
        bool fRet= m_fCannotConnect; m_fCannotConnect = false; return fRet;
    }
    void SetFailed() {m_fCannotConnect = true; }
private:
    void DoFailMoveCopyMsg(bool fMove);
    int m_cOpenedHandles;
    bool m_fCannotConnect;
    MAPFINDFILE m_mappdmwdFindFile;
};

