/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    PrfData.cpp

Abstract:

    This module implements the C++ template classes for Performance
    Object data types.

Author:

    Jeffrey M. Richter (v-jeffrr) 01-Aug-1998

--*/

#pragma once

#include <WinPerf.H>       // Not included by "Windows.H"
#include "Optex.h"         // High-speed, cross-process, critical section
#include "RegSettings.h"   // Class to make it easy to work with the registry

//
// I use this to declare type-safe values
//

#define DECLARE_VALUE(name)          \
   struct name##__ { BYTE unused; }; typedef struct name##__ *name

class CPrfData {

public:     // Static member functions for install/uninstall
   // Installs the performance DLL info into the registry
   static void Install(LPCWSTR pszDllPathname);

   // Removes the performance DLL info from the registry
   static void Uninstall();


public:     // Static member function to activate counter information
   // Creates and initializes the shared counter data block
   static DWORD Activate(); // Returns last error


public:
   // The following should not be used directly. They are public because
   // of the BEGIN_PRFITEM_MAP/PRFOBJ/PRFCTR/END_PRFITEM_MAP macros (below)

   // The types of record that can be in a PRFITEM map
   typedef enum { PIT_END, PIT_OBJECT, PIT_COUNTER } PRFITMTYPE;

   // Data types to make the code more readable
   typedef int PIINDEX;
   DECLARE_VALUE(OBJORD);
   DECLARE_VALUE(CTRORD);
   DECLARE_VALUE(OBJID);
   DECLARE_VALUE(CTRID);
   DECLARE_VALUE(INSTID);

   // Each object/counter is identified with this information
   typedef struct _PRFITM {
      PRFITMTYPE pit;
      // The following fields apply to both PIT_OBJECT & PIT_COUNTER
      DWORD    dwId;
      LPCWSTR  pwszName;
      LPCWSTR  pwszHelp;
      DWORD    dwDetailLevel;

      // The following fields apply to PIT_OBJECT only
      CTRID    DefCounter;
      INSTID   MaxInstances;
      DWORD    cchMaxInstName;
      CTRORD   NumCounters;
      PIINDEX  IndexNextObj;
      DWORD    cbOffsetToNextObj;
      BOOL     fCollectThisObj;

      // The following fields apply to PIT_COUNTER only
      DWORD    dwDefaultScale;
      DWORD    dwCounterType;

      // The actual DM counter information
      DM_COUNTINFO  dmci;
   } PRFITM, *PPRFITM;


public:  // Public member functions
   // Constructs a CPrfData
   CPrfData(LPCWSTR pszAppName, PPRFITM pPrfItms);

   // Destructs a CPrfData
   ~CPrfData();

   // Functions to allow mutual exclusive access to counter data
   void LockCtrs() const;
   BOOL TryLockCtrs() const;
   void UnlockCtrs() const;

   // Adds an instance to an object (returns InstId)
   INSTID AddInstance(OBJID ObjId, LONG lUniqueId,
      OBJID ObjIdParent = 0, INSTID InstIdParent = 0);
   INSTID AddInstance(OBJID ObjId, LPCWSTR pszInstName,
      OBJID ObjIdParent = 0, INSTID InstIdParent = 0);

   // Removes an instance from an object
   void RemoveInstance(OBJID ObjId, INSTID InstId);

   // Returns 32-bit address of counter in shared data block
   LONG& GetCtr32(CTRID CtrId, INSTID InstId = 0) const;

   // Returns 64-bit address of counter in shared data block
   __int64& GetCtr64(CTRID CtrId, INSTID InstId = 0) const;

   // Returns pointer to DM_COUNTINFO of the counter
   const DM_COUNTINFO* GetDmCtr(CTRID CtrId) const;

   // Assigns dynamic PRFITM
   void AssignDynamicPerfItems(PPRFITM pPrfItms);

private: // For debugging
   BOOL     IsValidObjOrd(OBJORD ObjOrd) const;
   BOOL     IsValidInstId(OBJORD ObjOrd, INSTID InstId) const;


private: // Fuctions for install/remove
   void     VerifyPrfItemTable();
   void     InstallPrfData(LPCWSTR pszDllPathname);
   void     UninstallPrfData();
   void     AppendRegStrings(CRegSettings& regPerfLib,
                             BOOL fCounter, PDWORD pdwIndex);
   void     RemoveRegStrings(CRegSettings& regPerfLib,
                             BOOL fCounter,DWORD dwIndexLo, DWORD dwIndexHi);


private: // Data types used internally by the class
   enum { kMaxCounterSize = sizeof(__int64) };
   enum {
      PERF_MASK_SIZE_FLD      = 0x00000300,
      PERF_MASK_CTR_TYPE      = 0x00000C00,
      PERF_MASK_CTR_SUBTYPE   = 0x000F0000,
      PERF_MASK_TIME_BASE     = 0x00300000,
      PERF_MASK_CALC_MODS     = 0x0FC00000,
      PERF_MASK_DISPLAY_FLAGS = 0xF0000000
   };
   typedef struct {
      BOOL                      fSupportsInstances;
      long                      MaxInstances;
      PPERF_OBJECT_TYPE         pPOT;
      int                       cbPOT;
      PPERF_COUNTER_DEFINITION  pPCD;
      int                       cbPCD;
      PPERF_INSTANCE_DEFINITION pPID;
      int                       cbPID;
      PPERF_COUNTER_BLOCK       pPCB;
      int                       cbPCB;
      LPWSTR                    pPIN;
      int                       cbPIN;
   } PRFMETRICS, *PPRFMETRICS;


private: // Functions to convert betwen value types
   DWORD   ActivatePrfData(); // Returns last error

   void    CalcPrfMetrics(OBJORD ObjOrd, INSTID InstId,
      PPRFMETRICS pPM, PPRFITM* ppPrfItm = NULL) const;

   PIINDEX CvtIdToPrfItmIndex(BOOL fObjectId, DWORD dwId) const;

   PIINDEX CvtObjIdToPrfItmIndex(OBJID ObjId) const;
   OBJORD  CvtObjIdToObjOrd(OBJID ObjId) const;
   PIINDEX CvtObjOrdToPrfItmIndex(OBJORD ObjOrd) const;
   PPRFITM CvtObjOrdToPrfItm(OBJORD ObjOrd) const;
   PPRFITM CvtObjIdToPrfItm(OBJID ObjId) const;

   PIINDEX CvtCtrIdToPrfItmIndex(CTRID CtrId, int* pnCtrIndexInObj = NULL) const;
   PPRFITM CvtCtrOrdToPrfItm(OBJORD ObjOrd, CTRORD CtrOrd) const;
   OBJORD  CvtCtrIdToObjOrd(CTRID CtrId, int* pnCtrIndexInObj = NULL) const;
   PPRFITM CvtCtrIdToPrfItm(CTRID CtrId) const;

   PBYTE   GetCtr(CTRID CtrId, INSTID InstId) const;
   INSTID  FindFreeInstance(OBJORD ObjOrd) const;
   INSTID  AddInstance(OBJID ObjId, LPCWSTR pszInstName,
      LONG lUniqueId, OBJID ObjIdParent, INSTID InstIdParent);


#ifdef PRFDATA_COLLECT_SUPPORTED
public:  // Static function required to support PrfData collection
   static DWORD Collect(LPWSTR lpValueName, PBYTE* ppbData,
      LPDWORD lpcbTotalBytes, LPDWORD lpNumObjectTypes);

private: // Functions required to support PrfData collection
   void     DetermineObjsToCollect(OBJORD ObjOrd) const;
   DWORD    CollectAllObjs(LPWSTR lpValueName, PBYTE *ppbData,
      LPDWORD lpcbTotalBytes, LPDWORD lpNumObjectTypes) const;
   DWORD    CollectAnObj(OBJORD ObjOrd, PBYTE *ppbData) const;
   DWORD    CalcBytesForPrfObj(OBJORD ObjOrd) const;
   int      HowManyInstancesInUse(OBJORD ObjOrd) const;
   int      CvtInstIdToInUseInstId(OBJORD ObjOrd, INSTID InstId) const;
#endif


private: // static member that points to the 1 instance of this class
   static CPrfData* sm_pPrfData;


private: // Internal data members to support the class
   LPCWSTR  m_pszAppName;     // App name for registry
   DWORD    m_dwFirstCounter; // 1st object/counter index
   DWORD    m_dwFirstHelp;    // 1st object/counter help index
   DWORD    m_dwLastCounter;  // Last object/counter index
   DWORD    m_dwLastHelp;     // Last object/counter help index

   PPRFITM  m_pPrfItms;       // Array of object/counter structures
   PIINDEX  m_nNumPrfItems;   // Number of items in the array
   OBJORD   m_nNumObjects;    // Num objects in array

   HANDLE   m_hfm;            // File mapping for shared data
   PBYTE    m_pPrfData;       // Mapped view of shared data
   mutable  COptex m_Optex;   // Protects access to shared data

private:
   HANDLE   m_hEvent;
public:
   HANDLE   GetUpdateEventHandle() { return m_hEvent; }
};

///////////////////////////////////////////////////////////////////////////////
// Inline functions.  See above for descriptions

inline void CPrfData::Install(LPCWSTR pszDllPathname)
{
   sm_pPrfData->InstallPrfData(pszDllPathname);
}

inline void CPrfData::Uninstall()
{
   sm_pPrfData->UninstallPrfData();
}

inline DWORD CPrfData::Activate()
{
   return(sm_pPrfData->ActivatePrfData());
}

inline BOOL CPrfData::IsValidObjOrd(OBJORD ObjOrd) const
{
   return(chINRANGE(0, ObjOrd, m_nNumObjects - 1));
}

inline BOOL CPrfData::IsValidInstId(OBJORD ObjOrd, INSTID InstId) const
{
   return(IsValidObjOrd(ObjOrd) && chINRANGE(0, InstId,
      (CvtObjOrdToPrfItm(ObjOrd)->MaxInstances ==
         (INSTID) PERF_NO_INSTANCES) ? 0 :
         CvtObjOrdToPrfItm(ObjOrd)->MaxInstances - 1));
}

inline void CPrfData::LockCtrs() const
{
    m_Optex.Enter();
}

inline BOOL CPrfData::TryLockCtrs() const
{
    return(m_Optex.TryEnter());
}

inline void CPrfData::UnlockCtrs() const
{
    m_Optex.Leave();
}

inline CPrfData::INSTID CPrfData::AddInstance(OBJID ObjId,
    LONG lUniqueId, OBJID ObjIdParent, INSTID InstIdParent)
{
   return(AddInstance((OBJID) ObjId, NULL,
      lUniqueId, ObjIdParent, InstIdParent));
}

inline CPrfData::INSTID CPrfData::AddInstance(OBJID ObjId,
   LPCWSTR pszInstName, OBJID ObjIdParent, INSTID InstIdParent) {
   return(AddInstance((OBJID) ObjId, pszInstName,
      PERF_NO_UNIQUE_ID, ObjIdParent, InstIdParent));
}

inline LONG& CPrfData::GetCtr32(CTRID CtrId, INSTID InstId) const
{
   // Make sure the caller wants the right-size for this counter
   chASSERT((CvtCtrIdToPrfItm(CtrId)->dwCounterType & PERF_MASK_SIZE_FLD)
      == PERF_SIZE_DWORD);
   return(* (PLONG) GetCtr(CtrId, InstId));
}

inline __int64& CPrfData::GetCtr64(CTRID CtrId, INSTID InstId) const
{
   // Make sure the caller wants the right-size for this counter
   chASSERT((CvtCtrIdToPrfItm(CtrId)->dwCounterType & PERF_MASK_SIZE_FLD)
      == PERF_SIZE_LARGE);
   return(* (__int64*) GetCtr(CtrId, InstId));
}

inline const DM_COUNTINFO* CPrfData::GetDmCtr(CTRID CtrId) const
{
    return &CvtCtrIdToPrfItm(CtrId)->dmci;
}

inline CPrfData::PIINDEX CPrfData::CvtObjIdToPrfItmIndex(OBJID ObjId) const
{
   return((CPrfData::PIINDEX) CvtIdToPrfItmIndex(TRUE, (DWORD) ObjId));
}

inline CPrfData::PPRFITM CPrfData::CvtObjIdToPrfItm(OBJID ObjId) const
{
   return(&m_pPrfItms[(int) CvtObjIdToPrfItmIndex(ObjId)]);
}

inline CPrfData::PPRFITM CPrfData::CvtCtrIdToPrfItm(CTRID CtrId) const
{
   return(&m_pPrfItms[(int) CvtCtrIdToPrfItmIndex(CtrId)]);
}

inline void CPrfData::AssignDynamicPerfItems(PPRFITM pPrfItms)
{
   m_pPrfItms = pPrfItms;
   VerifyPrfItemTable();
}

extern CPrfData g_PrfData;
