/*****************************************************************************/
/* winappc.h                                                                 */
/* Copyright Data Connection Ltd. 1989 - 2000                                */
/* Copyright (c) Microsoft Corporation.  All rights reserved.                */
/*                                                                           */
/* APPC constant, vcb and function definitions                               */
/*****************************************************************************/

#ifndef APPC_C_INCLUDED
#define APPC_C_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* Set WINSNA if this is Win16 or Win32                                      */
/*****************************************************************************/
#if ( defined(WINDOWS) || defined(WIN32) )
 #define WINSNA
#endif




/* ========================================================================= */
/*****************************************************************************/
/* ------------------------- macro definitions ----------------------------- */
/*****************************************************************************/

/*****************************************************************************/
/* for Win32 environment:                                                    */
/* -  ensure correct packing                                                 */
/* -  remove references to far                                               */
/*****************************************************************************/
#ifdef WIN32
#ifdef _WIN64
 #pragma pack()
#else
 #pragma pack(4)
#endif
#define far

#endif

/*****************************************************************************/
/* define type conversion macros                                             */
/*****************************************************************************/
#define APPC_LO_UC(w)    ((unsigned char)(((unsigned short)(w)) & 0xFF))
#define APPC_HI_UC(w)    ((unsigned char)(((unsigned short)(w) >> 8)  \
                            & 0xff))
#define APPC_MAKUS(l, h) (((unsigned short)(l)) |                     \
                          ((unsigned short)(h)) << 8)
#define APPC_MAKUL(l, h) ((unsigned long)(((unsigned short)(l))|      \
                          ((unsigned long)((unsigned short)(h))) << 16))
#define APPC_LO_US(l)   ((unsigned short)(((unsigned long)(l)) & 0xFFFF))
#define APPC_HI_US(l)    ((unsigned short)(((unsigned long)(l) >> 16) \
                           & 0xffff))

/*****************************************************************************/
/* IF non-Intel THEN                                                         */
/*   set flipping to nop for now                                             */
/*****************************************************************************/
#ifdef NON_INTEL_BYTE_ORDER
#define APPC_FLIPI(X)  (X)
#define APPC_FLIPL(X)  (X)

/*****************************************************************************/
/* ELSE (Intel environment)                                                  */
/*   define flipping macros                                                  */
/*****************************************************************************/
#else
#define APPC_FLIPI(X) APPC_MAKUS(APPC_HI_UC(X),APPC_LO_UC(X))
#define APPC_FLIPL(X) APPC_MAKUL(APPC_FLIPI(APPC_HI_US(X)),           \
                                 APPC_FLIPI(APPC_LO_US(X)))
#endif

#ifdef WIN32
#define far
#endif

/* ========================================================================= */
/*****************************************************************************/
/* ------------------------- verb opcodes ---------------------------------- */
/*****************************************************************************/

/*****************************************************************************/
/* control verbs                                                             */
/*****************************************************************************/
#define AP_TP_STARTED                   APPC_FLIPI(0x1400)        /* X'1400' */
#define AP_TP_STARTED_EX                APPC_FLIPI(0x9400)        /* X'9400' */
#define AP_RECEIVE_ALLOCATE             APPC_FLIPI(0x1600)        /* X'1600' */
#define AP_TP_ENDED                     APPC_FLIPI(0x1300)        /* X'1300' */


/*****************************************************************************/
/* Activate/deactivate session verbs                                         */
/*****************************************************************************/
#define  AP_DEACTIVATE_SESSION          APPC_FLIPI(0x2E00)
#define  AP_ACTIVATE_SESSION            APPC_FLIPI(0x1410)

/*****************************************************************************/
/* Opcode for RCV_ALLOC with a list of TP names                              */
/*                                                                           */
/* Though defined for all OS, this verb will only be allowed for WINSNA      */
/* environments                                                              */
/*****************************************************************************/
#define AP_RECEIVE_ALLOCATE_X           APPC_FLIPI(0x9600) /* X'1600'+X'8000'*/
#define AP_RECEIVE_ALLOCATE_EX          APPC_FLIPI(0xF103)        /* X'F103' */
#define AP_RECEIVE_ALLOCATE_EX_END      APPC_FLIPI(0xF104)        /* X'F104' */


/*****************************************************************************/
/* management verbs                                                          */
/*****************************************************************************/
#define AP_CNOS                         APPC_FLIPI(0x1500)        /* X'1500' */
#define AP_DISPLAY                      APPC_FLIPI(0X1B00)        /* X'1B00' */

/*****************************************************************************/
/* mapped conversation verbs                                                 */
/*****************************************************************************/
#define AP_M_ALLOCATE                   APPC_FLIPI(0x0100)        /* X'0100' */
#define AP_M_CONFIRM                    APPC_FLIPI(0x0300)        /* X'0300' */
#define AP_M_CONFIRMED                  APPC_FLIPI(0x0400)        /* X'0400' */
#define AP_M_DEALLOCATE                 APPC_FLIPI(0x0500)        /* X'0500' */
#define AP_M_FLUSH                      APPC_FLIPI(0x0600)        /* X'0600' */
#define AP_M_GET_ATTRIBUTES             APPC_FLIPI(0x0700)        /* X'0700' */
#define AP_M_PREPARE_TO_RECEIVE         APPC_FLIPI(0x0a00)        /* X'0a00' */
#define AP_M_RECEIVE_AND_POST           APPC_FLIPI(0x0d00)        /* X'0d00' */
#define AP_M_RECEIVE_AND_WAIT           APPC_FLIPI(0x0b00)        /* X'0b00' */
#define AP_M_RECEIVE_IMMEDIATE          APPC_FLIPI(0x0c00)        /* X'0c00' */
#define AP_M_REQUEST_TO_SEND            APPC_FLIPI(0x0e00)        /* X'0e00' */
#define AP_M_SEND_CONVERSATION          APPC_FLIPI(0x1800)        /* X'1800' */
#define AP_M_SEND_DATA                  APPC_FLIPI(0x0f00)        /* X'0f00' */
#define AP_M_SEND_ERROR                 APPC_FLIPI(0x1000)        /* X'1000' */
#define AP_M_TEST_RTS                   APPC_FLIPI(0x1200)        /* X'1200' */
#define AP_M_TEST_RTS_AND_POST          APPC_FLIPI(0x8100)        /* X'8100' */
#ifdef WIN32
#define AP_M_POST_ON_RECEIPT            APPC_FLIPI(0x8200)        /* X'8200' */
#endif
#define AP_M_RECEIVE_LOG_DATA           APPC_FLIPI(0x8300)        /* X'8300' */

/*****************************************************************************/
/* basic conversation verbs                                                  */
/*****************************************************************************/
#define AP_B_ALLOCATE                   APPC_FLIPI(0x0100)        /* X'0100' */
#define AP_B_CONFIRM                    APPC_FLIPI(0x0300)        /* X'0300' */
#define AP_B_CONFIRMED                  APPC_FLIPI(0x0400)        /* X'0400' */
#define AP_B_DEALLOCATE                 APPC_FLIPI(0x0500)        /* X'0500' */
#define AP_B_FLUSH                      APPC_FLIPI(0x0600)        /* X'0600' */
#define AP_B_GET_ATTRIBUTES             APPC_FLIPI(0x0700)        /* X'0700' */
#define AP_B_PREPARE_TO_RECEIVE         APPC_FLIPI(0x0a00)        /* X'0a00' */
#define AP_B_RECEIVE_AND_POST           APPC_FLIPI(0x0d00)        /* X'0d00' */
#define AP_B_RECEIVE_AND_WAIT           APPC_FLIPI(0x0b00)        /* X'0b00' */
#define AP_B_RECEIVE_IMMEDIATE          APPC_FLIPI(0x0c00)        /* X'0c00' */
#define AP_B_REQUEST_TO_SEND            APPC_FLIPI(0x0e00)        /* X'0e00' */
#define AP_B_SEND_CONVERSATION          APPC_FLIPI(0x1800)        /* X'1800' */
#define AP_B_SEND_DATA                  APPC_FLIPI(0x0f00)        /* X'0f00' */
#define AP_B_SEND_ERROR                 APPC_FLIPI(0x1000)        /* X'1000' */
#define AP_B_TEST_RTS                   APPC_FLIPI(0x1200)        /* X'1200' */
#define AP_B_TEST_RTS_AND_POST          APPC_FLIPI(0x8100)        /* X'8100' */
#ifdef WIN32
#define AP_B_POST_ON_RECEIPT            APPC_FLIPI(0x8200)        /* X'8200' */
#endif
#define AP_B_RECEIVE_LOG_DATA           APPC_FLIPI(0x8300)        /* X'8300' */

/*****************************************************************************/
/* type independent verbs                                                    */
/*****************************************************************************/
#define AP_GET_TP_PROPERTIES            APPC_FLIPI(0x1700)        /* X'1700' */
#define AP_GET_TYPE                     APPC_FLIPI(0x0800)        /* X'0800' */
#define AP_GET_STATE                    APPC_FLIPI(0x0900)        /* X'0900' */
#ifdef SYNCPT_SUPPORT
#define AP_ENLIST                       APPC_FLIPI(0x7d00)        /* X'7d00' */
#define AP_SET_TP_PROPERTIES            APPC_FLIPI(0x7e00)        /* X'7e00' */
#define AP_GET_LU_STATUS                APPC_FLIPI(0x7f00)        /* X'7f00' */
#endif

/* ========================================================================= */
/*****************************************************************************/
/* ------------------- primary and secondary return codes ------------------ */
/*****************************************************************************/
#define AP_OK                           APPC_FLIPI(0x0000)      /* X'0000'   */

#define AP_PARAMETER_CHECK              APPC_FLIPI(0x0001)      /* X'0001'   */
  #define AP_BAD_TP_ID                   APPC_FLIPL(0x00000001) /*X'00000001'*/
  #define AP_BAD_CONV_ID                 APPC_FLIPL(0x00000002) /*X'00000002'*/
  #define AP_BAD_LU_ALIAS                APPC_FLIPL(0x00000003) /*X'00000003'*/
  #define AP_INVALID_DATA_SEGMENT        APPC_FLIPL(0x00000006) /*X'00000006'*/
  #define AP_BAD_CONV_TYPE               APPC_FLIPL(0x00000011) /*X'00000011'*/
  #define AP_BAD_SYNC_LEVEL              APPC_FLIPL(0x00000012) /*X'00000012'*/
  #define AP_BAD_SECURITY                APPC_FLIPL(0x00000013) /*X'00000013'*/
  #define AP_BAD_RETURN_CONTROL          APPC_FLIPL(0x00000014) /*X'00000014'*/
  #define AP_PIP_LEN_INCORRECT           APPC_FLIPL(0x00000016) /*X'00000016'*/
  #define AP_NO_USE_OF_SNASVCMG          APPC_FLIPL(0x00000017) /*X'00000017'*/
  #define AP_UNKNOWN_PARTNER_MODE        APPC_FLIPL(0x00000018) /*X'00000018'*/
  #define AP_CONFIRM_ON_SYNC_LEVEL_NONE  APPC_FLIPL(0x00000031) /*X'00000031'*/
  #define AP_DEALLOC_BAD_TYPE            APPC_FLIPL(0x00000051) /*X'00000051'*/
  #define AP_DEALLOC_LOG_LL_WRONG        APPC_FLIPL(0x00000057) /*X'00000057'*/
  #define AP_P_TO_R_INVALID_TYPE         APPC_FLIPL(0x000000a1) /*X'000000a1'*/
  #define AP_RCV_AND_WAIT_BAD_FILL       APPC_FLIPL(0x000000b5) /*X'000000b5'*/
  #define AP_RCV_IMMD_BAD_FILL           APPC_FLIPL(0x000000c4) /*X'000000c4'*/
  #define AP_RCV_AND_POST_BAD_FILL       APPC_FLIPL(0x000000d5) /*X'000000d5'*/
  #define AP_INVALID_SEMAPHORE_HANDLE    APPC_FLIPL(0x000000d6) /*X'000000d6'*/
  #define AP_BAD_RETURN_STATUS_WITH_DATA APPC_FLIPL(0x000000d7) /*X'000000d7'*/
  #define AP_BAD_LL                      APPC_FLIPL(0x000000f1) /*X'000000f1'*/
  #define AP_SEND_DATA_INVALID_TYPE      APPC_FLIPL(0x000000f4) /*X'000000f4'*/
  #define AP_INVALID_SESSION_ID          APPC_FLIPL(0x00000512) /*X'00000512'*/
  #define AP_INVALID_POLARITY            APPC_FLIPL(0x00000513) /*X'00000513'*/
  #define AP_INVALID_TYPE                APPC_FLIPL(0x00000514) /*X'00000514'*/
  #define AP_INVALID_LU_ALIAS            APPC_FLIPL(0x00000515) /*X'00000515'*/
  #define AP_INVALID_PLU_ALIAS           APPC_FLIPL(0x00000516) /*X'00000516'*/
  #define AP_INVALID_MODE_NAME           APPC_FLIPL(0x00000517) /*X'00000517'*/
  #define AP_INVALID_TRANSACT_ID         APPC_FLIPL(0x00000518) /*X'00000518'*/



  #define AP_SEND_DATA_CONFIRM_SYNC_NONE APPC_FLIPL(0x000000f5) /*X'000000f5'*/
  #define AP_BAD_PARTNER_LU_ALIAS        APPC_FLIPL(0x0000015b) /*X'0000015b'*/

  #define AP_SEND_ERROR_LOG_LL_WRONG     APPC_FLIPL(0x00000102) /*X'00000102'*/
  #define AP_SEND_ERROR_BAD_TYPE         APPC_FLIPL(0x00000103) /*X'00000103'*/
  #define AP_BAD_ERROR_DIRECTION         APPC_FLIPL(0x00000105) /*X'00000105'*/
  #define AP_TOO_MANY_TPS                APPC_FLIPL(0x00000243) /*X'00000243'*/
  #define AP_BAD_TYPE                    APPC_FLIPL(0x00000250) /*X'00000250'*/
  #define AP_UNDEFINED_TP_NAME           APPC_FLIPL(0x00000506) /*X'00000506'*/
  #define AP_INVALID_SET_PROT            APPC_FLIPL(0x00000700) /*X'00000700'*/
  #define AP_INVALID_NEW_PROT            APPC_FLIPL(0x00000701) /*X'00000701'*/
  #define AP_INVALID_SET_UNPROT          APPC_FLIPL(0x00000702) /*X'00000702'*/
  #define AP_INVALID_NEW_UNPROT          APPC_FLIPL(0x00000703) /*X'00000703'*/
  #define AP_INVALID_SET_USER            APPC_FLIPL(0x00000704) /*X'00000704'*/
  #define AP_INVALID_DATA_TYPE           APPC_FLIPL(0x00000705) /*X'00000705'*/
  #define AP_BAD_LOCAL_LU_ALIAS          APPC_FLIPL(0x01000003) /*X'01000003'*/
  #define AP_BAD_REMOTE_LU_ALIAS         APPC_FLIPL(0x02000003) /*X'02000003'*/
#ifdef WIN32
  #define AP_POST_ON_RECEIPT_BAD_FILL    APPC_FLIPL(0x03000003) /*X'03000003'*/
#endif
#define AP_STATE_CHECK                  APPC_FLIPI(0x0002)      /* X'0002'   */
  #define AP_CONFIRM_BAD_STATE           APPC_FLIPL(0x00000032) /*X'00000032'*/
  #define AP_CONFIRM_NOT_LL_BDY          APPC_FLIPL(0x00000033) /*X'00000033'*/
  #define AP_CONFIRMED_BAD_STATE         APPC_FLIPL(0x00000041) /*X'00000041'*/
  #define AP_DEALLOC_FLUSH_BAD_STATE     APPC_FLIPL(0x00000052) /*X'00000052'*/
  #define AP_DEALLOC_CONFIRM_BAD_STATE   APPC_FLIPL(0x00000053) /*X'00000053'*/
  #define AP_DEALLOC_NOT_LL_BDY          APPC_FLIPL(0x00000055) /*X'00000055'*/
  #define AP_FLUSH_NOT_SEND_STATE        APPC_FLIPL(0x00000061) /*X'00000061'*/
  #define AP_P_TO_R_NOT_LL_BDY           APPC_FLIPL(0x000000a2) /*X'000000a2'*/
  #define AP_P_TO_R_NOT_SEND_STATE       APPC_FLIPL(0x000000a3) /*X'000000a3'*/
  #define AP_RCV_AND_WAIT_BAD_STATE      APPC_FLIPL(0x000000b1) /*X'000000b1'*/
  #define AP_RCV_AND_WAIT_NOT_LL_BDY     APPC_FLIPL(0x000000b2) /*X'000000b2'*/
  #define AP_RCV_IMMD_BAD_STATE          APPC_FLIPL(0x000000c1) /*X'000000c1'*/
  #define AP_RCV_AND_POST_BAD_STATE      APPC_FLIPL(0x000000d1) /*X'000000d1'*/
  #define AP_RCV_AND_POST_NOT_LL_BDY     APPC_FLIPL(0x000000d2) /*X'000000d2'*/
  #define AP_R_T_S_BAD_STATE             APPC_FLIPL(0x000000e1) /*X'000000e1'*/
  #define AP_SEND_DATA_NOT_SEND_STATE    APPC_FLIPL(0x000000f2) /*X'000000f2'*/
  #define AP_SEND_DATA_NOT_LL_BDY        APPC_FLIPL(0x000000f6) /*X'000000f6'*/
  #define AP_ATTACH_MANAGER_INACTIVE     APPC_FLIPL(0x00000508) /*X'00000508'*/
  #define AP_ALLOCATE_NOT_PENDING        APPC_FLIPL(0x00000509) /*X'00000509'*/
  #define AP_LU_ALREADY_REGISTERED       APPC_FLIPL(0x0000050A) /*X'0000050A'*/
  #define AP_INVALID_PROCESS             APPC_FLIPL(0x00000525) /*X'00000525'*/

#define AP_ALLOCATION_ERROR             APPC_FLIPI(0x0003)      /* X'0003'   */
  #define AP_ALLOCATION_FAILURE_NO_RETRY APPC_FLIPL(0x00000004) /*X'00000004'*/
  #define AP_ALLOCATION_FAILURE_RETRY    APPC_FLIPL(0x00000005) /*X'00000005'*/
  #define AP_SECURITY_NOT_VALID          APPC_FLIPL(0x080f6051) /*X'080f6051'*/
  #define AP_TRANS_PGM_NOT_AVAIL_RETRY   APPC_FLIPL(0x084b6031) /*X'084b6031 */
  #define AP_TRANS_PGM_NOT_AVAIL_NO_RETRY APPC_FLIPL(0x084c0000) /*X'084c0000*/
  #define AP_TP_NAME_NOT_RECOGNIZED      APPC_FLIPL(0x10086021) /*X'10086021'*/
  #define AP_PIP_NOT_ALLOWED             APPC_FLIPL(0x10086031) /*X'10086031'*/
  #define AP_PIP_NOT_SPECIFIED_CORRECTLY APPC_FLIPL(0x10086032) /*X'10086032'*/
  #define AP_CONVERSATION_TYPE_MISMATCH  APPC_FLIPL(0x10086034) /*X'10086034'*/
  #define AP_SYNC_LEVEL_NOT_SUPPORTED    APPC_FLIPL(0x10086041) /*X'10086041'*/

  #define AP_SECURITY_NOT_VALID_PASSWORD_EXPIRED                      APPC_FLIPL(0x080fff00)
  #define AP_SECURITY_NOT_VALID_PASSWORD_INVALID                      APPC_FLIPL(0x080fff01)
  #define AP_SECURITY_NOT_VALID_USERID_REVOKED                        APPC_FLIPL(0x080fff02)
  #define AP_SECURITY_NOT_VALID_USERID_INVALID                        APPC_FLIPL(0x080fff03)
  #define AP_SECURITY_NOT_VALID_USERID_MISSING                        APPC_FLIPL(0x080fff04)
  #define AP_SECURITY_NOT_VALID_PASSWORD_MISSING                      APPC_FLIPL(0x080fff05)
  #define AP_SECURITY_NOT_VALID_GROUP_INVALID                         APPC_FLIPL(0x080fff06)
  #define AP_SECURITY_NOT_VALID_USERID_REVOKED_IN_GROUP               APPC_FLIPL(0x080fff07)
  #define AP_SECURITY_NOT_VALID_USERID_NOT_DEFD_TO_GROUP              APPC_FLIPL(0x080fff08)
  #define AP_SECURITY_NOT_VALID_NOT_AUTHORIZED_AT_REMOTE_LU           APPC_FLIPL(0x080fff09)
  #define AP_SECURITY_NOT_VALID_NOT_AUTHORIZED_FROM_LOCAL_LU          APPC_FLIPL(0x080fff0a)
  #define AP_SECURITY_NOT_VALID_NOT_AUTHORIZED_TO_TRANSACTION_PROGRAM APPC_FLIPL(0x080fff0b)
  #define AP_SECURITY_NOT_VALID_INSTALLATION_EXIT_FAILED              APPC_FLIPL(0x080fff0c)
  #define AP_SECURITY_NOT_VALID_PROCESSING_FAILURE                    APPC_FLIPL(0x080fff0d)
  #define AP_SECURITY_NOT_VALID_PROTOCOL_VIOLATION                    APPC_FLIPL(0x080fff0e)

#define AP_DEALLOC_ABEND                APPC_FLIPI(0x0005)      /* X'0005'   */
#define AP_DEALLOC_ABEND_PROG           APPC_FLIPI(0x0006)      /* X'0006'   */
#define AP_DEALLOC_ABEND_SVC            APPC_FLIPI(0x0007)      /* X'0007'   */
#define AP_DEALLOC_ABEND_TIMER          APPC_FLIPI(0x0008)      /* X'0008'   */
#define AP_DEALLOC_NORMAL               APPC_FLIPI(0x0009)      /* X'0009'   */
#define AP_PROG_ERROR_NO_TRUNC          APPC_FLIPI(0x000c)      /* X'000c'   */
#define AP_PROG_ERROR_TRUNC             APPC_FLIPI(0x000d)      /* X'000d'   */
#define AP_PROG_ERROR_PURGING           APPC_FLIPI(0x000e)      /* X'000e'   */
#define AP_CONV_FAILURE_RETRY           APPC_FLIPI(0x000f)      /* X'000f'   */
#define AP_CONV_FAILURE_NO_RETRY        APPC_FLIPI(0x0010)      /* X'0010'   */
#define AP_SVC_ERROR_NO_TRUNC           APPC_FLIPI(0x0011)      /* X'0011'   */
#define AP_SVC_ERROR_TRUNC              APPC_FLIPI(0x0012)      /* X'0012'   */
#define AP_SVC_ERROR_PURGING            APPC_FLIPI(0x0013)      /* X'0013'   */
#define AP_UNSUCCESSFUL                 APPC_FLIPI(0x0014)      /* X'0014'   */
#define AP_CONVERSATION_TYPE_MIXED      APPC_FLIPI(0x0019)      /* X'0019'   */
#define AP_CANCELLED                    APPC_FLIPI(0x0021)      /* X'0021'   */
#define AP_SECURITY_REQUESTED_NOT_SUPPORTED  APPC_FLIPI(0x0022) /* X'0022'   */
#define AP_TP_BUSY                      APPC_FLIPI(0xf002)      /* X'f002'   */

#ifdef SYNCPT_SUPPORT
#define AP_BACKED_OUT                   APPC_FLIPI(0x0023)       /* X'0022'  */
  #define AP_BO_NO_RESYNC            APPC_FLIPL(0x08240000)   /* X'08240000' */
  #define AP_BO_RESYNC               APPC_FLIPL(0x08240001)   /* X'08240001' */
#endif

#define  AP_ACTIVATION_FAIL_RETRY       APPC_FLIPI(0x0310)
#define  AP_ACTIVATION_FAIL_NO_RETRY    APPC_FLIPI(0x0311)
#define  AP_SESSION_LIMITS_CLOSED       APPC_FLIPI(0x0312)
#define  AP_SESSION_LIMITS_EXCEEDED     APPC_FLIPI(0x0313)
#define  AP_VERB_IN_PROGRESS            APPC_FLIPI(0x0314)
#define  AP_SESSION_DEACTIVATED         APPC_FLIPI(0x0315)

#define AP_COMM_SUBSYSTEM_ABENDED       APPC_FLIPI(0xf003)      /* X'f003'   */
#define AP_COMM_SUBSYSTEM_NOT_LOADED    APPC_FLIPI(0xf004)      /* X'f004'   */
#define AP_CONV_BUSY                    APPC_FLIPI(0xf005)      /* X'f005'   */
#define AP_THREAD_BLOCKING              APPC_FLIPI(0xf006)      /* X'f006'   */
#define AP_INVALID_VERB_SEGMENT         APPC_FLIPI(0xf008)      /* X'f008'   */
#define AP_CANT_ENLIST_TX               APPC_FLIPI(0xf009)      /* X'f009'   */
#define AP_CANT_CONTACT_DTC             APPC_FLIPI(0xf00A)      /* X'f00A'   */
#define AP_UNEXPECTED_DOS_ERROR         APPC_FLIPI(0xf011)      /* X'f011'   */
#define AP_STACK_TOO_SMALL              APPC_FLIPI(0xf015)      /* X'f015'   */
#define AP_INVALID_VERB                 APPC_FLIPI(0xffff)      /* X'ffff'   */

/* ========================================================================= */
/*****************************************************************************/
/* ---------------------------- parameters --------------------------------- */
/*****************************************************************************/

/*****************************************************************************/
/* opext and conv_type                                                       */
/*****************************************************************************/
#define AP_BASIC_CONVERSATION             0x00
#define AP_BASIC                          0x00
#define AP_MAPPED_CONVERSATION            0x01
#define AP_MAPPED                         0x01

/*****************************************************************************/
/* rts_rcvd and return_status_with_data                                      */
/*****************************************************************************/
#define AP_NO                             0x00
#define AP_YES                            0x01

/****************************************************************************/
/* sync_level, security and send_type                                        */
/*****************************************************************************/
#define AP_NONE                           0x00

/*****************************************************************************/
/* sync_level                                                                */
/*****************************************************************************/
#define AP_CONFIRM_SYNC_LEVEL             0x01
#define AP_CONFIRM                        0x01
#ifdef SYNCPT_SUPPORT
#define AP_SYNCPT                         0x02
#endif

/*****************************************************************************/
/* security                                                                  */
/*****************************************************************************/
#define AP_SAME                           0x01
#define AP_PGM                            0x02
#define AP_STRONG                         0x03
#define AP_PROXY                          0x80
#define AP_PROXY_SAME                     0x81
#define AP_PROXY_PGM                      0x82
#define AP_PROXY_STRONG                   0x83

/*****************************************************************************/
/* dealloc_type and ptr_type                                                 */
/*****************************************************************************/
#define AP_SYNC_LEVEL                     0x00

/*****************************************************************************/
/* dealloc_type and ptr_type                                                 */
/*****************************************************************************/
#define AP_FLUSH                          0x01
#ifdef SYNCPT_SUPPORT
#define AP_CONFIRM_TYPE                   0x0B
#endif

/*****************************************************************************/
/* dealloc_type                                                              */
/*****************************************************************************/
#define AP_ABEND_PROG                     0x02
#define AP_ABEND_SVC                      0x03
#define AP_ABEND_TIMER                    0x04
#define AP_ABEND                          0x05
#ifdef SYNCPT_SUPPORT
#define AP_TP_NOT_AVAIL_RETRY                                               0x06
#define AP_TP_NOT_AVAIL_NO_RETRY                                            0x07
#define AP_TPN_NOT_RECOGNIZED                                               0x08
#define AP_PIP_DATA_NOT_ALLOWED                                             0x09
#define AP_PIP_DATA_INCORRECT                                               0x0A
#define AP_RESOURCE_FAILURE_NO_RETRY                                        0x0C
#define AP_CONV_TYPE_MISMATCH                                               0x0D
#define AP_SYNC_LVL_NOT_SUPPORTED                                           0x0E
#define AP_SECURITY_PARAMS_INVALID                                          0x0F
#define AP_DEALLOC_SECURITY_NOT_VALID_PASSWORD_EXPIRED                      0x10
#define AP_DEALLOC_SECURITY_NOT_VALID_PASSWORD_INVALID                      0x11
#define AP_DEALLOC_SECURITY_NOT_VALID_USERID_REVOKED                        0x12
#define AP_DEALLOC_SECURITY_NOT_VALID_USERID_INVALID                        0x13
#define AP_DEALLOC_SECURITY_NOT_VALID_USERID_MISSING                        0x14
#define AP_DEALLOC_SECURITY_NOT_VALID_PASSWORD_MISSING                      0x15
#define AP_DEALLOC_SECURITY_NOT_VALID_GROUP_INVALID                         0x16
#define AP_DEALLOC_SECURITY_NOT_VALID_USERID_REVOKED_IN_GROUP               0x17
#define AP_DEALLOC_SECURITY_NOT_VALID_USERID_NOT_DEFD_TO_GROUP              0x18
#define AP_DEALLOC_SECURITY_NOT_VALID_NOT_AUTHORIZED_AT_REMOTE_LU           0x19
#define AP_DEALLOC_SECURITY_NOT_VALID_NOT_AUTHORIZED_FROM_LOCAL_LU          0x1A
#define AP_DEALLOC_SECURITY_NOT_VALID_NOT_AUTHORIZED_TO_TRANSACTION_PROGRAM 0x1B
#define AP_DEALLOC_SECURITY_NOT_VALID_INSTALLATION_EXIT_FAILED              0x1C
#define AP_DEALLOC_SECURITY_NOT_VALID_PROCESSING_FAILURE                    0x1D
#define AP_DEALLOC_SECURITY_NOT_VALID_PROTOCOL_VIOLATION                    0x1E
#endif

/*****************************************************************************/
/* locks                                                                     */
/*****************************************************************************/
#define AP_SHORT                          0x00
#define AP_LONG                           0x01

#ifdef SYNCPT_SUPPORT
/*****************************************************************************/
/* data_type                                                                 */
/*****************************************************************************/
#define AP_APPLICATION                    0x00
#define AP_USER_CONTROL_DATA              0x01
#define AP_PS_HEADER                      0x02
#endif

/*****************************************************************************/
/* what_rcvd                                                                 */
/*****************************************************************************/
#define AP_DATA                          APPC_FLIPI(0x0001)     /* X'0001'   */
#define AP_DATA_COMPLETE                 APPC_FLIPI(0x0002)     /* X'0002'   */
#define AP_NOT_DATA                      APPC_FLIPI(0x0003)     /* X'0003'   */
#define AP_DATA_INCOMPLETE               APPC_FLIPI(0x0004)     /* X'0004'   */
#define AP_SEND                          APPC_FLIPI(0x0100)     /* X'0100'   */
#define AP_CONFIRM_WHAT_RECEIVED         APPC_FLIPI(0x0200)     /* X'0200'   */
#define AP_CONFIRM_SEND                  APPC_FLIPI(0x0300)     /* X'0300'   */
#define AP_CONFIRM_DEALLOCATE            APPC_FLIPI(0x0400)     /* X'0400'   */

#define AP_DATA_COMPLETE_SEND            APPC_FLIPI(0x0102)     /* X'0102'   */
#define AP_DATA_COMPLETE_CONFIRM         APPC_FLIPI(0x0202)     /* X'0202'   */
#define AP_DATA_COMPLETE_CONFIRM_SEND    APPC_FLIPI(0x0302)     /* X'0302'   */
#define AP_DATA_COMPLETE_CONFIRM_DEALL   APPC_FLIPI(0x0402)     /* X'0402'   */

#define AP_DATA_SEND                     APPC_FLIPI(0x0101)     /* X'0101'   */
#define AP_DATA_CONFIRM                  APPC_FLIPI(0x0201)     /* X'0201'   */
#define AP_DATA_CONFIRM_SEND             APPC_FLIPI(0x0301)     /* X'0301'   */
#define AP_DATA_CONFIRM_DEALLOCATE       APPC_FLIPI(0x0401)     /* X'0401'   */

#ifdef SYNCPT_SUPPORT
#define AP_USER_CONTROL_DATA_COMPLETE    APPC_FLIPI(0x0008)     /* X'0008'   */
#define AP_USER_CONTROL_DATA_INCOMPLETE  APPC_FLIPI(0x0010)     /* X'0010'   */
#define AP_PS_HEADER_COMPLETE            APPC_FLIPI(0x0020)     /* X'0020'   */
#define AP_PS_HEADER_INCOMPLETE          APPC_FLIPI(0x0040)     /* X'0040'   */
#endif

/*****************************************************************************/
/* fill                                                                      */
/*****************************************************************************/
#define AP_BUFFER                         0x00
#define AP_LL                             0x01

/*****************************************************************************/
/* rtn_ctl                                                                   */
/*****************************************************************************/
#define AP_WHEN_SESSION_ALLOCATED         0x00
#define AP_IMMEDIATE                      0x01
#define AP_WHEN_SESSION_FREE              0x02
#define AP_WHEN_CONWINNER_ALLOCATED       0x03
#define AP_WHEN_CONV_GROUP_ALLOCATED      0x04

/*****************************************************************************/
/* err_type                                                                  */
/*****************************************************************************/
#define AP_PROG                           0x00
#define AP_SVC                            0x01
#ifdef SYNCPT_SUPPORT
#define AP_BACKOUT_NO_RESYNC              0x02
#define AP_BACKOUT_RESYNC                 0x03
#endif

/*****************************************************************************/
/* send_type                                                                 */
/*****************************************************************************/
#define AP_SEND_DATA_FLUSH                0x01
#define AP_SEND_DATA_CONFIRM              0x02
#define AP_SEND_DATA_P_TO_R_FLUSH         0x03
#define AP_SEND_DATA_P_TO_R_SYNC_LEVEL    0x04
#define AP_SEND_DATA_DEALLOC_FLUSH        0x05
#define AP_SEND_DATA_DEALLOC_SYNC_LEVEL   0x06
#define AP_SEND_DATA_DEALLOC_ABEND        0x07
#ifdef SYNCPT_SUPPORT
#define AP_SEND_DATA_P_TO_R_CONFIRM       0x08
#define AP_SEND_DATA_DEALLOC_CONFIRM      0x09
#endif

/*****************************************************************************/
/* err_dir                                                                   */
/*****************************************************************************/
#define  AP_RCV_DIR_ERROR                 0x00
#define  AP_SEND_DIR_ERROR                0x01

/*****************************************************************************/
/* end_type                                                                  */
/*****************************************************************************/
#define AP_SOFT                           0x00
#define AP_HARD                           0x01
#define AP_DELAY                          0x02

/*****************************************************************************/
/* conv_state                                                                */
/*****************************************************************************/
#define AP_RESET_STATE            1
#define AP_SEND_STATE             2
#define AP_RECEIVE_STATE          3
#define AP_CONFIRM_STATE          4
#define AP_CONFIRM_SEND_STATE     5
#define AP_CONFIRM_DEALL_STATE    6
#define AP_PEND_POST_STATE        7
#define AP_PEND_DEALL_STATE       8
#define AP_END_CONV_STATE         9
#define AP_SEND_PENDING_STATE    10

#ifdef SYNCPT_SUPPORT
/*****************************************************************************/
/* values for type parameter on implied forget callback                      */
/*****************************************************************************/
#define AP_DATA_FLOW                      0x00
#define AP_UNBIND                         0x01
#define AP_FAILURE                        0x02

#endif


/*****************************************************************************/
/* ACTS Values for Polarity on ACTIVATE_SESSION verb                         */
/*****************************************************************************/
#define  AP_POL_EITHER                    0x00
#define  AP_POL_FIRST_SPEAKER             0x01
#define  AP_POL_BIDDER                    0x02

/*****************************************************************************/
/* ACTS Values for activation type on ACTIVATE_SESSION verb                  */
/*****************************************************************************/
#define  AP_ACT_ACTIVE                    0x00
#define  AP_ACT_PASSIVE                   0x01
#define  AP_DACT_ACTIVE                   0x02
#define  AP_DACT_PASSIVE                  0x03

/*****************************************************************************/
/* ACTS Values for deactivation type on DEACTIVATE_SESSION verb              */
/*****************************************************************************/
#define  AP_DEACT_CLEANUP                 0x02
#define  AP_DEACT_NORMAL                  0x01

/*****************************************************************************/
/* extension bit for the opext field                                         */
/*****************************************************************************/
#define AP_EXTD_VCB                       0x80

/* ========================================================================= */
/*****************************************************************************/
/* ------------------------- vcb structures -------------------------------- */
/*****************************************************************************/

/*****************************************************************************/
/* common vcb header                                                         */
/*****************************************************************************/
struct appc_hdr
{
  unsigned short opcode;
  unsigned char  opext;
  unsigned char  reserv2;
  unsigned short primary_rc;
  unsigned long  secondary_rc;
};


/*****************************************************************************/
/* ACTS: activate/deactivate session vcbs                                    */
/*****************************************************************************/

typedef struct activate_session
{
  unsigned short opcode;                  /* verb operation code             */
  unsigned char  reserv2[2];              /* reserved                        */
  unsigned short primary_rc;              /* primary return code             */
  unsigned long  secondary_rc;            /* secondary return code           */
  unsigned char  reserv3[8];              /* reserved                        */
  unsigned char  lu_alias[8];             /* local LU alias                  */
  unsigned char  plu_alias[8];            /* partner LU alias                */
  unsigned char  mode_name[8];            /* mode name                       */
  unsigned char  fqplu_name[17];          /* fully qualified partner LU name */
  unsigned char  polarity;                /* polarity                        */
  unsigned char  session_id[8];           /* session id                      */
  unsigned long  conv_group_id;           /* conversation group id           */
  unsigned char  reserv4[1];              /* reserved                        */
  unsigned char  type;                    /* activation type                 */
  HANDLE         deactivation_event;      /* deactivation event              */
  unsigned short * p_deactivation_status; /* deactivation completion status  */
  unsigned char  complete_verb_synchronously; /* synchronous completion flag */
  unsigned char  reserv5[9];              /* reserved                        */
}ACTIVATE_SESSION;

typedef struct deactivate_session
{
  unsigned short opcode;                  /* verb operation code             */
  unsigned char  reserv2[2];              /* reserved                        */
  unsigned short primary_rc;              /* primary return code             */
  unsigned long  secondary_rc;            /* secondary return code           */
  unsigned char  reserv3[8];              /* reserved                        */
  unsigned char  lu_alias[8];             /* local LU alias                  */
  unsigned char  session_id[8];           /* session identifier              */
  unsigned char  plu_alias[8];            /* partner LU alias                */
  unsigned char  mode_name[8];            /* mode name                       */
  unsigned char  type;                    /* deactivation type               */
  unsigned char  reserv4[3];              /* reserved                        */
  unsigned short sense_data;              /* deactivation sense data         */
  unsigned char  fqplu_name[17];          /* fully qualified partner LU name */
  unsigned char  complete_verb_synchronously; /* synchronous completion flag */
  unsigned char  reserv5[16];             /* reserved                        */
}DEACTIVATE_SESSION;


/*****************************************************************************/
/* control vcbs                                                              */
/*****************************************************************************/
struct tp_started
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     lu_alias[8];
  unsigned char     tp_id[8];
  unsigned char     tp_name[64];
#ifdef SYNCPT_SUPPORT
  unsigned char     syncpoint_rqd;         /* present if opext & AP_EXTD_VCB */
#endif
};

struct tp_started_ex
{
  unsigned short    opcode;                /* AP_TP_STARTED_EX               */
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     lu_alias[8];           /* Local LU alias                 */
  unsigned char     tp_id[8];
  unsigned char     tp_name[64];
  unsigned char     syncpoint_rqd;
  unsigned char     fqllu_name[17];        /* fully qualified local LU Name  */
  unsigned char     reserv4[50];
};


/*****************************************************************************/
/* WARNING: If you change this structure you must also change the            */
/* receive_allocate_ex strucure                                              */
/*****************************************************************************/
struct receive_allocate
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_name[64];
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     sync_level;         /* AP_NONE                           */
                                        /* AP_CONFIRM_SYNC_LEVEL             */
                                        /* AP_SYNCPT                         */
  unsigned char     conv_type;          /* AP_BASIC_CONVERSATION             */
                                        /* AP_MAPPED_CONVERSATION            */
  unsigned char     user_id[10];
  unsigned char     lu_alias[8];
  unsigned char     plu_alias[8];
  unsigned char     mode_name[8];
  unsigned char     reserv3[2];
  unsigned long     conv_group_id;      /* conversation group id             */
  unsigned char     fqplu_name[17];     /* fully qualified lu name           */
#ifdef SYNCPT_SUPPORT
  unsigned char     pip_incoming;       /* Supplied:                         */
                                        /*   AP_NO:  TP does not accept PIP  */
                                        /*           data                    */
                                        /*   AP_YES: TP does accept PIP data */
                                        /* Returned:                         */
                                        /*   AP_NO:  PIP data not available  */
                                        /*   AP_YES: PIP data available      */
  unsigned char     syncpoint_rqd;      /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv4[3];
#else
  unsigned char     reserv4[5];
#endif
};

/*****************************************************************************/
/* WARNING: If you change this structure you must also change the            */
/* receive_allocate strucure                                                 */
/*****************************************************************************/
struct receive_allocate_ex
{
  unsigned short   opcode;
  unsigned char    opext;
  unsigned char    format;
  unsigned short   primary_rc;
  unsigned long    secondary_rc;
  unsigned char    tp_name[64];
  unsigned char    tp_id[8];
  unsigned long    conv_id;
  unsigned char    sync_level;
  unsigned char    conv_type;
  unsigned char    user_id[10];
  unsigned char    lu_alias[8];
  unsigned char    plu_alias[8];
  unsigned char    mode_name[8];
  unsigned char    reserv3[2];
  unsigned long    conv_group_id;
  unsigned char    fqplu_name[17];
  unsigned char    pip_incoming;
  unsigned long    timeout;
  unsigned char    password[10];
  unsigned char    reserv5[2];
  unsigned char    attach_id[8];
};

struct receive_allocate_ex_end
{
  unsigned short   opcode;
  unsigned char    reserv2[2];
  unsigned short   primary_rc;
  unsigned long    secondary_rc;
  unsigned char    tp_name[64];
  unsigned char    lu_alias[8];
  unsigned char    reserved3[20];
};


















struct tp_ended
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned char     type;               /* AP_SOFT                           */
                                        /* AP_HARD                           */
};


/*****************************************************************************/
/* mapped conversation vcbs                                                  */
/*****************************************************************************/
struct mc_allocate
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     reserv3;
  unsigned char     sync_level;         /* AP_NONE                           */
                                        /* AP_CONFIRM_SYNC_LEVEL             */
                                        /* AP_SYNCPT                         */
  unsigned char     reserv4[2];
  unsigned char     rtn_ctl;            /* AP_WHEN_SESSION_ALLOCATED         */
                                        /* AP_IMMEDIATE                      */
                                        /* AP_WHEN_SESSION_FREE              */
                                        /* AP_WHEN_CONWINNER_ALLOCATED       */
                                        /* AP_WHEN_CONV_GROUP_ALLOCATED      */
  unsigned char     reserv5;
  unsigned long     conv_group_id;      /* conversation group id             */
  unsigned long     sense_data;         /* alloc failure sense data          */
  unsigned char     plu_alias[8];
  unsigned char     mode_name[8];
  unsigned char     tp_name[64];
  unsigned char     security;           /* AP_NONE                           */
                                        /* AP_SAME                           */
                                        /* AP_PGM                            */
                                        /* AP_PROXY_NONE                     */
                                        /* AP_PROXY_SAME                     */
                                        /* AP_PROXY_PGM                      */
  unsigned char     reserv6[11];
  unsigned char     pwd[10];
  unsigned char     user_id[10];
  unsigned short    pip_dlen;
  unsigned char far *pip_dptr;
  unsigned char     reserv7;            /* reserved for NS/2 compat.         */
  unsigned char     fqplu_name[17];     /* fully qualified PLU name          */
  unsigned char     reserv8[8];         /* reduced this from 26 to 8         */
#ifdef WIN32
  LPWSTR            proxy_user;         /* present if opext&AP_EXTD_VCB      */
  LPWSTR            proxy_domain;       /* present if opext&AP_EXTD_VCB      */
  unsigned char     reserv9[16];        /* present if opext&AP_EXTD_VCB      */
#endif
};


struct mc_confirm
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
};


struct mc_confirmed
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
};


struct mc_deallocate
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     reserv3;
  unsigned char     dealloc_type;       /* AP_SYNC_LEVEL                     */
                                        /* AP_FLUSH                          */
                                        /* AP_CONFIRM_TYPE                   */
                                        /* AP_ABEND                          */
                                        /* AP_TP_NOT_AVAIL_RETRY             */
                                        /* AP_TP_NOT_AVAIL_NO_RETRY          */
                                        /* AP_TPN_NOT_RECOGNIZED             */
                                        /* AP_PIP_DATA_NOT_ALLOWED           */
                                        /* AP_PIP_DATA_INCORRECT             */
                                        /* AP_RESOURCE_FAILURE_NO_RETRY      */
                                        /* AP_CONV_TYPE_MISMATCH             */
                                        /* AP_SYNC_LVL_NOT_SUPPORTED         */
                                        /* AP_SECURITY_PARAMS_INVALID        */
  unsigned char     reserv4[2];
  unsigned char     reserv5[4];
#ifdef SYNCPT_SUPPORT
  void       (WINAPI *callback)();      /* present if opext&AP_EXTD_VCB      */
  void             *correlator;         /* present if opext&AP_EXTD_VCB      */
  unsigned char     reserv6[4];         /* present if opext&AP_EXTD_VCB      */
#endif
};


struct mc_flush
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
};


struct mc_get_attributes
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     reserv3;
  unsigned char     sync_level;         /* AP_NONE                           */
                                        /* AP_CONFIRM_SYNC_LEVEL             */
                                        /* AP_SYNCPT                         */
  unsigned char     mode_name[8];
  unsigned char     net_name[8];
  unsigned char     lu_name[8];
  unsigned char     lu_alias[8];
  unsigned char     plu_alias[8];
  unsigned char     plu_un_name[8];
  unsigned char     reserv4[2];
  unsigned char     fqplu_name[17];
  unsigned char     reserv5;
  unsigned char     user_id[10];
  unsigned long     conv_group_id;      /* conversation group id             */
  unsigned char     conv_corr_len;      /* conversation correlator lgth      */
  unsigned char     conv_corr[8];       /* conversation correlator           */
  unsigned char     reserv6[13];
#ifdef SYNCPT_SUPPORT
  unsigned char     luw_id[26];          /* present if opext&AP_EXTD_VCB     */
  unsigned char     sess_id[8];          /* present if opext&AP_EXTD_VCB     */
#endif
};


struct mc_prepare_to_receive
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     ptr_type;           /* AP_SYNC_LEVEL                     */
                                        /* AP_FLUSH                          */
                                        /* AP_CONFIRM_TYPE                   */
  unsigned char     locks;              /* AP_SHORT                          */
                                        /* AP_LONG                           */
};


struct mc_receive_and_post
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned short    what_rcvd;          /* AP_NONE                           */
                                        /* AP_DATA                           */
                                        /* AP_CONFIRM_WHAT_RECEIVED          */
                                        /* AP_CONFIRM_SEND                   */
                                        /* AP_CONFIRM_DEALLOCATE             */
  unsigned char     rtn_status;         /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv4;
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv5;
  unsigned short    max_len;
  unsigned short    dlen;
  unsigned char far *dptr;
  unsigned char far *sema;
  unsigned char     reserv6;
};


struct mc_receive_and_wait
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned short    what_rcvd;          /* AP_NONE                           */
                                        /* AP_DATA                           */
                                        /* AP_SEND                           */
                                        /* AP_CONFIRM_WHAT_RECEIVED          */
                                        /* AP_CONFIRM_SEND                   */
                                        /* AP_CONFIRM_DEALLOCATE             */
  unsigned char     rtn_status;         /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv4;
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv5;
  unsigned short    max_len;
  unsigned short    dlen;
  unsigned char far *dptr;
  unsigned char     reserv6[5];
};


struct mc_receive_immediate
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned short    what_rcvd;          /* AP_NONE                           */
                                        /* AP_DATA                           */
                                        /* AP_SEND                           */
                                        /* AP_CONFIRM_WHAT_RECEIVED          */
                                        /* AP_CONFIRM_SEND                   */
                                        /* AP_CONFIRM_DEALLOCATE             */
  unsigned char     rtn_status;         /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv4;
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv5;
  unsigned short    max_len;
  unsigned short    dlen;
  unsigned char far *dptr;
  unsigned char     reserv6[5];
};


struct mc_request_to_send
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
};


struct mc_send_conversation
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned char     reserv3[8];
  unsigned char     rtn_ctl;            /* AP_WHEN_SESSION_ALLOCATED         */
                                        /* AP_IMMEDIATE                      */
                                        /* AP_WHEN_SESSION_FREE              */
                                        /* AP_WHEN_CONWINNER_ALLOCATED       */
                                        /* AP_WHEN_CONV_GROUP_ALLOCATED      */
  unsigned char     reserv4;
  unsigned long     conv_group_id;      /* conversation group id             */
  unsigned long     sense_data;         /* alloc failure sense data          */
  unsigned char     plu_alias[8];
  unsigned char     mode_name[8];
  unsigned char     tp_name[64];
  unsigned char     security;           /* AP_NONE                           */
                                        /* AP_SAME                           */
                                        /* AP_PGM                            */
  unsigned char     reserv5[11];
  unsigned char     pwd[10];
  unsigned char     user_id[10];
  unsigned short    pip_dlen;
  unsigned char far *pip_dptr;
  unsigned char     reserv6;
  unsigned char     fqplu_name[17];
  unsigned char     reserv7[8];
  unsigned short    dlen;
  unsigned char far *dptr;
};


struct mc_send_data
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
#ifdef SYNCPT_SUPPORT
  unsigned char     data_type;           /* type of data to be sent          */
                                         /* AP_APPLICATION                   */
                                         /* AP_USER_CONTROL_DATA             */
                                         /* AP_PS_HEADER                     */
#else
  unsigned char     reserv3;
#endif
  unsigned short    dlen;
  unsigned char far *dptr;
  unsigned char     type;               /* AP_SEND_DATA_FLUSH                */
                                        /* AP_SEND_DATA_CONFIRM              */
                                        /* AP_SEND_DATA_P_TO_R_FLUSH         */
                                        /* AP_SEND_DATA_P_TO_R_SYNC_LEVEL    */
                                        /* AP_SEND_DATA_P_TO_R_CONFIRM       */
                                        /* AP_SEND_DATA_DEALLOC_FLUSH        */
                                        /* AP_SEND_DATA_DEALLOC_SYNC_LEVEL   */
                                        /* AP_SEND_DATA_DEALLOC_CONFIRM      */
                                        /* AP_SEND_DATA_DEALLOC_ABEND        */
  unsigned char     reserv4;
};


struct mc_send_error
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
#ifdef SYNCPT_SUPPORT
  unsigned char     err_type;            /* type of error                    */
                                         /* AP_PROG                          */
                                         /* AP_BACKOUT_NO_RESYNC             */
                                         /* AP_BACKOUT_RESYNC                */
#else
  unsigned char     reserv3;
#endif
  unsigned char     err_dir;            /* AP_RCV_DIR_ERROR                  */
                                        /* AP_SEND_DIR_ERROR                 */
  unsigned char     reserv4;
  unsigned short    log_dlen;
  unsigned char far *log_dptr;
};

struct mc_test_rts
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     reserv3;
};

struct mc_test_rts_and_post
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     reserv3;
  unsigned long     handle;
};

#ifdef WIN32
struct mc_post_on_receipt
{
  unsigned short        opcode;
  unsigned char         opext;
  unsigned char         reserv1;
  unsigned short        primary_rc;
  unsigned long         secondary_rc;
  unsigned char         tp_id[8];
  unsigned long         conv_id;
  unsigned short        reserv2;
  unsigned char         reserv3;
  unsigned char         reserv4;
  unsigned short        max_len;
  unsigned short        reserv5;
  unsigned char far *   reserv6;
  unsigned char         reserv7[5];
  unsigned long         sema;
};
#endif

struct mc_receive_log_data
{
  unsigned short        opcode;
  unsigned char         opext;
  unsigned char         reserv1;
  unsigned short        primary_rc;
  unsigned long         secondary_rc;
  unsigned char         tp_id[8];
  unsigned long         conv_id;
  unsigned short        log_dlen;
  unsigned char far *   log_dptr;
};

/*****************************************************************************/
/* basic conversation vcbs                                                   */
/*****************************************************************************/
struct allocate
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     conv_type;          /* AP_BASIC_CONVERSATION             */
                                        /* AP_MAPPED_CONVERSATION            */
  unsigned char     sync_level;         /* AP_NONE                           */
                                        /* AP_CONFIRM_SYNC_LEVEL             */
                                        /* AP_SYNCPT                         */
  unsigned char     reserv3[2];
  unsigned char     rtn_ctl;            /* AP_WHEN_SESSION_ALLOCATED         */
                                        /* AP_IMMEDIATE                      */
                                        /* AP_WHEN_SESSION_FREE              */
                                        /* AP_WHEN_CONWINNER_ALLOCATED       */
                                        /* AP_WHEN_CONV_GROUP_ALLOCATED      */
  unsigned char     reserv4;
  unsigned long     conv_group_id;      /* conversation group id             */
  unsigned long     sense_data;         /* alloc failure sense data          */
  unsigned char     plu_alias[8];
  unsigned char     mode_name[8];
  unsigned char     tp_name[64];
  unsigned char     security;           /* AP_NONE                           */
                                        /* AP_SAME                           */
                                        /* AP_PGM                            */
                                        /* AP_PROXY_NONE                     */
                                        /* AP_PROXY_SAME                     */
                                        /* AP_PROXY_PGM                      */
  unsigned char     reserv5[11];
  unsigned char     pwd[10];
  unsigned char     user_id[10];
  unsigned short    pip_dlen;
  unsigned char far *pip_dptr;
  unsigned char     reserv7;            /* for NS/2 compatability            */
  unsigned char     fqplu_name[17];     /* fully qualified PLU name          */
  unsigned char     reserv8[8];         /* reduced this from 26 to 8         */
#ifdef WIN32
  LPWSTR            proxy_user;         /* present if opext&AP_EXTD_VCB      */
  LPWSTR            proxy_domain;       /* present if opext&AP_EXTD_VCB      */
  unsigned char     reserv9[16];        /* present if opext&AP_EXTD_VCB      */
#endif
};


struct confirm
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
};


struct confirmed
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
};


struct deallocate
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     reserv3;
  unsigned char     dealloc_type;       /* AP_SYNC_LEVEL                     */
                                        /* AP_FLUSH                          */
                                        /* AP_CONFIRM_TYPE                   */
                                        /* AP_ABEND_PROG                     */
                                        /* AP_ABEND_SVC                      */
                                        /* AP_ABEND_TIMER                    */
                                        /* AP_TP_NOT_AVAIL_RETRY             */
                                        /* AP_TP_NOT_AVAIL_NO_RETRY          */
                                        /* AP_TPN_NOT_RECOGNIZED             */
                                        /* AP_PIP_DATA_NOT_ALLOWED           */
                                        /* AP_PIP_DATA_INCORRECT             */
                                        /* AP_RESOURCE_FAILURE_NO_RETRY      */
                                        /* AP_CONV_TYPE_MISMATCH             */
                                        /* AP_SYNC_LVL_NOT_SUPPORTED         */
                                        /* AP_SECURITY_PARAMS_INVALID        */
  unsigned short    log_dlen;
  unsigned char far *log_dptr;
#ifdef SYNCPT_SUPPORT
  void       (WINAPI *callback)();      /* present if opext&AP_EXTD_VCB      */
  void             *correlator;         /* present if opext&AP_EXTD_VCB      */
  unsigned char     reserv6[4];         /* present if opext&AP_EXTD_VCB      */
#endif
};


struct flush
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
};


struct get_attributes
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     reserv3;
  unsigned char     sync_level;         /* AP_NONE                           */
                                        /* AP_CONFIRM_SYNC_LEVEL             */
                                        /* AP_SYNCPT                         */
  unsigned char     mode_name[8];
  unsigned char     net_name[8];
  unsigned char     lu_name[8];
  unsigned char     lu_alias[8];
  unsigned char     plu_alias[8];
  unsigned char     plu_un_name[8];
  unsigned char     reserv4[2];
  unsigned char     fqplu_name[17];
  unsigned char     reserv5;
  unsigned char     user_id[10];
  unsigned long     conv_group_id;      /* conversation group id             */
  unsigned char     conv_corr_len;      /* conversation correlator length    */
  unsigned char     conv_corr[8];       /* conversation correlator           */
  unsigned char     reserv6[13];
#ifdef SYNCPT_SUPPORT
  unsigned char     luw_id[26];          /* present if opext&AP_EXTD_VCB     */
  unsigned char     sess_id[8];          /* present if opext&AP_EXTD_VCB     */
#endif
};


struct prepare_to_receive
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     ptr_type;           /* AP_SYNC_LEVEL                     */
                                        /* AP_FLUSH                          */
                                        /* AP_CONFIRM_TYPE                   */
  unsigned char     locks;              /* AP_SHORT                          */
                                        /* AP_LONG                           */
};


struct receive_and_post
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned short    what_rcvd;          /* AP_NONE                           */
                                        /* AP_DATA                           */
                                        /* AP_DATA_COMPLETE                  */
                                        /* AP_DATA_INCOMPLETE                */
                                        /* AP_SEND                           */
                                        /* AP_CONFIRM_WHAT_RECEIVED          */
                                        /* AP_CONFIRM_SEND                   */
                                        /* AP_CONFIRM_DEALLOCATE             */
  unsigned char     rtn_status;         /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     fill;               /* AP_BUFFER                         */
                                        /* AP_LL                             */
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv4;
  unsigned short    max_len;
  unsigned short    dlen;
  unsigned char far *dptr;
  unsigned char far *sema;
  unsigned char     reserv5;
};


struct receive_and_wait
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned short    what_rcvd;          /* AP_NONE                           */
                                        /* AP_DATA                           */
                                        /* AP_DATA_COMPLETE                  */
                                        /* AP_DATA_INCOMPLETE                */
                                        /* AP_SEND                           */
                                        /* AP_CONFIRM_WHAT_RECEIVED          */
                                        /* AP_CONFIRM_SEND                   */
                                        /* AP_CONFIRM_DEALLOCATE             */
  unsigned char     rtn_status;         /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     fill;               /* AP_BUFFER                         */
                                        /* AP_LL                             */
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv4;
  unsigned short    max_len;
  unsigned short    dlen;
  unsigned char far *dptr;
  unsigned char     reserv5[5];
};


struct receive_immediate
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned short    what_rcvd;          /* AP_NONE                           */
                                        /* AP_DATA                           */
                                        /* AP_DATA_COMPLETE                  */
                                        /* AP_DATA_INCOMPLETE                */
                                        /* AP_SEND                           */
                                        /* AP_CONFIRM_WHAT_RECEIVED          */
                                        /* AP_CONFIRM_SEND                   */
                                        /* AP_CONFIRM_DEALLOCATE             */
  unsigned char     rtn_status;         /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     fill;               /* AP_BUFFER                         */
                                        /* AP_LL                             */
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     reserv4;
  unsigned short    max_len;
  unsigned short    dlen;
  unsigned char far *dptr;
  unsigned char     reserv5[5];
};


struct request_to_send
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
};


struct send_conversation
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned char     reserv3[8];
  unsigned char     rtn_ctl;            /* AP_WHEN_SESSION_ALLOCATED         */
                                        /* AP_IMMEDIATE                      */
                                        /* AP_WHEN_SESSION_FREE              */
                                        /* AP_WHEN_CONWINNER_ALLOCATED       */
                                        /* AP_WHEN_CONV_GROUP_ALLOCATED      */
  unsigned char     reserv4;
  unsigned long     conv_group_id;      /* conversation group id             */
  unsigned long     sense_data;         /* allocation failure sense data     */
  unsigned char     plu_alias[8];
  unsigned char     mode_name[8];
  unsigned char     tp_name[64];
  unsigned char     security;           /* AP_NONE                           */
                                        /* AP_SAME                           */
                                        /* AP_PGM                            */
  unsigned char     reserv5[11];
  unsigned char     pwd[10];
  unsigned char     user_id[10];
  unsigned short    pip_dlen;
  unsigned char far *pip_dptr;
  unsigned char     reserv6;
  unsigned char     fqplu_name[17];
  unsigned char     reserv7[8];
  unsigned short    dlen;
  unsigned char far *dptr;
};


struct send_data
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
#ifdef SYNCPT_SUPPORT
  unsigned char     data_type;           /* type of data to be sent          */
                                         /* AP_APPLICATION                   */
                                         /* AP_USER_CONTROL_DATA             */
                                         /* AP_PS_HEADER                     */
#else
  unsigned char     reserv3;
#endif
  unsigned short    dlen;
  unsigned char far *dptr;
  unsigned char type;                   /* AP_SEND_DATA_FLUSH                */
                                        /* AP_SEND_DATA_CONFIRM              */
                                        /* AP_SEND_DATA_P_TO_R_FLUSH         */
                                        /* AP_SEND_DATA_P_TO_R_SYNC_LEVEL    */
                                        /* AP_SEND_DATA_P_TO_R_CONFIRM       */
                                        /* AP_SEND_DATA_DEALLOC_FLUSH        */
                                        /* AP_SEND_DATA_DEALLOC_SYNC_LEVEL   */
                                        /* AP_SEND_DATA_DEALLOC_CONFIRM      */
                                        /* AP_SEND_DATA_DEALLOC_ABEND        */
  unsigned char     reserv4;
};


struct send_error
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     rts_rcvd;           /* AP_NO                             */
                                        /* AP_YES                            */
  unsigned char     err_type;           /* AP_PROG                           */
                                        /* AP_SVC                            */
                                        /* AP_BACKOUT_NO_RESYNC              */
                                        /* AP_BACKOUT_RESYNC                 */
  unsigned char     err_dir;            /* AP_RCV_DIR_ERROR                  */
                                        /* AP_SEND_DIR_ERROR                 */
  unsigned char     reserv3;
  unsigned short    log_dlen;
  unsigned char far *log_dptr;
};


struct test_rts
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     reserv3;
};

struct test_rts_and_post
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     reserv3;
  unsigned long     handle;
};

#ifdef WIN32
struct post_on_receipt
{
  unsigned short        opcode;
  unsigned char         opext;
  unsigned char         reserv1;
  unsigned short        primary_rc;
  unsigned long         secondary_rc;
  unsigned char         tp_id[8];
  unsigned long         conv_id;
  unsigned short        reserv2;
  unsigned char         fill;
  unsigned char         reserv4;
  unsigned short        max_len;
  unsigned short        reserv5;
  unsigned char far *   reserv6;
  unsigned char         reserv7[5];
  unsigned long         sema;
};
#endif

struct receive_log_data
{
  unsigned short        opcode;
  unsigned char         opext;
  unsigned char         reserv1;
  unsigned short        primary_rc;
  unsigned long         secondary_rc;
  unsigned char         tp_id[8];
  unsigned long         conv_id;
  unsigned short        log_dlen;
  unsigned char far *   log_dptr;
};


/*****************************************************************************/
/* type independent vcbs                                                     */
/*****************************************************************************/
typedef struct luw_id_overlay
{
  unsigned char     fqlu_name_len;
  unsigned char     fqlu_name[17];
  unsigned char     instance[6];
  unsigned char     sequence[2];
} LUW_ID_OVERLAY;

struct get_tp_properties
{
  unsigned short    opcode;
#ifdef SYNCPT_SUPPORT
  unsigned char     opext;
  unsigned char     reserv2;
#else
  unsigned char     reserv2[2];
#endif
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned char     tp_name[64];
  unsigned char     lu_alias[8];
  unsigned char     luw_id[26];         /* Logical Unit of Work id           */
  unsigned char     fqlu_name[17];      /* fully qualified LU name           */
  unsigned char     reserv3[10];
  unsigned char     user_id[10];
#ifdef SYNCPT_SUPPORT
  unsigned char     prot_luw_id[26];    /* present if opext & AP_EXTD        */
#endif
};

#ifdef SYNCPT_SUPPORT
struct set_tp_properties
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned char     set_prot_id;        /* AP_NO, AP_YES                     */
  unsigned char     new_prot_id;        /* AP_NO, AP_YES                     */
  unsigned char     prot_id[26];
  unsigned char     set_unprot_id;      /* AP_NO, AP_YES                     */
  unsigned char     new_unprot_id;      /* AP_NO, AP_YES                     */
  unsigned char     unprot_id[26];
  unsigned char     set_user_id;        /* AP_NO, AP_YES                     */
  unsigned char     reserv3;
  unsigned char     user_id[10];
  unsigned char     reserv4[10];
};

struct get_lu_status
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned char     plu_alias[8];
  unsigned short    active_sess;
  unsigned char     zero_sess;          /* AP_NO, AP_YES                     */
  unsigned char     local_only;         /* AP_NO, AP_YES                     */
  unsigned char     syncpoint;          /* AP_NO, AP_YES                     */
  unsigned char     pool_member;        /* AP_NO, AP_YES                     */
  unsigned char     fqplu_support;      /* AP_NO, AP_YES                     */
  unsigned char     reserv3[3];
  unsigned char     fqplu_name[17];     /* fully qualified PLU name          */
};
#endif

struct get_type
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     conv_type;          /* AP_BASIC_CONVERSATION             */
                                        /* AP_MAPPED_CONVERSATION            */
};


struct get_state
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned char     conv_state;                             /* AP_RESET etc. */
};


/*****************************************************************************/
/* When DLL is handling transaction interfaces with DTC, this verb           */
/* supports explicit enlistment at TP's request.                             */
/*****************************************************************************/

struct enlist
{
  unsigned short    opcode;
  unsigned char     opext;
  unsigned char     reserv2;
  unsigned short    primary_rc;
  unsigned long     secondary_rc;
  unsigned char     tp_id[8];
  unsigned long     conv_id;
  unsigned long     tran_id;                                   /* DTC TranId */
};


/*****************************************************************************/
/* Add typedefs for OS/2 ES compatability.                                   */
/*****************************************************************************/
typedef struct tp_started            TP_STARTED;
typedef struct tp_started_ex         TP_STARTED_EX;
typedef struct receive_allocate      RECEIVE_ALLOCATE;
typedef struct receive_allocate_ex   RECEIVE_ALLOCATE_EX;
typedef struct receive_allocate_ex_end RECEIVE_ALLOCATE_EX_END;
typedef struct tp_ended              TP_ENDED;
typedef struct mc_allocate           MC_ALLOCATE;
typedef struct mc_confirm            MC_CONFIRM;
typedef struct mc_confirmed          MC_CONFIRMED;
typedef struct mc_deallocate         MC_DEALLOCATE;
typedef struct mc_flush              MC_FLUSH;
typedef struct mc_get_attributes     MC_GET_ATTRIBUTES;
typedef struct mc_prepare_to_receive MC_PREPARE_TO_RECEIVE;
typedef struct mc_receive_and_post   MC_RECEIVE_AND_POST;
typedef struct mc_receive_and_wait   MC_RECEIVE_AND_WAIT;
typedef struct mc_receive_immediate  MC_RECEIVE_IMMEDIATE;
typedef struct mc_request_to_send    MC_REQUEST_TO_SEND;
typedef struct mc_send_conversation  MC_SEND_CONVERSATION;
typedef struct mc_send_data          MC_SEND_DATA;
typedef struct mc_send_error         MC_SEND_ERROR;
typedef struct mc_test_rts           MC_TEST_RTS;
typedef struct mc_test_rts_and_post  MC_TEST_RTS_AND_POST;
#ifdef WIN32
typedef struct mc_post_on_receipt    MC_POST_ON_RECEIPT;
#endif
typedef struct mc_receive_log_data   MC_RECEIVE_LOG_DATA;
typedef struct allocate              ALLOCATE;
typedef struct confirm               CONFIRM;
typedef struct confirmed             CONFIRMED;
typedef struct deallocate            DEALLOCATE;
typedef struct flush                 FLUSH;
typedef struct get_attributes        GET_ATTRIBUTES;
typedef struct prepare_to_receive    PREPARE_TO_RECEIVE;
typedef struct receive_and_post      RECEIVE_AND_POST;
typedef struct receive_and_wait      RECEIVE_AND_WAIT;
typedef struct receive_immediate     RECEIVE_IMMEDIATE;
typedef struct request_to_send       REQUEST_TO_SEND;
typedef struct send_conversation     SEND_CONVERSATION;
typedef struct send_data             SEND_DATA;
typedef struct send_error            SEND_ERROR;
typedef struct test_rts              TEST_RTS;
typedef struct test_rts_and_post     TEST_RTS_AND_POST;
#ifdef WIN32
typedef struct post_on_receipt       POST_ON_RECEIPT;
#endif
typedef struct receive_log_data      RECEIVE_LOG_DATA;
typedef struct get_tp_properties     GET_TP_PROPERTIES;
#ifdef SYNCPT_SUPPORT
typedef struct set_tp_properties     SET_TP_PROPERTIES;
typedef struct get_lu_status         GET_LU_STATUS;
#endif
typedef struct get_type              GET_TYPE;
typedef struct get_state             GET_STATE;

/* ========================================================================= */
/*****************************************************************************/
/* ---------------------- APPC function prototypes ------------------------- */
/*****************************************************************************/

#ifdef WIN32
 #ifndef WINSNA
  extern void pascal APPC(long);
 #endif
extern void pascal APPC_P(long);
extern void pascal APPC_C(long);
#else
 #ifndef WINSNA
  extern void pascal far _loadds APPC(long);
 #endif
extern void pascal far _loadds APPC_P(long);
extern void pascal far _loadds APPC_C(long);
#endif


/*****************************************************************************/
/* WinSNA function prototypes                                                */
/*****************************************************************************/
#ifdef WINSNA


/*---------------------------------------------------------------------------*/
/* IOCP - Structure and function prototype                                   */
/*---------------------------------------------------------------------------*/
typedef struct
{
  HANDLE       APPC_CompletionPort;
  DWORD        APPC_NumberOfBytesTransferred;
  DWORD        APPC_CompletionKey;
  LPOVERLAPPED APPC_pOverlapped;

} APPC_IOCP_INFO;

extern HANDLE WINAPI WinAsyncAPPCIOCP(APPC_IOCP_INFO*, long);

/*---------------------------------------------------------------------------*/
/* Windows APPC Extension Return Codes.                                      */
/*---------------------------------------------------------------------------*/

#define WAPPCALREADY         0xF000  /* An async call is already outstanding */
#define WAPPCINVALID         0xF001  /* Async Task Id is invalid             */
#define WAPPCCANCEL          0xF002  /* Blocking call was cancelled          */
#define WAPPCSYSNOTREADY     0xF003  /* Underlying subsystem not started     */
#define WAPPCVERNOTSUPPORTED 0xF004  /* Application version not supported    */


/*---------------------------------------------------------------------------*/
/* Api structre definitions                                                  */
/*---------------------------------------------------------------------------*/
#define WAPPCDESCRIPTION_LEN    127

typedef struct tagWAPPCDATA
{
  WORD    wVersion;
  char    szDescription[WAPPCDESCRIPTION_LEN+1];
} WAPPCDATA, * PWAPPCDATA, FAR * LPWAPPCDATA;


 extern void    WINAPI APPC(long);
 extern HANDLE  WINAPI WinAsyncAPPC( HWND, long);
 extern HANDLE  WINAPI WinAsyncAPPCEx( HANDLE, long);
 extern BOOL    WINAPI WinAPPCCleanup(void);
 extern BOOL    WINAPI WinAPPCIsBlocking(void);
 extern int     WINAPI WinAPPCCancelAsyncRequest( HANDLE );
 extern BOOL    WINAPI WinAPPCCancelBlockingCall(void);
 extern int     WINAPI WinAPPCStartup(WORD, LPWAPPCDATA);
 extern FARPROC WINAPI WinAPPCSetBlockingHook(FARPROC);
 extern BOOL    WINAPI WinAPPCUnhookBlockingHook(void);
 extern int     WINAPI GetAppcReturnCode (struct appc_hdr FAR * vpb,
                                          UINT        buffer_length,
                                          unsigned char FAR * buffer_addr);

 #define WIN_APPC_ASYNC_COMPLETE_MESSAGE "WinAsyncAPPC"


#endif

/*****************************************************************************/
/* Keywords used for single sign-on.                                         */
/*****************************************************************************/
#define APPC_SSO_USERNAME      "MS$SAME"
#define APPC_SSO_PASSWORD      "MS$SAME"

/*****************************************************************************/
/* for Win32 environment, restore original packing mode                      */
/*****************************************************************************/
#ifdef WIN32

#pragma pack()

#endif

#ifdef __cplusplus
}
#endif

#endif

/* winappc.h */
