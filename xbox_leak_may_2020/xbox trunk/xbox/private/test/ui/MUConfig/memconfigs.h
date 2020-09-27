/*****************************************************
*** memconfigs.h
***
*** Header file for our library of functions that
*** will allows us to configure various memory
*** areas in different ways
***
*** by James N. Helm
*** June 20th, 2001
***
*****************************************************/

#ifndef _MEMCONFIGS_H_
#define _MEMCONFIGS_H_

#include "memoryarea.h"
#include "memoryunit.h"

HRESULT CreateMU_A( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_B( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_C( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_D( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_E( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_F( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_G( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_H( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_I( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_J( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_K( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_L( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_M( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_N( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_O( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_P( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_Q( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_R( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_S( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_T( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_U( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_V( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_W( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_X( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_Y( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_Z( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_AA( CMemoryUnit* pMemoryUnit );
HRESULT CreateMU_BB( CMemoryUnit* pMemoryUnit );

void CreateSortOrderGames( CMemoryArea* pMemoryArea, bool english );
void CreateMetaVarGames( CMemoryArea* pMemoryArea );
void CreateGamesAndSaves( CMemoryUnit* pMemoryUnit, unsigned int uiNumGames, unsigned int uiNumSaved );
void CreateGenGame( CMemoryArea* pMemoryArea, DWORD dwTitleID, WCHAR* pwszGameName, unsigned int uiNumSavedGames, BOOL bPrependLangCodeToName=TRUE );

#define MEMCONFIGS_32X32_IMAGE_FILENAME         "32x32image.xbx"
#define MEMCONFIGS_32X32_IMAGE_PATHINFO         FILE_DATA_DATA_DIRECTORY_A "\\" MEMCONFIGS_32X32_IMAGE_FILENAME
#define MEMCONFIGS_128X128_IMAGE_FILENAME       "128x128image.xbx"
#define MEMCONFIGS_128X128_IMAGE_PATHINFO       FILE_DATA_DATA_DIRECTORY_A "\\" MEMCONFIGS_128X128_IMAGE_FILENAME
#define MEMCONFIGS_256X256_IMAGE_FILENAME       "256x256image.xbx"
#define MEMCONFIGS_256X256_IMAGE_PATHINFO       FILE_DATA_DATA_DIRECTORY_A "\\" MEMCONFIGS_256X256_IMAGE_FILENAME
#define MEMCONFIGS_1024X1024_IMAGE_FILENAME     "1024x1024image.xbx"
#define MEMCONFIGS_1024X1024_IMAGE_PATHINFO     FILE_DATA_DATA_DIRECTORY_A "\\" MEMCONFIGS_1024X1024_IMAGE_FILENAME
#define MEMCONFIGS_CORRUPT_IMAGE_FILENAME       "corruptimage.xbx"
#define MEMCONFIGS_CORRUPT_IMAGE_PATHINFO       FILE_DATA_DATA_DIRECTORY_A "\\" MEMCONFIGS_CORRUPT_IMAGE_FILENAME

#endif //_MEMCONFIGS_H_