class CGameController
{
  public:
    CGameController(){}
    ~CGameController();
    void    Init(DWORD dwPort, INT iLeft, INT iTop);
    void    Inserted();
    void    Removed();
    void    Draw(CDraw& draw);
  private:
    // work is done in a separate thread
    static  DWORD WINAPI WorkerThreadEntry(CGameController *pGameController);
    void    WorkerThread();
    
    CRITICAL_SECTION m_CriticalSection;  //Protects the variables shared between the worker thread and the main thread.
    HANDLE           m_hSignalEvent;     //This event is used to signal the worker thread that there may be work to do.
    int              m_iState;           //State of the device
    int              m_iReenumCount;     //Count of successful retries
    DWORD            m_dwPort;

    //Draw Coordinates
    BOOL             m_fDisconnectGamepadHub;
    INT              m_iLeft;
    INT              m_iTop;
    
};

#define DEVICE_STATE_NOT_INSERTED   0   //Device is not inserted - initial value
#define DEVICE_STATE_INSERTED       1   //Device is inserted - set by Inserted()
#define DEVICE_STATE_REENUMERATING  2   //Device has been reset waiting for Inserted()
#define DEVICE_STATE_FAILED         3   //Device has failed reenumeration.  Wait for reenumertion has timed out
                                        //and hub confirms that device is still present.
class CGameControllers
{
    public:
        CGameControllers();
        ~CGameControllers();
        void CheckForHotplugs();
        void Draw(CDraw& draw);
    private:
        CGameController  *m_pGameControllers[4];
};

