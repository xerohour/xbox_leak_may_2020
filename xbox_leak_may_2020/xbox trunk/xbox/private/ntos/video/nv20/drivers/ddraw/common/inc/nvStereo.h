/***************************************************************************\
* Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.            *
* THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO       *
* NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY  *
* IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.                *
*                                                                           *
*  Portions: Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.*
*                                                                           *
* Module: Stereo.h                                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Andrei Osnovich    02/08/00  Created                                *
*                                                                           *
\***************************************************************************/

#ifndef _STEREO_H_
#define _STEREO_H_

#ifdef  STEREO_SUPPORT

#define LASER_SIGHT
#define SNAP_SHOT

#ifdef	DEBUG
#define	STEREO_DEBUG
#else
#ifdef	DEVELOP
#define	STEREO_DEBUG
#endif
#endif

#include <nvRmStereo.h>
#include "duma.h"
#include "StereoDDK.h"

#define UPTODATE_STEREO_SUPPORT (SVF_OVERANDUNDER | SVF_FRAMESEQUENTIAL)

//Eye's IDs.
#define EYE_NONE  0
#define EYE_LEFT  1
#define EYE_RIGHT 2
#define EYE_COUNT EYE_RIGHT+1

#define DEFAULT_STEREO_SEPARATION               0.08f
#define DEFAULT_STEREO_CONVERGENCE              4.0f
#define DEFAULT_STEREO_CONVERGENCE_MULTIPLIER   5.0f
#define CONVERGENCE_THRESHOLD                   0.01f

//FrustumAdjustMode values
#define NO_FRUSTUM_ADJUST                   0
#define FRUSTUM_STRETCH                     1
#define FRUSTUM_CLEAR_EDGES                 2

//PartialClearMode values
#define DO_AS_REQUESTED                     0
#define DO_A_FULL_CLEAR                     1
#define DO_NOTHING                          2

//Possible bit values in the pStereoData->dwFlags.
#define STEREO_PROJECTIONMATRIXVALID        0x00000001
#define STEREO_LOCKCACHEABLE                0x00000002
#define STEREO_FORCEDOFF                    0x00000004
#define STEREO_FORCEDTOGDI                  0x00000008
#define STEREO_LOCKACTIVE                   0x00000010
#define STEREO_LOCKINIT                     0x00000020
#define STEREO_RHWGREATERATSCREEN           0x00000040
#define STEREO_RHWEQUALATSCREEN             0x00000080
#define STEREO_RHWLESSATSCREEN              0x00000100
#define STEREO_DISALLOWED_NOT_FULL_SCREEN   0x00000200
#define STEREO_DISALLOWED_NO_RESOURCES      0x00000400
#define STEREO_DISALLOWED                   0x00000600
#define STEREO_AUTOCONVERGENCE              0x00000800
#define STEREO_DISABLETnL                   0x00001000
#define STEREO_RIGHTEYEINIT                 0x00002000
#define STEREO_FAVORSZOVERRHW               0x00004000
#define STEREO_LASERSIGHT                   0x00008000
#define STEREO_LASERGETRHWSCALE             0x00010000
#define STEREO_CLEAROCCURED                 0x00020000
#define STEREO_DEFAULTON                    0x00040000
#define STEREO_CONVERGENCE                  0x00080000
#define STEREO_CONVERGENCEMULTIPLIER        0x00100000
#define STEREO_SEPARATION                   0x00200000
#define STEREO_LASERSIGHTENABLED            0x00400000
#define STEREO_ANAGLYPH                     0x00800000
#define STEREO_GAMMACORRECTION              0x01000000
#define STEREO_MIXEDTNLS					0x02000000

#define STEREO_CUSTOMSETTINGSMASK           (STEREO_RHWGREATERATSCREEN|STEREO_RHWEQUALATSCREEN|STEREO_RHWLESSATSCREEN|STEREO_AUTOCONVERGENCE|STEREO_CONVERGENCE|STEREO_CONVERGENCEMULTIPLIER|STEREO_SEPARATION|STEREO_FAVORSZOVERRHW)      

#define STEREO_LOG_DIRECTORY		        "C:\\NVSTEREO.LOG"
#define STEREO_IMG_DIRECTORY		        "C:\\NVSTEREO.IMG"

#ifdef  LASER_SIGHT_
typedef unsigned short* (__cdecl *GetLaserSightType)    (DWORD &dwWidth, DWORD &dwHeight, COLORREF &crRef, DWORD &dwTransparency);	
typedef void            (__cdecl *ReleaseLaserSightType)(unsigned short *pusData);
#endif  //LASER_SIGHT
#ifdef  LASER_SIGHT
typedef unsigned short* (__cdecl *GetLaserSight)    (DWORD &dwWidth, DWORD &dwHeight, DWORD &dwColorKey);	
typedef void            (__cdecl *ReleaseLaserSight)(unsigned short *pusData);
#endif  //LASER_SIGHT

typedef struct _per_vertex_storage_Flip
{
    LPD3DVALUE      pVertex;
    D3DVALUE        OrigX;
    D3DVALUE        XCorrection;
} PERVERTEXSTORAGE_FLIP, *LPPERVERTEXSTORAGE_FLIP;

/*****************************************************************************
 * CVertexRegister
 * This class is used to determine which vertices are unique in a list. Required when we modify
 * post-transformed vertices for the left and right eyes.
 */
class CVertexRegister
{
    /*
     * constants
     */
public:
    enum
    {   //m_dwStage possible values.
        FIRST_EYE           = 0,
        SECOND_EYE          = 1,
        RESTORE             = 2,
    };
    /*
     * members
     */
protected:
    LPBYTE                  m_pInUse;
    DWORD                   m_dwInUseSize;
    LPPERVERTEXSTORAGE_FLIP m_VertexStorage_Flip;
    DWORD                   m_dwVertexCount;
    DWORD                   m_dwStage; // 0 - first eye, 1 - second eye, 2 - restore

    /*
     * methods
     */
public:
    inline void cleanupInUse(void)
    {
        memset ((LPVOID)m_pInUse, 0, m_dwInUseSize);
        //Pleeease don't touch m_dwStage. It lives its own life.
    }
    BOOL isInUse(DWORD dwVertex);
    BOOL isInUseTag(DWORD dwVertex);
    inline void UpdateStage(void)
    {
        if (++m_dwStage > RESTORE)
            m_dwStage = FIRST_EYE;
    }
    inline void SaveX(LPD3DVALUE pX)
    {
        m_VertexStorage_Flip[m_dwVertexCount].pVertex  = pX;
        m_VertexStorage_Flip[m_dwVertexCount].OrigX    = pX[0];
    }
    inline void SaveXCorrection_Flip(D3DVALUE XCorrection)
    {
        m_VertexStorage_Flip[m_dwVertexCount].XCorrection  = XCorrection;
        m_dwVertexCount++;
    }
    //Next functions' behaviour is based on the current stage. If stage is FIRST_EYE the function does nothing
    //and report that. The caller is responsible to fill out the m_pVertexData and next time the DoAutomaticStage
    //will take care of everything automatically.
    DWORD   DoAutomaticStage_Flip(void);

    /*
     * construction
     */
public:
    CVertexRegister (DWORD dwNumVertices);
    ~CVertexRegister (void);
};

#define	STEREO_CONFIG_ASSIST
#ifdef	STEREO_CONFIG_ASSIST
typedef struct _CONFIG_ASSIST_INFO
{
    HOTKEY      SuggestKey;
    HOTKEY      UnsuggestKey;
    HOTKEY      WriteKey;
    HOTKEY      DeleteKey;
    HOTKEY      FrustumAdjust;
    HOTKEY      ToggleAutoConvergence;
    HOTKEY      ToggleAutoConvergenceRestore;
    HOTKEY      RHWAtScreenMore;
    HOTKEY      RHWAtScreenLess;
    HOTKEY      RHWLessAtScreenMore;
    HOTKEY      RHWLessAtScreenLess;
    HOTKEY      LaserXPlusKey;
    HOTKEY      LaserXMinusKey;
    HOTKEY      LaserYPlusKey;
    HOTKEY      LaserYMinusKey;
    HOTKEY      GammaAdjustMore;
    HOTKEY      GammaAdjustLess;
    DWORD       dwFlags;

    D3DVALUE    rhwMin;
    D3DVALUE    rhwMax;
    D3DVALUE    rhwMin2D;
    D3DVALUE    rhwMax2D;
    D3DVALUE    szMin;
    D3DVALUE    szMax;
    D3DVALUE    szMin2D;
    D3DVALUE    szMax2D;

    D3DVALUE    fLastNonAutoConvergenceMultiplier;
    HANDLE      FileHandle;
    char        Buf[1024];
} CONFIG_ASSIST_INFO;
#endif //STEREO_CONFIG_ASSIST

typedef struct _STEREOSETTINGS {
    DWORD               dwFlags;
    float               fStereoSeparation;      //Half way horizontal separation if screen width is 1.0 (0.04).
    float               fScreenStereoSeparation;//Half way horizontal separation in pixels based on mode.
    float               fStereoConvergence;     //Convergence degree (in the range [0.0 - 0.15]).
    float               fPostStereoConvergence; //Convergence degree for post-transform (game specific).
    float               fPostStereoXStretch;    //Stretching of the field of view for post-transform.
    float               fPostStereoXShift;      //Shifting of the field of view for post-transform.
    float               fRHWGreaterAtScreen;    //All objects closer than this are considered to be 2D.
    float               fRHWLessAtScreen;       //All objects farther than this are considered to be 2D.
    float               fConvergenceMultiplier; //fPostStereoConvergence = fStereoConvergence*fConvergenceMultiplier
    DWORD               FrustumAdjustMode;      //Tells how to adjust frustum for possible lack of X information after eye shift.
    DWORD               PartialClearMode;       //Tells how to do a partial clear (as is, replace with a full screen clear or ignore).
    float               AutoConvergenceAdjustPace;
    float               fStereoGamma;
#ifdef  LASER_SIGHT
    float               fLaserXAdjust;
    float               fLaserYAdjust;
#endif  //LASER_SIGHT
} STEREOSETTINGS, *LPSTEREOSETTINGS;

typedef struct _STEREOHOKEYS {
    LPCHOTKEYINTERFACE  pHotKeyI;
    HOTKEY              StereoToggle;
    HOTKEY              StereoSeparationAdjustMore;
    HOTKEY              StereoSeparationAdjustLess;
    HOTKEY              StereoConvergenceAdjustMore;
    HOTKEY              StereoConvergenceAdjustLess;
} STEREOHOKEYS, *LPSTEREOHOKEYS;

typedef struct _STEREOEYE {
    D3DHAL_DP2VIEWPORTINFO  Viewport;               //Eye viewport
    D3DMATRIX               ProjMatrix;             //Eye projection matrix
} STEREOEYE, *LPSTEREOEYE;

typedef struct _STEREODATA {
    STEREOSETTINGS          StereoSettings;
    BOOL                    bStereoActivated;       //Stereo is currently activated.
    DWORD                   dwHWTnL;                //Total amount of currently allocated non transformed VBs.
    BYTE                    StereoViewerName[128];  //Stereo viewer driver name.
#ifdef  LASER_SIGHT
    BYTE                    LaserSightFile[128];    //Stereo laser sight file name.
#endif  //LASER_SIGHT
    DWORD                   dwStereoVideoMode;      //Current video mode used for stereo.
#ifdef  STEREO_VIEWER_DRV
    STEREOCAPS              StereoViewerCaps;       //Stereo viewer capabilities.
    DWORD                   dwStereoFormat;         //Method of displaying stereo.
    LPSTEREOVIEWER          pStereoViewer;          //Pointer to stereo viewer object
    HMODULE                 hStereoDrv;             //Stereo viewer driver handle.
#else   //STEREO_VIEWER_DRV==0
    DWORD                   dwStereoHWType;         //Stereo Viewer HW type.
    DWORD                   dwLineCodeColor;
	DWORD					dwLeftAnaglyphFilter;
	DWORD					dwRightAnaglyphFilter;
#endif  //STEREO_VIEWER_DRV
    float                   fFrameRHWMax;           //Closes vertex RHW for a scene.
    float                   fRHW2DDetectionMin;     //RHW far limit for 2D object detection.
    DWORD                   dwLastEye;              //The eye representing the current stereo context.
    PNVD3DCONTEXT           dwContext;
    DWORD                   StereoModeNumber;       //Video mode for which stereo had been activated
    DWORD                   dwVertexRegisterSize;   //Max # of vertices we accept.
    CVertexRegister        *pVertexRegister;        //Pointer to an object used to modify post-transformed vertices.
    LPDDRAWI_DDRAWSURFACE_LCL pLockSubstituteLcl;   //Lock substitute local for emulating front-back-z locks.
    CSimpleSurface         *pLastLockedSurface;
    LPDDRAWI_DDRAWSURFACE_LCL pLockTargetLcl;       //Our fake persistent Lcl for delayed lock processing
    DWORD                   dwSurfaceCount;         //# of stereo surfaces
    char                    bAppName[128];          //Current application name returned by GetCommandLine (might be bogus).
    char                    bAppRealName[128];      //Current application name retreived from the bAppName (if not a symbolic link).
    WORD                    SaveCurrentGamma[256*3];
    CSimpleSurface         *pRenderTargets[3];      
    NV_CFG_STEREO_PARAMS    RmStereoParams;         //Shared structure between D3D driver & resman for flipped stereo.
    STEREOHOKEYS            StereoKeys;             //Stereo control hotkeys.
    STEREOEYE               Eye[EYE_COUNT];         //Eye specific stereo data
} STEREODATA, *LPSTEREODATA;

typedef struct StereoBltPassInfo
{
    union {
        DWORD   dwDstOffset;
        DWORD   dwDstY;
        FLATPTR fpDstVidMem;
        LONG    lTop;
    };
    union {
        DWORD   dwSrcOffset;
        DWORD   dwSrcY;
        FLATPTR fpSrcVidMem;
        LONG    lBottom;
    };
    DWORD   dwPass;
} STEREOBLTPASSINFO, *LPSTEREOBLTPASSINFO;

typedef DWORD (*PSTEREOEYEFUNC)(PNVD3DCONTEXT pContext, LPBYTE pVertices, DWORD dwCount);
typedef DWORD (*PSTEREOEYEFUNCINDEX)(PNVD3DCONTEXT pContext, LPBYTE pVertices, LPBYTE pIndices, DWORD dwCount);
typedef BOOL  (*PSTEREORHWFUNC)(PNVD3DCONTEXT pContext, LPBYTE pVertices, DWORD dwCount, DWORD dwCondition);
typedef BOOL  (*PSTEREORHWFUNCINDEX)(PNVD3DCONTEXT pContext, LPBYTE pVertices, LPBYTE pIndices, DWORD dwCount, DWORD dwCondition);

extern  void                        LoadStereoRegistry(void);
extern  void                        StereoAdjustmentMonitor(PNVD3DCONTEXT pContext);
extern  void                        StereoContextCreate(void);
extern  void                        StereoContextDestroy(PNVD3DCONTEXT pContext);
extern  void                        DestroyStereo(void);
extern  LPD3DMATRIX                 SetStereoProjection(LPD3DMATRIX pMat, PNVD3DCONTEXT pContext);
extern  void                        SetupStereoContext(PNVD3DCONTEXT pContext);
extern  void                        AlternateStereoContext(PNVD3DCONTEXT pContext);
extern  DWORD                       GetStereoOffset(CSimpleSurface *pSurf);
extern  DWORD                       StereoBltAdjustParameters(SURFINFO &src, SURFINFO &dst, STEREOBLTPASSINFO &StereoBltPass);
extern  BOOL                        CreateLockSubstitute(LPDDRAWI_DDRAWSURFACE_LCL pDDSLcl);
extern  BOOL                        StereoLockSubstitute(LPDDHAL_LOCKDATA lpLockData);
extern  BOOL                        StereoLockSubstituteRestore(LPDDHAL_UNLOCKDATA lpUnlockData);
extern  BOOL                        StereoLockSubstituteFlush(void);
extern  void                        StereoFlipNotify(LPDDHAL_FLIPDATA pfd);
extern  void                        StereoModeSetProcessing(void);
extern  void                        StereoModifyDDHALInfo(void);
extern  void                        StereoBackend(LPDDHAL_FLIPDATA pfd);
extern  void __cdecl                LOG(char * szFormat, ...);

#ifdef  PER_PRIMITIVE_SYNC
extern  void						StereoSync(void);
#endif	//PER_PRIMITIVE_SYNC

#ifdef  LASER_SIGHT
//extern  void                        ClearLaserSight(PNVD3DCONTEXT pContext);
#endif  //LASER_SIGHT

extern  LPSTEREODATA                pStereoData;
extern  nvDP2FunctionTable nvDP2SetFuncs_Stereo_Flip;
extern  nvDP2FunctionTable nvDP2SetFuncs_Orig;

#define NvSetStereoDataPtrFromDriverData  pStereoData=(LPSTEREODATA)pDriverData->pStereoData;

#define STEREO_ENABLED              (pStereoData) //Stereo can be potentially activated.
#define STEREO_ACTIVATED            (STEREO_ENABLED && pStereoData->bStereoActivated)
#define ORIG_VIEWPORT               (pStereoData->Eye[EYE_NONE].Viewport)
#define ORIG_PROJECTION             (pStereoData->Eye[EYE_NONE].ProjMatrix)
#define STEREO_R_US(pCtxt)          (pStereoData->dwContext==pCtxt) // BUGBUG remove me eventually
#define STEREODATA(field)           pStereoData->field

#define CREATESTEREOHOTKEY(HotKey)  CREATEHOTKEY(pStereoData->StereoKeys.pHotKeyI,HotKey)
#define DESTROYSTEREOHOTKEY(HotKey) DESTROYHOTKEY(pStereoData->StereoKeys.pHotKeyI,HotKey)
#define POLLSTEREOHOTKEY(HotKey)    POLLHOTKEY(pStereoData->StereoKeys.pHotKeyI,HotKey)
#define ENFORCEKEYBOARDFILTER()     ENFORCEFILTER(pStereoData->StereoKeys.pHotKeyI)

/*********************************************************************************************************/

#else  // !STEREO_SUPPORT

#define NvSetStereoDataPtrFromDriverData

/*********************************************************************************************************/

#endif  // !STEREO_SUPPORT

#endif  //_STEREO_H_

