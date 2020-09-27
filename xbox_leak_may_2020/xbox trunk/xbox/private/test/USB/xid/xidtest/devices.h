class CGameController
{
  public:
    CGameController(DWORD dwPortNumber);
    ~CGameController();
    void    Open(XINPUT_POLLING_PARAMETERS *pPollingParameters);
    void    Close();
    void    PollDevice();
    void    UpdateState();
    void    Draw(CDraw& draw);
    LPCWSTR GetDPadDirectionText();
    LPCWSTR GetButtonStateText(DWORD dwButtonMask);
  private:
    DWORD           m_dwPortNumber;
    BOOL            m_fOpened;
    HANDLE          m_hDevice;
    XINPUT_STATE    m_gpState;
    DWORD           m_dwPacketNumber;
    int             m_iLeftEdge;
    int             m_iTopEdge;
    int             m_iLabelColorOpened;
    int             m_iLabelColorClosed;
    int             m_iStateColor;
    volatile XINPUT_FEEDBACK m_Feedback;
};

class CGameControllers
{
    public:
        CGameControllers(XINPUT_POLLING_PARAMETERS *pPollingParameters);
        ~CGameControllers();
        void CheckForHotplugs();
        void UpdateState();
        void Draw(CDraw& draw);
        void PollDevices();
    private:
        CGameController           *m_pGameControllers[4];
        XINPUT_POLLING_PARAMETERS  *m_pPollingParameters;
};

