/*
 * NOTE:
 * The appropriate system value must be defined for your platform.
 * The following system values are supported:
 *     CM_OS2
 *     CM_DOS
 *     CM_AIX
 *     CM_MVS
 *     CM_VM
 *     CM_OS400
 *     CM_WINNT
 *
 * This is necessary for the proper setting of
 * AFTP_ENTRY and AFTP_PTR below.
 */

/*
 * This file is organized as follows:
 *  - product specific preprocessor directives that must be before all
 *    other declarations
 *  - AFTP type definitions
 *  - AFTP constants definitions
 *  - AFTP function long to short name mapping
 *  - AFTP function prototypes
 */

#ifndef _aftpapi_h
#define _aftpapi_h


#if defined (CM_WINNT)                                                  
                                                                        
#define WINSNA                                                          
#define AFTP_ENTRY extern void WINAPI                                   
#define AFTP_PTR *                                                      
                                                                        
#elif   defined( CM_DOS ) || defined( CM_OS2 )

#if   !defined(__32BIT__)
#     define AFTP_ENTRY extern void pascal far _loadds
#     define AFTP_PTR far *
#else

#     define AFTP_ENTRY extern void _Far16 _Pascal
#     define AFTP_PTR * _Seg16

#endif

#elif defined( CM_MVS ) || defined( CM_OS400 ) || \
      defined( CM_VM ) || defined(CM_AIX)

#     define AFTP_ENTRY extern void
#     define AFTP_PTR *

#endif


/**********************************************************************
 * AFTP Type Definitions
 **********************************************************************/

typedef unsigned char AFTP_HANDLE_TYPE[8];

typedef unsigned long AFTP_ALLOCATION_SIZE_TYPE;

typedef unsigned long AFTP_BLOCK_SIZE_TYPE;

typedef unsigned long AFTP_BOOLEAN_TYPE;


#if defined (CM_WINNT)                                                  
typedef signed long AFTP_DATA_TYPE_TYPE;                                
#else                                                                   
typedef unsigned long AFTP_DATA_TYPE_TYPE;
#endif                                                                  

typedef unsigned long AFTP_DATE_MODE_TYPE;

typedef unsigned long AFTP_DETAIL_LEVEL_TYPE;

typedef unsigned long AFTP_FILE_MODE_TYPE;

typedef unsigned long AFTP_FILE_SIZE_TYPE;

typedef unsigned long AFTP_FILE_TIME_TYPE;

typedef unsigned long AFTP_FILE_TYPE_TYPE;

typedef unsigned long AFTP_INFO_LEVEL_TYPE;

typedef unsigned long AFTP_LENGTH_TYPE;

typedef unsigned long AFTP_RETURN_CODE_TYPE;

typedef unsigned long AFTP_RECORD_FORMAT_TYPE;

typedef unsigned long AFTP_RECORD_LENGTH_TYPE;

typedef unsigned long AFTP_SECURITY_TYPE;

typedef unsigned long AFTP_TRACE_LEVEL_TYPE;

typedef unsigned long AFTP_VERSION_TYPE;

typedef unsigned long AFTP_WRITE_MODE_TYPE;


/**********************************************************************
 *  AFTP Constant Definitions
 **********************************************************************/

/* Supported Data Transfer Types */

#define AFTP_ASCII            	((AFTP_DATA_TYPE_TYPE)0)
#define AFTP_BINARY           	((AFTP_DATA_TYPE_TYPE)1)
#define AFTP_EBCDIC           	((AFTP_DATA_TYPE_TYPE)2)
#define AFTP_DEFAULT_DATA_TYPE  ((AFTP_DATA_TYPE_TYPE)3)               /*W32*/


/* Supported File Date Modes */

#define AFTP_OLDDATE          ((AFTP_DATE_MODE_TYPE)0)
#define AFTP_NEWDATE          ((AFTP_DATE_MODE_TYPE)1)

/* Format Error Detail Levels */

#define AFTP_DETAIL_RC      ((AFTP_DETAIL_LEVEL_TYPE)1)
#define AFTP_DETAIL_SECOND  ((AFTP_DETAIL_LEVEL_TYPE)2)
#define AFTP_DETAIL_LOG     ((AFTP_DETAIL_LEVEL_TYPE)4)
#define AFTP_DETAIL_INFO    ((AFTP_DETAIL_LEVEL_TYPE)8)
#define AFTP_DETAIL_ALL     ((AFTP_DETAIL_LEVEL_TYPE)15)

/* File Types, Requested and Returned */

#define AFTP_FILE             ((AFTP_FILE_TYPE_TYPE)1)
#define AFTP_DIRECTORY        ((AFTP_FILE_TYPE_TYPE)2)
#define AFTP_ALL_FILES        ((AFTP_FILE_TYPE_TYPE) \
                                  (AFTP_FILE | AFTP_DIRECTORY))

/* Info Level for aftp_dir_open() */

#define AFTP_NATIVE_FILENAMES   ((AFTP_INFO_LEVEL_TYPE)0)
#define AFTP_NATIVE_ATTRIBUTES  ((AFTP_INFO_LEVEL_TYPE)1)

/* Return Codes */

#define AFTP_RC_OK                     ((AFTP_RETURN_CODE_TYPE)0)
#define AFTP_RC_COMM_FAIL_NO_RETRY     ((AFTP_RETURN_CODE_TYPE)1)
#define AFTP_RC_COMM_FAIL_RETRY        ((AFTP_RETURN_CODE_TYPE)2)
#define AFTP_RC_COMM_CONFIG_LOCAL      ((AFTP_RETURN_CODE_TYPE)3)
#define AFTP_RC_COMM_CONFIG_REMOTE     ((AFTP_RETURN_CODE_TYPE)4)
#define AFTP_RC_SECURITY_NOT_VALID     ((AFTP_RETURN_CODE_TYPE)5)
#define AFTP_RC_FAIL_INPUT_ERROR       ((AFTP_RETURN_CODE_TYPE)6)
#define AFTP_RC_FAIL_RETRY             ((AFTP_RETURN_CODE_TYPE)7)
#define AFTP_RC_FAIL_NO_RETRY          ((AFTP_RETURN_CODE_TYPE)8)
#define AFTP_RC_FAIL_FATAL             ((AFTP_RETURN_CODE_TYPE)9)
#define AFTP_RC_PROGRAM_INTERNAL_ERROR ((AFTP_RETURN_CODE_TYPE)10)
#define AFTP_RC_PARAMETER_CHECK        ((AFTP_RETURN_CODE_TYPE)11)
#define AFTP_RC_HANDLE_NOT_VALID       ((AFTP_RETURN_CODE_TYPE)12)
#define AFTP_RC_STATE_CHECK            ((AFTP_RETURN_CODE_TYPE)13)
#define AFTP_RC_BUFFER_TOO_SMALL       ((AFTP_RETURN_CODE_TYPE)14)

/* Supported Record Formats */

#define AFTP_DEFAULT_RECORD_FORMAT ((AFTP_RECORD_FORMAT_TYPE)0)
#define AFTP_V                     ((AFTP_RECORD_FORMAT_TYPE)1)
#define AFTP_VA                    ((AFTP_RECORD_FORMAT_TYPE)2)
#define AFTP_VB                    ((AFTP_RECORD_FORMAT_TYPE)3)
#define AFTP_VM                    ((AFTP_RECORD_FORMAT_TYPE)4)
#define AFTP_VS                    ((AFTP_RECORD_FORMAT_TYPE)5)
#define AFTP_VBA                   ((AFTP_RECORD_FORMAT_TYPE)6)
#define AFTP_VBM                   ((AFTP_RECORD_FORMAT_TYPE)7)
#define AFTP_VBS                   ((AFTP_RECORD_FORMAT_TYPE)8)
#define AFTP_VSA                   ((AFTP_RECORD_FORMAT_TYPE)9)
#define AFTP_VSM                   ((AFTP_RECORD_FORMAT_TYPE)10)
#define AFTP_VBSA                  ((AFTP_RECORD_FORMAT_TYPE)11)
#define AFTP_VBSM                  ((AFTP_RECORD_FORMAT_TYPE)12)
#define AFTP_F                     ((AFTP_RECORD_FORMAT_TYPE)13)
#define AFTP_FA                    ((AFTP_RECORD_FORMAT_TYPE)14)
#define AFTP_FB                    ((AFTP_RECORD_FORMAT_TYPE)15)
#define AFTP_FM                    ((AFTP_RECORD_FORMAT_TYPE)16)
#define AFTP_FBA                   ((AFTP_RECORD_FORMAT_TYPE)17)
#define AFTP_FBM                   ((AFTP_RECORD_FORMAT_TYPE)18)
#define AFTP_FBS                   ((AFTP_RECORD_FORMAT_TYPE)19)
#define AFTP_FBSM                  ((AFTP_RECORD_FORMAT_TYPE)20)
#define AFTP_FBSA                  ((AFTP_RECORD_FORMAT_TYPE)21)
#define AFTP_U                     ((AFTP_RECORD_FORMAT_TYPE)22)
#define AFTP_UA                    ((AFTP_RECORD_FORMAT_TYPE)23)
#define AFTP_UM                    ((AFTP_RECORD_FORMAT_TYPE)24)

/* Conversation Security Type Values */

#define AFTP_SECURITY_NONE     ((AFTP_SECURITY_TYPE)0)
#define AFTP_SECURITY_SAME     ((AFTP_SECURITY_TYPE)1)
#define AFTP_SECURITY_PROGRAM  ((AFTP_SECURITY_TYPE)2)

/* Trace Levels */

#define AFTP_LVL_NO_TRACING            ((AFTP_TRACE_LEVEL_TYPE)   (0))
#define AFTP_LVL_FAILURES              ((AFTP_TRACE_LEVEL_TYPE)  (10))
#define AFTP_LVL_API                   ((AFTP_TRACE_LEVEL_TYPE)  (20))
#define AFTP_LVL_MODULE                ((AFTP_TRACE_LEVEL_TYPE)  (30))
#define AFTP_LVL_FUNCTION              ((AFTP_TRACE_LEVEL_TYPE)  (40))
#define AFTP_LVL_IO_OPEN_CLOSE         ((AFTP_TRACE_LEVEL_TYPE)  (50))
#define AFTP_LVL_LINE_FLOW             ((AFTP_TRACE_LEVEL_TYPE) (100))
#define AFTP_LVL_IO_READ_WRITE         ((AFTP_TRACE_LEVEL_TYPE) (110))
#define AFTP_LVL_STATUS_DUMP           ((AFTP_TRACE_LEVEL_TYPE) (120))
#define AFTP_LVL_VARIABLE              ((AFTP_TRACE_LEVEL_TYPE) (130))
#define AFTP_LVL_LOCATION              ((AFTP_TRACE_LEVEL_TYPE) (140))
#define AFTP_LVL_DATA_TRACE            ((AFTP_TRACE_LEVEL_TYPE) (170))
#define AFTP_LVL_LOOPLOC               ((AFTP_TRACE_LEVEL_TYPE) (200))

#define AFTP_MAX_TRACE_LVL              AFTP_LVL_LOOPLOC

/* Supported File Write Modes */

#define AFTP_REPLACE          ((AFTP_WRITE_MODE_TYPE)0)
#define AFTP_APPEND           ((AFTP_WRITE_MODE_TYPE)1)
#define AFTP_NOREPLACE        ((AFTP_WRITE_MODE_TYPE)2)
#define AFTP_STOREUNIQUE      ((AFTP_WRITE_MODE_TYPE)3)

/* AFTP buffer lengths */

#define AFTP_FILE_NAME_SIZE     ((AFTP_LENGTH_TYPE)512)
#define AFTP_FQLU_NAME_SIZE     ((AFTP_LENGTH_TYPE)18)
#define AFTP_MODE_NAME_SIZE     ((AFTP_LENGTH_TYPE)9)
#define AFTP_PASSWORD_SIZE      ((AFTP_LENGTH_TYPE)11)
#define AFTP_TP_NAME_SIZE       ((AFTP_LENGTH_TYPE)65)
#define AFTP_USERID_SIZE        ((AFTP_LENGTH_TYPE)11)
#define AFTP_MESSAGE_SIZE       ((AFTP_LENGTH_TYPE)512)
#define AFTP_DATA_TYPE_SIZE     ((AFTP_LENGTH_TYPE)7)
#define AFTP_DATE_MODE_SIZE     ((AFTP_LENGTH_TYPE)4)
#define AFTP_RECORD_FORMAT_SIZE ((AFTP_LENGTH_TYPE)8)
#define AFTP_WRITE_MODE_SIZE    ((AFTP_LENGTH_TYPE)12)
#define AFTP_INI_MESSAGE_SIZE   ((AFTP_LENGTH_TYPE)1024)
#define AFTP_SYSTEM_INFO_SIZE   ((AFTP_LENGTH_TYPE)512)

/**********************************************************************
 *  AFTP Long to Short Function Name Mapping
 **********************************************************************/

#define aftp_change_dir                 FTCD
#define aftp_close                      FTCLOSE
#define aftp_connect                    FTCONN
#define aftp_create                     FTCREATE
#define aftp_create_dir                 FTCRTDIR
#define aftp_delete                     FTDEL
#define aftp_destroy                    FTDESTRY
#define aftp_dir_close                  FTDIRCLS
#define aftp_dir_open                   FTDIROPN
#define aftp_dir_read                   FTDIRRD
#define aftp_extract_allocation_size    FTEAS
#define aftp_extract_block_size         FTEBS
#define aftp_extract_date_mode          FTEDM
#define aftp_extract_destination        FTEDST
#define aftp_extract_data_type          FTEDT
#define aftp_extract_mode_name          FTEMN
#define aftp_extract_partner_LU_name    FTEPLU
#define aftp_extract_password           FTEPW
#define aftp_extract_record_format      FTERF
#define aftp_extract_record_length      FTERL
#define aftp_extract_security_type      FTEST
#define aftp_extract_tp_name            FTETPN
#define aftp_extract_trace_level        FTETL
#define aftp_extract_userid             FTEUI
#define aftp_extract_write_mode         FTEWM
#define aftp_format_error               FTFE
#define aftp_get_date_mode_string       FTGDMS
#define aftp_get_data_type_string       FTGDTS
#define aftp_get_record_format_string   FTGRFS
#define aftp_get_write_mode_string      FTGWMS
#define aftp_local_change_dir           FTLCD
#define aftp_local_dir_close            FTLDC
#define aftp_local_dir_open             FTLDO
#define aftp_local_dir_read             FTLDR
#define aftp_load_ini_file              FTLIF
#define aftp_local_query_current_dir    FTLQCD
#define aftp_query_bytes_transferred    FTQBT
#define aftp_query_current_dir          FTQCD
#define aftp_query_local_system_info    FTQLSI
#define aftp_query_local_version        FTQLV
#define aftp_query_system_info          FTQSYS
#define aftp_remove_dir                 FTRD
#define aftp_receive_file               FTRECV
#define aftp_rename                     FTREN
#define aftp_set_allocation_size        FTSAS
#define aftp_set_block_size             FTSBS
#define aftp_set_destination            FTSDEST
#define aftp_set_date_mode              FTSDM
#define aftp_set_data_type              FTSDT
#define aftp_send_file                  FTSEND
#define aftp_set_mode_name              FTSMN
#define aftp_set_password               FTSP
#define aftp_set_record_format          FTSRF
#define aftp_set_record_length          FTSRL
#define aftp_set_security_type          FTSST
#define aftp_set_trace_filename         FTSTF
#define aftp_set_trace_level            FTSTL
#define aftp_set_tp_name                FTSTP
#define aftp_set_userid                 FTSU
#define aftp_set_write_mode             FTSWM

#define ftcd                            FTCD
#define ftclose                         FTCLOSE
#define ftconn                          FTCONN
#define ftcreate                        FTCREATE
#define ftcrtdir                        FTCRTDIR
#define ftdel                           FTDEL
#define ftdestry                        FTDESTRY
#define ftdircls                        FTDIRCLS
#define ftdiropn                        FTDIROPN
#define ftdirrd                         FTDIRRD
#define fteas                           FTEAS
#define ftebs                           FTEBS
#define ftedm                           FTEDM
#define ftedst                          FTEDST
#define ftedt                           FTEDT
#define ftemn                           FTEMN
#define fteplu                          FTEPLU
#define ftepw                           FTEPW
#define fterf                           FTERF
#define fterl                           FTERL
#define ftest                           FTEST
#define ftetpn                          FTETPN
#define fteui                           FTEUI
#define ftewm                           FTEWM
#define ftfe                            FTFE
#define ftgdms                          FTGDMS
#define ftgdts                          FTGDTS
#define ftgrfs                          FTGRFS
#define ftgwms                          FTGWMS
#define ftlcd                           FTLCD
#define ftldc                           FTLDC
#define ftldo                           FTLDO
#define ftldr                           FTLDR
#define ftlif                           FTLIF
#define ftlqcd                          FTLQCD
#define ftqbt                           FTQBT
#define ftqcd                           FTQCD
#define ftqlsi                          FTQLSI
#define ftqlv                           FTQLV
#define ftqsys                          FTQSYS
#define ftrd                            FTRD
#define ftrecv                          FTRECV
#define ftren                           FTREN
#define ftsas                           FTSAS
#define ftsbs                           FTSBS
#define ftsdest                         FTSDEST
#define ftsdm                           FTSDM
#define ftsdt                           FTSDT
#define ftsend                          FTSEND
#define ftstl                           FTSTL
#define ftstf                           FTSTF
#define ftetl                           FTETL
#define ftsmn                           FTSMN
#define ftsp                            FTSP
#define ftsrf                           FTSRF
#define ftsrl                           FTSRL
#define ftsst                           FTSST
#define ftstp                           FTSTP
#define ftsu                            FTSU
#define ftswm                           FTSWM

/**********************************************************************
 *  AFTP Function Prototypes
 **********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


AFTP_ENTRY
aftp_change_dir(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  directory,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_close(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_connect(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_create(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_create_dir(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  directory,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_delete(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  filename,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_destroy(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_dir_close(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_dir_open(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  filespec,
    AFTP_LENGTH_TYPE                        length,
    AFTP_FILE_TYPE_TYPE                     file_type,
    AFTP_INFO_LEVEL_TYPE                    info_level,
    unsigned char AFTP_PTR                  path,
    AFTP_LENGTH_TYPE                        path_buffer_length,
    AFTP_LENGTH_TYPE AFTP_PTR               path_returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_dir_read(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  dir_entry,
    AFTP_LENGTH_TYPE                        dir_entry_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_BOOLEAN_TYPE AFTP_PTR              no_more_entries,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_allocation_size(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_ALLOCATION_SIZE_TYPE AFTP_PTR      allocation_size,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_block_size(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_BLOCK_SIZE_TYPE AFTP_PTR           block_size,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_data_type(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_DATA_TYPE_TYPE AFTP_PTR            data_type,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_date_mode(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_DATE_MODE_TYPE AFTP_PTR            date_mode,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_destination(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  destination,
    AFTP_LENGTH_TYPE                        destination_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_mode_name(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  mode_name,
    AFTP_LENGTH_TYPE                        mode_name_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_partner_LU_name(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  partner_LU_name,
    AFTP_LENGTH_TYPE                        partner_LU_name_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_password(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  password,
    AFTP_LENGTH_TYPE                        password_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_record_format(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RECORD_FORMAT_TYPE AFTP_PTR        record_format,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_record_length(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RECORD_LENGTH_TYPE AFTP_PTR        record_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_security_type(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_SECURITY_TYPE AFTP_PTR             security_type,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_tp_name(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  tp_name,
    AFTP_LENGTH_TYPE                        tp_name_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_userid(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  userid,
    AFTP_LENGTH_TYPE                        userid_size,
    AFTP_LENGTH_TYPE AFTP_PTR               length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_write_mode(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_WRITE_MODE_TYPE AFTP_PTR           write_mode,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_get_data_type_string(
    AFTP_DATA_TYPE_TYPE                     data_type,
    unsigned char AFTP_PTR                  data_type_string,
    AFTP_LENGTH_TYPE                        data_type_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_get_date_mode_string(
    AFTP_DATE_MODE_TYPE                     date_mode,
    unsigned char AFTP_PTR                  date_mode_string,
    AFTP_LENGTH_TYPE                        date_mode_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_get_record_format_string(
    AFTP_RECORD_FORMAT_TYPE                 record_format,
    unsigned char AFTP_PTR                  record_format_string,
    AFTP_LENGTH_TYPE                        record_format_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_get_write_mode_string(
    AFTP_WRITE_MODE_TYPE                    write_mode,
    unsigned char AFTP_PTR                  write_mode_string,
    AFTP_LENGTH_TYPE                        write_mode_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_load_ini_file(
    unsigned char AFTP_PTR                  filename,
    AFTP_LENGTH_TYPE                        filename_size,
    unsigned char AFTP_PTR                  program_path,
    AFTP_LENGTH_TYPE                        path_size,
    unsigned char AFTP_PTR                  error_string,
    AFTP_LENGTH_TYPE                        error_string_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_local_change_dir(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  directory,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_local_dir_close(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_local_dir_open(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  filespec,
    AFTP_LENGTH_TYPE                        length,
    AFTP_FILE_TYPE_TYPE                     file_type,
    AFTP_INFO_LEVEL_TYPE                    info_level,
    unsigned char AFTP_PTR                  path,
    AFTP_LENGTH_TYPE                        path_buffer_length,
    AFTP_LENGTH_TYPE AFTP_PTR               path_returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_local_dir_read(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  dir_entry,
    AFTP_LENGTH_TYPE                        dir_entry_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_BOOLEAN_TYPE AFTP_PTR              no_more_entries,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_local_query_current_dir(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  directory,
    AFTP_LENGTH_TYPE                        directory_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_query_bytes_transferred(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_LENGTH_TYPE AFTP_PTR               bytes_transferred,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_query_current_dir(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  directory,
    AFTP_LENGTH_TYPE                        directory_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_query_local_version(
    AFTP_VERSION_TYPE AFTP_PTR              major_version,
    AFTP_VERSION_TYPE AFTP_PTR              minor_version,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_query_local_system_info(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  system_info,
    AFTP_LENGTH_TYPE                        system_info_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_query_system_info(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  system_info,
    AFTP_LENGTH_TYPE                        system_info_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_receive_file(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  local_file,
    AFTP_LENGTH_TYPE                        local_file_length,
    unsigned char AFTP_PTR                  remote_file,
    AFTP_LENGTH_TYPE                        remote_file_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_remove_dir(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  directory,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_rename(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  oldfile,
    AFTP_LENGTH_TYPE                        oldlength,
    unsigned char AFTP_PTR                  newfile,
    AFTP_LENGTH_TYPE                        newlength,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_send_file(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  local_file,
    AFTP_LENGTH_TYPE                        local_file_length,
    unsigned char AFTP_PTR                  remote_file,
    AFTP_LENGTH_TYPE                        remote_file_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_allocation_size(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_ALLOCATION_SIZE_TYPE               allocation_size,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_block_size(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_BLOCK_SIZE_TYPE                    block_size,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_data_type(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_DATA_TYPE_TYPE                     data_type,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_date_mode(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_DATE_MODE_TYPE                     date_mode,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_destination(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  destination,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_trace_level(
    AFTP_TRACE_LEVEL_TYPE                   trace_level,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_extract_trace_level(
    AFTP_TRACE_LEVEL_TYPE AFTP_PTR          trace_level,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_trace_filename(
    unsigned char AFTP_PTR                  filename,
    AFTP_LENGTH_TYPE                        filename_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_mode_name(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  mode_name,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_password(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  password,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_record_format(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RECORD_FORMAT_TYPE                 record_format,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_record_length(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_RECORD_LENGTH_TYPE                 record_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_security_type(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_SECURITY_TYPE                      security_type,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_tp_name(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  tp_name,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_userid(
    unsigned char AFTP_PTR                  connection_id,
    unsigned char AFTP_PTR                  userid,
    AFTP_LENGTH_TYPE                        length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_set_write_mode(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_WRITE_MODE_TYPE                    write_mode,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

AFTP_ENTRY
aftp_format_error(
    unsigned char AFTP_PTR                  connection_id,
    AFTP_DETAIL_LEVEL_TYPE                  detail_level,
    unsigned char AFTP_PTR                  error_str,
    AFTP_LENGTH_TYPE                        error_str_size,
    AFTP_LENGTH_TYPE AFTP_PTR               returned_length,
    AFTP_RETURN_CODE_TYPE AFTP_PTR          rc);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
