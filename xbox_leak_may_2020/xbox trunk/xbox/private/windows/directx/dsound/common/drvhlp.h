/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       drvhlp.h
 *  Content:    Miscelaneous NT-style driver helper functions and objects.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  08/07/01    dereks  Created.
 *
 ****************************************************************************/

#ifndef __DRVHLP_H__
#define __DRVHLP_H__

#if defined(_XBOX) && defined(__cplusplus)

// 
// Raised IRQL object
//

namespace DirectSound
{
    class CIrql
    {
    private:
        KIRQL                   m_irql;
        BOOL                    m_fRaised;

    public:
        CIrql(void);

    public:
        void Raise(void);
        void Lower(void);
    };

    __inline CIrql::CIrql(void)
    {
        m_fRaised = FALSE;
    }
    
    __inline void CIrql::Raise(void)
    {
        if(m_fRaised = (KeGetCurrentIrql() < DISPATCH_LEVEL))
        {
            m_irql = KfRaiseIrql(DISPATCH_LEVEL);
        }
    }

    __inline void CIrql::Lower(void)
    {
        if(m_fRaised)
        {
            KfLowerIrql(m_irql);
            m_fRaised = FALSE;
        }
    }
}

//
// Automatic (function-scope) raised IRQL
//

namespace DirectSound
{
    class CAutoIrql
        : public CIrql
    {
    public:
        CAutoIrql(void);
        ~CAutoIrql(void);
    };

    __inline CAutoIrql::CAutoIrql(void)
    {
        Raise();
    }

    __inline CAutoIrql::~CAutoIrql(void)
    {
        Lower();
    }
}

#define AutoIrql() \
    DirectSound::CAutoIrql __AutoIrql

//
// Floating point state
//

namespace DirectSound
{
    class CFpState
    {
    private:
        static DWORD            m_dwRefCount;
        static KFLOATING_SAVE   m_fps;

    public:
        void Save(void);
        void Restore(void);
    };

    __inline void CFpState::Save(void)
    {
        if(KeIsExecutingDpc())
        {
            if(!m_dwRefCount++)
            {
                KeSaveFloatingPointState(&m_fps);
            }
        }
    }

    __inline void CFpState::Restore(void)
    {
        if(KeIsExecutingDpc())
        {
            if(!--m_dwRefCount)
            {
                KeRestoreFloatingPointState(&m_fps);
            }
        }
    }
}

//
// Automatic (function-scope) floating-point state
//

namespace DirectSound
{
    class CAutoFpState
        : private CFpState
    {
    public:
        CAutoFpState(void);
        ~CAutoFpState(void);
    };

    __inline CAutoFpState::CAutoFpState(void)
    {
        Save();
    }

    __inline CAutoFpState::~CAutoFpState(void)
    {
        Restore();
    }
}

#define AutoFpState() \
    DirectSound::CAutoFpState __AutoFpState

#endif // defined(_XBOX) && defined(__cplusplus)

#ifdef __cplusplus

//
// Interlocked and/or operations
//

static void __fastcall and(volatile unsigned short *dst, unsigned short src)
{
    __asm
    {
        and word ptr [ecx], dx
    }
}

static void __fastcall or(volatile unsigned short *dst, unsigned short src)
{
    __asm
    {
        or word ptr [ecx], dx
    }
}

static void __fastcall and(volatile unsigned long *dst, unsigned long src)
{
    __asm
    {
        and dword ptr [ecx], edx
    }                        
}

static void __fastcall or(volatile unsigned long *dst, unsigned long src)
{
    __asm
    {
        or dword ptr [ecx], edx
    }
}

#endif // __cplusplus

#endif // __DRVHLP_H__
