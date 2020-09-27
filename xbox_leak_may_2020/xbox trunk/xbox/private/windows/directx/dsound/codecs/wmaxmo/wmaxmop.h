//
// Debug flags

#if DBG
#define MYCHECK(x) {RIP_ON_NOT_TRUE("WMA XMO", x);}
#else
#define MYCHECK(msg) {}
#endif

#if DBG

#define DEBUG_LEVEL_ERROR 0
#define DEBUG_LEVEL_WARNING 1
#define DEBUG_LEVEL_INFO 2
#define DEBUG_LEVEL_SPAM 5

extern ULONG g_WmaXmoDebugLevel;
#define DBG_SPAM(_exp_) {if (g_WmaXmoDebugLevel >= DEBUG_LEVEL_SPAM) DbgPrint _exp_;}
#define DBG_INFO(_exp) {if (g_WmaXmoDebugLevel >= DEBUG_LEVEL_INFO) DbgPrint _exp;}
#define DBG_ERROR(_exp) {if (g_WmaXmoDebugLevel >= DEBUG_LEVEL_ERROR) DbgPrint _exp;}
#define DBG_WARN(_exp) {if (g_WmaXmoDebugLevel >= DEBUG_LEVEL_WARNING) DbgPrint _exp;}

#else

#define DBG_SPAM(_exp_)
#define DBG_INFO(_exp_)
#define DBG_ERROR(_exp_)
#define DBG_WARN(_exp_)

#endif

#define ENTER_CS(pCS) {\
    EnterCriticalSection(pCS);\
}

#define LEAVE_CS(pCS) {\
    LeaveCriticalSection(pCS);\
}


// BUGBUG
// Trial-and-error has shown that WMA has a limit on the number
// of samples it is willing to consider decoding at once.
// If you ask for (say) a million samples, you'll get 0 samples.
//
// Worse, asking for 2049 samples at a time appears to cause heap
// corruption.
//
// Asking for 2048 samples works fine. It is lame because that's
// the frame size, so every other request returns 0 samples.
// That's twice the overhead as if we asked for 2049 samples,
// but it avoids corrupting the heap.

const DWORD WMA_MAX_SRC_PACKET_SIZE = 128;
const DWORD MAX_SAMPLES_PER_REQUEST = 2048;
const DWORD WMA_SRC_BUFFER_SIZE = max(4096*2,WMAMO_HEADER_SIZE);

const DWORD WMA_MAX_CONTEXTS = 10;
