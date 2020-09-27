class CGameControllers
{
    public:
        CGameControllers();
        ~CGameControllers();
        void SelectGameController();
        BOOL GetSelection(int *piControllerNumber);
        BOOL GetState(XINPUT_STATE *m_gpState);

    private:
        void DetectHotPlugs();
        BOOLEAN m_fIsSelected;
        int m_iControllerNumber;
        HANDLE m_hGameControllerArray[XGetPortCount()];
};

