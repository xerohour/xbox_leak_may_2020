//======================================================================
// Module:		ddnvstat.cpp
// Description:	NVidia statistics driver routines, for NV3/NV4
// 
//  Copyright (C) 1999 NVidia Corporation.  All Rights Reserved.
//======================================================================
// NB: This file is 'opted out' of using pre-compiled headers
#include <windows.h>
#include <tchar.h>

#define STAT_INSTANIATE_TABLE
#include "statdef.h"			// has to be included before nvprecomp.

#include "nvprecomp.h"


#ifndef WINNT

#undef NVSTATNT_IOCTL
#define NVSTATNT_IOCTL(x) x

#endif

NVSTATLOCKINDEX nvStatLockIndex = NVSTAT_LOCK_UNKNOWN;
STATINDEX *gNVStatIndex;

HANDLE OpenDriver (TCHAR *pDeviceName);
BOOL CloseDriver(HANDLE hDriver);
DWORD GetDriverVersion(HANDLE hDriver);

HANDLE gDriverHandle = INVALID_HANDLE_VALUE;
NVSTATCNF statCfg;
NVDXTICK *DXStatTickPtr = NULL;

//======================================================================
// Function:	nvStatLookupIndex
// Description	Lookup and Index and fill in StatIndex structure
//				log/capture timings for d3d/ddraw functions.
// Parameters:	.
// Returns:		.
//======================================================================
void __cdecl nvStatLookupIndex(char *fmt, ...)
{
	DWORD nParms;
	DWORD sLen;
	DWORD ID;

	if (gNVStatIndex)
	{
		if (gNVStatIndex->index != INVALID_INDEX)
		{
			// log parameters 
			if (DXStatTickPtr && (DXStatTickPtr->logFormat & LOG_MEDIUM_BIT))
			{
				nParms = gNVStatIndex->nParms;
				if (nParms > 0)
				{
					ID = gNVStatIndex->index | D3D_LOG_PARAMETERS;
					nvStatWriteLogOpCode(nParms+1, ID);
					nvStatWriteLogData(nParms, (DWORD *)(&fmt+1));
				}
			}
			return;
		}

		__asm {
			push eax
			push ebx
			push ecx
			push edx
			push esi
			push edi

			mov	esi, fmt
			sub	esi, 1
			mov	ecx, STAT_HASH_TABLE_SIZE+1
			xor	eax,eax
			xor edx,edx
			mov bl, '%'
	loopSize:
			add	esi, 1
			dec	ecx
			je	done
			cmp	byte ptr [esi],al
			je	done
			cmp	[esi], bl
			jne	loopSize
			inc	edx
			jmp	loopSize
	done:
			neg	ecx
			add	ecx, STAT_HASH_TABLE_SIZE
			mov	sLen, ecx
			mov nParms, edx

			mov	ebx,ecx
			shl	ebx,3			// multiply by size of LENHASH structure
			add	ebx, OFFSET statDefHash
			mov	eax, dword ptr ([ebx]statDefHash.numEntries)
			mov	edx,[ebx]statDefHash.offset
			add	edx, OFFSET statDefList
	strLoop:
			mov	esi, edx
			mov edi, fmt
			mov	ecx, sLen
			add	edx, ecx
			repe	cmpsb
			je	foundName
			add	edx, 1			// + \0
			add	eax, (1 SHL 16)	// update ID in high word of eax
			dec	ax
			jne	strLoop
			mov	eax,-1
	foundName:
			shr		eax, 16
			cwde
			mov	ID, eax

			pop	edi
			pop	esi
			pop	edx
			pop	ecx
			pop	ebx
			pop	eax
		};

		gNVStatIndex->index = ID;
		gNVStatIndex->nParms = nParms;

		if (DXStatTickPtr)
		{
			if (ID != INVALID_INDEX)
			{
				nvStatWriteLogOpCode(1, ID);
				if (DXStatTickPtr->logFormat & LOG_MEDIUM_BIT)
				{
					NVStatLogMedium(0, ID);
				}
				else if (DXStatTickPtr->logFormat & LOG_MAXIMUM_BIT)
				{
					NVStatLogMaximum(0, ID);
				}
			}
		}
	}
}

//======================================================================
// Function:	AttachNVStat
// Description	Attaches to statistics Driver, so we can
//				log/capture timings for d3d/ddraw functions.
// Parameters:	.
// Returns:		.
//======================================================================
void __stdcall AttachNVStat()
{
	DWORD bytesReturned;

	gDriverHandle = OpenDriver(_T("nvstat"));

	if (gDriverHandle != INVALID_HANDLE_VALUE)
	{
		if (NvDeviceIoControl(gDriverHandle, NVSTATNT_IOCTL(GETCONFIGURATION), NULL, 0, &statCfg, sizeof(NVSTATCNF), 
			&bytesReturned, NULL) != 0)
		{
			// make sure configuration size is matches..
			if (bytesReturned == sizeof(NVSTATCNF))
			{
				// make sure at least the structure sizes match.
				if (statCfg.captureSize == sizeof(NVDXTICK))
					DXStatTickPtr = (NVDXTICK *)statCfg.captureMem;
			}
		}
	}
}

//======================================================================
// Function:	DetachNVStat
// Description	Detach statistics Driver (nvstat.Driver), freeing up resources
// Parameters:	.
// Returns:		.
//======================================================================
void __stdcall DetachNVStat()
{
	DXStatTickPtr = NULL;

	CloseDriver(gDriverHandle);	

	gDriverHandle = INVALID_HANDLE_VALUE;

}


//======================================================================
// Function:	OpenDriver
// Description:	opens the Driver for communication between ring 3 and 
//				ring 0
// Parameters:	pDeviceName = name of Driver
// returns:		HANDLE = opened file handle
//======================================================================
HANDLE OpenDriver (TCHAR *pDeviceName)
{

    HANDLE hDriver = 0;
    TCHAR device[64];

#ifdef WINNT
    nvSprintf( device, _T("%s"), pDeviceName);
#else
    PCHAR pExt = _tcsrchr( pDeviceName, '.');
    if (pExt && _tcsicmp( pExt, _T(".vxd")) == 0)
       *pExt = 0;

    nvSprintf( device, _T("\\\\.\\%s"), pDeviceName);
#endif

    hDriver = NvCreateFile( device, GENERIC_EXECUTE | GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (hDriver == INVALID_HANDLE_VALUE)
    {
#ifdef WINNT
        nvStrCat( device, _T(".sys"));
#else
        nvStrCat( device, _T(".vxd"));
#endif

	    hDriver = CreateFile( device, GENERIC_EXECUTE | GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    }
    return hDriver;
}


//======================================================================
// Function:	CloseDriver
// Description:	closes the Driver for freeing up resources
// Parameters:	HANDLE = opened file handle
// Returns:		!0 = success
//======================================================================
BOOL CloseDriver(HANDLE hDriver)
{
   BOOL retval;

   if (hDriver == INVALID_HANDLE_VALUE)
      return 0;

#ifdef WINNT
   retval = NvCloseHandle(hDriver);
#else
   retval = CLOSE_HANDLE( hDriver);
#endif

   return retval;
}

//======================================================================
// Function:	GetDriverVersion
// Description:	Retreives the Driver Version through an IOCTL
// Parameters:	HANDLE = opened file handle
// Returns:		packed version information
//======================================================================
DWORD GetDriverVersion(HANDLE hDriver)
{
    DWORD  cbBytesReturned;
    struct { BYTE minor; BYTE major; WORD product; } verinfo = {0,0,0};

    if (hDriver == INVALID_HANDLE_VALUE)
       return 0;

    if (!NvDeviceIoControl( hDriver,NVSTATNT_IOCTL(GETVERSION),0,0,&verinfo,sizeof(verinfo),&cbBytesReturned,NULL))
       return 0;

    return ( (verinfo.major << 8) | verinfo.minor);
}

//=====================================================================
// Function:	nvStatCheckLogSpace
// Description:	check for space in log file memory area
// Parameters:	nDWords = number of DWORDS we are going to write
// Returns:		TRUE if space in log memory area
//=====================================================================
BOOL nvStatCheckLogSpace(DWORD nDWords)
{
	DWORD freeSize;
	DWORD kLogSize;

	if (DXStatTickPtr->logFull)
		return TRUE;

	kLogSize = (DXStatTickPtr->logSize << 10);
	if (DXStatTickPtr->logHead > DXStatTickPtr->logTail)
	{
		// this really shouldn't happen, as the log file
		// stops when logTail reaches the end of the buffer, this is no longer
		// a circular buffer...But just in case figure something out.
		DPF("For some reason logging head pointer passed logging tail pointer????");
		freeSize = (DXStatTickPtr->logHead - DXStatTickPtr->logTail);
	}
	else
	{
		freeSize = kLogSize - (DXStatTickPtr->logTail - DXStatTickPtr->logHead);
	}

	if (freeSize >= (nDWords << 2))
		return TRUE;

	return FALSE;

}

//=====================================================================
// Function:	nvStatWriteLogOpCode
// Description:	write an opcode for upcoming log entries
// Parameters:	nDWords = number of DWORDS we are going to write
//							including opCode itself
//				opCode = the opcode to write to log memory
// Returns:		.
//=====================================================================
void nvStatWriteLogOpCode(DWORD nDWords, DWORD opCode)
{
	DWORD *tPtr;
	DWORD logSize;

	if (DXStatTickPtr == NULL)
		return;

	if (DXStatTickPtr->captureOn == 0)
		return;

	if (!(DXStatTickPtr->logFormat & (LOG_MINIMUM_BIT | LOG_MEDIUM_BIT | LOG_MAXIMUM_BIT)))
		return;

	if (DXStatTickPtr->logFull)
		return;

	if (nvStatCheckLogSpace(nDWords))
	{
		tPtr = (DWORD *)((BYTE *)DXStatTickPtr->logMem + DXStatTickPtr->logTail);
		*tPtr = (opCode << 16) | (nDWords & 0xffff);

		logSize = DXStatTickPtr->logSize << 10;

		// we only wrote out one of the words 
		DXStatTickPtr->logTail = DXStatTickPtr->logTail + (1 << 2);

		if (DXStatTickPtr->logTail >= logSize)
		{
			// stop logging when buffer is full
			DXStatTickPtr->logFull = TRUE;
		}
	}
	else
	{
		// stop logging when buffer is full
		DXStatTickPtr->logFull = TRUE;
		DXStatTickPtr->logMissed += 1;
	}


}

//=====================================================================
// Function:	nvStatWriteLogOpData
// Description:	write data for log entries
// Parameters:	nDWords = number of DWORDS we are going to write
//							including opCode itself
//				theData = pointer to dword data
// Returns:		.
//=====================================================================
void nvStatWriteLogData(DWORD nDWords, DWORD *theData)
{
	DWORD *tPtr;
	DWORD kLogSize;
	DWORD dwCount;

	if (DXStatTickPtr == NULL)
		return;

	if (DXStatTickPtr->captureOn == 0)
		return;

	if (DXStatTickPtr->logFull)
		return;

	if (!(DXStatTickPtr->logFormat & (LOG_MINIMUM_BIT | LOG_MEDIUM_BIT | LOG_MAXIMUM_BIT)))
		return;

	if (nvStatCheckLogSpace(nDWords))
	{
		tPtr = (DWORD *)((BYTE *)DXStatTickPtr->logMem + DXStatTickPtr->logTail);
		dwCount = nDWords;
		while (dwCount)
		{
			*tPtr++ = *theData++;
			dwCount--;
		}

		kLogSize = DXStatTickPtr->logSize << 10;

		DXStatTickPtr->logTail = DXStatTickPtr->logTail + (nDWords << 2);

		if (DXStatTickPtr->logTail == DXStatTickPtr->logHead)
		{
			// stop logging when buffer is full
			DXStatTickPtr->logFull = TRUE;
		}
	}
	else
	{
		// stop logging when buffer is full
		DXStatTickPtr->logFull = TRUE;
		DXStatTickPtr->logMissed += nDWords;
	}
}

//======================================================================
// Function:	nvStatLogError
// Description:	Log an error to the log file
// Parameters:	.
// Returns:		.
//======================================================================
void __cdecl nvStatLogError(DWORD dwLevel, LPSTR szFormat, ...)
{
	TCHAR tBuff[256];
	DWORD sLen;

	if (DXStatTickPtr == NULL)
		return;

	if (!(DXStatTickPtr->logFormat & (LOG_MINIMUM_BIT | LOG_MEDIUM_BIT | LOG_MAXIMUM_BIT)))
		return;

	if (!(dwLevel & (NVDBG_LEVEL_ERROR | NVDBG_LEVEL_PERFORMANCE)))
		return;

	nvSprintf(tBuff, szFormat, (va_list)(&szFormat+1));

	// include ending 0
	sLen = nvStrLen(tBuff)+1;

	// turn into # of double words
	sLen = (sLen + 3) >> 2;

	if (dwLevel & NVDBG_LEVEL_ERROR)
	{
		nvStatWriteLogOpCode(sLen+1, D3D_LOG_ERROR);
		nvStatWriteLogData(sLen, (LPDWORD)tBuff);
	}
	else if (DXStatTickPtr->logFormat & LOG_PERFORMANCE_BIT)
	{
		nvStatWriteLogOpCode(sLen+1, D3D_LOG_PERFORMANCE);
		nvStatWriteLogData(sLen, (LPDWORD)tBuff);
	}


}

//======================================================================
// Function:	nvStatLogState
// Description:	Log state change to log file
// Parameters:	state = which state has changed
//				context = which context changed the state
// Returns:		.
//======================================================================
void nvStatLogState(DWORD state, PNVD3DCONTEXT context)
{
}

//======================================================================
// Function:	nvStatLogPrimitives
// Description:	Log primitve information to log file
// Parameters:	command = primitive command to log
//				commandCount = # commands of this type
// Returns:		.
//======================================================================
void nvStatLogPrimitives(BYTE command,WORD commandCount)
{

}

//======================================================================
// Function:	nvStatLogLight
// Description:	log light parameters which come from DX7
// Parameters:	pLight7 pointer ot light structure
// Returns:		.
//======================================================================
void nvStatLogLight(LPD3DLIGHT7 pLight7)
{
}


//======================================================================
// Function:	nvStatLogDwordData
// Description:	log some dword data to the log file
// Parameters:	parmEnum = enumeration for parameter type
//				count = number of dwords to write
//				... = dword parameters
// Returns:		.
//======================================================================
void nvStatLogDwordData(DWORD parmEnum, int count, ...)
{
	va_list marker;
	DWORD curDW;

	if (DXStatTickPtr == NULL)
		return;

	if (!(DXStatTickPtr->logFormat & (LOG_MEDIUM_BIT | LOG_MAXIMUM_BIT)))
		return;

	if (DXStatTickPtr->logFull)
		return;


	nvStatWriteLogOpCode(count+1, parmEnum);

	va_start(marker, count);

	while (count)
	{
		curDW = va_arg(marker, DWORD);
		nvStatWriteLogData(1, &curDW);
		count--;
	}

	va_end(marker);

}

void __stdcall nvCelsiusDispatchPrintVertex (PNVD3DCONTEXT pContext,DWORD dwPutAddress)
{
#if 0
	DWORD *pdwData;
	DWORD flags;
	DWORD nUserUVS;
	DWORD nDWORDS;

	if (DXStatTickPtr == NULL)
		return;

	if (!(DXStatTickPtr->logFormat & LOG_MAXIMUM_BIT))
		return;

	pdwData = ((DWORD*)dwPutAddress) + 1;

	nDWORDS = 0;
	flags = 0;
	nUserUVS = 0;

	if (fvfData.dwBMask)
	{
		flags |= VERTSBMASK;
		nDWORDS++;
	}

	if (fvfData.dwNormalMask)
	{
		flags |= VERTSNORMALMASK;
		nDWORDS += 3;
	}

	if (pContext->celsiusState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(1))
	{
		DWORD dwD3DStage = pContext->celsiusState.dwTexUnitToTexStageMapping[1];
        DWORD dwTCIndex  = (pContext->celsiusState.dwTexCoordIndices >> 16) & 0xffff;
        DWORD dwCount    = (pContext->celsiusState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(1) | CELSIUS_FLAG_TEXMATRIXSWFIX(1))) ?
                           4 : fvfData.dwUVCount[dwTCIndex];
		flags |= VERTSUSERUV1;
		nDWORDS += dwCount;
		nUserUVS = dwCount << 16;
	}

	if (pContext->celsiusState.dwStateFlags & CELSIUS_FLAG_USERTEXCOORDSNEEDED(0))
	{
		DWORD dwD3DStage = pContext->celsiusState.dwTexUnitToTexStageMapping[0];
        DWORD dwTCIndex  = (pContext->celsiusState.dwTexCoordIndices >>  0) & 0xffff;
        DWORD dwCount    = (pContext->celsiusState.dwStateFlags & (CELSIUS_FLAG_PASSTHROUGHCUBEMAPPING(0) | CELSIUS_FLAG_TEXMATRIXSWFIX(0))) ?
                           4 : fvfData.dwUVCount[dwTCIndex];
		flags |= VERTSUSERUV0;
		nDWORDS += dwCount;
		nUserUVS |= dwCount;
	}

	if (fvfData.dwSpecularMask)
	{
		flags |= VERTSSPECULAR;
		nDWORDS++;

	}

	if (fvfData.dwDiffuseMask)
	{
		flags |= VERTSDIFFUSE;
		nDWORDS++;
	}

	if (fvfData.dwXYZMask)
	{
		flags |= VERTSXYZ;
		nDWORDS += 3;
	}

	if (fvfData.dwRHWMask)
	{
		if (pdwData[nDWORDS] == 0.0f)
		{
			nvStatLogError(NVDBG_LEVEL_ERROR, "RHW = 0.0f, it should equal 1.0f");
		}
		flags |= VERTSRHW;
		nDWORDS++;
	}

	nvStatWriteLogOpCode(nDWORDS+3, D3D_LOG_DX7_PRIMITIVES);
	nvStatWriteLogData(1, &flags);
	nvStatWriteLogData(1, &nUserUVS);
	nvStatWriteLogData(nDWORDS, pdwData);
#endif
}

void __stdcall nvKelvinDispatchPrintVertex (PNVD3DCONTEXT pContext,DWORD dwPutAddress)
{
#if 0
    static DWORD  dwVertexCount = 0;
           DWORD *pdwData       = ((DWORD*)dwPutAddress) + 1;

	if (DXStatTickPtr == NULL)
		return;

	if (!(DXStatTickPtr->logFormat & (LOG_MEDIUM_BIT | LOG_MAXIMUM_BIT)))
		return;

    if (DXStatTickPtr->logFormat & LOG_MEDIUM_BIT)
    {
        dwVertexCount ++;

        DPF ("Vertex %d",dwVertexCount);

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_BLENDWEIGHT])) {
            DPF ("         B = %08x",pdwData[0]);
            pdwData += 1;
        }

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_NORMAL])) {
            DPF ("    Normal = %f, %f, %f", FLOAT_FROM_DWORD(pdwData[0]),FLOAT_FROM_DWORD(pdwData[1]),FLOAT_FROM_DWORD(pdwData[2]));
            DPF ("           = [%08x, %08x, %08x]", pdwData[0],pdwData[1],pdwData[2]);
            pdwData += 3;
        }

        if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(1)) {
            DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[1];
            DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 8) & 0xff;
            DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            DWORD dwCount    = (pContext->hwState.pVertexShader->getVASize(dwVAIndex) >> 2);
            for (DWORD i = 0; i < dwCount; i++)
            {
                DPF ("   UV1[%d] = %f [%08x]", i,FLOAT_FROM_DWORD(pdwData[0]),pdwData[0]);
                pdwData += 1;
            }
        }

        if (pContext->hwState.dwStateFlags & KELVIN_FLAG_USERTEXCOORDSNEEDED(0)) {
            DWORD dwD3DStage = pContext->hwState.dwTexUnitToTexStageMapping[0];
            DWORD dwTCIndex  = (pContext->hwState.dwTexCoordIndices >> 0) & 0xff;
            DWORD dwVAIndex  = defaultInputRegMap[D3DVSDE_TEXCOORD0+dwTCIndex];
            DWORD dwCount    = (pContext->hwState.pVertexShader->getVASize(dwVAIndex) >> 2);
            for (DWORD i = 0; i < dwCount; i++)
            {
                DPF ("   UV0[%d] = %f [%08x]", i,FLOAT_FROM_DWORD(pdwData[0]),pdwData[0]);
                pdwData += 1;
            }
        }

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_SPECULAR])){
            DPF ("  Specular = %08x",pdwData[0]);
            pdwData += 1;
        }

        if (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_DIFFUSE])) {
            DPF ("   Diffuse = %08x",pdwData[0]);
            pdwData += 1;
        }

        nvAssert (pContext->hwState.pVertexShader->bVAExists(defaultInputRegMap[D3DVSDE_POSITION]));
        DPF ("       XYZ = %f, %f, %f", FLOAT_FROM_DWORD(pdwData[0]),FLOAT_FROM_DWORD(pdwData[1]),FLOAT_FROM_DWORD(pdwData[2]));
        DPF ("           = [%08x, %08x, %08x]", pdwData[0],pdwData[1],pdwData[2]);
        pdwData += 3;

        if (pContext->hwState.pVertexShader->getVASize(defaultInputRegMap[D3DVSDE_POSITION]) == 4*sizeof(DWORD)) {
            DPF ("       RHW = %f [%08x]", FLOAT_FROM_DWORD(pdwData[0]),pdwData[0]);
            pdwData += 1;
        }
        DPF ("");
    }
#endif
}

//======================================================================
// Function:	nvStatLogVertexData
// Description:	Log an error to the log file
// Parameters:	dx6 = flag indicating dx6 or not
//				pVertex = pointer to vertex data to dump to log file
// Returns:		.
//======================================================================
void nvStatLogVertexData(BOOL dx6, DWORD *pVertex)
{
	if (DXStatTickPtr == NULL)
		return;

	if (!(DXStatTickPtr->logFormat & LOG_MAXIMUM_BIT))
		return;

	if (DXStatTickPtr->logFull)
		return;

	if (dx6)
	{
		nvStatWriteLogOpCode(11, D3D_LOG_DX6_PRIMITIVES);
		nvStatWriteLogData(10, pVertex);
	}
	else
	{
		nvStatWriteLogOpCode(9, D3D_LOG_DX5_PRIMITIVES);
		nvStatWriteLogData(8, pVertex);
	}
}

//======================================================================
// Function:	NVStatLogMedium
// Description:	Log medium amount of data depending on log opcode
// Parameters:	pContext = pointer to context for this DX module
//				DXFunction = enumeration number of function
// Returns:		.
//======================================================================
void NVStatLogMedium(DWORD DXFunction, DWORD value)
{
	DWORD opCode;


	if ((DXFunction >= START_RENDER_STATE) && (DXFunction <= END_RENDER_STATE))
	{
		opCode = DXFunction|D3D_LOG_PARAMETERS;
		nvStatWriteLogOpCode(2, opCode);
		nvStatWriteLogData(1, &value);
	}

	if (nvStatLockIndex != NVSTAT_LOCK_UNKNOWN)
	{
		opCode = D3D_LOG_LOCK_INDEX|D3D_LOG_PARAMETERS;
		nvStatWriteLogOpCode (2, opCode);
		nvStatWriteLogData(1, (DWORD *)&nvStatLockIndex);
		nvStatLockIndex = NVSTAT_LOCK_UNKNOWN;
	}
}

//======================================================================
// Function:	NVStatLogMaximum
// Description:	Log maximum amount of data depending on log opcode
// Parameters:	pContext = pointer to context for this DX module
//				DXFunction = enumeration number of function
// Returns:		.
//======================================================================
void NVStatLogMaximum(DWORD DXFunction, DWORD value)
{
	NVStatLogMedium(DXFunction, value);

	// add more dumping here...???
}

