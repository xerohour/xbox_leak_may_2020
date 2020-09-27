/*************************************************************************
 	msodebug.h

 	Owner: rickp
 	Copyright (c) 1994 Microsoft Corporation

	Standard debugging definitions for the shared Office libraries.
	Includes asserts, tracing, and other cool stuff like that.
*************************************************************************/

#if !defined(CMDEBUG_H)
#define CMDEBUG_H

#if !defined(CMSTD_H)
#include <cmstd.h>
#endif

#if MAC && !defined(__TYPES__)
#include <macos\types.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/*************************************************************************
	Random useful macros
*************************************************************************/

#if DEBUG
	#define Debug(e) e
	#define DebugOnly(e) e
	#define DebugElse(s, t)	s
#else
	#define Debug(e)
	#define DebugOnly(e)
	#define DebugElse(s, t) t
#endif


/****************************************************************************
   This enum contains the Debug "Messages" that are sent to the FDebugMessage
   Method
 ****************************************************************** JIMMUR **/
enum
{
	msodmWriteBe = 1, /* write out the BE record for this object */

	/* Here begin drawing only debug messages */
	msodmDgvRcvOfHsp = 2001,
		/* Ask a DGV for the bounding rectangle (if any) of an HSP.
			Assumes lParam is really a pointer to an MSODGDB, looks at the
			hsp field thereof and fills out the rcv field. */
	msodmDgsWriteBePvAnchor,
		/* Write out the BE record for a host allocated pvAnchor. */
	msodmDgsWriteBePvClient,
		/* Write out the BE record for any host allocated client data. */
	msodmDgvsAfterMouseInsert,
		/* Passed to IMsoDrawingViewSite after a shape is interactively
			inserted with the mouse. lParam is really the inserted HSP. */
	msodmDgvsAfterMarquee,
		/* Passed to IMsoDrawingViewSite after one drags out a rectangle
			with the pointer tool selecting zero or more shapes. */
	msodmIsNotMso96,
		/* Returns FALSE if the specified object is implemented by MSO96.DLL.
			Allows sleazy up-casts, for example, from IMsoDrawingView *
			to DGV *. */
	msodmGetHdesShape,
		/* Ask a DGVs for its m_hdesShape (in *(MSOHDES *)lParam).  Returns
			FALSE if it filled out an HDES. */
	msodmGetHdesSelection,
		/* Ask a DGVs for its m_hdesSelection (in *(MSOHDES *)lParam).
			Returns FALSE if it filled out an HDES. */
	msodmDguiWriteBeForDgc,
		/* Ask a DGUI to write BEs for a DGC it allocated. */
	msodmDgsWriteBeTxid,
		/* Write out the BE record for the attached text of a shape. */
	msodmDgsWriteBePvAnchorUndo,
		/* Write out the BE record for a host anchor in the undo stack. */
	msodmDgvsDragDrop,
		/* Let the host know that I just did a drag-drop from this window. */
};

enum
{
   msodmbtDoNotWriteObj = 0,    // Do Not write out the object 
   msodmbtWriteObj,             // Do write out the object and
                                    // embedded pointers
};


enum
{
	msocchBt = 20,						// Maximum size of a Bt description String
};

/* Some debug messages need more arguments than fit through
	the arguments to FDebugMethod.  For these there are various
	MSODMBfoo structs, usually defined near the objects they're passed
	to. */


/****************************************************************************
    Interface debug routine
 ****************************************************************** JIMMUR **/
#if DEBUG
   #define MSODEBUGMETHOD  MSOMETHOD_(BOOL, FDebugMessage) (THIS_ HMSOINST hinst, \
         UINT message, WPARAM wParam, LPARAM lParam) PURE;

   #define MSODEBUGMETHODIMP MSOMETHODIMP_(BOOL) FDebugMessage (HMSOINST hinst, \
         UINT message, WPARAM wParam, LPARAM lParam); \
         static BOOL FCheckObject(LPVOID pv, int cb);
			
	#define MSOMACDEBUGMETHODIMP MSOMACPUB	 MSOMETHODIMP_(BOOL) FDebugMessage (HMSOINST hinst, \
         UINT message, WPARAM wParam, LPARAM lParam); \
         static BOOL FCheckObject(LPVOID pv, int cb);

   #define DEBUGMETHOD(cn,bt) STDMETHODIMP_(BOOL) cn::FDebugMessage \
         (HMSOINST hinst, UINT message, WPARAM wParam, LPARAM lParam) \
         { \
            if (msodmWriteBE == message) \
               {  \
                  return MsoFSaveBe(hinst,lParam,(void*)this,sizeof(cn),bt); \
               } \
            return FALSE; \
         }
 #else
   #define MSODEBUGMETHOD  MSOMETHOD_(BOOL, FDebugMessage) (THIS_ HMSOINST hinst, \
         UINT message, WPARAM wParam, LPARAM lParam) PURE;

   #define MSODEBUGMETHODIMP MSOMETHODIMP_(BOOL) FDebugMessage (HMSOINST hinst, \
         UINT message, WPARAM wParam, LPARAM lParam);
			
   #define MSOMACDEBUGMETHODIMP MSOMACPUB MSOMETHODIMP_(BOOL) FDebugMessage (HMSOINST hinst, \
         UINT message, WPARAM wParam, LPARAM lParam);
	
   #define DEBUGMETHOD(cn,bt)  STDMETHODIMP_(BOOL) cn::FDebugMessage (HMSOINST, \
         UINT, WPARAM, LPARAM) { return TRUE; }
#endif



/*************************************************************************
	Enabling/disabling debug options
*************************************************************************/

enum
{
	msodcAsserts = 0,	/* asserts enabled */
	msodcPushAsserts = 1, /* push asserts enabled */
	msodcMemoryFill = 2,	/* memory fills enabled */
	msodcMemoryFillCheck = 3,	/* check memory fills */
	msodcTrace = 4,	/* trace output */
	msodcHeap = 5,	/* heap checking */
	msodcMemLeakCheck = 6,
	msodcMemTrace = 7,	/* memory allocation trace */
	msodcGdiNoBatch = 8,	/* don't batch GDI calls */
	msodcShakeMem = 9,	/* shake memory on allocations */
	msodcReports = 10,	/* report output enabled */
	msodcMsgTrace = 11,	/* WLM message trace - MAC only */
	msodcWlmValidate = 12,	/* WLM parameter validation - MAC only */
	msodcGdiNoExcep = 13,  /* Don't call GetObjectType for debug */
	msodcDisplaySlowTests = 14, /* Do slow (O(n^2) and worse) Drawing debug checks */
	msodcDisplayAbortOften = 15, /* Check for aborting redraw really often. */
	msodcDisplayAbortNever = 16, /* Don't abort redraw */
	msodcPurgedMaxSmall = 17,
	msodcSpare18 = 18, /* USE ME */
	msodcSpare19 = 19, /* USE ME */
	msodcSpare20 = 20, /* USE ME */
	msodcSpare21 = 21, /* USE ME */
	msodcSpare22 = 22, /* USE ME */
	msodcMax = 23,
};


#if DEBUG
	#define MsoFGetDebugCheck(dc)	(TRUE)
#else
	#define MsoFGetDebugCheck(dc)	(FALSE)
#endif

/*************************************************************************
	Debugger breaks
*************************************************************************/

/* Breaks into the debugger.  Works (more or less) on all supported
 	systems. */
#if X86
	#define MsoDebugBreakInline() {__asm int 3}
#elif MAC
	#define MsoDebugBreakInline() Debugger()
#else
	#define MsoDebugBreakInline() \
		{ \
		__try { DebugBreak(); } \
		__except(EXCEPTION_EXECUTE_HANDLER) { OutputDebugStringA("DebugBreak"); } \
		}
#endif

/*	A version of debug break that you can actually call, instead of the
	above inline weirdness we use in most cases.  Can therefore be used in
	expressions. Returns 0 */
#if DEBUG
	MSOAPI_(int) MsoDebugBreak(void);
#else
	#define MsoDebugBreak() (0)
#endif


/*************************************************************************
	Assertion failures
*************************************************************************/

#if !defined(MSO_NO_ASSERTS)

/*	Displays the assert message, including flushing any assert stack.
	szFile and li are the filename and line number of the failure,
	and szMsg is an optional message to display with the assert.
	Returns FALSE if the caller should break into the debugger. */
#if DEBUG
	MSOAPI_(BOOL) MsoFAssert(const CHAR* szFile, int li, const CHAR* szMsg);
#else
	#define MsoFAssert(szFile, li, szMsg) (TRUE)
#endif

/*	Same as MsoFAssert above, except an optional title string can be
	displayed. */
#if DEBUG
	MSOAPI_(BOOL) MsoFAssertTitle(const CHAR* szTitle,
			const CHAR* szFile, int li, const CHAR* szMsg);
#else
	#define MsoFAssertTitle(szTitle, szFile, li, szMsg) (TRUE)
#endif

/*	Same as MsoFAssertTitle above, except you can pass in your own 
	MessageBox flags */
#if DEBUG
	MSOAPIXX_(BOOL) MsoFAssertTitleMb(const CHAR* szTitle,
			const CHAR* szFile, int li, const CHAR* szMsg, UINT mb);
#else
	#define MsoFAssertTitleMb(szTitle, szFile, li, szMsg, mb) (TRUE)
#endif

/*	Same as MsoFAssertTitleMb above, except you can pass in your own 
	assert output type */
#if DEBUG
	MSOAPI_(BOOL) MsoFAssertTitleAsoMb(int iaso, const CHAR* szTitle,
			const CHAR* szFile, int li, const CHAR* szMsg, UINT mb);
#else
	#define MsoFAssertTitleAsoMb(iaso, szTitle, szFile, li, szMsg, mb) (TRUE)
#endif

/*	To save space used by debug strings, you must declare the assert data
	in any file that uses asserts */
#if DEBUG
	#define AssertData static const CHAR vszAssertFile[] = __FILE__;
#else
	#define AssertData
#endif



/*	The actual guts of the assert.  if the flag f is FALSE, then we kick
	of the assertion failure, displaying the optional message szMsg along
	with the filename and line number of the failure */
#if !DEBUG
	#define AssertMsg(f, szMsg)
	#define AssertMsgInline(f, szMsg)
	#define AssertMsgTemplate(f, szMsg)
#else
	#define AssertMsg(f, szMsg) \
		do { \
		if (!(f) && \
				!MsoFAssert(vszAssertFile, __LINE__, (const CHAR*)(szMsg))) \
			MsoDebugBreakInline(); \
		} while (0)
	#define AssertMsgInline(f, szMsg) \
		do { \
		if (!(f) && \
				!MsoFAssert(__FILE__, __LINE__, (const CHAR*)(szMsg))) \
			MsoDebugBreakInline(); \
		} while (0)
	// Template inlines don't like the inline __asm for some reason
	#define AssertMsgTemplate(f, szMsg) \
		do { \
		if (!(f) && \
				!MsoFAssert(__FILE__, __LINE__, (const CHAR*)(szMsg))) \
			MsoDebugBreak(); \
		} while (0)
#endif


/*	Tells if the Office is currently displaying an alert message box */
#if !DEBUG
	#define MsoFInAssert() (FALSE)
#else
	MSOAPI_(BOOL) MsoFInAssert(void);
#endif

/*	Random compatability versions of the assert macros */

#define VSZASSERT AssertData
#if MSO_ASSERT_EXP
	#define Assert(f) AssertMsg((f), #f)
	#define AssertInline(f) AssertMsgInline((f), #f)
	#define AssertTemplate(f) AssertMsgTemplate((f), #f)
#else
	#define Assert(f) AssertMsg((f), NULL)
	#define AssertInline(f) AssertMsgInline((f), NULL)
	#define AssertTemplate(f) AssertMsgTemplate((f), NULL)
#endif
#define AssertExp(f) AssertMsg((f), #f)
#define AssertSz(f, szMsg) AssertMsg((f), szMsg)


/*************************************************************************
	Verification failures
*************************************************************************/

/*	Like Assert, except the test is still made in the ship version
 	of the application.  Especially useful for tests that have
 	required side-effects. */

#if !DEBUG
	#define VerifyMsg(f, szMsg) (f)
	#define VerifyMsgInline(f, szMsg) (f)
#else
	#define VerifyMsg(f, szMsg) \
		do { \
		if (!(f) && \
				!MsoFAssert(vszAssertFile, __LINE__, (const CHAR*)(szMsg))) \
			MsoDebugBreakInline(); \
		} while (0)
	#define VerifyMsgInline(f, szMsg) \
		do { \
		if (!(f) && \
				!MsoFAssert(__FILE__, __LINE__, (const CHAR*)(szMsg))) \
			MsoDebugBreakInline(); \
		} while (0)
#endif

#if MSO_ASSERT_EXP
	#define Verify(f) VerifyMsg((f), #f)
#else
	#define Verify(f) VerifyMsg((f), NULL)
#endif	
#define VerifyExp(f) VerifyMsg((f), #f)
#define SideAssert(f) Verify(f)
#define AssertDo(f) Verify(f)

/*************************************************************************
	Untested notifications
*************************************************************************/

#if !DEBUG
	#define UntestedMsg(szMsg)
#else
	#define UntestedMsg(szMsg) \
		do { \
		if ( \
				!MsoFAssertTitle("Untested", vszAssertFile, __LINE__, (const CHAR*)(szMsg))) \
			MsoDebugBreakInline(); \
		} while (0)
#endif

#define Untested() UntestedMsg(NULL)


/*************************************************************************
	Unreached notifications
*************************************************************************/

#if !DEBUG
	#define UnreachedMsg(szMsg)
#else
	#define UnreachedMsg(szMsg) \
		do { \
		if ( \
				!MsoFAssertTitle("Unreached", vszAssertFile, __LINE__, (const CHAR*)(szMsg))) \
			MsoDebugBreakInline(); \
		} while (0)
#endif

#define Unreached() UnreachedMsg(NULL)


/*************************************************************************
	Scratch GDI Objects
*************************************************************************/

/*	Routines to ensure only single access to global scratch GDI objects */

#if !DEBUG

	#define MsoUseScratchObj(hobj, szObjName)
	#define MsoReleaseScratchObj(hobj, szObjName)
	#define UseScratchDC(hdc)
	#define ReleaseScratchDC(hdc)
	#define UseScratchRgn(hrgn)
	#define ReleaseScratchRgn(hrgn)

#else

	/* mask that contains unused bits in the handle */
	#if MAC
		#define msohInUse (0x00000003)
		#define MsoFObjInUse(hobj) (((int)(hobj)&msohInUse)==0)
	#else
		#define msohInUse (0xffffffff)
		// REVIEW: any handle bits we can rely on to make this test more correct?
		#define MsoFObjInUse(hobj) (GetObjectType(hobj) != 0)
	#endif

	#define MsoUseScratchObj(hobj, szObjName) \
			do { \
			if (MsoFObjInUse(hobj) && \
					!MsoFAssert(vszAssertFile, __LINE__, "Scratch " szObjName " " #hobj " already in use")) \
				MsoDebugBreakInline(); \
			*(int*)&(hobj) ^= msohInUse; \
			} while (0)

	#define MsoReleaseScratchObj(hobj, szObjName) \
			do { \
			if (!MsoFObjInUse(hobj) && \
					!MsoFAssert(vszAssertFile, __LINE__, "Scratch " szObjName " " #hobj " not in use")) \
				MsoDebugBreakInline(); \
			*(int*)&(hobj) ^= msohInUse; \
			} while (0)

	#define UseScratchDC(hdc) MsoUseScratchObj(hdc, "DC")
	#define ReleaseScratchDC(hdc) MsoReleaseScratchObj(hdc, "DC")
	#define UseScratchRgn(hrgn) MsoUseScratchObj(hrgn, "region")
	#define ReleaseScratchRgn(hrgn) MsoReleaseScratchObj(hrgn, "region")

#endif


/*************************************************************************
	Reports
*************************************************************************/

#else // MSO_NO_ASSERTS

	/* When MSO_NO_ASSERTS is defined, we still need to define something for
		these special ones because they can be used inside our headers. */
	#define AssertMsgInline(f, szMsg)
	#define AssertMsgTemplate(f, szMsg)

#endif // MSO_NO_ASSERTS


/*************************************************************************
	Tracing
*************************************************************************/


/*************************************************************************
	Debug fills
*************************************************************************/

enum
{
	msomfSentinel,	/* sentinel fill value */
	msomfFree,	/* free fill value */
	msomfNew,	/* new fill value */
	msomfMax
};

/*	Fills the memory pointed to by pv with the fill value lFill.  The
	area is assumed to be cb bytes in length.  Does nothing in the
	non-debug build */
#if DEBUG
	MSOAPIXX_(void) MsoDebugFillValue(void* pv, int cb, DWORD lFill);
#else
	#define MsoDebugFillValue(pv, cb, lFill)
#endif

/*	In the debug version, used to fill the area of memory pointed to by
	pv with a the standard fill value specified by mf.  The memory is 
	assumed to be cb bytes long. */
#if DEBUG
	MSOAPI_(void) MsoDebugFill(void* pv, int cb, int mf);
#else
	#define MsoDebugFill(pv, cb, mf)
#endif

/*	Checks the area given by pv and cb are filled with the debug fill
	value lFill. */
#if DEBUG
	MSOAPIXX_(BOOL) MsoFCheckDebugFillValue(void* pv, int cb, DWORD lFill);
#else
	#define MsoFCheckDebugFillValue(pv, cb, lFill) (TRUE)
#endif

/*	Checks the area given by pv and cb are filled with the debug fill
	of type mf. */
#if DEBUG
	MSOAPIXX_(BOOL) MsoFCheckDebugFill(void* pv, int cb, int mf);
#else
	#define MsoFCheckDebugFill(pv, cb, mf) (TRUE)
#endif

/* Returns the fill value corresponding to the given fill value type mf. */
#if DEBUG
	MSOAPI_(DWORD) MsoLGetDebugFillValue(int mf);
#else
	#define MsoLGetDebugFillValue(mf) ((DWORD)0)
#endif

/*	Sets the fill value to lFill for the given memory fill type mf.
	Returns the previous fill value. */
#if DEBUG
	MSOAPI_(DWORD) MsoSetDebugFillValue(int mf, DWORD lFill);
#else
	#define MsoSetDebugFillValue(mf, lFill) ((DWORD)0)
#endif

#define MsoDebugFillLocal(l) MsoDebugFill(&(l), sizeof(l), msomfFree)

/*************************************************************************
	Debug APIs
*************************************************************************/

/*************************************************************************
	Standard debugging UI for controlling Office debug options from
	within the app.
*************************************************************************/

/*************************************************************************
	Debug Monitors
*************************************************************************/

#if defined(__cplusplus)
}
#endif


#endif // CMDEBUG_H
