#pragma once

#define MAX_SAVED_GAMES 4096

extern const TCHAR szTitleDataXBX [];
extern const TCHAR szTitleImageXBX [];
extern const TCHAR szSaveDataXBX [];
extern const TCHAR szSaveImageXBX [];


#define Dev1Unit1	0
#define Dev1Unit2	1
#define Dev2Unit1	2
#define Dev2Unit2	3
#define Dev3Unit1	4
#define Dev3Unit2	5
#define Dev4Unit1	6
#define Dev4Unit2	7
#define Dev0		8



#define BLOCK_SIZE	16384 // Should be max of hard drive and flash block size...


struct CSave
{
//	DWORD m_dwID;
	TCHAR m_szDirName[16];
	FILETIME m_filetime;
	DWORD m_dwFlags;

	bool SetDirName(const TCHAR* szDirName);
	const TCHAR* GetDirName();
};

#define SAVEFLAG_HASIMAGE	0x00000001
#define SAVEFLAG_UNKIMAGE	0x00000002

class CTitle
{
public:
	TCHAR* m_szID;
	TCHAR* m_szName;
	int m_nSavedGameCount;
	CSave* m_rgsaves;
	int m_nSavedGameBlocks;
	int m_nTotalBlocks;
	bool m_bBroken;
};

class CTitleArray
{
public:
	CTitleArray();
	~CTitleArray();

	void Update();
	void DeleteAll(bool bUpdate = true);

	void SetRoot(TCHAR chNewRoot, bool bUpdate = true);

	int GetTitleCount();
	bool IsBroken(int nTitle);
	const TCHAR* GetTitleID(int nTitle);
	const TCHAR* GetTitleName(int nTitle);
	int GetTitleTotalBlocks(int nTitle, HANDLE hCancelEvent);

	int GetSavedGameCount(int nTitle, HANDLE hCancelEvent = NULL);
	const TCHAR* GetSavedGameID(int nTitle, int nSavedGame);
	void GetSavedGameImageName(int nTitle, int nSavedGame, TCHAR* szPath/*[MAX_PATH]*/);
	FILETIME GetSavedGameTime(int nTitle, int nSavedGame);

	void AddSavedGame(const TCHAR* szTitleID, const TCHAR* szDirName, FILETIME saveTime);

	bool IsValid() const;
	bool IsDirty() const;
	const TCHAR* GetTData() const;
	const TCHAR* GetUData() const;

	void RemoveTitle(int nTitle);
	void RemoveSavedGame(int nTitle, int nSavedGame);
	void RemoveSavedGame(const TCHAR* szTitleID, const TCHAR* szDirName);
    bool IsPublisherExists(const TCHAR* szPublisherID) const;

protected:
	int FindTitle(const TCHAR* szTitleID);
	void AddTitle(const TCHAR* szTitleID);

	TCHAR m_szRoot [4];
	bool m_bDirty;
	int m_nTitleCount;
	int m_nTitleAlloc;
	CTitle* m_rgtitle;

    CRITICAL_SECTION m_RootLock;

	friend class CTitleCollection;
	friend void TitleArray_Init();
};

/*
class CTitleCollection : public CNode
{
	DECLARE_NODE(CTitleCollection, CNode)
public:
	CTitleCollection();
	~CTitleCollection();

	void Advance(float nSeconds);

	void SetLanguage(int nLanguage);
	int GetTitleCount();
	CStrObject* GetTitleID(int nTitle);
	CStrObject* GetTitleName(int nTitle);

	int GetSavedGameCount(int nTitle);
	CStrObject* GetSavedGameID(int nTitle, int nSavedGame);

//	int GetTitleSavedGameBlocks(int nTitle);
	int GetTitleTotalBlocks(int nTitle);

protected:
	int m_nCurLanguage;

	DECLARE_NODE_FUNCTIONS()
};
*/


extern int ScanSavedGames(const TCHAR* szRoot, const TCHAR* szTitleID, CSave* rgsaves /*[MAX_SAVED_GAMES]*/, HANDLE hCancelEvent);
//extern int ComputeTitleSavedGameBlocks(const TCHAR* szRoot, const TCHAR* szTitleID);
extern int ComputeTitleTotalBlocks(const TCHAR* szRoot, const TCHAR* szTitleID, HANDLE hCancelEvent);

extern CTitleArray g_titles [9];
