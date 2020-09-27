/*******************************************************************
*
*    Copyright (c) 1999  Microsoft Corporation
*
*    DESCRIPTION:
*       Use PDB information to get type info and decode a symbol address
*
*    AUTHOR: Kshitiz K. Sharma
*
*    DATE:4/19/1999
*
*******************************************************************/


#define PDB_LIBRARY
#include <cvinfo.h>
#include <pdb.h>
#include <private.h>
#include <winnt.h>
#include <windef.h>
#include "symbols.h"

#define MAX_NAME                         2000

/*
 *Function Name: GetSymbolType
 *
 *Description: Returns PDB type index / enumerates the symbol.
 *
 *Returns: BOOL
 *
 */
BOOL
GetSymbolType(
   IN GSI* pgsi,
   IN OUT TCHAR *symName,
   IN BOOL PrefixMatch,
   OUT TI *ptypeIndex,
   OUT PTYPE_ENUM_INFO pEnumInfo
   )
{
   BYTE Sym[512], prevSym[512], *symReturned, sInfo[sizeof(IMAGEHLP_SYMBOL) + MAX_NAME];
   BOOL found=FALSE, hashedStart=TRUE, foundOnce=FALSE;
   WORD i, length, symIndex;
   PDWORD typeIndex;
   DWORD indx=0;
   TCHAR name[MAX_NAME];
   PIMAGEHLP_SYMBOL symInfo;

   symReturned = NULL;
   symInfo = (PIMAGEHLP_SYMBOL) &(sInfo[0]);
   symInfo->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
   symInfo->MaxNameLength = MAX_NAME;

   if (pgsi!=NULL) {
      // Try exact match using hash first, if failed it will try best match
      symReturned = GSIHashSym(pgsi, symName, NULL);
      if ((symReturned==NULL) || pEnumInfo) { // Enumerate all
         hashedStart = FALSE;
         symReturned = GSINextSym(pgsi, NULL);
      }
   }

   while ((symReturned!=NULL) && (!found || pEnumInfo)) {
      typeIndex = NULL;
      ZeroMemory(name, MAX_NAME);
      symIndex = *((PWORD) (symReturned+=2));
      symReturned+=2;
      switch (symIndex) {
      case S_COMPILE : // 0x0001   Compile flags symbol
      case S_REGISTER_16t : { // 0x0002   Register variable
         break;
      }
      case S_CONSTANT_16t : { // 0x0003   constant symbol
         DWORD len=4, val;

         // GetNumericValue(symReturned+4, &val, &len);
         val = *((PDWORD )(symReturned +4));
         typeIndex = (PDWORD) symReturned;
         strncpy(name, symReturned+5+len, (UCHAR) symReturned[4+len]);
         break;
      }
      case S_UDT_16t : { // 0x0004   User defined type
         typeIndex = ((PDWORD) (symReturned+2));
         strncpy(name, symReturned + 7, (UCHAR) symReturned[6]);
         break;
      }
      case S_SSEARCH : // 0x0005   Start Search
      case S_END : // 0x0006   Block, procedure, "with" or thunk end
      case S_SKIP : // 0x0007   Reserve symbol space in $$Symbols table
      case S_CVRESERVE : // 0x0008   Reserved symbol for CV internal use
      case S_OBJNAME : // 0x0009   path to object file name
      case S_ENDARG : // 0x000a   end of argument/return list
      case S_COBOLUDT_16t : // 0x000b   special UDT for cobol that does not symbol pack
      case S_MANYREG_16t : // 0x000c   multiple register variable
      case S_RETURN : // 0x000d   return description symbol
      case S_ENTRYTHIS : // 0x000e   description of this pointer on entry
      case S_BPREL16 : // 0x0100   BP-relative
      case S_LDATA16 : // 0x0101   Module-local symbol
      case S_GDATA16 : // 0x0102   Global data symbol
      case S_PUB16 : // 0x0103   a public symbol
      case S_LPROC16 : // 0x0104   Local procedure start
      case S_GPROC16 : // 0x0105   Global procedure start
      case S_THUNK16 : // 0x0106   Thunk Start
      case S_BLOCK16 : // 0x0107   block start
      case S_WITH16 : // 0x0108   with start
      case S_LABEL16 : // 0x0109   code label
      case S_CEXMODEL16 : // 0x010a   change execution model
      case S_VFTABLE16 : // 0x010b   address of virtual function table
      case S_REGREL16 : // 0x010c   register relative address
      case S_BPREL32_16t : { // 0x0200   BP-relative
         break;
      }
      case S_LDATA32_16t :// 0x0201   Module-local symbol
      case S_GDATA32_16t :// 0x0202   Global data symbol
      case S_PUB32_16t : { // 0x0203   a public symbol (CV internal reserved)
         DATASYM32_16t *pData;

         pData = (DATASYM32_16t *) (symReturned -4);
         indx = pData->typind; typeIndex = &indx;
         strncpy(name, &pData->name[1], (UCHAR) pData->name[0]);
         break;
      }
      case S_LPROC32_16t : // 0x0204   Local procedure start
      case S_GPROC32_16t : { // 0x0205   Global procedure start
         typeIndex = ((PDWORD) symReturned) + 6;
         strncpy(name, symReturned + 36, (UCHAR) symReturned[35]);
         break;
      }
      case S_THUNK32 : // 0x0206   Thunk Start
      case S_BLOCK32 : // 0x0207   block start
      case S_WITH32 : // 0x0208   with start
      case S_LABEL32 : // 0x0209   code label
      case S_CEXMODEL32 : // 0x020a   change execution model
      case S_VFTABLE32_16t : // 0x020b   address of virtual function table
      case S_REGREL32_16t : // 0x020c   register relative address
      case S_LTHREAD32_16t : // 0x020d   local thread storage
      case S_GTHREAD32_16t : // 0x020e   global thread storage
      case S_SLINK32 : // 0x020f   static link for MIPS EH implementation
      case S_LPROCMIPS_16t : // 0x0300   Local procedure start
      case S_GPROCMIPS_16t : { // 0x0301   Global procedure start
         break;
      }

      case S_PROCREF : { // 0x0400   Reference to a procedure
         // typeIndex = ((PDWORD) symReturned) + 3;
         // strncpy(name, symReturned + 13, (char) *(symReturned+12));
         break;
      }
      case S_DATAREF : // 0x0401   Reference to data
      case S_ALIGN : // 0x0402   Used for page alignment of symbols
      case S_LPROCREF : // 0x0403   Local Reference to a procedure

         // sym records with 32-bit types embedded instead of 16-bit
         // all have  0x1000 bit set for easy identification
         // only do the 32-bit target versions since we don't really
         // care about 16-bit ones anymore.
      case S_TI16_MAX : // 0x1000,
      case S_REGISTER : { // 0x1001   Register variable
         break;
      }

      case S_CONSTANT : { // 0x1002   constant symbol
         DWORD len=4, val;

         // GetNumericValue(symReturned+4, &val, &len);
         val = *((PDWORD )(symReturned +4));
         typeIndex = (PDWORD) symReturned;
         strncpy(name, symReturned+5+len, (UCHAR) symReturned[4+len]);
         break;
      }
      case S_UDT : { // 0x1003   User defined type
         typeIndex = (PDWORD) symReturned;
         strncpy(name, symReturned+5, (UCHAR) symReturned[4]);
         break;
      }

      case S_COBOLUDT : // 0x1004   special UDT for cobol that does not symbol pack
      case S_MANYREG : // 0x1005   multiple register variable
      case S_BPREL32 : { // 0x1006   BP-relative
         break;
      }

      case S_LDATA32 : // 0x1007   Module-local symbol
      case S_GDATA32 : // 0x1008   Global data symbol
      case S_PUB32 : { // 0x1009   a public symbol (CV internal reserved)
         typeIndex = (PDWORD) symReturned;
         strncpy(name, symReturned+11, (UCHAR) symReturned[10]);

         break;
      }
      case S_LPROC32 :  // 0x100a   Local procedure start
      case S_GPROC32 : { // 0x100b   Global procedure start
         typeIndex = (PDWORD) (symReturned+24);
         strncpy(name, symReturned+32, (UCHAR) symReturned[31]);
         break;
      }

      case S_VFTABLE32 : // 0x100c   address of virtual function table
      case S_REGREL32 : // 0x100d   register relative address
      case S_LTHREAD32 : // 0x100e   local thread storage
      case S_GTHREAD32 : // 0x100f   global thread storage
      case S_LPROCMIPS : // 0x1010   Local procedure start
      case S_GPROCMIPS : // 0x1011   Global procedure start
      case S_FRAMEPROC : // 0x1012   extra frame and proc information
      case S_COMPILE2 : // 0x1013   extended compile flags and info
      case S_MANYREG2 : // 0x1014   multiple register variable
      case S_LPROCIA64 : // 0x1015   Local procedure start (IA64)
      case S_GPROCIA64 : // 0x1016   Global procedure start (IA64)
      case S_RECTYPE_MAX :
      default:
         break;

      } /* switch */

      if (*((ULONG *) &name[0]) != 0x435f3f3f) {   // Not intrested in names like "??_C"*
         if (symName[0] != '_') {
            // We can undecorate name and check if it matches
            TCHAR tmpName[MAX_NAME];

            strcpy(tmpName, name);
            SymUnDNameInternal( name, strlen(name), tmpName, MAX_NAME);
         }

         if (hashedStart || !PrefixMatch) {
            found = !(strcmp(name, symName));
         } else {
            found = !(strncmp(name, symName, strlen(symName)));
         }
         if (found) {
            foundOnce = TRUE;
            if (typeIndex && ptypeIndex) {
               *ptypeIndex = *typeIndex;
            }
            if (pEnumInfo) { // Do a callback
               if (! (*((PSYMBOL_TYPE_NAME_CALLBACK) pEnumInfo->CallbackRoutine)) (
                        &name[0],
                        (typeIndex != NULL) ? *typeIndex : 0,
                        pEnumInfo->CallerData))
                  return TRUE;
            }
            if (!PrefixMatch) {
               return TRUE;
            }
         }

      }

      symReturned = GSINextSym(pgsi, symReturned-4);

      if (hashedStart && (symReturned==NULL)) {
         // Did not fint exact match  - try incomplete match
         hashedStart = FALSE;
         symReturned = GSINextSym(pgsi, NULL);
      }

   } /* while */
   return(foundOnce);
} /* End function GetSymbolType */

/*
 ³ Function Name: GetPdbTypeInfo
 ³
 ³ Parameters:
 ³            pgsi, pDbi - Pointer to Global and Debug symbol Info
 ³            symName - name of symbol,
 ³            PrefixMatch - if given should matched exact or not,
 ³            ReturnType - Describes if symbols should be enumerated
 ³
 ³ Description: Gets the type index of given symbol and enumerates all matching
 * symbols depending on ReturnType
 ³
 ³ Returns: TRUE on success
 ³
 */
BOOL
GetPdbTypeInfo(
   DBI *pDbi,
   GSI *pGsi,
   LPSTR symName,
   BOOL PrefixMatch,
   IMAGEHLP_TYPES ReturnType,
   PBYTE pRetVal
   )
{
   TPI *typeInfo=NULL;
   GSI *Publics, *Globals;
   DBI *pDBI;
   BOOL openedDBI=FALSE, openedGSI=FALSE, openedPGSI=FALSE;
   CV_typ_t typ, *typeIndex;
   PTYPE_ENUM_INFO pEnumInfo;

   Publics = pGsi; Globals=NULL; pDBI=NULL;
   if ((pDbi == NULL) || (pGsi == NULL) || (pRetVal == NULL)) {
      return FALSE;
   }

   if (ReturnType == IMAGEHLP_TYPEID_INDEX) {
      typeIndex = (CV_typ_t *) pRetVal;
      pEnumInfo = NULL;
   } else if (ReturnType == IMAGEHLP_TYPEID_TYPE_ENUM_INFO) {
      typeIndex = &typ;
      pEnumInfo = (PTYPE_ENUM_INFO) pRetVal;
   } else {
      return FALSE;
   }

   return (GetSymbolType(pGsi, symName, PrefixMatch, typeIndex, pEnumInfo)) ;

} /* End function GetPdbTypeInfo */

