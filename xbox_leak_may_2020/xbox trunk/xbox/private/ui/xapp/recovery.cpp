////////////////////////////////////////////////////////////////////////////
//

#include "std.h"
#include "xapp.h"
#include "node.h"
#include "runner.h"

////////////////////////////////////////////////////////////////////////////

bool ResetScreenSaver();

extern "C" {
typedef VOID (CALLBACK *PFNRECPROGRESSCALLBACK)(UINT uPercent, ULONG_PTR Context);
ULONG WINAPI DashRecovery(PFNRECPROGRESSCALLBACK pfnCallback, ULONG_PTR Context);
}

////////////////////////////////////////////////////////////////////////////
//
class CRecovery : public CNode
{
    DECLARE_NODE(CRecovery, CNode)

public:
    CRecovery();
    ~CRecovery();
    void StartRecovery();
    void FinishRecovery();
    float m_recoveryProgress;

protected:
    void Advance(float nSeconds);
    DECLARE_NODE_PROPS()
    DECLARE_NODE_FUNCTIONS()

private:
    bool m_done;
    bool m_bRecoverying;
    UINT m_recoveryPercentage;
    UINT m_lastRecoveryPercentage;
    static void CALLBACK RecoveryCallback(UINT percentage, CRecovery* rec);
    static void CALLBACK RecoveryThread(CRecovery* rec);
};

////////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_NODE("Recovery", CRecovery, CNode)

START_NODE_PROPS(CRecovery, CNode)
    NODE_PROP(pt_number, CRecovery, recoveryProgress)
END_NODE_PROPS()

START_NODE_FUN(CRecovery, CNode)
    NODE_FUN_VV(StartRecovery)
    NODE_FUN_VV(FinishRecovery)
END_NODE_FUN()

////////////////////////////////////////////////////////////////////////////

CRecovery::CRecovery() :
    m_done(false),
    m_bRecoverying(false),
    m_recoveryProgress(0.0f),
    m_recoveryPercentage(0),
    m_lastRecoveryPercentage(0)
{
}

CRecovery::~CRecovery()
{
}

void CALLBACK CRecovery::RecoveryThread(CRecovery* rec)
{
    ASSERT(rec);

#ifdef RECOVERY_TESTING
    for (int i=0; i<=10; i++) {
        Sleep(1000);
        RecoveryCallback(i*10, rec);
    }
#else
    DashRecovery((PFNRECPROGRESSCALLBACK)RecoveryCallback, (ULONG_PTR)rec);
#endif

    rec->m_done = true;
}

void CALLBACK CRecovery::RecoveryCallback(UINT percentage, CRecovery* rec)
{
    ASSERT(rec);
    rec->m_recoveryPercentage = percentage;
}

void CRecovery::StartRecovery()
{
    m_done = false;
    m_bRecoverying = true;
    m_lastRecoveryPercentage = m_recoveryPercentage;

    HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RecoveryThread, (LPVOID)this, 0, 0);
    ASSERT(hThread);

    if (hThread) {
        CloseHandle(hThread);
    } else {
        // REVIEW: should we report error to user?
        m_recoveryProgress = 1.0f;
        m_done = true;
    }
}

void CRecovery::FinishRecovery()
{
    HalReturnToFirmware(HalRebootRoutine);
}

void CRecovery::Advance(float nSeconds)
{
    CNode::Advance(nSeconds);

    if (m_bRecoverying) {
        if (m_done) {
            m_bRecoverying = false;
            CallFunction(this, _T("OnRecoveryComplete"));
        } else if (m_lastRecoveryPercentage != m_recoveryPercentage) {
            CallFunction(this, _T("OnRecoveryProgressChanged"));
            m_lastRecoveryPercentage = m_recoveryPercentage;
            m_recoveryProgress = m_recoveryPercentage / 100.0f;
            ResetScreenSaver();
        }
    }
}
