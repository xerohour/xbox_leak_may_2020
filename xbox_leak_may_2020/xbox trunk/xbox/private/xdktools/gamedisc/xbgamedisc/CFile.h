// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFile.h
// Contents:  
// Revisions: 8-Jul-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define FILE_MEMORY 0x0001
#define FILE_DISK   0x0002

#define FILE_READ   0x0004
#define FILE_WRITE  0x0008

class CFile : public CInitedObject
{
public:
    CFile(char *szName, DWORD dwFlags = FILE_READ | FILE_DISK);
    ~CFile();
    void Close();
    bool WriteData(void *pvData, DWORD cBytes);
    DWORD GetCurLoc();
    bool WriteWORD(WORD wData);
    bool WriteBYTE(BYTE byData);
    bool WriteDWORD(DWORD dwData);
    bool WriteString(char *sz);

    bool ReadData(void *pvData, DWORD cBytes);
    bool ReadBYTE(BYTE *pbyData);
    bool ReadWORD(WORD *pwData);
    bool ReadDWORD(DWORD *pdwData);
    bool ReadString(char *sz);

    // true if file is stored only in memory
    bool m_fMemoryFile;
    bool SetPos(DWORD dwPos);
    char m_szName[MAX_PATH];
    
private:
    HANDLE m_hfile;
    DWORD m_dwMemSize;
    BYTE *m_rgbyData;
    void IncreaseMemSize(DWORD cBytes);
    BYTE *m_pbyMemCur;
    bool m_fClosed;
};

