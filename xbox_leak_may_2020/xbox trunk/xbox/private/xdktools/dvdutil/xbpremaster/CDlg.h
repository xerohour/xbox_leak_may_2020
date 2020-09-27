// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CDlg.h
// Contents:  Encapsulates the UI for the steps that appear in this application
// Revisions: 31-May-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// MAX_VISIBLE_CONTROLS     -- Maximum number of controls visible on the dialog at one time.
#define MAX_VISIBLE_CONTROLS 100

#define MAX_OVERLOADED_HWNDS 10


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASSES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Forward Class declarations
class CStep;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Class:   CDlg
// Purpose: Encapsulates the UI for the steps that appear in this application.  
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CDlg : public CInitedObject
{
public:

    // ++++ CONSTRUCTION-RELATED FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // CDlg             -- CDlg constructor
    CDlg(HINSTANCE hinstApp);
    
    // ~CDlg            -- CDlg destructor
    ~CDlg();


    // ++++ CONTROL-RELATED FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // DisplayControl   -- Displays the control with the specified resource id
    HRESULT DisplayControl(int nControl);

    // HideAllControls  -- Hides and disables all currently visible controls on the dialog box.
    HRESULT HideAllControls();

    // SetControlEnable -- Enables or disables the specified control
    HRESULT SetControlEnable(int nControl, bool fEnabled);

    // SetControlState  -- Sets the checked/unchecked-pressed/unpressed state of a control
    HRESULT SetControlState(int nControl, bool fChecked);

    // SetControlText   -- Sets the text that appears in/on the specified control
    HRESULT SetControlText(int nControl, char *szText);

    // GetControlText   -- Gets the text that appears in/on the specified control
    HRESULT GetControlText(int nControl, char *szText, int cbyBufLen);

    // SetCurControl    -- Sets the current keyboard focus to the specified control
    HRESULT SetCurControl(int nControl);

    // SetProgressPos   -- Sets the position of the specified progress bar
    HRESULT SetProgressPos(int nControl, int nPos);

    // SetProgressRange -- Sets the range of the specified progress bar
    HRESULT SetProgressRange(int nControl, int nStart, int nEnd);

    // SetControlFont   -- Sets the font of the control
    HRESULT SetControlFont(int nControl, int nHeight, char *szFace, int nWeight);

    HRESULT OverloadBkColor(int nControl);

    HRESULT AddToComboBox(int nControl, char *szString);

    HRESULT ClearComboBox(int nControl);

    HRESULT GetComboSel(int nControl, char *szBuffer);
    

    // ++++ EXECUTION FLOW-RELATED FUNCTION +++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Run              -- Runs the specified step.
    CStep *Run(CStep *pstepCur);

    // DlgProc          -- The main message-handling procedure for the dialog
    BOOL DlgProc(UINT uMsg, WPARAM wparam, LPARAM lparam);

    // SetNextStep      -- Sets the next step be performed by the dialog.  Also marks that the
    //                     current step has ended.
    void SetNextStep(CStep *pstepNext);

    
    // ++++ MISCELLANEOUS FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // GetPathNameCommon -- Opens up the Win32 common directory dialog and obtains a directory name
    bool GetPathNameCommon(char *szTitle, char *szPath);

    // GetFileNameCommon -- Opens up the Win32 common file dialog and obtains a filename.
    bool GetFileNameCommon(char *szTitle, char *szExt, char *szFilter, char *szFile, bool fOpen);

    // GetHwnd          -- Returns the handle of this dialog.
    HWND GetHwnd() {return m_hdlg; }

    // SetCaption       -- Sets the text that appears at the top of the dialog
    void SetCaption(char *szCaption);

    // GetInst          -- Returns the Win32 instance of this application
    HINSTANCE GetInst() {return m_hinst; }

    // m_pstepCur       -- The Step currently being executed
    CStep *m_pstepCur;

private:

    // OverloadedBkColor    -- Determines if the specified control was overridden to display a
    //                         white background instead of the default grey
    bool OverloadedBkColor(HWND hwndControl);

    // m_hdlg           -- Handle to the Win32 dialog that this object encapsulates
    HWND m_hdlg;

    // m_rgnVisibleControl  -- The list of currently visible controls
    int m_rgnVisibleControl[MAX_VISIBLE_CONTROLS];

    // m_cVisibleControls   -- The number of currently visible controls
    int m_cVisibleControls;

    HWND m_rghwndOverload[MAX_OVERLOADED_HWNDS];
    
    int m_cOverloadBkColor;

    // m_fStepEnded     -- Tracks if the current step has already ended.
    bool m_fStepEnded;

    // m_pstepNext      -- The next Step to execute.
    CStep *m_pstepNext;

    // m_hinst          -- Win32 instance of this application.
    HINSTANCE m_hinst;
};
