/*  MySlider.h
*/

#ifndef _INC_MYSLIDER
#define _INC_MYSLIDER

#define MYSLIDER_MIDIRANGE			1
#define MYSLIDER_LFORANGE			2
#define MYSLIDER_TIMECENTS			3
#define MYSLIDER_VOLUMECENTS		4
#define MYSLIDER_PITCHCENTS			5
#define MYSLIDER_PERCENT			6
#define MYSLIDER_PAN				7
#define MYSLIDER_VOLUME				8
#define MYSLIDER_LFODELAY			9
#define MYSLIDER_NOTE				10
#define MYSLIDER_GROUP				11
#define MYSLIDER_TIMESCALE			12
#define MYSLIDER_FILTERPITCHCENTS	13
#define MYSLIDER_FILTERGAIN			14
#define MYSLIDER_FILTERABSPITCH		15

/*--------------------------------------------------------------------------
MySlider

Creates a slider with optional edit and spinner controls, all linked together.

Slider: control must be in dialog resource, specify ID in Init call

Edit:
- control must be in dialog resource, specify DisplayID in Init call
- call HandleDisplayMessage unless using MySliderCollection

Spinner:
- resource (not control) must be in resource (VC: View\Resource Symbols\New)
- specify SpinnerID in Init call
- call HandleSpinnerMessage unless using MySliderCollection
- watch the edit size, it usually needs to be at least 35 pixels wide
--------------------------------------------------------------------------*/
class MySlider {
public:
    MySlider();
    ~MySlider();
    
    void            SetValue(CWnd *pWnd, long lData);
    BOOL			Init(CWnd *pWnd, UINT nID, UINT nDisplayID, DWORD dwType, long * plData, UINT nSpinnerID = 0);
    bool			Init(CWnd *pWnd, UINT nID, UINT nDisplayID, UINT nSpinnerID, DWORD dwType, long lDefaultValue, UINT uStrRes, long * plData);
    long            SetPosition(CWnd *pWnd, UINT nSBCode, UINT nPos, bool bUpdateDisplay = true);
    void            Update(CWnd *pWnd, bool bUpdateDisplay = true);
	CString			GetPanString(float fPan);
	void			SetDataValueToEdit(long * plData) { m_plData = plData; }

	/* sets the slider position to undetermined */
	void SetUndetermined(CWnd *pWnd);
	
	/* sets the control to the display value and returns new position. If bUpdateDisplay is true, recomputes display text using new position. */
    long SetPositionFromDisplay(CWnd *pWnd, bool bUpdateDisplay);

	/* enables or disables control and display */
	void EnableControl(CWnd *pWnd, bool bEnable);

	/* handles edit notifications to update position */
	void HandleDisplayMessage(CWnd *pWnd, WORD wMsg);

	/* handles spinner notifications to update position */
	void HandleSpinnerMessage(CWnd *pWnd, NMHDR* pNMHDR);

	/* accessors */
	long GetPosition() { return m_lPosition; }
	long GetValue() { return Position2Value(m_lPosition); }
	bool GetFocus() { return m_bFocus; }
	UINT SliderID() { return m_nID; }
	UINT DisplayID() { return m_nDisplayID; }
	UINT SpinnerID() { return m_nSpinnerID; }
	long GetDefaultValue() { return m_lDefaultValue; }
	UINT GetUndoStringID() { return m_uStrRes; }

	/* returns the position corresponding to the value */
	long Value2Position(long lValue);

	/* computes new position given slider change */
	long ComputeNewPositionFromSlider(UINT nSBCode, UINT nPos);
	
private:
	/* returns the value corresponding to the position */
	long Position2Value(long lPosition);

    long            m_lPosition;    // Slider position.
    long*			m_plData;       // Converted data in native format.
    long            m_lBase;        // Base for conversion.
    long            m_lRange;       // Range for slider and spinner
    long            m_lPageSize;    // For slider and spinner
    long            m_lLineSize;	// For slider and spinner
    UINT            m_nID;          // ID for slider control.
    UINT            m_nDisplayID;   // ID for display control.
    DWORD           m_dwType;       // Type of data managed by slider.
    bool			m_bFocus;		// has the focus and accepts display updates
    bool			m_bDisplayEdited; // true if display contents were edited by the user
    bool			m_bUndetermined;	// slider value should be displayed as "----"
	CSpinButtonCtrl *m_pSpinButtonCtrl; // spinner control, if any
	UINT			m_nSpinnerID; // ID for spinner control
	UINT			m_uStrRes;		// ID for undo string
	long			m_lDefaultValue; // default value for Ctrl+click
};


#define dwmscupdfStart		0x1
#define dwmscupdfEnd		0x2

/*--------------------------------------------------------------------------
CSliderCollection

Holds a collection of MySlider objects, and simplifies message processing for host
dialogs. To insert a MySlider control in your dialog:
- make your dialog inherit from CSliderCollection
- instead of calling MySlider.Init, call CSliderCollection.Insert
- add a OnNotify handler to your dialog (ClassWizard), and call OnNotify
- add a OnCommand handler to your dialog (ClassWizard), and call OnCommand
- add a OnHScroll handler to your dialog (ClassWizard), and call OnHScroll
- override the OnSliderUpdate method to process start/end of updates
See WavePropPg.* for a property page example
See PitchDialog.* for a dialog example
--------------------------------------------------------------------------*/
class CSliderCollection {
public:
	CSliderCollection(WORD cMaxSliders);
	~CSliderCollection();

	// this must be called before any other method, and succeed
	void Init(CWnd *pWnd);
	
	/* creates a new slider/edit/spinner control. Returns pointer to spinner control if successful. */
    MySlider *Insert(UINT nID, UINT nDisplayID, UINT nSpinnerID, DWORD dwType,
		long lDefaultValue, UINT uStrRes, long * plData);

	/* handles onCommand messages (for edit controls). If the message was processed, returns true,
		and fills *plResult with the message return value. */
	bool OnCommand(WPARAM wParam, LPARAM lParam, LRESULT *plResult);

	/* handles onNotify messages (for spinner controls). If the message was processed, returns true,
		and fills *plResult with the message return value. */
	bool OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *plResult);

	/* handles OnHScroll messages (for sliders). Returns true if message was handled. Calls
		OnSaveUndoState and OnEndUpdate as necessary. */
	bool OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	// free all contained controls (call on property page switch, when spinners need to be recreated)
	void Free();

	/* called when parameter values start or end changing. Return true on success. This method
		is intended for override.
			dwmscupdfStart: update starts. If you return false, SliderCollection will assume that
							you reset the state, and will call Start again on next update.
			dwmscupdfEnd: update ends. You can do expensive operations there, like commit &
							download. Return value is ignored. */
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);

private:
	CWnd*		m_pWnd;				// cache the dialog window
	WORD		m_cSliders;			// number of slider controls in m_rgpSliders
	WORD		m_cMaxSliders;		// maximum number of sliders
	MySlider*	m_prgSliders;		// array of sliders
	MySlider*	m_pmsFocus;			// slider in array which has focus, NULL if none
	bool		m_bSaveUndo;		// whether to call undo
};


/*--------------------------------------------------------------------------
Utilities
--------------------------------------------------------------------------*/
void notetostring(DWORD note, char *string);
short stringtonote(char *string);

#endif

