/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1996-1999 Microsoft Corporation.
*
* Component:
*
* File: capsize.cpp
*
* File Comments:
*
*
***********************************************************************/

#include "capsize.h"
#include "vulcancmdline.h"
#include "vulcanapi.h"

#include <direct.h>
#include <sys/stat.h>

bool  fDlp;                            // Use DLP style instrumentation
char  *szAsm;                          // Asm filename
char  *szCmd;                          // Cmdfile name
char  *szDll;                          // Name of DLL containing callback
char  *szEntry;                        // Name of callback entry point
char  *szO;                            // Output file name
char  *szPdb;                          // Name of pdb file.
char  *szCmdCmd;                       // Cmd file specified on /cmd
char  *szCmdAsm;                       // Asm file specified on /asm
char  *szCmdPdb;                       // Pdb file specified on /pdb
char  *szCmdO;                         // Output file specified on /o
char  *szOrgDir;                       // Directory for input files
char  *szCapDir;                       // Directory for Output files

char  *szValidFlag;
long  cbNullStub = 0;

bool  fnocmd;                          // True if /nodefaultcmd specified
bool  fout;                            // True if /o specified
bool  fcmd;                            // True if /cmd specified
bool  fasm;                            // True if /asm specified
bool  fpdb;                            // True if /pdb specified
bool  fCompact;                        // True if /compact specified
bool  fBlockInfo;                      // True if /fBlockInfo specified
bool  fWriteFast;                      // True if /writefast specified
bool  fDoHelp;                         // True if /?
bool  fVerbose = false;
bool  fTerse = false;
bool  fNoLogo = false;
int   iArg;                            // First non-option argument

char  *szImage;

VComp *pComp;


   // Type                 OptionName     fOptReq  pvValue

const CMDLINE::OPTDEF rgoptdef[] =
{
   { CMDLINE::argtHelp,       "?",           false,   &fDoHelp                   },
   { CMDLINE::argtOptString,  "asm",         false,   &szCmdAsm,        &fasm    },
   { CMDLINE::argtOptString,  "asmemit",     false,   &szCmdAsm,        &fasm    },
   { CMDLINE::argtNil,        "blockinfo",   false,   &fBlockInfo,               }, // not in usage
   { CMDLINE::argtString,     "cmd",         false,   &szCmdCmd,        &fcmd    },
   { CMDLINE::argtNil,        "compact",     false,   &fCompact                  },
   { CMDLINE::argtString,     "dll",         false,   &szDll                     },
   { CMDLINE::argtNil,        "dlp",         false,   &fDlp                      },
   { CMDLINE::argtString,     "entry",       false,   &szEntry                   },
   { CMDLINE::argtHelp,       "help",        false,   &fDoHelp                   },
   { CMDLINE::argtNil,        "nocmd",       false,   &fnocmd                    },
   { CMDLINE::argtInt,        "nullstub",    false,   &cbNullStub,               },
   { CMDLINE::argtOptString,  "o",           false,   &szCmdO ,         &fout    },
   { CMDLINE::argtOptString,  "pdb",         false,   &szCmdPdb,        &fpdb    },
   { CMDLINE::argtString,     "validflag",   false,   &szValidFlag               },
   { CMDLINE::argtNil,        "verbose"  ,   false,   &fVerbose                  },
   { CMDLINE::argtNil,        "writefast",   false,   &fWriteFast                },
   { CMDLINE::argtNil,        "terse"    ,   false,   &fTerse                    },
   { CMDLINE::argtNil,        "q"    ,       false,   &fTerse                    },
   { CMDLINE::argtNil,        "nologo"   ,   false,   &fNoLogo                   },
   { CMDLINE::argtEnd,        NULL,          false,   &iArg                      },
};


const CMDFILEDEF::CMDDEF CMDFILEDEF::rgcmddef[] =
{
   { cmdtNil, "capsize.exclude",   (PFNCMD) &Exclude            },
   { cmdtEnd, NULL,                NULL                         }
};


const char *RgszCapsizeCmdDict [] =
{

   "capsize.exclude",       // CAPSIZE

   NULL                 // nothing comes after ME!
};

SZDICTIONARY gszdictCmd(RgszCapsizeCmdDict);

class RGSZ
{
public:
  RGSZ()
  {
    m_rgpsz = NULL;
    m_cpsz = 0;
    m_cpszmax = 0;
  }

  ~RGSZ()
  {
    for (int i = 0 ; i < m_cpsz ; i++)
    {
      free(m_rgpsz[i]);
    }

    free(m_rgpsz);
  }

  void Insert(const char *sz)
/***********************************************************************
* Add an element to the list of "excluded" functions
***********************************************************************/
  {
    if (m_cpsz >= m_cpszmax)
    {
      m_cpszmax = m_cpsz + (m_cpsz >> 1) + 100;
      m_rgpsz = (char **) realloc(m_rgpsz, m_cpszmax * sizeof(m_rgpsz));
    }
    m_rgpsz[m_cpsz++] = _strdup(sz);
  }

  int CpSz()
  {
        return(m_cpsz);
  }

  char ** Prgpsz()
  {
        return(m_rgpsz);
  }

private:
  char **m_rgpsz;
  int  m_cpsz;
  int  m_cpszmax;
};


RGSZ rgsz;

int __cdecl SortByIncreasingFunctionNames(const void *pv1, const void *pv2)
{
    return(strcmp(*((char **)pv1), *(char **)(pv2)));
}


void Error(const unsigned flags, const char *fmt, ...)
{
    char buf[512];

    va_list items;
    va_start(items, fmt);

    vsprintf(buf, fmt, items);
    va_end(items);

    VErr verr(buf, flags);
    verr.DoErrHandler();
}

void Usage(int iExit)
{
   printf("Microsoft (R) Vulcan IceCAP Preparation Tool\n");
   printf("Usage: CAPSIZE [options] Executable\n\n"
          "Options:\n\n"
          "/?                  Display this help\n"
          "/asmemit [filename] Write a disassembly listing for output binary\n"
          "/cmd     filename   Read the specified command file\n"
          "/compact            Use compact bblocks\n"
          "/dll     dllname    Name of DLL containing entry (default = ICAP.DLL/ICECAP.DLL)\n"
          "/dlp                Use DLP style profiling (IceCAP 4.0)\n"
          "/entry   name       Name of callback entry point (default = _penter2)\n"
          "/help               Display this help\n"
          "/nocmd              Do not use a command file\n"
          "/nologo             Do not display a banner message\n"
          "/o       filename   Output binary name\n"
          "/pdb     filename   Pdb output filename\n"
          "/q                  Terse output mode\n"
          "/verbose            Verbose output mode\n"
          "/writefast          Generate output file with VComp->WriteFast intead of Write\n"
          );

   exit(iExit);
}


void InitParams(int argc, char *argv[])
{
   char szDrive[_MAX_DRIVE];
   char szDir[_MAX_DIR];
   char szFname[_MAX_FNAME];
   char szExt[_MAX_EXT];
   char szBuf[5000];
   struct _stat rgStatBuf;

   bool fSuccess = true;                // avoid uninitialized var warning.

   /***********************************************************************
   * Call the Command Line processor to parse the argyuments using rgoptdef
   * as the strucuture to define how to handle the various arguements.
   ***********************************************************************/

   try
   {
      fSuccess = CMDLINE::FProcessArgs("CAPSIZE",
                                       argc,
                                       (const char **) argv,
                                       rgoptdef);
   }

   /***********************************************************************
   * Handle the various error situations and validate correct usage
   ***********************************************************************/

   catch (...)
   {
      Usage(1);
   }

   if (!fSuccess)
   {
      Usage(1);
   }

   if (fDoHelp)
   {
      Usage(0);
   }

   if (iArg >= argc)
   {
      Usage(1);
   }

   szImage = argv[iArg++];

   if (iArg != argc)
   {
      Usage(1);
   }

   if ((cbNullStub != 0) && (szDll != NULL))
   {
      Error(VErr::FLG_FATAL, "Cannot specify /Nullstub and /DLL");

      exit(1);
   }

   if ((szValidFlag != NULL) && (cbNullStub == 0))
   {
      Error(VErr::FLG_FATAL,
            "Must specify Nullstub when you specify ValidFlag");

      exit(1);
   }

   if (cbNullStub == 0)
   {
      if (szDll == NULL)
      {
         if (fDlp)
         {
            szDll = "ICECAP.DLL";
         }

         else
         {
            szDll = "ICAP.DLL";
         }
      }

      else
      {
         char szDrive[_MAX_DRIVE];
         char szDir[_MAX_DIR];

         _splitpath(szDll, szDrive, szDir, NULL, NULL);

         if ((szDrive[0] != '\0') || (szDir[0] != '\0'))
         {
            Error(VErr::FLG_FATAL,
                  "Cannot specify a pathname for with /NULLSTUB");

            exit(1);
         }
      }
   }

   if (szEntry == NULL)
   {
      szEntry = "_penter2";
   }

   /***********************************************************************
   * Display the startup banner for this utility
   ***********************************************************************/

   if (!fNoLogo)
      printf("Microsoft (R) Vulcan IceCAP Instrumentation Preparation Tool\n");

   if (fout)
   {
      _splitpath(szCmdO, szDrive, szDir, szFname, szExt);
      if (strlen(szDir) + strlen(szDrive))
      {
         szCapDir = new char[strlen(szDir) + strlen(szDrive) + 1];
         sprintf(szCapDir, "%s%s", szDrive, szDir);
         szCapDir[strlen(szCapDir)-1] = '\0';

         if ((strlen(szDir) > 1) && (_stat(szCapDir, &rgStatBuf)))
         {
            char szbuf[512 + _MAX_DIR + _MAX_DRIVE];
            sprintf(szbuf,
                    "The /o option specifies a directory '%s' "
                    "which does not exist.\n",
                    szCapDir);

            Error(VErr::FLG_FATAL, szbuf);
         }

         sprintf(szCapDir, "%s%s", szDrive, szDir);
      }
      else
      {
         szCapDir = new char[3];
         strcpy(szCapDir, ".\\");
      }
   }

   else
   {
      szCapDir = new char[sizeof(DEFAULTCAPDIR)+1];
      strcpy(szCapDir, DEFAULTCAPDIR);
      szCapDir[strlen(szCapDir) - 1] = '\0';

      // Create the Capsize Directory if it doesn't yet exist

      size_t staterr = _stat(szCapDir, &rgStatBuf);
      if (staterr || (rgStatBuf.st_mode & _S_IFREG))
      {
         if(_mkdir(szCapDir))
         {
            throw VErr("Unable to create capsize directory", VErr::FLG_FATAL);
         }
      }
      strcpy(szCapDir, DEFAULTCAPDIR);
   }

   _splitpath(szImage, szDrive, szDir, szFname, szExt);

   // Setup the org and Capsize directory names


   // The orgdir is constructed from the name of input binary.

   if (strlen(szDir))
   {
      szOrgDir = new char[strlen(szDir) + strlen(szDrive) + 1];
      sprintf(szOrgDir, "%s%s", szDrive, szDir);
   }
   else
   {
      szOrgDir = new char[3];
      strcpy(szOrgDir, ".\\");
   }

   if (!fcmd)
   {
      if (fnocmd)
      {
         szCmd = NULL;
      }
      else
      {
         sprintf(szBuf, "%s%s%s%s", szOrgDir, szFname, szExt, DEFAULTCMDEXT);
         szCmd = new char[strlen(szBuf) + 1];
         strcpy(szCmd, szBuf);

         if (_stat(szCmd, &rgStatBuf))
         {
            szCmd = NULL;
         }
      }
   }

   else
   {
      szCmd = szCmdCmd;
   }


   // Setup file names for the pdb, cmd, asm and output files

   if (fout)
   {
      _splitpath(szCmdO, szDrive, szDir, szFname, szExt);
   }

   if (fasm)
   {
      if (szCmdAsm)
      {
         szAsm = szCmdAsm;
      }
      else
      {
         sprintf(szBuf, "%s%s%s%s", szCapDir, szFname, szExt, DEFAULTASMEXT);
         szAsm = new char[strlen(szBuf) + 1];
         strcpy(szAsm, szBuf);
      }
   }

   if (!fpdb)
   {
      sprintf(szBuf, "%s%s%s%s", szCapDir, szFname, szExt, DEFAULTPDBEXT);
      szPdb = new char[strlen(szBuf) + 1];
      strcpy(szPdb, szBuf);
   }
   else
   {
      char szCmdPdbDrive[_MAX_DRIVE];
      char szCmdPdbDir[_MAX_DIR];
      char szCmdPdbFname[_MAX_FNAME];
      char szCmdPdbExt[_MAX_EXT];

      _splitpath(szCmdPdb, szCmdPdbDrive, szCmdPdbDir, szCmdPdbFname, szCmdPdbExt);
      if (strlen(szCmdPdbDir) + strlen(szCmdPdbDrive))
      {
         char *szPdbDir = new char[strlen(szCmdPdbDir) + strlen(szCmdPdbDrive) + 1];
         sprintf(szPdbDir, "%s%s", szCmdPdbDrive, szCmdPdbDir);
         szPdbDir[strlen(szPdbDir) -1] = '\0';

         if ((strlen(szCmdPdbDir) > 1) && (_stat(szPdbDir, &rgStatBuf)))
         {
            char szbuf[512 + _MAX_DRIVE +_MAX_DIR];
            sprintf(szbuf, "The /pdb option specifies a directory '%s' which does not exist.\n", szPdbDir);
            Error(VErr::FLG_FATAL, szbuf);
         }
      }

      szPdb = szCmdPdb;
   }

   if (!fout)
   {
      sprintf(szBuf, "%s%s%s", szCapDir, szFname, szExt);
      szO = new char[strlen(szBuf) + 1];
      strcpy(szO, szBuf);
   }
   else
   {
      szO = szCmdO;
   }

}


void ReportCapFiles()
{
   if (!fTerse)
   {
      char szFullPath[_MAX_PATH];

      fprintf(stdout, "Input Files:\n");
      _fullpath(szFullPath, szImage, _MAX_PATH);
           fprintf(stdout, "\tInput Binary:\t%s\n", szFullPath);

           if (szCmd)
           {
              _fullpath(szFullPath, szCmd, _MAX_PATH);
              fprintf(stdout, "\tCommand File:\t%s\n", szFullPath);
           }

      fprintf(stdout, "Output Files:\n");
      _fullpath(szFullPath, szO, _MAX_PATH);
      fprintf(stdout, "\tOutput Binary:\t%s\n", szFullPath);

      if ((szPdb != NULL) && strlen(szPdb))
      {
         _fullpath(szFullPath, szPdb, _MAX_PATH);
              fprintf(stdout, "\tPDB File:\t%s\n", szFullPath);
      }

      if (fasm)
      {
         _fullpath(szFullPath, szAsm, _MAX_PATH);
              fprintf(stdout, "\tAsm File:\t%s\n", szFullPath);
      }
   }
}


int __cdecl cmpsz(const void *pv1, const void *pv2)
{
   return(strcmp(*((char **)pv1), *((char **)pv2)));
}


bool Include(const char *sz)
{
   /***********************************************************************
   * Utility to see if the named funtion is in the list of
   * functions to be excluded.
   ***********************************************************************/

   char *psz = (char *) bsearch(&sz, rgsz.Prgpsz(), rgsz.CpSz(), sizeof(char *), cmpsz);

   if (psz == NULL)
   {
      return(true);
   }
   else
   {
      return(false);
   }
}


void CMDFILEDEF::Exclude(unsigned, const char *sz)
{
   /***********************************************************************
   * Every capsize.exclude item is checked to make sure it is a valid
   * symbol in the executable and then, if valid, is added to a list to exclude it
   * from instrumentation.
   ***********************************************************************/

   if ((pComp->BlockFromSymName(sz)) == NULL)
   {
      char szbuf[512];
      sprintf(szbuf, "Undefined exclude symbol '%s' in command file '%s'.\n", sz, szCmd);
      Error(VErr::FLG_WARNING, szbuf);
   }
   else
   {
      rgsz.Insert(sz);
   }
}


bool HasJumpToBlock(VBlock *pTo, VProc *pProc)
{
   for (VBlock *pFrom = pProc->FirstAllBlock();
        pFrom != NULL;
        pFrom = pFrom->NextAll())
   {
      if (!pFrom->IsDataBlock())
      {
         if (pFrom->BlockTarget() == pTo)
         {
            return true;
         }
      }
      else
      {
         for (VRelocIter relocs(pFrom->FirstReloc());
              !relocs.Done();
              relocs.Next())
         {
            if (relocs.Curr()->RelocTarget() == pTo)
            {
               return true;
            }
         }
      }
   }

   return false;
}


bool HasFallToBlock(VBlock *pTo, VProc *pProc)
{
   VBlock *pFrom = pTo->PrevAll();

   if (pFrom == NULL)
   {
      // Perhaps a fall from last block of previous block

      VProc *pProcFrom = pProc->PrevAll();

      if (pProcFrom)
      {
         pFrom = pProcFrom->LastAllBlock();
      }
   }

   if (pFrom && !pFrom->IsDataBlock())
   {
      switch (pFrom->BlockTerminationType())
      {
         // It is far better to be safe than sorry

         case BlockTermUnknown:
         case BlockTermFallThrough:
         case BlockTermTrap:
         case BlockTermTrapCond:
         case BlockTermBranchCond:
         case BlockTermBranchCondIndirect:
         case BlockTermCall:
         case BlockTermCallIndirect:
         case BlockTermCallCond:
         case BlockTermCallCondIndirect:
         case BlockTermNop:
             return true;

         case BlockTermBranchIndirect:
         case BlockTermBranch:
         case BlockTermRet:
             return false;
      }
   }

   return false;
}

void MoveAllInstructions(VBlock *pFrom, VBlock *pTo)
{
   VInst *pInstPrev;

   for (VInst *pInst = pFrom->LastInst(); pInst; pInst = pInstPrev)
   {
      pInstPrev = pInst->Prev();

      pInst->Remove();
      pTo->InsertFirstInst(pInst);
   }
}


void RedictLocalJumps(VProc *pProc, VBlock *pFrom, VBlock *pTo)
{
   for (VBlock *pBlk = pProc->FirstAllBlock(); pBlk; pBlk = pBlk->NextAll())
   {
      if (pBlk->BlockTarget() == pFrom)
      {
         pBlk->SetBlockTarget( pTo );
      }
      else
      {
         for (VRelocIter relocs(pBlk->FirstReloc());
              !relocs.Done();
              relocs.Next())
         {
            VReloc *pReloc = relocs.Curr();

            if (pReloc->RelocTarget() == pFrom)
            {
               pReloc->SetRelocTarget(pTo);
            }
         }
      }
   }
}



void CapInst(VProg* vprog)
{
   /***********************************************************************
   * In CapInst() the first thing that is checked is if a pdb file can be written
   * and if a pdb file must be written.  If a pdb file is required but not
   * specified then a fatal error is displayed.  If a pdb file is specified but
   * cannot be written a warning is displayed.  Next the output name of the
   * executable and the pdb file is specified and the command file name (if one
   * was specified) is supplied.  These operations must be performed BEFORE an IR
   * is built by Vulcan and the next thing we do will build the IR,  so they have
   * to be taken care of first.
   ***********************************************************************/

   VProtoList *DlpEntryProtolist = VProtoList::CreateProtoList();
   VProtoList *DlpExitProtolist = VProtoList::CreateProtoList();

   for (pComp = vprog->FirstComp(); pComp; pComp = pComp->Next() )
   {
      if (pComp->CanWritePdb() && (szPdb == NULL))
      {
         Error(VErr::FLG_FATAL,
               "This executable (VC6 or higher) requires the /pdb option.");
      }

      if (!pComp->CanWritePdb() && (szPdb != NULL))
      {
         /*********************************************************************************
         * If the /pdb switch was explicitly set and the binary is not VC6 or greater
         * then produce a warning that the PDB cannot be written.   If the /pdb switch
         * was not explicitly set,  then the szPdb contains a pdb name generated as the
         * default name and since it is not VC6 or greater the pdb name should be ignored.
         *********************************************************************************/

         if(!fpdb)
         {
            szPdb = NULL;
         }
         else
         {
            Error(VErr::FLG_WARNING,
                  "The option '/pdb' is not required for linking VC50 or "
                  "earlier binaries - No PDB file generated.\n");
         }
      }

      if (pComp->IsEmittedByVulcan())
      {
         if (pComp->IsEmittedByBBT())
         {
            if (pComp->IsInstrumentedByBBT())
            {
               Error(VErr::FLG_FATAL,
                     "CAPSIZE cannot process a binary that was "
                     "emitted by the BBT tool BBINSTR.");
            }
         }

         else if (!pComp->IsRereadable())
         {
            Error(VErr::FLG_FATAL,
                  "CAPSIZE can only process an original binary, a BBOPT "
                  "binary or a DLLMERGE binary.");
         }

      }

      ReportCapFiles();
      size_t cInstProc = 0;
      size_t cExclude = 0;
      if (szCmd != NULL)
      {
         pComp->SetCommandFile(szCmd, &gszdictCmd);
      }

      /***********************************************************************
      * Next the command file is processed to interpret the commands for Capsize.  In
      * this case the "capsize.exclude" command is processed.  The command file
      * interpreter is managed by the RgszCapsizeCmdDict data structure.
      ***********************************************************************/

      if (szCmd != NULL)
      {
         CMDFILEDEF cmdfiledef;
         cmdfiledef.ProcessFile(szCmd, CMDFILEDEF::rgcmddef, &gszdictCmd);

         qsort((void *)rgsz.Prgpsz(), rgsz.CpSz(), sizeof(char *), SortByIncreasingFunctionNames);
      }

      VBlock *vbblock = pComp->CreateImport(szDll, szEntry);

      /***********************************************************************
      * Next an import is created to the ICAP.DLL and a JMP to that import is built.
      ***********************************************************************/

      // Create a thunk to to the import block

      VInst *vinst = VInst::Create(COp::JMP, VAddress::Create(vbblock));
      VBlock *vbblockImport = VBlock::CreateCodeBlock(pComp);
      unsigned int blkidIceCAPImport = vbblockImport->BlockId();
      vbblockImport->InsertFirstInst(vinst);
      VProc *vprocNew = VProc::Create(pComp, (char *) NULL); //->NewProc();
      pComp->InsertLastProc(vprocNew);
      vprocNew->InsertFirstBlock(vbblockImport);
      VProto* DlpEntryProto = NULL;
      VProto* DlpExitProto = NULL;

      if (fDlp)
      {
         DlpEntryProto = DlpEntryProtolist->CreateProto(pComp,
                                                        szDll,
                                                        "_CAP_EnterFunction()",
                                                        FALSE);

         DlpExitProto = DlpExitProtolist->CreateProto(pComp,
                                                      szDll,
                                                      "_CAP_ExitFunction()",
                                                      FALSE);
      }

      if (fVerbose || fTerse)
      {
         if (fDlp)
         {
            printf("Instrumenting: %s\n"
                   "         with: %s:%s\n"
                   "           to: %s\n", szImage, "Dlp", szDll, szO);
         }
         else
         {
            printf("Instrumenting: %s\n"
                   "         with: %s:%s\n"
                   "           to: %s\n", szImage, szEntry, szDll, szO);
         }

         if (fVerbose)
            printf("\nProcedures: \n");
      }


      /***********************************************************************
      * Instrumentation is added at each entry point
      ***********************************************************************/

      for (VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next() )
      {
         VBlock *vbblock = pProc->FirstBlock();
         char szName[4096];

         if (!vbblock->SymName(szName, sizeof(szName), pComp, 0))
         {
            continue;
         }

         if (strcmp(szName, "_setjmp") == 0)
         {
            continue;
         }

         if (strcmp(szName, "_setjmp3") == 0)
         {
            continue;
         }

         if (strcmp(szName, "__setjmp3") == 0)
         {
            // Excel copied the CRT implementation to their own source tree
            // and added an extra leading underscore to the proc name because
            // they don't rely on MASM to decorate names and add an underscore
            // to public symbol names.

            continue;
         }

         if (strncmp(szName, "__tailMerge_", 12) == 0)
         {
            continue;
         }

         if (vbblock->BlockId() == blkidIceCAPImport)
         {
            // We don't want to instrument our insert import thunk

            continue;
         }

         if (Include(szName))
         {
             cInstProc++;

             if (fVerbose)
             {
                     printf("\t%s\n",szName);
             }

             if (fDlp)
             {
                     DlpEntryProto->AddCall(pProc, BEFORE);
                     DlpExitProto->AddCall(pProc, AFTER);
             }
             else
             {
                 // We need to add _penter2 calls at the beginning of each
                 // entry point into the procedure.  However, we need to
                 // avoid calling _penter2 when we fall through or jump to
                 // these entry points.

                 for ( vbblock = pProc->FirstBlock();
                       vbblock;
                       vbblock = vbblock->Next())
                 {

                     // First, handle jumps to other procedures by patching
                     // the target if it hasn't already been done.

                     BlockTermType trmt = vbblock->BlockTerminationType();

                     if ( trmt == BlockTermBranch  ||
                          trmt == BlockTermBranchCond  ||
                          trmt == BlockTermBranchCondIndirect ||
                          trmt == BlockTermBranchIndirect )
                     {
                        VBlock *pBlkTarget = vbblock->BlockTarget();
                        if (pBlkTarget && pBlkTarget->IsEntryBlock())
                        {
                           VProc *pProcTarget = pBlkTarget->ParentProc();
                           if (pProcTarget != pProc)
                           {
                              // We have a branch or jump to another procedure.
                              // We need to make sure that we do not hit
                              // _penter2 when going into that proc.  There are
                              // four cases to deal with:

                              VInst *pInstInstr = pBlkTarget->FirstInst();

                              if (pInstInstr)
                              {
                                 COp::EOp opInstr = pInstInstr->Opcode();
                                 if (opInstr == COp::CALL &&
                                     pInstInstr->BlockTarget() == vbblockImport)
                                 {
                                    // We have already added instrumentation

                                    if (pInstInstr->Next() == NULL)
                                    {
                                       // Case 1: This entry point has already
                                       // been patched and instrumented.

                                       vbblock->SetBlockTarget(pBlkTarget->Next());
                                    }
                                    else
                                    {
                                       // Case 2: This entry point has been
                                       // instrumented, but not patched.

                                       VBlock *pOldInsts = VBlock::CreateCodeBlock(pComp);
                                       pBlkTarget->InsertNext(pOldInsts);

                                       pInstInstr->Remove();
                                       MoveAllInstructions(pBlkTarget, pOldInsts);
                                       pBlkTarget->InsertFirstInst(pInstInstr);

                                       vbblock->SetBlockTarget(pOldInsts);
                                    }
                                 }
                                 else
                                 {
                                    // Case 3: this proc has not been
                                    // instrumented and has not been patched.

                                    VBlock *pOldInsts = VBlock::CreateCodeBlock(pComp);
                                    pBlkTarget->InsertNext(pOldInsts);

                                    MoveAllInstructions(pBlkTarget, pOldInsts);
                                    vbblock->SetBlockTarget(pOldInsts);
                                 }
                              }
                              else
                              {
                                 // Case 4: this proc has been patched, but not
                                 // instrumented.

                                 vbblock->SetBlockTarget( pBlkTarget->Next() );
                              }
                           }
                        }
                     }

                     // Now actually do the instrumentation of the entry blocks

                     if (vbblock->IsEntryBlock())
                     {
                         VBlock *pOldInsts = NULL;
                         bool fJumpedTo = HasJumpToBlock(vbblock, pProc);
                         bool fFallTo   = HasFallToBlock(vbblock, pProc);

                         if (fJumpedTo || fFallTo)
                         {
                            if (NULL != vbblock->FirstInst())
                            {
                               // We need to patch the entry block
                               pOldInsts = VBlock::CreateCodeBlock(pComp);
                               vbblock->InsertNext(pOldInsts);

                               MoveAllInstructions(vbblock, pOldInsts);
                            }
                            else
                            {
                               // We have already patched this entry
                               pOldInsts = vbblock->Next();
                            }

                            RedictLocalJumps(pProc, vbblock, pOldInsts);
                         }

                         if (fFallTo)
                         {
                             // Add a jump to handle the fall into as well
                             // without calling _penter2.

                             VBlock *pDummy = VBlock::CreateCodeBlock(pComp);
                             vbblock->InsertPrev(pDummy);

                             // Add an instruction to skip over the _penter2 block

                             pDummy->InsertFirstInst( VInst::Create(COp::JMP,
                                                      pOldInsts) );
                         }

                         // Add the instrumentation instruction

                         vbblock->InsertFirstInst(VInst::Create(COp::CALL,
                                                                vbblockImport));
                     }
                 }
             }
         }

         else
         {
            cExclude++;
         }
      }

      if (fVerbose || fTerse)
      {
         printf("%u procedures have been instrumented.\n", cInstProc);

         if (cExclude > 0)
         {
              printf("%u procedures have been excluded from instrumentation.", cExclude);
         }
      }

      /**********************************************************************
      * The vprog object is committed and written.  The write will create the
      * specified PDB and executables that were named in the OutputPDBName
      * and OutputName methods.
      ***********************************************************************/

      if (fDlp)
      {
         DlpEntryProtolist->Commit();
         DlpExitProtolist->Commit();
      }

      if ( fWriteFast )
      {
         pComp->WriteFast(szO, szPdb);
      }

      else
      {
         pComp->Write(szO, szPdb);
      }

      if (szAsm != NULL)
      {
         pComp->PrintAsm(szAsm);
      }
   }
}


int RealMain(int argc, char *argv[])
{
   /***********************************************************************
   *
   * RealMain() uses the Vulcan supplied command line interpreter to interpret the
   * arguments (inside InitParams) and then makes sure that output file selected
   * for the instrumented executable is not the same as the input executable.
   * Finally the constructor for the Vulcan program is called and then the Capsize
   * process can begin.
   *
   ***********************************************************************/

   InitParams(argc, argv);

   if (CMDLINE::FSamePath(szO, szImage))
   {
      Error(VErr::FLG_FATAL,"Capsized binary will overwrite input image file");
   }

   DWORD openFlags = 0;
   if ( fCompact )
   {
      openFlags |= Open_CompactBlocks;
   }
   VProg *vprog = VProg::Open( szImage, szCmd, openFlags );

   // Currently only X86 PE binaries are supported.
   //
   if (vprog->FirstComp()->InputType() != VComp::PE)
   {
      Error(VErr::FLG_FATAL, "Unsupported architecture.");
      exit(1);
   }

   CapInst(vprog);

   // To save time, we don't destroy the program object
   // vprog->Destroy();

   // Some info about the block types.
   if ( fBlockInfo )
   {
      VBlock::PrintBlockStats( "" );
   }

   return(0);
}


int __cdecl main(int argc, char *argv[])
{
   /***********************************************************************
   *
   * main() establishes the name of the utility as CAPSIZE for the default
   * error handler and then goes on to try running the real program.  If a
   * throw occurs whiles processing with Vulcan,  the error is flagged
   * as fatal and the default error handler is called to display the message.
   *
   ***********************************************************************/

   VErr::SetErrHandler("CAPSIZE");

   try
   {
      return RealMain(argc, argv);
   }
   catch (VErr &verr)
   {
      verr.SetFatal();
      verr.DoErrHandler();
   }

   return 1;
}
