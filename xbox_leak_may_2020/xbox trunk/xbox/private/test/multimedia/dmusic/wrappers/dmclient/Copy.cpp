#include "trans.h"
#include "getrefs.h"

/****************************************************************
Must be called at the beginning of our host app.
****************************************************************/
/*
HRESULT XBoxSendFile(char *szSrcFile)
{
    static char szString[512] = {0};

    sprintf(szString, "xbcp -y %s xt:\\trans\\%s", szSrcFile, szSrcFile);
    system(szString);
    return S_OK;
}
*/



/****************************************************************
This copies a piece of DirectMusic content and all related content
to the XBox

I'm ripping stuff out of DMFILER to put in here.  Hopefully it's easy :)
****************************************************************/
HRESULT CopyDMusicFileToXBox(char *szSrcFile)
{
HRESULT hr = S_OK;
char **ppszFileNames = NULL;
DWORD dwCount = 0;
DWORD i = 0;
char *szFileName = NULL;


    //Copy the original file.  We wait for it, because it had better copy.
    hr = XBoxSendFile(szSrcFile, TRUE);

    //Get a list of the referenced files.
    hr = GetReferencedFileNames(szSrcFile, &ppszFileNames, &dwCount);

    for (i=0; i<dwCount; i++)
    {
        //We're not gonna wait for these since they don't exist.  However we shoul
        //  have a warning mechanism...
        hr = XBoxSendFile(ppszFileNames[i], FALSE);
    }

    hr = DeleteReferenceFileNames(ppszFileNames, dwCount);
    if (FAILED(hr))
    {
        return hr;
    }
    return S_OK;

};




HRESULT CopyDMusicFileToXBox(WCHAR *wszSrcFile)
{
char szSrcFile[MAX_PATH] = {0};
wcstombs(szSrcFile, wszSrcFile, MAX_PATH);
return CopyDMusicFileToXBox(szSrcFile);
};

