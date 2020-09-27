
#if DIRECTINPUT_VERSION <= 0x0300
/*
 *  Old GUIDs from DX3 that were never used but which we can't recycle
 *  because we shipped them.
 */
DEFINE_GUID(GUID_RAxis,   0xA36D02E3,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_UAxis,   0xA36D02E4,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
DEFINE_GUID(GUID_VAxis,   0xA36D02E5,0xC9F3,0x11CF,0xBF,0xC7,0x44,0x45,0x53,0x54,0x00,0x00);
#endif
#define DIEFT_PREDEFMIN             0x00000001
#define DIEFT_PREDEFMAX             0x00000005
//#define DIEFT_PREDEFMAX             0x00000006
#define DIEFT_TYPEMASK              0x000000FF

#define DIEFT_FORCEFEEDBACK         0x00000100
#define DIEFT_VALIDFLAGS            0x0000FE00
#define DIEFT_ENUMVALID             0x040000FF
/*
 *  Name for the latest structures, in places where we specifically care.
 */
#if (DIRECTINPUT_VERSION >= 900)
typedef       DIEFFECT      DIEFFECT_DX9;
typedef       DIEFFECT   *LPDIEFFECT_DX9;
#else
typedef       DIEFFECT      DIEFFECT_DX6;
typedef       DIEFFECT   *LPDIEFFECT_DX6;
#endif

BOOL static __inline
IsValidSizeDIEFFECT(DWORD cb)
{
    return cb == sizeof(DIEFFECT_DX6)
        || cb == sizeof(DIEFFECT_DX5);
}


#define DIEFFECT_MAXAXES            32
#define DIEFF_OBJECTMASK            0x00000003
#define DIEFF_ANGULAR               0x00000060
#define DIEFF_COORDMASK             0x00000070
#define DIEFF_REGIONANGULAR         0x00006000
#define DIEFF_REGIONCOORDMASK       0x00007000

#define DIEFF_VALID                 0x00000073
#define DIEP_GETVALID_DX5           0x000001FF
#define DIEP_SETVALID_DX5           0xE00001FF
#define DIEP_GETVALID               0x000003FF
#define DIEP_SETVALID               0xE00003FF
#define DIEP_USESOBJECTS            0x00000028
#define DIEP_USESCOORDS             0x00000040
#define DIES_VALID                  0x80000001
#define DIES_DRIVER                 0x00000001
#define DIDEVTYPE_MAX           5
#define DI8DEVCLASS_MAX             5
#define DI8DEVTYPE_MIN              0x11
#define DI8DEVTYPE_GAMEMIN          0x14
#define DI8DEVTYPE_GAMEMAX          0x19
#define DI8DEVTYPE_MAX              0x1D
/* 
 *  If any change is made to the order or number of devices in the range 
 *  DI8DEVTYPE_GAMEMIN to DI8DEVTYPE_MAX CJoy_InitFromHwc must be updated.
 */
#define DI8DEVTYPEJOYSTICK_MIN_BUTTONS      5
#define DI8DEVTYPEGAMEPAD_MIN_BUTTONS       6
#define DI8DEVTYPEDRIVING_MIN_BUTTONS       4
#define DI8DEVTYPEFLIGHT_MIN_BUTTONS        4
#define DI8DEVTYPE1STPERSON_MIN_BUTTONS     4

#define DIDEVTYPE_TYPEMASK      0x000000FF
#define DIDEVTYPE_SUBTYPEMASK   0x0000FF00
#define DIDEVTYPE_ENUMMASK      0xFFFFFF00
#define DIDEVTYPE_ENUMVALID     0x00010000
#define DIDEVTYPE_RANDOM        0x80000000
#define MAKE_DIDEVICE_TYPE(maj, min)    MAKEWORD(maj, min) //
#define GET_DIDEVICE_TYPEANDSUBTYPE(dwDevType)    LOWORD(dwDevType) //
/*
 *  Name for the 5.0 structure, in places where we specifically care.
 */
typedef       DIDEVCAPS     DIDEVCAPS_DX5;
typedef       DIDEVCAPS  *LPDIDEVCAPS_DX5;

BOOL static __inline
IsValidSizeDIDEVCAPS(DWORD cb)
{
    return cb == sizeof(DIDEVCAPS_DX5) ||
           cb == sizeof(DIDEVCAPS_DX3);
}
#define DIDF_VALID              0x00000003  //
#define DIA_VALID               0x0000000F
#define DIAH_MAPMASK            0x0000003F
#define DIAH_VALID              0x8000003F
#define DIDBAM_VALID            0x00000007
#define DIDSAM_VALID            0x00000003
#define DICD_VALID              0x00000001
#define DIDIFTT_VALID           0x0000000F
#define DIDIFT_IMAGE2D_BMP      0x80000000
#define DIDIFTF_VALID           0x80010000
#define DIDIFT_VALID            ( DIDIFTT_VALID | DIDIFTF_VALID )
#define DIDAL_VALID         0x0000000F  //
#define HAVE_DIDEVICEOBJECTINSTANCE_DX5
typedef       DIDEVICEOBJECTINSTANCEA    DIDEVICEOBJECTINSTANCE_DX5A;
typedef       DIDEVICEOBJECTINSTANCEW    DIDEVICEOBJECTINSTANCE_DX5W;
typedef       DIDEVICEOBJECTINSTANCE     DIDEVICEOBJECTINSTANCE_DX5;
typedef       DIDEVICEOBJECTINSTANCEA *LPDIDEVICEOBJECTINSTANCE_DX5A;
typedef       DIDEVICEOBJECTINSTANCEW *LPDIDEVICEOBJECTINSTANCE_DX5W;
typedef       DIDEVICEOBJECTINSTANCE  *LPDIDEVICEOBJECTINSTANCE_DX5;
typedef const DIDEVICEOBJECTINSTANCEA *LPCDIDEVICEOBJECTINSTANCE_DX5A;
typedef const DIDEVICEOBJECTINSTANCEW *LPCDIDEVICEOBJECTINSTANCE_DX5W;
typedef const DIDEVICEOBJECTINSTANCE  *LPCDIDEVICEOBJECTINSTANCE_DX5;

BOOL static __inline
IsValidSizeDIDEVICEOBJECTINSTANCEW(DWORD cb)
{
    return cb == sizeof(DIDEVICEOBJECTINSTANCE_DX5W) ||
           cb == sizeof(DIDEVICEOBJECTINSTANCE_DX3W);
}

BOOL static __inline
IsValidSizeDIDEVICEOBJECTINSTANCEA(DWORD cb)
{
    return cb == sizeof(DIDEVICEOBJECTINSTANCE_DX5A) ||
           cb == sizeof(DIDEVICEOBJECTINSTANCE_DX3A);
}

#define DIDOI_NOTINPUT          0x80000000
#define DIDOI_ASPECTUNKNOWN     0x00000000
#define DIDOI_RANDOM            0x80000000
#define DIGDD_RESIDUAL      0x00000002  //
#define DIGDD_VALID         0x00000003  //
#define DISCL_EXCLMASK      0x00000003  //
#define DISCL_GROUNDMASK    0x0000000C  //
#define DISCL_VALID         0x0000001F  //
/*
 *  Name for the 5.0 structure, in places where we specifically care.
 */
typedef       DIDEVICEINSTANCEA    DIDEVICEINSTANCE_DX5A;
/*
 *  Name for the 5.0 structure, in places where we specifically care.
 */
typedef       DIDEVICEINSTANCEW    DIDEVICEINSTANCE_DX5W;
#ifdef UNICODE
typedef DIDEVICEINSTANCEW DIDEVICEINSTANCE;
typedef DIDEVICEINSTANCE_DX5W DIDEVICEINSTANCE_DX5;
#else
typedef DIDEVICEINSTANCEA DIDEVICEINSTANCE;
typedef DIDEVICEINSTANCE_DX5A DIDEVICEINSTANCE_DX5;
#endif // UNICODE
typedef       DIDEVICEINSTANCE     DIDEVICEINSTANCE_DX5;
typedef       DIDEVICEINSTANCEA *LPDIDEVICEINSTANCE_DX5A;
typedef       DIDEVICEINSTANCEW *LPDIDEVICEINSTANCE_DX5W;
#ifdef UNICODE
typedef LPDIDEVICEINSTANCE_DX5W LPDIDEVICEINSTANCE_DX5;
#else
typedef LPDIDEVICEINSTANCE_DX5A LPDIDEVICEINSTANCE_DX5;
#endif // UNICODE
typedef       DIDEVICEINSTANCE  *LPDIDEVICEINSTANCE_DX5;
typedef const DIDEVICEINSTANCEA *LPCDIDEVICEINSTANCE_DX5A;
typedef const DIDEVICEINSTANCEW *LPCDIDEVICEINSTANCE_DX5W;
#ifdef UNICODE
typedef DIDEVICEINSTANCEW DIDEVICEINSTANCE;
typedef LPCDIDEVICEINSTANCE_DX5W LPCDIDEVICEINSTANCE_DX5;
#else
typedef DIDEVICEINSTANCEA DIDEVICEINSTANCE;
typedef LPCDIDEVICEINSTANCE_DX5A LPCDIDEVICEINSTANCE_DX5;
#endif // UNICODE
typedef const DIDEVICEINSTANCE  *LPCDIDEVICEINSTANCE_DX5;

BOOL static __inline
IsValidSizeDIDEVICEINSTANCEW(DWORD cb)
{
    return cb == sizeof(DIDEVICEINSTANCE_DX5W) ||
           cb == sizeof(DIDEVICEINSTANCE_DX3W);
}

BOOL static __inline
IsValidSizeDIDEVICEINSTANCEA(DWORD cb)
{
    return cb == sizeof(DIDEVICEINSTANCE_DX5A) ||
           cb == sizeof(DIDEVICEINSTANCE_DX3A);
}

#define INITINSTANCEGUID(guidInstance, bDeviceId)\
{\
	(guidInstance).Data1 = (DWORD)bDeviceId;\
	(guidInstance).Data2 = 0;\
	(guidInstance).Data3 = 0;\
	*((PLONGLONG)&(guidInstance).Data4) = 0;\
}

#define IS_VALID_GUID_INSTANCE(guidInstance)\
	(((256 > (guidInstance).Data1)&&\
    (0 == (guidInstance).Data2)&&\
	(0 == (guidInstance).Data3)&&\
	(0 == *((PLONGLONG)&(guidInstance).Data4))) ? TRUE : FALSE)

#define DEVICEIDFROMGUID(guidInstance) ((BYTE)((guidInstance).Data1))

#define DIRCP_MODAL         0x00000001  //
#define DIRCP_VALID         0x00000000  //
#define DISFFC_NULL             0x00000000
#define DISFFC_VALID            0x0000003F
#define DISFFC_FORCERESET       0x80000000
#define DIGFFS_RANDOM           0x40000000
#define DISDD_VALID             0x00000001
#define DIECEFL_VALID       0x00000000
#define DIFEF_ENUMVALID             0x00000011
#define DIFEF_WRITEVALID            0x00000001
#if DIRECTINPUT_VERSION >= 0x0700           //
#define DIMOUSESTATE_INT DIMOUSESTATE2      //
#define LPDIMOUSESTATE_INT LPDIMOUSESTATE2  //
#else                                       //
#define DIMOUSESTATE_INT DIMOUSESTATE       //
#define LPDIMOUSESTATE_INT LPDIMOUSESTATE   //
#endif                                      //
#define DIKBD_CKEYS         256     /* Size of buffers */       //
                                                                //
#define DIK_PRTSC           DIK_SNAPSHOT        /* Print Screen */