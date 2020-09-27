#include "cl.h"

class CCallsView : public CMultiEdit {
	DECLARE_DYNCREATE(CCallsView)

	private:
		DLA 		m_dla;
		int			m_radixLast;
		BOOL		m_fShowBlank;
		UINT		m_WidgetFlags;

		BOOL		DispCallInfo( int );
		void		DumpHfme( HFME, int, WORD, char * );
		
	public:
		/* default constructor/destructor */
		CCallsView();
		virtual ~CCallsView();

		virtual DWORD GetHelpID() { return HID_WND_CALL_STACK; }

		virtual void ShowContextPopupMenu(CPoint pt);

		WORD 		CbGetLineBuf( DWORD, WORD, char *, PDLA &);
		DWORD		CLinesInBuf( void );
		void		DrawLineGraphics( CDC *, RECT &, DWORD );

		LRESULT	LTextProc( UINT, WPARAM, LPARAM );

		virtual BOOL FCanUseMarginSelect( void ) { return TRUE; }

	private:
        friend class CDebugPackage;

		// g_cRef is the count of instantiations of CTextView, used to
		// initialize g_pFontInfo which is the pointer to the CFontInfo
		// for this view type
		static UINT g_cRef;
		// Pointer to the font information for this view type
		static CFontInfo *g_pFontInfo;
};
