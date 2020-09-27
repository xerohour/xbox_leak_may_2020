//-----------------------------------------------------------------------------
// FILE: MEMCPY2.H
//
// Desc: MEMCPY2 header file
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define PAGE_SIZE 0x1000

#define ARRAYSIZE(_a)     (sizeof(_a) / sizeof((_a)[0]))

//=========================================================================
// Formatted debug squirty
//=========================================================================
inline void _cdecl dprintf(LPCSTR lpFmt, ...)
{
    va_list arglist;
    static char lpOutput[256];

    va_start(arglist, lpFmt);
    _vsnprintf(lpOutput, sizeof(lpOutput), lpFmt, arglist);
    va_end(arglist);

    OutputDebugStringA(lpOutput);
}

//=========================================================================
// Class to help time things
//=========================================================================
class CTimer
{
public:
    // constructor
    CTimer()
    {
        m_fRunning = false;
        QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSecond);
        ticksPerSecond /= 1000;
    }

    ~CTimer() {}

    void Start()
    {
        m_fRunning = true;
        QueryPerformanceCounter((LARGE_INTEGER *)&ticksstart);
    }

    void Stop()
    {
        QueryPerformanceCounter((LARGE_INTEGER *)&ticksend);
        m_fRunning = false;
    }

    float getTime()
        {
            if(m_fRunning)
                QueryPerformanceCounter((LARGE_INTEGER *)&ticksend);
            return ((ticksend - ticksstart) * 1.0f / ticksPerSecond);
        }

    bool m_fRunning;
    _int64 ticksstart;
    _int64 ticksend;
    _int64 ticksPerSecond;
};

//=========================================================================
// Helper routine for rdpmc
//=========================================================================
__forceinline ULONGLONG ReadPerformanceCounter(DWORD counter)
{
    _asm mov ecx, (counter)
    _asm rdpmc
}

//=========================================================================
// Stupid PIII counter class
//=========================================================================
class CProfiler
{
public:
    //=========================================================================
    // CProfiler c'tor - enables counters
    //=========================================================================
    CProfiler(DWORD Ctr0, DWORD Ctr1)
    {
        /*
            186H    EVNTSEL0

            7:0     Event Select (Refer to Performance Counter section for a list of event encodings)
            15:8    UMASK:  Unit Mask Register Set to 0 to enable all count options
            16      USER:   Controls the counting of events at Privilege levels of 1, 2, and 3
            17      OS:     Controls the counting of events at Privilege level of 0
            18      E:      Occurrence/Duration Mode Select
                                1 = Occurrence
                                0 = Duration
            19      PC:     Enabled the signaling of performance counter overflow via BP0 pin
            20      INT:    Enables the signaling of counter overflow via input to APIC
                                1 = Enable
                                0 = Disable
            22      ENABLE: Enables the counting of performance events in both counters
                                1 = Enable
                                0 = Disable
            23      INV:    Inverts the result of the CMASK condition
                                1 = Inverted
                                0 = Non-Inverted
            31:24   CMASK:  Counter Mask
        */

        rgCtrs[0] = Ctr0;
        rgCtrs[1] = Ctr1;

        for(int Counter = 0; Counter < 2; Counter++)
        {
            // Select the correct MSR for the specified counter
            DWORD dwMSR = 0x186 + Counter;

            DWORD dwValue = rgCtrs[Counter];

            if(dwValue)
            {
                // Enable counter
                dwValue |=  (1<<22) | (1<<17);

                _asm
                {
                    mov     ecx, dwMSR
                    mov     eax, dwValue
                    mov     edx, 0

                    // WRMSR: Write the value in EDX:EAX to MSR specified by ECX
                    wrmsr
                }
            }
        }
    }

    //=========================================================================
    // CProfiler d'tor - disables counters set in rgCtrs
    //=========================================================================
    ~CProfiler()
    {
        // Select the correct MSR for the specified counter
        for(DWORD Counter = 0; Counter < 2; Counter++)
        {
            DWORD dwMSR = 0x186 + Counter;
            DWORD dwValue = rgCtrs[Counter];

            if(dwValue)
            {
                // Disable counter
                _asm
                {
                    mov     ecx, dwMSR
                    mov     eax, dwValue
                    mov     edx, 0

                    // WRMSR: Write the value in EDX:EAX to MSR specified by ECX
                    wrmsr
                }
            }
        }
    }

    void Start()
    {
        ctrstart0 = ReadPerformanceCounter(0);
        ctrstart1 = ReadPerformanceCounter(1);
    }

    void Stop()
    {
        ctrend0 = ReadPerformanceCounter(0);
        ctrend1 = ReadPerformanceCounter(1);
    }

    __int64 getCtr0()
        { return ctrend0 - ctrstart0; }
    __int64 getCtr1()
        { return ctrend1 - ctrstart1; }

    __int64 ctrstart0, ctrstart1;
    __int64 ctrend0, ctrend1;

    DWORD rgCtrs[2];
};

/*
 * Various PIII counter defines
 */

// data cache unit
#define PERF_DCU_MEMORY_REFERENCES                                  0x00000043
#define PERF_DCU_LINES_ALLOCATED                                    0x00000045
#define PERF_DCU_LINES_MODIFIED                                     0x00000046
#define PERF_DCU_MODFIED_LINES_EVICTED                              0x00000047
#define PERF_DCU_CYCLES_MISS_OUTSTANDING                            0x00000048

// instruction fetch unit
#define PERF_IFU_INSTRUCTION_FETCHES                                0x00000080
#define PERF_IFU_CACHE_MISSES                                       0x00000081
#define PERF_IFU_ITLB_MISSES                                        0x00000085
#define PERF_IFU_CYCLES_DECODER_STALLED                             0x00000086
#define PERF_IFU_CYCLES_DECODER_LENGTH_STALLED                      0x00000087

// L2 cache
#define PERF_L2_INSTRUCTION_FETCH                                   0x00000f28
#define PERF_L2_DATA_LOAD                                           0x00000f29
#define PERF_L2_DATA_STORE                                          0x00000f2a
#define PERF_L2_LINES_ALLOCATED                                     0x00000024
#define PERF_L2_LINES_REMOVED                                       0x00000026
#define PERF_L2_LINES_MODIFIED                                      0x00000025
#define PERF_L2_MODIFIED_LINES_REMOVED                              0x00000027
#define PERF_L2_REQUESTS                                            0x00000f2E
#define PERF_L2_ADDRESS_STROBES                                     0x00000021
#define PERF_L2_DATA_BUS_BUSY                                       0x00000022
#define PERF_L2_DATA_BUS_BUSY_XFER_TO_CPU                           0x00000023

// external bus logic
#define PERF_BUS_CYCLES_CPU_DRDY                                    0x00000062
#define PERF_BUS_CYCLES_LOCKED                                      0x00000063
#define PERF_BUS_REQUESTS_OUTSTANDING                               0x00000060
#define PERF_BUS_BUST_READ                                          0x00000065
#define PERF_BUS_READ_FOR_OWNERSHIP                                 0x00000066
#define PERF_BUS_WRITE_BACK                                         0x00000067
#define PERF_BUS_INSTRUCTION_FETCH                                  0x00000068
#define PERF_BUS_COMPLETED_INVALIDATE_TRANSACTIONS                  0x00000069
#define PERF_BUS_COMPLETED_PARTIAL_WRITES                           0x0000006a
#define PERF_BUS_COMPLETED_PARTIAL_TRANSACTIONS                     0x0000006b
#define PERF_BUS_COMPLETED_IO_TRANSACTIONS                          0x0000006c
#define PERF_BUS_DEFERRED_TRANSACTIONS                              0x0000006d
#define PERF_BUS_COMPLETED_BURSTS                                   0x0000006e
#define PERF_BUS_TOTAL_COMPLETED_TRANSACTIONS                       0x00000070
#define PERF_BUS_COMPLETED_MEMORY_TRANSACTIONS                      0x0000006f
#define PERF_BUS_CYCLES_PROCESSOR_RECEIVING_DATA                    0x00000064
#define PERF_BUS_CYCLES_PROCESSOR_DRIVING_BNR                       0x00000061
#define PERF_BUS_CYCLES_PROCESSOR_DRIVING_HIT                       0x0000007a
#define PERF_BUS_CYCLES_PROCESSOR_DRIVING_HITM                      0x0000007b
#define PERF_BUS_CYCLES_SNOOP_STALLED                               0x0000007e

// floating point unit
#define PERF_FPU_FLOATING_POINT_OPERATIONS                          0x000000c1
#define PERF_FPU_COMPUTATIONAL_OPERATIONS                           0x00000010
#define PERF_FPU_MICROCODE_EXCEPTIONS                               0x00000011
#define PERF_FPU_MULTIPLIES                                         0x00000012
#define PERF_FPU_DIVIDES                                            0x00000013
#define PERF_FPU_CYCLES_DIVIDER_BUSY                                0x00000014

// memory ordering
#define PERF_MO_STORE_BUFFER_BLOCKS                                 0x00000003
#define PERF_MO_STORE_BUFFER_DRAIN_CYCLES                           0x00000004
#define PERF_MO_MISALIGNED_MEMORY_REFERENCE                         0x00000005
#define PERF_MO_KNI_PREFETCH_NTA                                    0x00000007
#define PERF_MO_KNI_PREFETCH_T1                                     0x00000107
#define PERF_MO_KNI_PREFETCH_T2                                     0x00000207
#define PERF_MO_KNI_WEAKLY_ORDERED_STORES                           0x00000307
#define PERF_MO_KNI_PREFETCH_MISS_CACHES_NTA                        0x0000004b
#define PERF_MO_KNI_PREFETCH_MISS_CACHES_T1                         0x0000014b
#define PERF_MO_KNI_PREFETCH_MISS_CACHES_T2                         0x0000024b
#define PERF_MO_KNI_WEAKLY_ORDERED_STORE_MISS_CACHES                0x0000034b

// instruction decoding and retiring
#define PERF_IDR_INSTRUCTIONS_RETIRED                               0x000000c0
#define PERF_IDR_MICRO_OPS_RETIRED                                  0x000000c2
#define PERF_IDR_INSTRUCTIONS_DECODED                               0x000000d0
#define PERF_IDR_KNI_ALL_INSTRUCTIONS_RETIRED                       0x000000d8
#define PERF_IDR_KNI_SCALAR_INSTRUCTIONS_RETIRED                    0x000001d8
#define PERF_IDR_KNI_ALL_COMPUTATIONAL_INSTRUCTIONS_RETIRED         0x000000d9
#define PERF_IDR_KNI_SCALAR_COMPUTATIONAL_INSTRUCTIONS_RETIRED      0x000001d9

// interrupts
#define PERF_INT_RECEIVED                                           0x000000c8
#define PERF_INT_CYCLES_MASKED                                      0x000000c6
#define PERF_INT_CYCLES_MASKED_AND_INT_PENDING                      0x000000c7

// branches
#define PERF_BR_INSTRUCTIONS                                        0x000000c4
#define PERF_BR_MISPREDICTED_RETIRED                                0x000000c5
#define PERF_BR_TAKEN_RETIRED                                       0x000000c9
#define PERF_BR_TAKEN_MISPREDICT_RETIRED                            0x000000ca
#define PERF_BR_DECODED                                             0x000000e0
#define PERF_BR_NO_BTB_PREDICT                                      0x000000e2
#define PERF_BR_BOGUS                                               0x000000e4
#define PERF_BR_STATIC_PREDICTIONS                                  0x000000e6

// stalls
#define PERF_RES_STALLS                                             0x000000a2
#define PERF_RES_PARTIAL_STALL                                      0x000000d2

// segment register loads
#define PERF_SREG_LOADS                                             0x00000006

// clocks
#define PERF_CLOCK_UNHALTED                                         0x00000079

// mmx unit
#define PERF_MMX_SATURATING                                         0x000000b1
#define PERF_MMX_MICRO_OPS_EXECUTED                                 0x000000b2
#define PERF_MMX_PACKED_MULTIPLY_EXECUTED                           0x000001b3
#define PERF_MMX_PACKED_SHIFT_EXECUTED                              0x000002b3
#define PERF_MMX_PACK_OPERATIONS_EXECUTED                           0x000004b3
#define PERF_MMX_UNPACK_OPERATIONS_EXECUTED                         0x000008b3
#define PERF_MMX_PACKED_LOGICAL_OPERATIONS_EXECUTED                 0x000010b3
#define PERF_MMX_PACKED_ARITHMETIC_OPERATIONS_EXECUTED              0x000020b3
#define PERF_MMX_TRANSITION_FROM_MMX_TO_FLOATING                    0x000000cc
#define PERF_MMX_TRANSITION_FROM_FLOATING_TO_MMX                    0x000001cc
#define PERF_MMX_EMMS_EXECUTED                                      0x000000cd

// segment register renaming
#define PERF_SEG_RENAME_STALL_ALL                                   0x00000fd4
#define PERF_SEG_RENAME_STALL_ES                                    0x000001d4
#define PERF_SEG_RENAME_STALL_DS                                    0x000002d4
#define PERF_SEG_RENAME_STALL_FS                                    0x000004d4
#define PERF_SEG_RENAME_STALL_GS                                    0x000008d4
#define PERF_SEG_NUMBER_RENAMES_ALL                                 0x00000fd5
#define PERF_SEG_NUMBER_RENAMES_ES                                  0x000001d5
#define PERF_SEG_NUMBER_RENAMES_DS                                  0x000002d5
#define PERF_SEG_NUMBER_RENAMES_FS                                  0x000004d5
#define PERF_SEG_NUMBER_RENAMES_GS                                  0x000008d5
#define PERF_NUMBER_SEG_RENAMES_RETIRED                             0x000000d6

