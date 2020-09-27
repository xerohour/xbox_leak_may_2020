// hack.h

#ifndef UNDER_CE
    #ifdef DEBUG
        extern DWORD DebugFlag;
        void LogMessage(TCHAR *pFormat, ...);
        #define DbgPrint LogMessage
        #define DEBUGREGISTER(p)
        #define DEBUGZONE(n) ((0x0001<<n) & DebugFlag)
        // #define DEBUGMSG(exp,m) ((exp) ? DbgPrint m : (0))
        #define DEBUGMSG(exp,m) (0)
        #define ASSERT(exp) (0)
    #else
        #define DEBUGREGISTER(exp)  (0)
        #define DEBUGMSG(exp, m)    (0)
        #define ASSERT(exp)         (0)
    #endif // DEBUG
#endif // !UNDER_CE
