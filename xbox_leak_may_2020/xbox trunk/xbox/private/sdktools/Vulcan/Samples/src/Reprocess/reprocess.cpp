/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1996-2000 Microsoft Corporation.
*
* Component:
*
* File: reprocess.cpp
*
* File Comments:
* Demonstrates how to generate a Re-Vulcanizable binary.
*
* Vulcan can now re-process a Vulcan processed executable.  To enable this
* functionality, pass the value TRUE for the fRereadable parameter to
* VComp::Write.  When fRereadable is TRUE, Write will create a new image which
* contains a copy of the IR.  The IR is stored in the output PDB file (VC 6.x or
* greater), or in the generated output image (VC 5.x).  Storing the IR allows
* another Vulcan application to determine the block level information of this new
* image.  When another Vulcan application opens the new image, the stored IR will
* be detected and used.
*
*
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <sys/stat.h>

#include "vulcanapi.h"
#include "vulcancmdfile.h"
#include "vulcancmdline.h"

#define DEFAULTOUTDIR "REPROCESS\\"
#define DEFAULTPDBEXT ".pdb"
#define DEFAULTCMDEXT ".bbtcmd"
#define DEFAULTASMEXT ".asm"

/***********************************************************************
* fRereadable is passed in as the fRereadable parameter value to VComp::Write
* When this is true, the IR will be saved in the output image's PDB (VC6x),
* or the output image (VC5x). You can override the default value of true by
* using the /noreprocess switch.
*
***********************************************************************/
bool  fRereadable = true;               // True to generate a Re-Vulcanizable image

char  *szAsm;                          // Asm filename
char  *szCmd;                          // Cmdfile name
char  *szO;                            // Output file name
char  *szPdb;                          // Name of pdb file.
char  *szCmdCmd;                       // Cmd file specified on /cmd
char  *szCmdAsm;                       // Asm file specified on /asm
char  *szCmdPdb;                       // Pdb file specified on /pdb
char  *szCmdO;                         // Output file specified on /o
char  *szOrgDir;                       // Directory for input files
char  *szOutDir;                       // Directory for Output files
char  *szImage;                        // Input Image

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
bool  fNoChange = false;
bool  fNoReprocess = false;
bool  fNoCount = false;
bool  fPrintNop = false;
bool  fSkipObsolete = true;
int   iArg;                            // First non-option argument

//
// Command line options
//    Type                 OptionName     fOptReq  pvValue
//
const CMDLINE::OPTDEF rgoptdef[] =
{
   { CMDLINE::argtHelp,       "?",           false,   &fDoHelp                   },
   { CMDLINE::argtOptString,  "asm",         false,   &szCmdAsm,        &fasm    },
   { CMDLINE::argtOptString,  "asmemit",     false,   &szCmdAsm,        &fasm    },
   { CMDLINE::argtNil,        "blockinfo",   false,   &fBlockInfo,               }, // not in usage
   { CMDLINE::argtString,     "cmd",         false,   &szCmdCmd,        &fcmd    },
   { CMDLINE::argtNil,        "compact",     false,   &fCompact                  },
   { CMDLINE::argtHelp,       "help",        false,   &fDoHelp                   },
   { CMDLINE::argtNil,        "nocmd",       false,   &fnocmd                    },
   { CMDLINE::argtOptString,  "o",           false,   &szCmdO ,         &fout    },
   { CMDLINE::argtOptString,  "pdb",         false,   &szCmdPdb,        &fpdb    },
   { CMDLINE::argtNil,        "verbose"  ,   false,   &fVerbose                  },
   { CMDLINE::argtNil,        "writefast",   false,   &fWriteFast                },
   { CMDLINE::argtNil,        "terse"    ,   false,   &fTerse                    },
   { CMDLINE::argtNil,        "q"    ,       false,   &fTerse                    },
   { CMDLINE::argtNil,        "nologo"   ,   false,   &fNoLogo                   },
   { CMDLINE::argtNil,        "nochange",    false,   &fNoChange,                },
   { CMDLINE::argtNil,        "noreprocess", false,   &fNoReprocess,             },
   { CMDLINE::argtNil,        "nocount",     false,   &fNoCount,                 },
   { CMDLINE::argtNil,        "printnop",    false,   &fPrintNop,                },
   { CMDLINE::argtNil,        "obsolete",    false,   &fSkipObsolete,            },
   { CMDLINE::argtEnd,        NULL,          false,   &iArg                      },
};

void __cdecl Error(const unsigned flags, const char *fmt, ...)
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
   printf("Microsoft (R) Vulcan Reprocess Sample\n");
   printf("Usage: REPROCESS [options] Executable\n\n"
          "Options:\n\n"
          "/?                  Display this help\n"
          "/asmemit [filename] Write a disassembly listing for output binary\n"
          "/cmd     filename   Read the specified command file\n"
//        "/compact            Use compact bblocks\n"
          "/help               Display this help\n"
          "/nocmd              Do not use a command file\n"
          "/nologo             Do not display a banner message\n"
          "/nochange           Do not add NOPs to the output image\n"
          "/noreprocess        Do not emit a reprocessible image\n"
          "/o       filename   Output binary name\n"
          "/pdb     filename   Pdb output filename\n"
          "/q                  Terse output mode\n"
//        "/verbose            Verbose output mode\n"
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
   //
   // Call the Command Line processor to parse the argyuments using rgoptdef
   // as the strucuture to define how to handle the various arguements.
   //
   try
   {
      fSuccess = CMDLINE::FProcessArgs("REPROCESS", argc, (const char **) argv, rgoptdef);
   }
   //
   // Handle the various error situations and validate correct usage
   //
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

   if (fTerse)
   {
      fNoLogo = true;
   }

   //
   // Display the startup banner for this utility
   //
   if (!fNoLogo)
   {
      printf("Microsoft (R) Vulcan Reprocess Sample\n");
   }

   //
   // Set the re-readable flag
   //
   if (fNoReprocess)
   {
      printf("Generating a non-reprocessible image\n");
      fRereadable = false;
   }

   //
   // Setup output directory for output file name
   //
   if (fout)
   {
      _splitpath(szCmdO, szDrive, szDir, szFname, szExt);
      if (strlen(szDir) + strlen(szDrive))
      {
         szOutDir = new char[strlen(szDir) + strlen(szDrive) + 1];
         sprintf(szOutDir, "%s%s", szDrive, szDir);
         szOutDir[strlen(szOutDir) -1] = '\0';

         if ((strlen(szDir) > 1) && (_stat(szOutDir, &rgStatBuf)))
         {
            char szbuf[512 + _MAX_DIR + _MAX_DRIVE];
            sprintf(szbuf, "The /o option specifies a directory '%s' which does not exist.\n", szOutDir);
            Error(VErr::FLG_FATAL, szbuf);
         }

         sprintf(szOutDir, "%s%s", szDrive, szDir);
      }
      else
      {
         szOutDir = new char[3];
         strcpy(szOutDir, ".\\");
      }
   }
   else
   {
      szOutDir = new char[sizeof(DEFAULTOUTDIR)+1];
      strcpy(szOutDir, DEFAULTOUTDIR);
      szOutDir[strlen(szOutDir) - 1] = '\0';

      // Create the Reprocess Directory if it doesn't yet exist

      size_t staterr = _stat(szOutDir, &rgStatBuf);
      if (staterr || (rgStatBuf.st_mode & _S_IFREG))
      {
         if(_mkdir(szOutDir))
         {
            throw VErr("Unable to create reprocess directory", VErr::FLG_FATAL);
         }
      }
      strcpy(szOutDir, DEFAULTOUTDIR);
   }

   _splitpath(szImage, szDrive, szDir, szFname, szExt);

   //
   // Setup the org and Reprocess directory names
   // The orgdir is constructed from the name of input binary.
   //

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

   //
   // Setup file names for the pdb, cmd, asm and output files
   //
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
         sprintf(szBuf, "%s%s%s%s", szOutDir, szFname, szExt, DEFAULTASMEXT);
         szAsm = new char[strlen(szBuf) + 1];
         strcpy(szAsm, szBuf);
      }
   }

   if (!fpdb)
   {
      sprintf(szBuf, "%s%s%s%s", szOutDir, szFname, szExt, DEFAULTPDBEXT);
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
         szPdbDir[strlen(szPdbDir)-1] = '\0';

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
      sprintf(szBuf, "%s%s%s", szOutDir, szFname, szExt);
      szO = new char[strlen(szBuf) + 1];
      strcpy(szO, szBuf);
   }
   else
   {
      szO = szCmdO;
   }
}

void ReportFiles()
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

void AddNop(VProg *pprog)
{
   if (fNoChange)
      return;

   int count = 0;

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      printf("%s\n",pComp->InputName());
      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         for(VBBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next())
         {
            if (fSkipObsolete && pBlk->IsObsolete())
            {
                continue;
            }

            //If we used FirstAllBlock and NextAll instead of FirstBlock and Next,
            //we would need to avoid data blocks (pBlk->IsDataBock())
            for(VInst *pInst = pBlk->FirstInst(); pInst; pInst = pInst->Next())
            {
               VInst * newInst;

               switch (pBlk->PlatformT())
               {

                  case platformtMSIL:
                       pInst->InsertPrev(VInst::Create(COp::CEE_NOP));
                       count++;
                       break;

                  case platformtX86:
                       pInst->InsertPrev(VInst::Create(COp::NOP));
                       count++;
                       break;

                  case platformtIA64:

                       // Alloc must always appear first in a bundle.
                       if (pInst->Opcode() != COp::ALLOC &&
                           // IA64 implements switch tables in code
                           // Hence there are some sequences that
                           // are not allowed to changed in size.
                           // These tests screen them out of consideration.
                           pInst->Opcode() != COp::NOP_M &&
                           pInst->Opcode() != COp::NOP_I &&
                           pInst->Opcode() != COp::NOP_B &&
                           pInst->Opcode() != COp::NOP_F &&
                           pInst->Opcode() != COp::NOP_X &&
                           pInst != pBlk->LastInst())
                       {
                          newInst = VInst::Create(COp::NOP_M, 0xbeef);
                          pInst->InsertPrev(newInst);
                          count++;
                       }
                       break;

                  default:
                       fprintf(stderr, "Unsupported architecture type\n");
                       exit(1);
               }
            }
         }
      }

      printf("%d nop's inserted.\n", count);
   }
}

void CountNop(VProg *pprog)
{
   if (fNoCount)
   {
      return;
   }

   int count = 0;

   for(VComp *pComp = pprog->FirstComp(); pComp; pComp = pComp->Next())
   {
      printf("%s\n",pComp->InputName());
      for(VProc *pProc = pComp->FirstProc(); pProc; pProc = pProc->Next())
      {
         for(VBBlock *pBlk = pProc->FirstBlock(); pBlk; pBlk = pBlk->Next())
         {
            if (fSkipObsolete && pBlk->IsObsolete())
            {
                continue;
            }

            //If we used FirstAllBlock and NextAll instead of FirstBlock and Next,
            //we would need to avoid data blocks (pBlk->IsDataBock())
            for(VInst *pInst = pBlk->FirstInst(); pInst; pInst = pInst->Next())
            {
               switch (pBlk->PlatformT())
               {

                  case platformtMSIL:
                     if (pInst->Opcode() == COp::CEE_NOP)
                     {
                       count++;
                       if (fPrintNop)
                       {
                           printf("NOP in block %8X addr=%08X\n", pBlk->BlockId(), pBlk->Addr());
                       }
                     }
                     break;

                  case platformtX86:
                     if (pInst->Opcode() == COp::NOP)
                     {
                       count++;
                       if (fPrintNop)
                       {
                           printf("NOP in block %8X addr=%08X\n", pBlk->BlockId(), pBlk->Addr());
                       }
                     }
                     break;

                  case platformtIA64:
                     if (pInst->Opcode() == COp::NOP_M ||
                         pInst->Opcode() == COp::NOP_I ||
                         pInst->Opcode() == COp::NOP_B ||
                         pInst->Opcode() == COp::NOP_F ||
                         pInst->Opcode() == COp::NOP_X)
                     {
                       count++;
                       if (fPrintNop)
                       {
                           printf("NOP in block %8X addr=%08X\n", pBlk->BlockId(), pBlk->Addr());
                       }
                     }
                     break;

                  default:
                       fprintf(stderr, "Unsupported architecture type\n");
                       exit(1);
               }
            }
         }
      }

      printf("Found %d nop's.\n", count);
   }
}

void GenerateReprocessibleImage()
{
   printf("Generating reprocessible image\n");

   DWORD openFlags = 0;
   if ( fCompact )
   {
      openFlags |= Open_CompactBlocks;
   }
   VProg *vprog = VProg::Open( szImage, szCmd, openFlags );
   VComp *pComp = vprog->FirstComp();

   // Currently only X86 PE binaries are supported.
   //
   if (pComp->InputType() != VComp::PE)
   {
      Error(VErr::FLG_FATAL, "Unsupported architecture.");
      exit(1);
   }

   if (pComp->CanWritePdb() && (szPdb == NULL))
   {
          Error(VErr::FLG_FATAL,"This executable (VC6 or higher) requires the /pdb option.");
   }

   if (!pComp->CanWritePdb() && (szPdb != NULL))
   {
      // If the /pdb switch was explicitly set and the binary is not VC6 or greater
      // then produce a warning that the PDB cannot be written.   If the /pdb switch
      // was not explicitly set,  then the szPdb contains a pdb name generated as the
      // default name and since it is not VC6 or greater the pdb name should be ignored.

      if(!fpdb)
      {
         szPdb = NULL;
      }
      else
      {
         Error(VErr::FLG_WARNING, "The option '/pdb' is not required for linking VC50 or earlier binaries - No PDB file generated.\n");
      }
   }

   if (pComp->IsEmittedByVulcan())
   {
      if (pComp->IsEmittedByBBT())
      {
         if (pComp->IsInstrumentedByBBT())
         {
            Error(VErr::FLG_FATAL,"REPROCESS cannot process a binary that was emitted by the BBT tool BBINSTR.");
         }
      }

      else if (!pComp->IsRereadable())
      {
         Error(VErr::FLG_FATAL,"REPROCESS can only process an original binary, a BBOPT binary or a DLLMERGE binary.");
      }
   }

   ReportFiles();

   //
   //  Check for Data / Resource Only binaries
   //  Cannot insert code into a binary which has no code section.
   //
   for (VSect* pSect = pComp->FirstSect(); pSect; pSect = pSect->Next())
   {
      if (pSect->IsExecutable())
         break;
   }
   if (!pSect)
   {
      printf("NOPs cannot be added to an binary which has no code.\n");
      fNoChange = true;
   }

   //
   // Count NOPs before any changes
   //
   CountNop(vprog);

   //
   // Add NOPs
   //
   AddNop(vprog);

   /***********************************************************************
   *
   * Now write the Re-Vulcanizable output image, which is enabled by passing
   * true for the fRereadable parameter to Write / WriteFast.
   * When fRereadable is TRUE, the output image can be reprocessed
   *
   ***********************************************************************/

   if ( fWriteFast )
   {
      pComp->WriteFast(szO, szPdb, NULL, fRereadable);
   }
   else
   {
      pComp->Write(szO, szPdb, NULL, fRereadable);
   }

   if (szAsm != NULL)
   {
       pComp->PrintAsm(szAsm);
   }

   vprog->Destroy();

   // Some info about the block types.
   if ( fBlockInfo )
   {
      VBlock::PrintBlockStats( "" );
   }
}

void ProcessReprocessibleImage()
{
   if (!fRereadable || fNoCount)
   {
      return;
   }

   printf("Processing reprocessible image\n");

   DWORD openFlags = 0;
   if ( fCompact )
   {
      openFlags |= Open_CompactBlocks;
   }
   //
   // Open the reprocessible image
   //
   VProg *vprog = VProg::Open( szO, szCmd, openFlags );

   // Currently only X86 PE binaries are supported.
   //
   if (vprog->FirstComp()->InputType() != VComp::PE)
   {
      Error(VErr::FLG_FATAL, "Unsupported architecture.");
      exit(1);
   }

   //
   // Count NOPs
   //
   CountNop(vprog);

   vprog->Destroy();
}

int RealMain(int argc, char *argv[])
{
/***********************************************************************
*
* RealMain() uses the Vulcan supplied command line interpreter to interpret the
* arguments (inside InitParams) and then makes sure that output file selected
* for the instrumented executable is not the same as the input executable.
* Finally the constructor for the Vulcan program is called and then the Reprocess
* process can begin.
*
***********************************************************************/

   InitParams(argc, argv);

   if (CMDLINE::FSamePath(szO, szImage))
   {
       Error(VErr::FLG_FATAL,"Reprocessed binary will overwrite input image file");
   }

   GenerateReprocessibleImage();
   ProcessReprocessibleImage();

   return(0);
}

int __cdecl main(int argc, char *argv[])
{

/***********************************************************************
*
* main() establishes the name of the utility as REPROCESS for the
* default error handler and then goes on to try running the real program.  If a
* throw occurs whiles processing with Vulcan,  the error is flagged as fatal
* and the default error handler is called to display the message.
*
***********************************************************************/

   VErr::SetErrHandler("REPROCESS");

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
