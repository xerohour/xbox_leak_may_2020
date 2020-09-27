class CKeyboard
{
  public:
    CKeyboard(DWORD dwPortNumber);
    ~CKeyboard();
    void    Open(XINPUT_POLLING_PARAMETERS *pPollingParameters);
    void    Close();
    void    PollDevice();
    void    UpdateState();
    void    Draw(CDraw& draw);
  private:
    DWORD           m_dwPortNumber;
    BOOL            m_fOpened;
    HANDLE          m_hDevice;
    XINPUT_STATE    m_kbdState;
    DWORD           m_dwPacketNumber;
    int             m_iLeftEdge;
    int             m_iTopEdge;
    int             m_iLabelColorOpened;
    int             m_iLabelColorClosed;
    int             m_iStateColor;
    volatile XINPUT_KEYBOARD_LEDS m_LEDs;
    BOOL            m_fScrollLock;
    BOOL            m_fNumLock;
    BOOL            m_fCapsLock;
};

class CKeyboards
{
    public:
        CKeyboards(XINPUT_POLLING_PARAMETERS *pPollingParameters);
        ~CKeyboards();
        void CheckForHotplugs();
        void UpdateState();
        void Draw(CDraw& draw);
        void PollDevices();
    private:
        CKeyboard                  *m_pKeyboards[4];
        XINPUT_POLLING_PARAMETERS  *m_pPollingParameters;
};

