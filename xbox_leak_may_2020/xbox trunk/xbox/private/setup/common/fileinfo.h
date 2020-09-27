//  FILEINFO.H
//
//  Created 30-Mar-2001 [JonT]

#ifndef _FILEINFO_H_
#define _FILEINFO_H_

//---------------------------------------------------------------------

class CFileInfo
{
private:
    DWORD m_dwSize;
    FILETIME m_ft;

public:
    BOOL Get(LPCSTR lpFile);
    int Compare(LPCSTR lpFile);
    void NoLessThan(FILETIME ft);
    BOOL Load(LPSTR lpIn);
    BOOL Save(LPSTR lpOut);
    DWORD GetSize() { return m_dwSize; }
    FILETIME* GetFiletime() { return &m_ft; }
};

// one byte per nibble + a zero byte
#define FILEINFO_SAVE_SIZE (sizeof (CFileInfo) * 2 + 1)

#endif // #ifndef _FILEINFO_H_
