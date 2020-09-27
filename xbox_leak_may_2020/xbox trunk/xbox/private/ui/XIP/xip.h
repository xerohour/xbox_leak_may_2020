#define MAGIC 0x30504958

#define XIP_TYPE_GENERIC			0
#define XIP_TYPE_MESH				1
#define XIP_TYPE_TEXTURE			2
#define XIP_TYPE_WAVE				3
#define XIP_TYPE_MESH_REFERENCE		4
#define XIP_TYPE_INDEXBUFFER		5
#define XIP_TYPE_VERTEXBUFFER		6

struct XIPHEADER
{
	DWORD m_dwMagic;
	DWORD m_dwDataStart;
	WORD m_wFileCount;
	WORD m_wNameCount;
	DWORD m_dwDataSize;
};

struct FILEDATA
{
	DWORD m_dwDataOffset;
	DWORD m_dwSize;
	DWORD m_dwType;
	DWORD m_dwTimestamp;
};

struct FILENAME
{
	WORD m_wFileDataIndex;
	WORD m_wNameOffset;
};

class CXipCreator
{
public:
	int m_nFileCount;
	int m_nNameCount;
	int m_nTotalSize;

	int m_nFileAlloc;
	int m_nNameAlloc;
	FILENAME* m_directory;
	FILEDATA* m_filedata;
	char** m_rgszFileName;

	int m_cchNames;
	int m_cchNamesAlloc;
	char* m_names;

	CXipCreator();
	~CXipCreator();
	bool AddFile(const char* szFileName);
};
