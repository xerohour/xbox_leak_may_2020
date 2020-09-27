//#define PACKAGE_DEBUG       90
#define PACKET_DEBUG        1

////////////////////////////////////////////////////////////////////////////
// Packages must provide exactly one instance of a sub-class of CPackage.
//

#ifdef DEBUGGER_AUTOMATION
class CAutoObj;
#endif // DEBUGGER_AUTOMATION

class CDebugPackage : public CPackage
{
    private:

       CFormatInfo                   m_FormatInfo;
       CFormatInfo                   m_FormatInfoDefault;

    private:

        void    InitColors( void );

    protected:
    	// Package interface map
    	BEGIN_INTERFACE_PART(DbgPkg, IDbgPkg)
            INIT_INTERFACE_PART(CDebugPackage, DbgPkg)

            STDMETHOD_(BOOL, DebuggeeAlive)(void);
            STDMETHOD_(BOOL, DebuggeeRunning)(void);
			STDMETHOD_(void *, DbgDAMHsfFromFile)(LPCTSTR);
            STDMETHOD_(BOOL, GetExecutablePath)(PSTR);
			STDMETHOD_(BOOL, CanShowDataTip)(void);
			STDMETHOD(EvaluateExpression)(LPTSTR szText, BOOL bFromSelection, LPTSTR szResult, ULONG cb);
			STDMETHOD(SetDebugLines)(CDocument *pDoc, BOOL ResetTraceInfo);
			STDMETHOD(AdjustDebugLines)(CDocument * pDoc, int StartLine, int NumberLines, BOOL fAdded, BOOL fLineBreak, BOOL fWhiteSpaceOnly);
			STDMETHOD(DeleteBreakpointLinesInDoc)(LPCTSTR sz);
            STDMETHOD_(BOOL, IsENCProjectFile)(CPath *ppath);
            STDMETHOD(VTestDebugActive)(BOOL *bActive);

		END_INTERFACE_PART(DbgPkg)

		BEGIN_INTERFACE_PART(DLLInfo, IDLLInfo)
			INIT_INTERFACE_PART(CDebugPackage, DLLInfo)

			STDMETHOD(DiffDLLs)(ULONG hTarget, CPtrList &listDLLs, INT *piRow, UINT *pidMsg);
			STDMETHOD(GetDLLCount)(ULONG hTarget, int *piCount);
			STDMETHOD(InitDLLList)(ULONG hTarget, CPtrList &listDLLs);
			STDMETHOD(SaveDLLList)(ULONG hTarget, CPtrList &listDLLs);
		END_INTERFACE_PART(DLLInfo)

		DECLARE_INTERFACE_MAP()

    public:
    	CDebugPackage();

 		GetApplicationExtensions(SApplicationExtension** ppAppExts);
 		
    	// Add flags here when overriding applicable package virtuals.
    	enum { PKS = (PKS_IDLE | PKS_PRETRANSLATE | PKS_COMMANDS |
                      PKS_NOTIFY | PKS_INIT | PKS_QUERYEXIT | PKS_SERIALIZE |
                      PKS_INTERFACES | PKS_FORMAT
#ifdef DEBUGGER_AUTOMATION
											| PKS_AUTOMATION
#endif // DEBUGGER_AUTOMATION
											) };

#ifdef DEBUGGER_AUTOMATION
		// OLE automation support
		virtual LPDISPATCH GetPackageExtension(LPCTSTR szExtensionName);
		virtual IDispatch* GetAutomationObject(BOOL bAddRef);
//		virtual void AutomationObjectReleased(); // called by Automation object when it goes away
#endif // DEBUGGER_AUTOMATION

        // Dockable window interface.  These functions must be overridden only if
        // the package provides new kinds of dockable windows, or default toolbars.
    	virtual HGLOBAL     GetDockInfo();
    	virtual HWND        GetDockableWindow(UINT nID, HGLOBAL hglob);
    	virtual HGLOBAL     GetToolbarData(UINT nID);
    	virtual HICON       GetDockableIcon(UINT nID);
    	virtual BOOL        AskAvailable(UINT nID);
//		virtual void        LoadGlobalAccelerators();
    	virtual BOOL        OnInit();
        virtual BOOL        OnNotify(UINT id, void *Hint, void *ReturnArea);
    	virtual DWORD       GetIdRange(RANGE_TYPE rt);
    	virtual void        OnExit();
        virtual BOOL        OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    	virtual void        SerializeWorkspaceSettings(CStateSaver &pStateSave, DWORD dwFlags);
        virtual void        AddOptDlgPages(class CTabbedDialog* pOptionsDialog, OPTDLG_TYPE type);
        virtual void        AddGoToItems(CGoToDialog *pDlg);

    //	Fonts/Colors Interface
    	virtual CFormatInfo *GetFormatInfo ();
    	virtual void        GetDefaultFormatInfo (CFormatInfo& fiDefault);
    	virtual void        OnFormatChanged ();
    	virtual void        OnFontChange ();
    	virtual void        OnColorChange ();

    // Registry settings save interface.  All settings that need to be saved in
    // the registry should be written here.  (Read them anytime.)
    	virtual void SaveSettings();
		virtual BOOL IsVisibleProjectCommand(UINT nID, GUID *pGuids, UINT cGuidCount);

    /*
    // Hook for idle processing; works just like the one in MFC's CWinApp.
    	virtual BOOL OnIdle(long lCount);

    // Hook for message dispatching; works just like the one in MFC's CWinApp.
    	virtual BOOL PreTranslateMessage(MSG* pMsg);
    */

    	void    OnDebugGo                       ();
    	void    OnDebugJITGo                    ();
    	void    OnExecuteTarget                 ();
        void    OnExceptions                    ();
        void    OnThreads                       ();
		void	OnFibers						();
        void    OnDebugRegFloat                 ();
        BOOL    OnDebugStackSet                 (UINT nIDCmd );
        void    OnDebugSourceAnnotate           ();
        void    OnDebugShowCodeBytes            ();
        void    OnToggleMemoryBar               ();
        BOOL    OnDebugMemorySet                (UINT nIDCmd );
        void    OnDebugRestart                  ();
        void    OnDebugStop                     ();
        void    OnDebugToCursor                 ();
        void    OnDebugTraceInto                ();
        void    OnDebugTraceFunction            ();
        void    OnUpdateTraceFunction			(CCmdUI* pCmdUI );
        void    OnDebugStepOver                 ();
        void    OnDebugSrcTraceInto             ();
        void    OnDebugSrcStepOver              ();
        void    OnDebugStopAfterReturn          ();
        void    OnBreakpoints                   ();
        void    OnUpdateClearBreakpoints        (CCmdUI* pCmdUI );
        void    OnClearBreakpoints              ();
        void    OnQuickWatch                    ();
        BOOL    OnDebugBreak                    (UINT nCmdId );
        void    OnDebugRadix                    ();
        void    OnNextMemWinFmt                 ();
        void    OnPrevMemWinFmt                 ();
        void    OnDisableAllBreakpoints         ();
        void    OnUpdateDisableAllBreakpoints   (CCmdUI* pCmdUI );
        void    OnDebugSetIP                    ();
        void    OnDebugActive                   ();
        BOOL    OnToggleBreakpoint              (UINT);
        void    OnEnableDisableBreakpoint       ();
        void    OnUpdateActivateSrcWnd          (CCmdUI* pCmdUI );
        void    OnActivateSrcWnd                (UINT nCmd );
        void    OnUpdateToggleSrcWnd            (CCmdUI* pCmdUI );
        void    OnToggleSrcWnd                  (UINT nCmd );
        void    OnUpdateDebugRadix              (CCmdUI* pCmdUI );
        void    OnUpdateDebugOptions            (CCmdUI* pCmdUI );
        void    OnUpdateToggleBreakpoint        (CCmdUI* pCmdUI );
        void    OnUpdateEnableDisableBreakpoint (CCmdUI* pCmdUI );
        void    OnRemoteDebugger                ();
        void    OnUpdateDebugActive             (CCmdUI* pCmdUI );
        BOOL    OnDebugGoPostMsg                (UINT);
        void    OnDebugRestartPostMsg           ();
        BOOL    OnDebugStopPostMsg              (UINT);
        void    OnDebugToCursorPostMsg          ();
        BOOL    OnDebugTraceIntoPostMsg         (UINT);
        void    OnDebugTraceFunctionPostMsg     ();
        BOOL    OnDebugStepOverPostMsg          (UINT);
        void    OnDebugSrcTraceIntoPostMsg      ();
        void    OnDebugSrcStepOverPostMsg       ();
        BOOL    OnDebugStopAfterReturnPostMsg   (UINT);
        void    OnUpdateProjectExecute          (CCmdUI* pCmdUI );
#ifdef CODECOVERAGE
        void    OnCodeCoverageToggle            ();
        void    OnCodeCoverageClear             ();
#endif
        void    OnGotoCurrentInstruction        ();
        void    OnUpdateGotoCurrentInstruction  (CCmdUI* pCmdUI );
		void	OnAttachToActive				();
		void	OnDebugUpdateImage				();
		void	OnDebugUpdateImagePostMsg		();
		void	OnDebugUpdateImageToggle		();
		void	OnUpdateDebugUpdateImage		(CCmdUI* pCmdUI );
		void	OnUpdateDebugUpdateImageToggle	(CCmdUI* pCmdUI );
		void	OnModuleView					();
		void	OnUpdateModuleView				(CCmdUI* pCmdUI );
		
    	DECLARE_MESSAGE_MAP()
};

/*
////////////////////////////////////////////////////////////////////////////
// This is the view class for the simple docking window

class CDebugDockWnd : public CDockablePartView
{
    public:

    	CDebugDockWnd();

    	        LRESULT WindowProc( UINT, WPARAM, LPARAM );
    	        void    OnNcDestroy();
    	afx_msg void    OnPaint();
    	
    	DECLARE_MESSAGE_MAP()
};
*/

HINSTANCE GetResourceHandle();
