
#include "stdafx.h"
#include "fxdspimg.h"

//
// builds scratch image to be loaded into offset 0 of scratch memory...
//


int CDspImageBuilder::BuildDspImage()
{

    int err = ERROR_SUCCESS;

	BOOL bResult = FALSE;
	DWORD dwTotalSize = 0;
	CFxData *pCurrentFx = NULL, *pPreviousFx = NULL;
    DWORD dwBytesWritten;

	DWORD dwGraphIndex, dwFxIndex;
	DWORD j;

	DWORD dwCodeIndex = DSP_COMMANDBLOCK_SCRATCHOFFSET+sizeof(HOST_TO_DSP_COMMANDBLOCK);

	DWORD dwStateBaseIndex = dwCodeIndex + m_dwTotalDspCodeSize;

	DWORD dwStateIndex = DSP_FX_STATE_XMEMORY_BASE_OFFSET;

	//
	// the internal temp frame buffers are in XRAM, placed right after the concatenated
	// state blocks for all FX.
	//

	DWORD dwTempBinBaseIndex = dwStateIndex + m_dwTotalDspStateSize - 
		(m_dwNumTempBins*DSP_BUFFER_FRAME_LENGTH*sizeof(DWORD));

	DWORD dwYMemIndex = DSP_FX_STATE_YMEMORY_BASE_OFFSET;

	DWORD dwScratchIndex = DSP_FX_DELAY_DATA_SCRATCHOFFSET;

	//
	// the FX description data struct follows the delay line scratch space and is kept
	// by the driver/Game in system ram. The dsp does not parse this array..
	//

	HOST_TO_DSP_COMMANDBLOCK dspCommandBlock;

	SCRATCH_IMAGE_DESCRIPTOR *pDescriptor;
    SCRATCH_IMAGE_FX_DESCRIPTION *pFxDesc;

	PUCHAR pDevKeys;
	DWORD dwDevKeyBufferIndex = 0;

	//
	// initialize descriptor
	//

	DWORD dwDescriptorSize = sizeof(SCRATCH_IMAGE_DESCRIPTOR) + 
		sizeof(SCRATCH_IMAGE_FX_DESCRIPTION)*m_dwTotalFX;

	pDescriptor = (SCRATCH_IMAGE_DESCRIPTOR *) new BYTE[dwDescriptorSize];

	if (pDescriptor == NULL) {
		err = ERROR_OUTOFMEMORY;
		goto errExit;
	}

	pFxDesc = (SCRATCH_IMAGE_FX_DESCRIPTION *) ((PUCHAR)pDescriptor + sizeof(SCRATCH_IMAGE_DESCRIPTOR));

	memset(pDescriptor,0,dwDescriptorSize);
	pDescriptor->dwNumFX = m_dwTotalFX;

	//
	// set total scratch size in the header
	//

	pDescriptor->dwTotalScratchSize = m_dwTotalScratchSize;

	//
	// allocate buffer to store dev keys used to encrypt individual DSP code images
	// this buffer gets placed in the scratch file after the SCRATCH_IMAGE_DESCRIPTOR array
	//

	pDevKeys = new BYTE[KEY_SIZE*m_dwTotalFX];
    if (pDevKeys == NULL) {
		err = ERROR_OUTOFMEMORY;
		goto errExit;
	}

	    //
    // open the output file used to generate the image
    //

    m_hOutputFile = CreateFile(
        (const char *)m_szScratchImageFileName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        0,
        NULL);

    if (m_hOutputFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        Print("\n Failed to open the dsp image file.Error 0x%x\n", err);
        return err;

    }

	for (dwGraphIndex=0;dwGraphIndex<m_dwGraphCount;dwGraphIndex++){


		for (dwFxIndex=0;dwFxIndex<m_pGraphs[dwGraphIndex]->GetNumEffects();dwFxIndex++){

			pCurrentFx = m_pGraphs[dwGraphIndex]->GetFxData(dwFxIndex);

			//
			// change the first OPCODE of the FX dsp code so it points to the correct
			// offset in X:RAM. This way it will load into register rD(r5), the proper state
			// variables
			//

			PDWORD opcode = (PDWORD)pCurrentFx->m_pDspCode;

			//
			// verify its a 
			// move x:(CONSTANT),r5 
			// opcode
			//

			if (*opcode == DSP_563000_OPCODE_MOVE_X_TO_R5_IMM) {

				//
				// the next word specifies the source memory address
				// change that to the correct X memory offset

				*opcode++;
				*opcode = dwStateIndex/sizeof(DWORD);

			}

			//
			// write code for this FX in the correct offset in the scratch image
			//

			dwTotalSize = SetFilePointer(m_hOutputFile,
				dwCodeIndex,
				NULL,
				FILE_BEGIN);

			if (dwTotalSize == -1) {
				err = GetLastError();
				goto errExit;
			}

			//
			// right before we write this to file, encode the dsp code
			// using the authors ey and our private key
			//

			err = XAudiopUtility_Encode(pCurrentFx->m_szDevKey,
				(PUCHAR)pCurrentFx->m_pDspCode,
				pCurrentFx->m_dwDspCodeSize,
				(PUCHAR)pCurrentFx->m_pDspCode,
				FALSE);
			
			if (err !=ERROR_SUCCESS) {
				goto errExit;
			}

			bResult = WriteFile(m_hOutputFile,
				pCurrentFx->m_pDspCode,
				pCurrentFx->m_dwDspCodeSize,
				&dwBytesWritten,
				0);
			
			if (!bResult) {
				
				err = GetLastError();
				DebugPrint(DBGLVL_ERROR,"Failed to write scratch image to file %s",m_szScratchImageFileName);
				goto errExit;
				
			}
	
			//
			// setup this FX description
			//

			pFxDesc->dwCodeOffset = dwCodeIndex;
			pFxDesc->dwCodeSize = pCurrentFx->m_dwDspCodeSize;

			//
			// create an absolute scratch space offset
			//

			pFxDesc->dwStateOffset = dwStateBaseIndex+dwStateIndex-DSP_FX_STATE_XMEMORY_BASE_OFFSET;
			pFxDesc->dwStateSize = pCurrentFx->m_dwDspStateSize;

			//
			// set the scratch delay line info
			//

			pFxDesc->dwScratchOffset = dwScratchIndex;
			pFxDesc->dwScratchSize = pCurrentFx->GetScratchSize();

			//
			// set the Y memory info
			//

			((PDSP_STATE_FIXED)pCurrentFx->m_pDspState)->dwYMemBase = dwYMemIndex;
			pFxDesc->dwYMemOffset = ((PDSP_STATE_FIXED)pCurrentFx->m_pDspState)->dwYMemBase;
			pFxDesc->dwYMemSize = ((PDSP_STATE_FIXED)pCurrentFx->m_pDspState)->dwYMemLength;

			dwYMemIndex += ((PDSP_STATE_FIXED)pCurrentFx->m_pDspState)->dwYMemLength;

			//
			// increment to next fx description array element
			//

			pFxDesc++;

			//
			// store the dev encryption key for the current dsp image into a buffer
			// that gets written after the FX description array
			//

			memcpy(pDevKeys + dwDevKeyBufferIndex,pCurrentFx->m_szDevKey,KEY_SIZE);
			dwDevKeyBufferIndex += KEY_SIZE;

			//
			// update code scratch image offset
			//

			dwCodeIndex += pCurrentFx->m_dwDspCodeSize;

			//
			// fixup the pointer addresses for inputs/outputs
			// the first dwords in the small state block for each module
			// point to the input/output locations. First you specify all
			// input pointers in a contigious array of WORDs, followed by all output
			// IDs specified in a array of WORDS
			//

			for (j = 0; j < pCurrentFx->m_dwNumInputs; j++) {

				((PDSP_STATE_FIXED)pCurrentFx->m_pDspState)->dwIoPtrs[j] = 
					BufferIDToDSPAddress(dwTempBinBaseIndex,pCurrentFx->m_dwInputIDs[j]);

			}				
			
			//
			// output dwords start after the last inputID  WORD.
			//

			DWORD base = j;
			for ( j = 0 ; j < pCurrentFx->m_dwNumOutputs; j++) {

				((PDSP_STATE_FIXED)pCurrentFx->m_pDspState)->dwIoPtrs[base+j] = 
					BufferIDToDSPAddress(dwTempBinBaseIndex,pCurrentFx->m_dwOutputIDs[j]);

			}
				

			//
			// based on the current scratch space usage, fixup the scratch space offsets
			// in the FX state buffer, so the FX knows where its scratch lives..
			// the index is in BYTES since DSP dma expects SM_OFFSET dma word in bytes
			//

			((PDSP_STATE_FIXED)pCurrentFx->m_pDspState)->dwScratchBase = dwScratchIndex;
			((PDSP_STATE_FIXED)pCurrentFx->m_pDspState)->dwScratchLength = pCurrentFx->GetScratchSize();

			//
			// increment our pointer into the scratch space, by the amount reserved
			// in this effect
			//

			dwScratchIndex += pCurrentFx->GetScratchSize();

			//
			// copy state variable block for this FX into global state block buffer
			// The state block needs to be placed at the right offset in scratch memory
			// The dsp execution engine then DMAs it in a Xmemory internal buffer, allocated
			// at offset DSP_XMEMORY_BASE_OFFSET. When we fix up dsp code here,
			// we need to do it like we are insid the dsp pointing at absolute X-Mem
			// addresses
			//

			dwTotalSize = SetFilePointer(m_hOutputFile,
				dwStateBaseIndex+dwStateIndex-DSP_FX_STATE_XMEMORY_BASE_OFFSET,
				NULL,
				FILE_BEGIN);

			if (dwTotalSize == -1) {
				err = GetLastError();
				goto errExit;
			}
			
			bResult = WriteFile(m_hOutputFile,
				pCurrentFx->m_pDspState,
				pCurrentFx->m_dwDspStateSize,
				&dwBytesWritten,
				0);
			
			if (!bResult) {
				
				err = GetLastError();
				DebugPrint(DBGLVL_ERROR,"Failed to write scratch image to file %s",m_szScratchImageFileName);
				goto errExit;
				
			}
				
			dwStateIndex += pCurrentFx->m_dwDspStateSize;

		}

	}	

	//
	// the very last word of the concatenated DSP fx code must be an RTS
	//

	j = DSP_563000_OPCODE_RTS;
	dwTotalSize = SetFilePointer(m_hOutputFile,
		dwCodeIndex,
		NULL,
		FILE_BEGIN);

	if (dwTotalSize == -1) {
		err = GetLastError();
		goto errExit;
	}
	
	bResult = WriteFile(m_hOutputFile,
		&j,
		sizeof(DWORD),
		&dwBytesWritten,
		0);
	
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to write scratch image to file %s",m_szScratchImageFileName);
		goto errExit;
		
	}

	// ************************************************************************
	//
	// write the FX description array right after the concatenated state blocks
	//
	// ************************************************************************

	dwTotalSize = SetFilePointer(m_hOutputFile,
		dwStateBaseIndex + m_dwTotalDspStateSize,
		NULL,
		FILE_BEGIN);
	
	if (dwTotalSize == -1) {
		err = GetLastError();
		goto errExit;
	}
	
	bResult = WriteFile(m_hOutputFile,
		pDescriptor,
		dwDescriptorSize,
		&dwBytesWritten,
		0);
	
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to write scratch image to file %s",m_szScratchImageFileName);
		goto errExit;
		
	}

	// ************************************************************************
	//
	// write the concatenated dev encryptions keys after the FX description array
	// first we scramble them with our internal key
	//
	// ************************************************************************

	UCHAR szKey[8];
	XAudiopUtility_GenerateKey(szKey);

	err = XAudiopUtility_Encode(szKey,
		pDevKeys,
		KEY_SIZE*m_dwTotalFX,
		pDevKeys,
		FALSE);
	
	if (err !=ERROR_SUCCESS) {
		goto errExit;
	}

	dwTotalSize = SetFilePointer(m_hOutputFile,
		dwStateBaseIndex + m_dwTotalDspStateSize + dwBytesWritten,
		NULL,
		FILE_BEGIN);
	
	if (dwTotalSize == -1) {
		err = GetLastError();
		goto errExit;
	}
	
	bResult = WriteFile(m_hOutputFile,
		pDevKeys,
		KEY_SIZE*m_dwTotalFX,
		&dwBytesWritten,
		0);
	
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to write scratch image to file %s",m_szScratchImageFileName);
		goto errExit;
		
	}

	// ************************************************************************
	// create a default command node and place it at the right offset
	// so the execution engine knows to upload all the FX and their state
	// ************************************************************************

	memset(&dspCommandBlock, 0 , sizeof(dspCommandBlock));

    dspCommandBlock.dwCommandFlags |= BIT_H2DCB_COMMAND_LOAD_CODE;
    dspCommandBlock.dwCommandFlags |= BIT_H2DCB_COMMAND_LOAD_XRAM_STATE;
    dspCommandBlock.dwCodeLength = m_dwTotalDspCodeSize/sizeof(DWORD);	
	dspCommandBlock.dwStateOffset = dwStateBaseIndex;
    dspCommandBlock.dwStateLength = m_dwTotalDspStateSize/sizeof(DWORD);

	// ************************************************************************
	// now create an image of the scratch space and store it in the output file
	//


	//
	// move file pointer to command node offset
	//

	dwTotalSize = SetFilePointer(m_hOutputFile, DSP_COMMANDBLOCK_SCRATCHOFFSET, NULL, FILE_BEGIN);
	if (dwTotalSize == -1) {
		err = GetLastError();
		goto errExit;
	}

	//
	// write command node
	//

	bResult = WriteFile(m_hOutputFile,
		&dspCommandBlock,
		sizeof(dspCommandBlock),
		&dwBytesWritten,
		0);
    
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to write scratch image to file %s",m_szScratchImageFileName);
		goto errExit;
		
	}


errExit:

	//
	// generate the output scratch space image
	//

	if (err != ERROR_SUCCESS) {

		//
		// delete output file
		//

		DeleteFile((const char *)m_szScratchImageFileName);

	}

	if (m_hOutputFile) {
		CloseHandle(m_hOutputFile);
		m_hOutputFile = NULL;
	}


	if (pDescriptor) {
		delete [] pDescriptor;
	}

	if (pDevKeys) {
		delete [] pDevKeys;
	}


    return err;

}


DWORD CDspImageBuilder::BufferIDToDSPAddress(DWORD dwTempBinBaseIndex,DWORD dwId)
{
	DWORD dwResult = -1;
	switch (dwId & FX_BUFFERID_MASK) {
	case FXOUTPUT_GPMIXBIN_BASE:

		//
		// the mem offsets were ORed when we parsed these
		// remove the ID bit
		//

		dwResult = dwId & ~FXOUTPUT_GPMIXBIN_BASE;
		break;

	case FXINPUT_GPMIXBIN_BASE:

		//
		// the mem offsets were ORed when we parsed these
		// remove the ID bit
		//

		dwResult = dwId & ~FXINPUT_GPMIXBIN_BASE;
		break;

	case FXINPUT_VPMIXBIN_BASE:

		//
		// parsing function setup the address offset
		//

		dwResult = dwId & ~FXINPUT_VPMIXBIN_BASE;
		break;

	case FXINPUT_GPTEMPBIN_BASE:

		dwResult = dwId & ~FXINPUT_GPTEMPBIN_BASE;

		//
		// create the base XRAM address for this mixbin
		//

		dwResult = dwTempBinBaseIndex/sizeof(DWORD) + dwResult*DSP_BUFFER_FRAME_LENGTH;
		break;

	case FXOUTPUT_GPTEMPBIN_BASE:

		dwResult = dwId & ~FXOUTPUT_GPTEMPBIN_BASE;

		//
		// create the base XRAM address for this mixbin
		//

		dwResult = dwTempBinBaseIndex/sizeof(DWORD) + dwResult*DSP_BUFFER_FRAME_LENGTH;
		break;

	}

	return dwResult;

}

#define INCR_BUFFER {\
    dwSize  += strlen(pszStr);\
	pszStr  += strlen(pszStr);}


int CDspImageBuilder::CreateEnumHeader()
{

    int err = ERROR_SUCCESS;

	BOOL bResult = FALSE;
	CFxData *pCurrentFx = NULL;
    DWORD dwBytesWritten,dwSize;

	DWORD dwGraphIndex, dwFxIndex;
	DWORD j;
	
	PCHAR pszString,pBuffer,pszStr;

	DeleteFile((const char *)m_szCHeaderFileName);

    m_hOutputFile = CreateFile(
        (const char *)m_szCHeaderFileName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        0,
        NULL);

    if (m_hOutputFile == INVALID_HANDLE_VALUE) {

        err = GetLastError();
        Print("\n Failed to open the generated header file.Error 0x%x\n", err);
        return err;

    }

	dwSize = SetFilePointer(m_hOutputFile,
		0,
		NULL,
		FILE_BEGIN);
	
	if (dwSize == -1) {
		err = GetLastError();
		goto errExit;
	}
	
	pszString = (PCHAR)new BYTE[MAX_FX_NAME];
	if (pszString == NULL) {
		err = ERROR_OUTOFMEMORY;
		goto errExit;
	}

	pBuffer = (PCHAR)new BYTE[MAX_FX_NAME*sizeof(DSP_STATE_FIXED)/sizeof(DWORD)];
	if (pBuffer == NULL) {
		err = ERROR_OUTOFMEMORY;
		goto errExit;
	}

	//
	// create the TYPEDEF ENUM string
	//

	sprintf(pszString,"\ntypedef enum _DSP_IMAGE_%s_FX_INDICES {\n",m_szImageFriendlyName);

	bResult = WriteFile(m_hOutputFile,
		pszString,
		strlen(pszString),
		&dwBytesWritten,
		0);
	
	if (!bResult) {
		
		err = GetLastError();
		DebugPrint(DBGLVL_ERROR,"Failed to write header file %s",m_szCHeaderFileName);
		goto errExit;
		
	}

	j = 0;

	for (dwGraphIndex=0;dwGraphIndex<m_dwGraphCount;dwGraphIndex++){
		for (dwFxIndex=0;dwFxIndex<m_pGraphs[dwGraphIndex]->GetNumEffects();dwFxIndex++){

			pCurrentFx = m_pGraphs[dwGraphIndex]->GetFxData(dwFxIndex);

			//
			// create an index for this effect
			//

			if (j == m_dwTotalFX-1) {

				CHAR string[256];

				sprintf(string,"DSP_IMAGE_%s_FX_INDICES",m_szImageFriendlyName);
				sprintf(pszString,"    %s_%s = %d\n} %s;\n",
					m_pGraphs[dwGraphIndex]->GetGraphName(),
					pCurrentFx->m_szFriendlyEffectName,
					j,
					string);

			} else {

				sprintf(pszString,"    %s_%s = %d,\n",
					m_pGraphs[dwGraphIndex]->GetGraphName(),
					pCurrentFx->m_szFriendlyEffectName,
					j);

			}

			bResult = WriteFile(m_hOutputFile,
				pszString,
				strlen(pszString),
				&dwBytesWritten,
				0);
			
			if (!bResult) {
				
				err = GetLastError();
				DebugPrint(DBGLVL_ERROR,"Failed to write header file %s",m_szCHeaderFileName);

				goto errExit;
				break;

			}
			
			j++;
		}
	}

	//
	// create a data structure for each FX state
	// it generate a data struct with the sam exact offset as DSP_STATE_FIXED
	// NOTE: YOU MUST KEEP THIS CODE IN SYNC WITH DSP_STATE_FIXED!!!!!
	//

	for (dwGraphIndex=0;dwGraphIndex<m_dwGraphCount;dwGraphIndex++){

		for (dwFxIndex=0;dwFxIndex<m_pGraphs[dwGraphIndex]->GetNumEffects();dwFxIndex++){

			pszStr = pBuffer;
			memset(pBuffer,0,MAX_FX_NAME*sizeof(DSP_STATE_FIXED)/sizeof(DWORD));
			
			dwSize = 0;

			pCurrentFx = m_pGraphs[dwGraphIndex]->GetFxData(dwFxIndex);

			sprintf(pszStr,"\ntypedef struct _%s_STATE {\n",pCurrentFx->m_szEffectName);
			INCR_BUFFER
			sprintf(pszStr,"    DWORD dwScratchOffset;        // Offset in bytes, of scratch area for this FX\n");
			INCR_BUFFER
			sprintf(pszStr,"    DWORD dwScratchLength;        // Length in DWORDS, of scratch area for this FX\n");
			INCR_BUFFER
			sprintf(pszStr,"    DWORD dwYMemoryOffset;        // Offset in DSP WORDS, of Y memory area for this FX\n");
			INCR_BUFFER
			sprintf(pszStr,"    DWORD dwYMemoryLength;        // Length in DSP WORDS, of Y memory area for this FX\n");
			INCR_BUFFER
			sprintf(pszStr,"    DWORD dwFlags;                // FX bitfield for various flags. See xgpimage documentation\n");
			INCR_BUFFER

				if (pCurrentFx->m_dwNumInputs > 0) {
					sprintf(pszStr,"    DWORD dwInMixbinPtrs[%d];      // XRAM offsets in DSP WORDS, of input mixbins\n",pCurrentFx->m_dwNumInputs);
					INCR_BUFFER
				}

				if (pCurrentFx->m_dwNumOutputs > 0) {
					sprintf(pszStr,"    DWORD dwOutMixbinPtrs[%d];     // XRAM offsets in DSP WORDS, of output mixbins\n",pCurrentFx->m_dwNumOutputs);
					INCR_BUFFER
				}

			sprintf(pszStr,"} %s_STATE, *LP%s_STATE;\n",pCurrentFx->m_szEffectName,pCurrentFx->m_szEffectName);
            INCR_BUFFER

			sprintf(pszStr,"\ntypedef const %s_STATE *LPC%s_STATE;\n",pCurrentFx->m_szEffectName,pCurrentFx->m_szEffectName);
            INCR_BUFFER

			bResult = WriteFile(m_hOutputFile,
				pBuffer,
				dwSize,
				&dwBytesWritten,
				0);
			
			if (!bResult) {
				
				err = GetLastError();
				DebugPrint(DBGLVL_ERROR,"Failed to write header file %s",m_szCHeaderFileName);

				goto errExit;
				break;

			}
			
		}
	}

errExit:
	if (pszString) {
		delete [] pszString;
	}

	if (pBuffer) {
		delete [] pBuffer;
	}

	CloseHandle(m_hOutputFile);
	m_hOutputFile = NULL;

	return err;

}
