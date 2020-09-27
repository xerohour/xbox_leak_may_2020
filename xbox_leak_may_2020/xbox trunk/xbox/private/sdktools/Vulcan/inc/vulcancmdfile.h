/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1994-1999 Microsoft Corporation.
*
* Component:
*
* File: vulcancmdfile.h
*
* File Comments:
*
*
***********************************************************************/

#pragma once

class VULCANDLL SZDICTIONARY
{
public:
   SZDICTIONARY(const char * const *);
   ~SZDICTIONARY();

   const char *Find(const char *sz) const;

private:
   static int __cdecl cmpsz(const void *, const void *);

   const char **m_rgsz;
   size_t       m_csz;
};


class VULCANDLL CMDFILE
{
public:
            typedef void (CMDFILE::*PFNCMD)(unsigned, const char *);

            enum CMDT { cmdtNil, cmdtLine, cmdtRptExist, cmdtEnd };

            struct CMDDEF
            {
               CMDT m_cmdt;
               char *m_szCmdName;
               PFNCMD m_pfncmd;
            };

            void ProcessFile(const char *, const CMDDEF *, SZDICTIONARY * = NULL);

private:
   static   const CMDDEF *PcmddefLookup(const CMDDEF *, const char *);
};
