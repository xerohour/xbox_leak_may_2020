#ifndef __REGION_H__
#define __REGION_H__


#define KEYBOARD_SIZE		127		// Assuming a 128 key keyboard
#define WHITE_KEY_WIDTH		8		// Width of the white key in pixels
#define BLACK_KEY_WIDTH		4		// Width of a black key in pixels
#define REGION_LAYER_HEIGHT	10		// Height of a region layer
#define DRAG_ZONE_WIDTH		3		// Drag zone for region boundaries

#define OCTAVE_WIDTH		WHITE_KEY_WIDTH * 7 // Seven white keys make up an octave

// Forward declaration 
class CRegionKeyboardCtrl;

class CRegion
{

public:
	CRegion(int nLayer = 0, 
			int nStartNote = 0, int nEndnote = KEYBOARD_SIZE,	// Spans the whole keyboard range by default
			int nStartVelocity = 0, int nEndVelocity = 127,		// Spans the entire velocity range by default
			int nRootnote = 60,									// Root note is C5 by default
			const CString& strWaveName = "");

// Implementation
public:
	
	void SetParentControl(CRegionKeyboardCtrl* pParent);

	void SetLayer(int nLayer);
	int GetLayer();
	
	void SetRootNote(int nNote);
	int	 GetRootNote();

	int GetStartNote();
	int	GetEndNote();

	void SetStartNote(int nNote);
	void SetEndNote(int nNote);

	void GetNoteRange(int& nStartNote, int& nEndNote);
	void SetNoteRange(int nStartnote = 0, int nEndNote = KEYBOARD_SIZE);

	void GetVelocityRange(int& nStartVelocity, int& nEndVelocity);
	void SetVelocityRange(int nStartVelocity = 0, int nEndVelocity = 127);

	void SetWaveName(LPCTSTR pszWaveName);
	
	void Draw(CDC* pDC, int nLayerAreaHeight, int nFirstVisibleLayer);
	void SetBackgroundColor(COLORREF colorBkgnd = RGB(255, 255, 255,)); // Default background is white
	void SetRegionColor(COLORREF colorRegion = RGB(0, 0, 0,));			// Default region is painted black

	BOOL IsPointInRegion(CPoint point);					// Returns TRUE if the point lies in a region
	BOOL IsPointInRegion(CPoint point, bool bIgnoreY);	// Returns TRUE if the point lies in a region; considers only the X-Co-ordinates if bIgnoreY is TRUE
	BOOL IsPointOnStartBoundary(CPoint point);			// Returns TRUE if the point lies around the start boundary of this region
	BOOL IsPointOnEndBoundary(CPoint point);			// Returns TRUE if the point lies around the end boundary of this region 
	
	BOOL IsNoteInRegion(int nNote);						// Returns TRUE if it finds region that has this note; does not check for the layer
	BOOL IsNoteInRegion(int nLayer, int nNote);			// Retruns TRUE if it finds a region in the passed layer that contains this note
	BOOL IsNoteInRegion(int nLayer, int nNote,				// Retruns TRUE if it finds a region in the passed layer that contains this note
						int nVelocity, bool bIgnoreLayer); // and is within the velocity range
	
	BOOL IsOverlappingRegion(int nLayer, int nStartnote, int nEndNote); // Returns TRUE if it's overlapping the region between the start and end notes

	BOOL IsSelected();
	void SetSelected(BOOL bSelected);

	void Enable(BOOL bEnable);
	void SetPlayState(BOOL bOn);

	CRect GetRect();
	
// Implementation
private: 
	int GetPointFromNote(bool bStartPoint, int nNote);
	void ValidateAndSetRanges();

private:
	
	CRegionKeyboardCtrl* m_pKeyboard;	

	int		m_nStartNote;
	int		m_nEndNote;
	int		m_nStartVelocity;
	int		m_nEndVelocity;
	int		m_nRootNote;
	int		m_nLayer;

	BOOL	m_bSelected;
	BOOL	m_bEnabled;
	BOOL	m_bPlaying;

	CRect	m_RegionRect;

	COLORREF	m_DrawColor;
	COLORREF	m_Layer0DrawColor;
	COLORREF	m_NoteOnColor;
	COLORREF	m_BkColor;


	static int m_arrKeyboardMap[12];	// Map of the keys for an Octave
	static int m_arrKeySizes[2];		// sizes for the black and white keys

	CString m_strWaveName;				// wave name

};



#endif //__REGION_H__

	
