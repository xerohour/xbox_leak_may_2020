#ifndef _ENC_H_
#define _ENC_H_

#if !defined(PROXY_IMPL)
#define PROXY_IMPORT_EXPORT   __declspec(dllimport)
#else
#define PROXY_IMPORT_EXPORT   __declspec(dllexport)
#endif

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;

#endif // !_HRESULT_DEFINED

// REVIEW: defined some random facility
#define FACILITY_ENC 101    
#define E_MEMMAP        MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 1 )
#define E_INITMEMALLOC  MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 2 )
#define E_EXPORTPUB     MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 3 )
#define E_FIXUP         MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 4 )
#define E_MEMWRITE      MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 5 )
#define E_STACKWRITE    MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 6 )
#define E_OBJOPEN       MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 7 )
#define E_THUNKWRITE    MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 8 )
#define E_NOPDBSUPPORT  MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 9 )
#define E_TYPEPACKFAILED MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 10 )

//
//  Stack editing error code are between FIRST_SE_ERROR and LAST_SE_ERROR
//
#define E_SPNOTCANONICAL	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 11 )

#define FIRST_SE_ERROR E_SPNOTCANONICAL

#define E_EXCEPTIONSTATE	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 12 )
#define E_LOCALVARCHANGE	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 13 )
#define E_IPNOTINEDIT		MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 14 )
#define E_FUNCSIGCHANGE     MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 15 )
#define E_FRAMESIZECHANGE   MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 16 )
#define E_LOCALREQCONDES    MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 17 )
#define E_UNSAFEFRAMECOND   MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 18 )
#define E_CANTFINDLINE      MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 19 )
#define E_DUPLICATEVAR      MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 20 )
#define E_FUNCBYTEMATCH     MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 21 )
#define E_REGISTERSPILL		MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 22 )

#define LAST_SE_ERROR E_REGISTERSPILL

#define E_NAMETYPECHANGE    MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 30 )
#define E_MISSINGSYMBOL     MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 31 )
#define E_IMAGEBASE         MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ENC, 32 )

//
// PAY ATTENTION: have you updated LAST_SE_ERROR!
//

#define S_IPPOSCHANGE		MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_ENC, 40 )
#define S_IPPOSNOCHANGE		MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_ENC, 41 )
#define S_IPPOSCHANGEEH	    MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_ENC, 42 )
#define S_NOCODECHANGE      MAKE_HRESULT( SEVERITY_SUCCESS, FACILITY_ENC, 43 )

interface Sapi;

namespace enc {

    interface EditHelper;
    interface ImageEdit;

    interface EditHelper
    {
        virtual bool ReadImage(     // read bytes from the image
            PADDR paddr, 
            ULONG cb, 
            BYTE* pb ) PURE;
        virtual bool WriteImage(    // write bytes to the image
            PADDR paddr, 
            ULONG cb, 
            BYTE* pb ) PURE;
        virtual bool FixupAddr(         // convert seg:off to flat addr
            PADDR paddr ) PURE;
        virtual bool UnFixupAddr(       // convert flat addr to seg:off
            PADDR paddr ) PURE;
        virtual DWORD ImageBase(
            HANDLE hexe ) PURE;
/*
        virtual bool GetRegDesc(      // calls into OSD
            HPID hpid,
            HTID htid,
            DWORD ird,
            LPRD lprd
            ) PURE;
        virtual bool ReadRegister(    // calls into OSD
            HPID hpid,
            HTID htid,
            DWORD dwid,
            LPVOID lpValue
            ) PURE;

        virtual bool WriteRegister(    // calls into OSD
            HPID hpid,
            HTID htid,
            DWORD dwId,
            LPVOID lpValue
            ) PURE;
*/
        virtual void Trace( int level, const char* ) PURE;

        virtual bool FindImage(   // return full path and base addr for named DLL
            const char* name, 
            char fullPath[ _MAX_PATH ],    // out
            PADDR paddrBase ) PURE;              // out
    };

	struct MemUpdate {
		ADDR		 addr;
		RefPtr< SimpleArray< BYTE > > data;
	};
	typedef SimpleArray< MemUpdate > MemUpList;
	typedef RefPtr< MemUpList > RefMUList;

    interface EnumLink: public Enum {
        virtual void get( LPCSTR* pszExePath, 
			LPCSTR* pszCwd,
			LPCSTR* pszCmd,
			LPCSTR* pszOutFile ) pure;
    };

    interface Enc
    {
        PROXY_IMPORT_EXPORT 
            static Enc* theEnc();

        virtual HRESULT FInit( const char* szLogFile, EditHelper* phelper, ISymbolHandler* psapi ) PURE;

        virtual HRESULT pushNewEdit() PURE;
        virtual HRESULT InsertObject( HANDLE hmod, const char *szObjPath ) PURE;
        virtual HRESULT ApplyCurrentEdit() PURE;

        virtual HRESULT PopEdit() PURE;
        virtual void Close() PURE;

        virtual ImageEdit* currentEdit() PURE;
        static BOOL PASCAL PDBOpenValidateEx(char* szPDB, char* szPathOrig,
                char* szSearchPath, char* szMode,   ULONG sig, ULONG age,
                OUT EC* pec, OUT char szError[cbErrMax], OUT PDB** pppdb);
        PROXY_IMPORT_EXPORT 
        static BOOL PDBOpen(char* szPDB, char* szMode, ULONG sigInitial,
                    OUT EC* pec, OUT char szError[cbErrMax], OUT PDB** pppdb);

		//
		// methods to support stack frame editing
		//
		virtual bool FEditReplacesAddr( const PADDR paIp, bool fTop ) PURE;
		virtual HRESULT UpdateIpToEditIp( 
            PADDR   paIp,         // [IN, OUT] current IP
            PADDR   paBp,         // [IN]
            PADDR   paSp,         // [IN]
            PADDR   paEH,         // [IN, OUT]
            bool    fTos,		  // true if this frame is at top of stack
            ULONG linenumHint,    // if non-zero then probably the right source line after the edit
			RefMUList& rlist) PURE;
        //
        // enum for each exe touched by the current set of edits
        //
        virtual bool getEnumLink( EnumLink** ppenum ) PURE;
        virtual bool isEditedObject( HANDLE hmod ) PURE;
        virtual HRESULT ReloadImage( HANDLE hexe ) PURE;
        virtual bool isEditedImage( HANDLE hexe ) PURE;
    };

};

#endif
