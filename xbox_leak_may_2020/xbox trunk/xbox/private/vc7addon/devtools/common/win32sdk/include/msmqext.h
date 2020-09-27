/*****************************************************************************
 *
 * Copyright (c) Microsoft Corporation.  All rights reserved.
 *
 * msmqext.h
 *
 *****************************************************************************
 *
 * MSMQ Extension API constants and function prototypes file                 
 *
 */


#ifndef _MSMQEXT_H_
#define _MSMQEXT_H_

/****************************  C interface.  Always present.  ******************/
#ifdef __cplusplus
extern "C" {
#endif


/* New error and information codes: */
#define MQ_ERROR_CORRUPTED_EXTENSION_BUFFER      (MQ_ERROR + 0x100)
#define MQ_ERROR_EXTENSION_FIELD_NOT_FOUND       (MQ_ERROR + 0x101)
#define MQ_ERROR_ALLOC_FAIL                      (MQ_ERROR + 0x102)


typedef enum { EP_CURRENT_FIELD,
               EP_NEXT_FIELD,
               EP_NEXT_KEY_FIELD } NAVTYPE;

typedef const GUID *PCGUID;

/*
// EPOpen
//     Creates new extension, unpacks the supplied extension buffer.
//
//   Parameters:
//     OUTPUT PHANDLE phExtension
//     INPUT  void*   pExtBuffer
//     INPUT  DWORD   dwExtBufLength
//
//   Returns:
//     MQ_OK
//     MQ_ERROR_INVALID_PARAMETER
//     MQ_ERROR_CORRUPTED_EXTENSION_BUFFER
//     MQ_ERROR_ALLOC_FAIL
//
//   Remarks:  1. If pExtBuffer is NULL of dwExtBufLength is 0,
//                empty extension is created.
*/
HRESULT EPOpen(PHANDLE phExtension, void *pExtBuffer, DWORD dwExtBufLength);


/*
// EPClose
//     Frees extension handle and associated memory.
//
//   Parameters:
//     INOUT PHANDLE hExtension
//
//   Returns:
//     MQ_OK
//     MQ_ERROR_INVALID_PARAMETER
//     MQ_ERROR_INVALID_HANDLE
*/
HRESULT EPClose(PHANDLE phExtension);


/*
// EPGetBuffer
//     Packs extension into the supplied buffer.
//
//   Parameters:
//     INPUT HANDLE hExtension
//     INPUT void*  pBuf
//     INPUT PDWORD pdwBufLength   - length of the extension buffer.
//
//   Returns:
//     MQ_OK
//     MQ_ERROR_INVALID_PARAMETER
//     MQ_ERROR_INVALID_HANDLE
//     MQ_ERROR_USER_BUFFER_TOO_SMALL
//
//   Remarks:  1. If the call was successful, *pdwBufLength contains the actual
//                length of the packed extension buffer.
//             2. If MQ_ERROR_USER_BUFFER_TOO_SMALL is returned, *pdwBufLength
//                contains the required buffer length.
*/
HRESULT EPGetBuffer(HANDLE hExtension, void *pBuf, PDWORD pdwBufLength);


/*
// EPGet
//     Positions to [and/or retrieves] a requested field.
//
//   Parameters:
//     INPUT  HANDLE  hExtension
//     INPUT  NAVTYPE Directive
//     INOUT  PHANDLE phCursor
//     INOUT  GUID*   pFieldId
//     OUTPUT void*   pFieldData
//     INOUT  PDWORD  pdwDataLength
//
//   Returns:
//     MQ_OK
//     MQ_ERROR_EXTENSION_FIELD_NOT_FOUND
//     MQ_ERROR_INVALID_PARAMETER
//     MQ_ERROR_INVALID_HANDLE
//     MQ_ERROR_USER_BUFFER_TOO_SMALL
//     MQ_ERROR_ALLOC_FAIL
//
//   Remarks:  
//     1.  Directive and *phCursor together define the behavior of the function.
//         If Directive is EP_CURRENT_FIELD, then
//            *phCursor must be a valid non-NULL cursor handle; the field
//            which it points to, will be returned.
//         If Directive is EP_NEXT_FIELD, then
//            if phCursor is NULL or *phCursor is NULL, then
//               The first field's ID and data are returned.
//               if phCursor is not NULL, *phCursor is set to the field.
//            otherwise (phCursor!=NULL and *phCursor!=NULL)
//               *phCursor goes to the next field, if such exists.
//               The field's ID and data are returned.
//         If Directive is EP_NEXT_KEY_FIELD, then
//            if phCursor is NULL or *phCursor is NULL, then
//               The function looks for a first field in extension with
//               FieldID == *pFieldID parameter.
//               If the field is found it's ID and data are returned, and
//               if phCursor is not NULL, *phCursor is set to the field.
//            otherwise (phCursor!=NULL and *phCursor!=NULL)
//               *phCursor goes to the next field with matching ID, if such exists.
//               The field's data is returned.
//     2.  If Directive is EP_NEXT_KEY_FIELD, and field with matching ID doesn't exist,
//         the funciton returns MQ_ERROR_EXTENSION_FIELD_NOT_FOUND and positions the
//         cursor to the field with the next greater ID.
//         If no field at all is available, the cursor is set to NULL, i.e. to the
//         beginning of the extension.
//     3.  If pFieldData parameter is NULL, no data is returned.
//         If pFieldData is not NULL, pdwDataLength must not be NULL.
//     4.  If pdwDataLength is not NULL, it must point to DWORD, which on input must
//         contain the size of buffer pointed by pFieldData (ignored if pFieldData==NULL).
//         On output *pdwDataLength will contain actual length of the FieldData
//         (independent on validity of pFieldData parameter)
//     5.  pFieldID must be non-NULL for Directive==EP_NEXT_KEY_FIELD.  For other
//         directives, if it is non-NULL, it will receive the retrieved field's ID.
*/
HRESULT EPGet(HANDLE hExtension, NAVTYPE Directive, PHANDLE phCursor,
                      GUID* pFieldId, void *pFieldData, PDWORD pdwDataLength);


/*
// EPAdd
//     adds a field to extension.
//
//   Parameters:
//     INPUT  HANDLE  hExtension
//     INPUT  PCGUID  pFieldId
//     INPUT  void*   pFieldData
//     INPUT  DWORD   dwDataLength
//     OUTPUT PHANDLE phCursor
//
//   Returns:
//     MQ_OK
//     MQ_ERROR_INVALID_PARAMETER
//     MQ_ERROR_INVALID_HANDLE
//     MQ_ERROR_ALLOC_FAIL
//
//   Remarks:  1. If phCursor is not NULL, the cursor will point to the
//                added field (if the call was successful).
*/
HRESULT EPAdd(HANDLE hExtension,PCGUID pFieldID, void *pFieldData, DWORD dwDataLength, 
                                                                        PHANDLE phCursor);


/*
// EPUpdate
//     writes new data to a field.
//
//   Parameters:
//     INPUT HANDLE hExtension
//     INPUT HANDLE hCursor
//     INPUT void*  pFieldData
//     INPUT DWORD  dwDataLength
//
//   Returns:
//     MQ_OK
//     MQ_ERROR_INVALID_PARAMETER
//     MQ_ERROR_INVALID_HANDLE
//     MQ_ERROR_ALLOC_FAIL
*/
HRESULT EPUpdate(HANDLE hExtension, HANDLE hCursor, void *pFieldData, DWORD dwDataLength);


/*
// EPDelete
//     deletes a field.
//
//   Parameters:
//     INPUT HANDLE  hExtension
//     INOUT PHANDLE phCursor
//
//   Returns:
//     MQ_OK
//     MQ_ERROR_INVALID_PARAMETER
//     MQ_ERROR_INVALID_HANDLE
//
//   Remarks:  1. After successful deletion the cursor is set to point to the next
//                field after the deleted one.  If the last field is deleted,
//                the cursor is set to NULL, i.e. to the beginning.
*/
HRESULT EPDelete(HANDLE hExtension, PHANDLE phCursor);


/*
// EPDeleteAll
//     deletes all fields with matching ID.
//
//   Parameters:
//     INPUT  HANDLE  hExtension
//     INPUT  PCGUID  pFieldsId
//     OUTPUT PHANDLE phCursor
//
//   Returns:
//     MQ_OK
//     MQ_ERROR_INVALID_PARAMETER
//     MQ_ERROR_INVALID_HANDLE
//     MQ_ERROR_EXTENSION_FIELD_NOT_FOUND
//
//   Remarks:  1. If the phCursor is not NULL, *phCursor will point to the next field
//                after all the deleted ones 
//                (even if MQ_ERROR_EXTENSION_FIELD_NOT_FOUND is returned)
//                If the last field is deleted, the cursor is set to NULL.
*/
HRESULT EPDeleteAll(HANDLE hExtension, PCGUID pFieldsID, PHANDLE phCursor);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*_MSMQEXT_H_ */
