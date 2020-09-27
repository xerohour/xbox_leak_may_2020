/*
 *
 * dmcount.h
 *
 * performance counters
 *
 */

typedef HRESULT (*PDM_CDPROC)(DWORD, struct _PFC *, PDM_COUNTDATA);

typedef struct _PFC {
    LIST_ENTRY leUpdate;
    DWORD dwType;
    union {
        PDM_COUNTDATA pdmcd;
        PDM_COUNTPROC pfnCount;
        PDM_CDPROC pfnCountData;
        PULONG pul;
        PLARGE_INTEGER pli;
        PVOID pv;
    };
    LARGE_INTEGER rgliValues[11];
    ULONG rgulTicks[11];
    ULONG rgulFrames[11];
    int iulCur;
} PFC, *PPFC;
