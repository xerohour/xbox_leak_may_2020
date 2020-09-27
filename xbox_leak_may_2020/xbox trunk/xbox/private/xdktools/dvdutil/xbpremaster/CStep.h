// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CStep.h
// Contents:  Contains declarations of base CStep class and derived steps
// Revisions: 31-May-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASSES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CStep         -- Defines the base Step class.  A Step is exposed to the user as a dialog box
//                  with options that control which step to move to next.  Note that all steps
//                  use the same dialog box.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CStep : public CInitedObject
{
public:
    CStep(CDlg *pdlg);
    virtual ~CStep();

    virtual bool DoModal() { return false; }    // by default steps are modeless. override this function to make a modal step
    virtual void HandleButtonPress(int nButtonId) = NULL;

protected:
    CDlg *m_pdlg;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CStep_UserAction  -- This is the first step that the user performs.  It prompts them with the
//                     desired action (write or verify).
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CStep_UserAction : public CStep
{
public:
    CStep_UserAction(CDlg *pdlg);
    void HandleButtonPress(int nButtonId);
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CStep_DestSource  -- This step walks the user through the process of setting us the destination
//                     (if action == 'write') or source (if action == 'verify') path and media
//                     (disk or tape).
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CStep_DestSource : public CStep
{
public:
    CStep_DestSource(CDlg *pdlg);
    void HandleButtonPress(int nButtonId);
    bool DoModal();

private:

    void SetMedia(eMedia media);
    void SetSource(eSource source);
    HRESULT GetPathNameCommon();
    bool ValidatePath();
    bool ValidateFiles();
    bool FLDAndFSTInSync(char *szFLDName, char *szFSTName);

    void UpdateAvailableTapeDrives();

    bool m_fAllowReadFromPackage, m_fAllowToDisk;
    int m_cTapeDrives;
    eMedia m_media;
    eSource m_source;
};

#define BATCH 10
struct sString
{
    char sz[MAX_PATH];
};
extern sString *g_rgstrInvalidXbeName;
extern int g_cInvalidXbes;
extern int g_cMaxInvalidXbes;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CStep_Progress    -- This step displays the progress of the current action.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CStep_Progress : public CStep
{
public:
    CStep_Progress(CDlg *pdlg, eSource source, eMedia media, char szDestPath[MAX_PATH],
                   char szFLDFile[MAX_PATH]);
    void HandleButtonPress(int nButtonId);

    // Override the virtual 'DoModal' function so that we can create a modal step.
    bool DoModal();
    bool CheckCancelled();

    CTapeDevice m_td;

private:
    DWORD WriteFiles();

    HRESULT PackageFiles(CFST *pfst, char *szRoot, char *szFLDName, char *szFSTName, char *szBinName);
    HRESULT ExtractPackagedFiles(char *szPackageName);

    bool GetFSTFileSizes(CFST *pfst, ULARGE_INTEGER *puliSize);

    HRESULT CreateXcontrolFile();
    HRESULT CreateDx2mlFile(CFLD *pfldfile, CDX2MLFile *pxml);
    HRESULT CreateXImageFile(int nLayer, CFST *pfst, CDX2MLFile *pxml);

    HRESULT AddMedia(DXML *pdxml);
    HRESULT AddLayer(CFLD *pfld, DXML *pdxml, int nLayer);

    HRESULT FillRandomBlocks(CFile *pfileOut, int nBlocks);
    HRESULT FillRandomBytes(CFile *pfileOut, int nBytes);
    bool IncProgressBar(int nBlocksWritten);

    void CheckInPlaceholder(CFile *pfile);
    void ClearInPlaceholder();

    eSource m_source;
    eMedia m_media;
    char m_szDestPath[MAX_PATH];
    char m_szSourceFile[MAX_PATH];
    HANDLE m_hthreadAction;
    bool m_fCancelled;
    DWORD m_rgdwPH[9];
    int m_iCurPH;
    int m_cBlocksWritten;
    char m_szPackageExtractFolder[MAX_PATH];
    char m_szPackageDVDRoot[MAX_PATH];
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// CStep_Completion  -- This step displays the results of the action (success or failure) and 
//                      presents the user with the option to verify (if action == write), perform
//                      a new action, or exit.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CStep_Completion : public CStep
{
public:
    CStep_Completion(CDlg *pdlg, eSource source, eMedia media, char szPath[MAX_PATH],
                      char szFLDFile[MAX_PATH], char szResult[1024]);
    void HandleButtonPress(int nButtonId);
private:

    void SaveLogFile(char *szText, char *szFile);

    eSource m_source;
    eMedia m_media;
};
