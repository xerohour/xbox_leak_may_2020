/*M*
//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 1995 Intel Corporation. All Rights Reserved.
//
//      $Workfile: nsperror.h $
//      $Revision: 3 $
//      $Modtime: Dec 16 1996 18:31:22 $
//
//  Purpose: NSP Error Handling Module
*M*/
#ifdef __cplusplus
extern "C" {
#endif

#if !defined (_NSPERROR_H) || defined (_OWN_BLDPCS)
    #define _NSPERROR_H

/* ------------------------------------------------------------------------*/

    #if !defined (_OWN_BLDPCS)

    typedef struct _NSPLibVersion
        {
        int    major;                   /* e.g. 3                          */
        int    minor;                   /* e.g. 00                         */
        int    build;                   /* e.g. 11                         */
        const char * Name;                    /* e.g. "nspp6l.lib","nspm5.dll"   */
        const char * Version;                 /* e.g. "v4.00"                    */
        const char * InternalVersion;         /* e.g. "[3.00.11, 07/12/96]"      */
        const char * BuildDate;               /* e.g. "Jun 1 96"                 */
        const char * CallConv;                /* e.g. "DLL",..                   */
        } NSPLibVersion;

/*--- NSPErrStatus,NSPErrMode Values Definition ---------------------------*/

/* common status code definitions */

        #define NSP_StsOk             0         /* everything is ok                */
        #define NSP_StsBackTrace     -1         /* pseudo error for back trace     */
        #define NSP_StsError         -2         /* unknown /unspecified error      */
        #define NSP_StsInternal      -3         /* internal error (bad state)      */
        #define NSP_StsNoMem         -4         /* out of memory                   */
        #define NSP_StsBadArg        -5         /* function arg/param is bad       */
        #define NSP_StsBadFunc       -6         /* unsupported function            */
        #define NSP_StsNoConv        -7         /* iter. didn't converge           */
        #define NSP_StsAutoTrace     -8         /* Tracing through nsptrace.h      */
        #define NSP_StsDivideByZero  -9
        #define NSP_StsNullPtr       -10
        #define NSP_StsBadSizeValue  -11
        #define NSP_StsBadPtr        -12
        #define NSP_StsBadStruct     -13
        #define NSP_StsBadLen        -14         /* bad vector length              */          

        #define NSP_ErrModeLeaf     0           /* Print error and exit program    */
        #define NSP_ErrModeParent   1           /* Print error and continue        */
        #define NSP_ErrModeSilent   2           /* Don't print and continue        */

/* custom status code definitions */
    /* nspsmpl */
        #define NSP_StsBadFact       -50        /* Negative FactorRange            */
    /* nspfirg */
        #define NSP_StsBadFreq       -51        /* bad frequency value             */
        #define NSP_StsBadRel        -52        /* bad relation between frequency  */ 



    typedef int (__STDCALL *NSPErrorCallBack)  (NSPStatus status,
                                                const char *funcname, const char *context,
                                                const char *file, int   line);

/* ------------------------------------------------------------------------*/
/* Flags for scaleMode parameter of nsp?Func(...,scaleMode,scaleFactor)    */

        #define NSP_NO_SCALE        0x0000      /* ignore factor                   */
        #define NSP_FIXED_SCALE     0x0002      /* fixed factor value              */
        #define NSP_AUTO_SCALE      0x0004      /* detect factor value             */

        #define NSP_OVERFLOW        0x0000      /* wrapround                       */
        #define NSP_SATURATE        0x0001      /* saturate                        */

/*--- Macros --------------------------------------------------------------*/

        #define NSP_ERROR(status,func,context)\
nspError((status),(func),(context),__FILE__,__LINE__)

        #define NSP_ERRCHK(func,context)\
   ((nspGetErrStatus() >= 0) ? NSP_StsOk \
: NSP_ERROR(NSP_StsBackTrace,(func),(context)))

        #define NSP_ASSERT(expr,func,context)\
   ((expr) ? NSP_StsOk \
: NSP_ERROR(NSP_StsInternal,(func),(context)))

        #define NSP_RSTERR() (nspSetErrStatus(NSP_StsOk))


    #endif
/*--- Get Library Version -------------------------------------------------*/
/* Returns pointer to NSP lib info structure                               */

    NSPAPI(const NSPLibVersion *,nspGetLibVersion,(void))

/*--- Get/Set ErrStatus ---------------------------------------------------*/

    NSPAPI(NSPStatus, nspGetErrStatus,(void))
    NSPAPI(void,      nspSetErrStatus,(NSPStatus status))

/*--- NspStdErrMode Declaration -------------------------------------------*/

    NSPAPI(int,  nspGetErrMode,(void))
    NSPAPI(void, nspSetErrMode,(int mode))

/*--- nspError,nspErrorStr Declaration ------------------------------------*/

    NSPAPI(NSPStatus,   nspError,(NSPStatus status, const char *func,
                                  const char *context, const char *file,int line))

    NSPAPI(const char*, nspErrorStr,(NSPStatus status))

/*--- nspRedirectError Declaration ----------------------------------------*/

    NSPAPI(NSPStatus, nspNulDevReport,(NSPStatus   status, const char *funcname,
                                       const char *context,const char *file,
                                       int   line))

    NSPAPI(NSPStatus, nspStdErrReport,(NSPStatus   status, const char *funcname,
                                       const char *context,const char *file,
                                       int   line))

    #if defined _WIN32   /* _WIN32 */
    NSPAPI(NSPStatus, nspGuiBoxReport,(NSPStatus   status, const char *funcname,
                                       const char *context,const char *file,
                                       int   line))
    #endif               /* _WIN32 */

    NSPAPI(NSPErrorCallBack, nspRedirectError,(NSPErrorCallBack nspErrorFunc))

#endif                                  /* _NSPERROR_H                     */
#ifdef __cplusplus
}
#endif
