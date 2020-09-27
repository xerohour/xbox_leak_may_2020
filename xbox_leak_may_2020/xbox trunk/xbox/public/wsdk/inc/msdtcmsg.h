/* IMPORTANT - PLEASE READ BEFORE EDITING FILE
  This file is divided into four sections. They are:
	1. Success Codes
	2. Information Codes
	3. Warning Codes
	4. Error Codes

  Please enter your codes in the appropriate section.
  All codes must be in sorted order.  Please use codes
  in the middle that are free before using codes in the end.
  Shaiwal Singh
*/
/******************************* Success Codes ***************************************/
/************************************ Information Codes ***************************************/
//
// MessageId: XACT_S_ASYNC
//
// MessageText:
//
//  An asynchronous abort was started.
//
#define XACT_S_ASYNC                     ((HRESULT)0x0004D000L)

//
// MessageId: XACT_S_ABORTING
//
// MessageText:
//
//  An abort operation was already in progress. This call was ignored.
//
#define XACT_S_ABORTING                  ((HRESULT)0x0004D008L)

/************************************ Warning Codes ***************************************/
/************************************ Error Codes ***************************************/
//
// MessageId: XACT_E_CANTRETAIN
//
// MessageText:
//
//  A retaining abort is not supported or a new unit of work could not be created.
//
#define XACT_E_CANTRETAIN                ((HRESULT)0x8004D001L)

//
// MessageId: XACT_E_COMMITFAILED
//
// MessageText:
//
//  The transaction failed to commit for an unknown reason.
//
#define XACT_E_COMMITFAILED              ((HRESULT)0x8004D002L)

//
// MessageId: XACT_E_ISOLATIONLEVEL
//
// MessageText:
//
//  Neither the isolation level or a strengthening of it can be supported.
//
#define XACT_E_ISOLATIONLEVEL            ((HRESULT)0x8004D008L)

//
// MessageId: XACT_E_NOENLIST
//
// MessageText:
//
//  The new transaction was unable to enlist in the specified transaction coordinator. 
//
#define XACT_E_NOENLIST                  ((HRESULT)0x8004D00AL)

//
// MessageId: XACT_E_NOISORETAIN
//
// MessageText:
//
//  The requested semantics of retention of isolation can not be supported.
//
#define XACT_E_NOISORETAIN               ((HRESULT)0x8004D00BL)

//
// MessageId: XACT_E_NOTRANSACTION
//
// MessageText:
//
//  No transaction is active.
//
#define XACT_E_NOTRANSACTION             ((HRESULT)0x8004D00EL)

//
// MessageId: XACT_E_NOTSUPPORTED
//
// MessageText:
//
//  Specified operation can not be supported.
//
#define XACT_E_NOTSUPPORTED              ((HRESULT)0x8004D00FL)

//
// MessageId: XACT_E_XTIONEXISTS
//
// MessageText:
//
//  Only one transaction can be active on this session.
//
#define XACT_E_XTIONEXISTS               ((HRESULT)0x8004D013L)

//
// MessageId: XACT_E_INDOUBT
//
// MessageText:
//
//  The transaction status is in doubt.
//
#define XACT_E_INDOUBT                   ((HRESULT)0x8004D016L)

//
// MessageId: XACT_E_NOTIMEOUT
//
// MessageText:
//
//  A time-out was specified, but time-outs are not supported.
//
#define XACT_E_NOTIMEOUT                 ((HRESULT)0x8004D017L)

//
// MessageId: XACT_E_ALREADYINPROGRESS
//
// MessageText:
//
//  A commit or abort operation was already in progress. This call was ignored.
//
#define XACT_E_ALREADYINPROGRESS         ((HRESULT)0x8004D018L)

//
// MessageId: XACT_E_ABORTED
//
// MessageText:
//
//  The transaction was aborted before commit was called.
//
#define XACT_E_ABORTED                   ((HRESULT)0x8004D019L)

//
// MessageId: XACT_E_LOGFULL
//
// MessageText:
//
//  Unable to begin a new transaction because the log file is full.
//
#define XACT_E_LOGFULL                   ((HRESULT)0x8004D01AL)

//
// MessageId: XACT_E_TMNOTAVAILABLE
//
// MessageText:
//
//  Unable to connect to the transaction manager or the transaction manager is unavailable.
//
#define XACT_E_TMNOTAVAILABLE            ((HRESULT)0x8004D01BL)

//
// MessageId: XACT_E_CONNECTION_DOWN
//
// MessageText:
//
//  The connection to the transaction manager failed.
//
#define XACT_E_CONNECTION_DOWN           ((HRESULT)0x8004D01CL)

//
// MessageId: XACT_E_CONNECTION_DENIED
//
// MessageText:
//
//  A new transaction could not be created due to a unspecified capacity issue.
//
#define XACT_E_CONNECTION_DENIED         ((HRESULT)0x8004D01DL)

//
// MessageId: XACT_E_CONNECTION_REQUEST_DENIED
//
// MessageText:
//
//  The transaction manager did not accept a connection request.
//
#define XACT_E_CONNECTION_REQUEST_DENIED ((HRESULT)0x8004D100L)

/* IMPORTANT - PLEASE READ BEFORE EDITING FILE
  This file is divided into four sections. They are:
	1. Success Codes
	2. Information Codes
	3. Warning Codes
	4. Error Codes

  Please enter your codes in the appropriate section.
  All codes must be in sorted order.  Please use codes
  in the middle that are free before using codes in the end.
  Shaiwal Singh - Please keep this comment in the end of the file
*/
