#ifndef __WIZBARAPI_H__
#define __WIZBARAPI_H__

#include "parse.h"  // for IINST mostly

interface ILanguageService;		// #include <wizapi.h> if you actually need to work with this

interface IWizardBar;
interface IWizBarClntEditor;
interface IWizBarClntLang;
interface IWizBarClntEditors;
interface IWizBarClntLangProvider;

typedef IWizardBar* LPWIZARDBAR;
typedef IWizBarClntEditor* LPWIZBARCLNTEDITOR;
typedef IWizBarClntLang* LPWIZBARCLNTLANG;
typedef IWizBarClntEditors* LPWIZBARCLNTEDITORS;
typedef IWizBarClntLangProvider* LPWIZBARCLNTLANGPROVIDER;

/////////////////////////////////////////////////////////////////////////////
// IWizardBar
// This interface exposes methods that the Wizard bar clients use, though ActivateWizBar 
// and WizBarInvalidate are used by "outsiders"


// structures required as passed arguments for IWizardBar and IWizardBarClient

enum WB_LIST_TYPE {WBL_INVALID = -1, WBL_CLASS = 0, WBL_OBJECT = 1, WBL_TARGET = 2}; // types of wizard bar lists
enum WB_CTL_TYPE  {WBC_INVALID = -1, WBC_CLASS = 0, WBC_OBJECT = 1, WBC_TARGET = 2, WBC_ACTION = 3}; // types of wizard bar controls

#define IW_PUBLIC          0
#define IW_PROTECTED       1
#define IW_PRIVATE         2

#define IMGLIST_BACKGROUND      0x0000ff00

enum WB_TARGET_TYPE 
{
	IW_CLASS = 0, 
	IW_CLASS_PROTECTED,
	IW_CLASS_PRIVATE,
	IW_CONSTANT, 
	IW_CONSTANT_PROTECTED,
	IW_CONSTANT_PRIVATE,
	IW_DELEGATE, 
	IW_DELEGATE_PROTECTED,
	IW_DELEGATE_PRIVATE,
	IW_ENUM, 
	IW_ENUM_PROTECTED,
	IW_ENUM_PRIVATE,
	IW_ENUMITEM, 
	IW_ENUMITEM_PROTECTED,
	IW_ENUMITEM_PRIVATE,
	IW_EVENT, 
	IW_EVENT_PROTECTED,
	IW_EVENT_PRIVATE,
	IW_EXCEPTION, 
	IW_EXCEPTION_PROTECTED,
	IW_EXCEPTION_PRIVATE,
	IW_FIELD, 
	IW_FIELD_PROTECTED,
	IW_FIELD_PRIVATE,
	IW_INTERFACE, 
	IW_INTERFACE_PROTECTED,
	IW_INTERFACE_PRIVATE,
	IW_MACRO, 
	IW_MACRO_PROTECTED,
	IW_MACRO_PRIVATE,
	IW_MAP, 
	IW_MAP_PROTECTED,
	IW_MAP_PRIVATE,
	IW_MAPITEM, 
	IW_MAPITEM_PROTECTED,
	IW_MAPITEM_PRIVATE,
	IW_FUNC, 
	IW_FUNC_PROTECTED,
	IW_FUNC_PRIVATE,
	IW_OVERLOADED,
	IW_OVERLOADED_PROTECTED,
	IW_OVERLOADED_PRIVATE,
	IW_MODULE, 
	IW_MODULE_PROTECTED,
	IW_MODULE_PRIVATE,
	IW_NAMESPACE, 
	IW_NAMESPACE_PROTECTED,
	IW_NAMESPACE_PRIVATE,
	IW_OPERATOR, 
	IW_OPERATOR_PROTECTED,
	IW_OPERATOR_PRIVATE,
	IW_PROPERTY, 
	IW_PROPERTY_PROTECTED,
	IW_PROPERTY_PRIVATE,
	IW_STRUCT, 
	IW_STRUCT_PROTECTED,
	IW_STRUCT_PRIVATE,
	IW_TEMPLATE, 
	IW_TEMPLATE_PROTECTED,
	IW_TEMPLATE_PRIVATE,
	IW_TYPEDEF, 
	IW_TYPEDEF_PROTECTED,
	IW_TYPEDEF_PRIVATE,
	IW_TYPE, 
	IW_TYPE_PROTECTED,
	IW_TYPE_PRIVATE,
	IW_UNION, 
	IW_UNION_PROTECTED,
	IW_UNION_PRIVATE,
	IW_VAR, 
	IW_VAR_PROTECTED,
	IW_VAR_PRIVATE,
	IW_VALUETYPE, 
	IW_VALUETYPE_PROTECTED,
	IW_VALUETYPE_PRIVATE,
	IW_ERROR,
	IW_CLASS_GRAY,
	IW_FUNC_PRIVATE_GRAY,
	IW_FUNC_PROTECTED_GRAY,
	IW_FUNC_GRAY,
	IW_BSCFILE,
	IW_IDL_LIBRARY,
	IW_UNKNOWN3,
	IW_UNKNOWN4,
	IW_UNKNOWN5,
	IW_UNKNOWN6,
	IW_UNKNOWN7,
	IW_UNKNOWN8,
	IW_PROJECT,
	IW_DIALOG_ID,
}; // ids of standard targets/glyphs

/*    IW_INTERFACE_DEF,
	IW_IFACE_METHOD,
	IW_IFACE_PROP,
	IW_DIALOG_ID,
	IW_FUNC_DEFAULT,
	IW_FUNC_JAVA_PROTECTED,
	IW_VAR_DEFAULT,
	IW_VAR_JAVA_PROTECTED,
*/

enum WB_ERROR_CODE
{
	WB_ERR_NO_DEF_FUNCTION = 1,
	WB_ERR_NO_NEXTPREV,
	WB_ERR_NO_DEF_CLASS
}; // ids for errors that ShowWBError can handle

const cItemContext = (WBL_TARGET - WBL_CLASS + 1);
const cWBControls =  (WBC_ACTION - WBC_CLASS + 1);

typedef struct _WBContext
{
	// an array of 3 items (class, filter, memner)
	void *rgwbcitem[cItemContext];
	BOOL fScopeIsProject;  // if true, include all project elements (e.g., classes.) if false limit to context
} WBContext; // Wizard Bar Context

// this is data gathered by an editor that does tracking. Its corresponding language/project
// knows how to interpret the data and return equivalent WBContext entries.
typedef struct _WBContextTrack
{
	// an array of 3 items (class, filter, member + 1 "extra item" for additional data)
	void *rgwbtitem[cItemContext + 1];
} WBContextTrack; // Wizard Bar Tracking Data for Context


// bit flag masks for pbfSupportsWB in FSupportsWBActivate
enum MASK_TYPES 
{
	mskSWANoSupport = 0,
	mskSWAProjectSupport = 1,
	mskSWATrackingSupport = 2,
	mskSWAFilterSupport = 4,
	mskSWAUninitialized = 8
};


// bit flag masks for GetWizBarFlags

#define mskWBFlagsNotExist 0
#define mskWBExists 1
#define mskWBEnabled 2
#define mskWBInvalid 4
#define mskWBTrackedByView 8
#define mskWBSetupTrackForCmd 16

// bit flag masks for puDrawStringFlags in DrawWBItemGlyph
// Note these are mutually exclusive flags. Setting more that more will only
// do 1 action. Don't do it!

#define mskWBDrawStringNormal 0
#define mskWBDrawStringGray 1
#define mskWBDrawStringBold 2

/////////////////////////////////////////////////////////////////////////////
//	forward declaration
struct POPDESC;
typedef POPDESC* LPPOPDESC;

#undef  INTERFACE
#define INTERFACE IWizardBar
DECLARE_INTERFACE_(IWizardBar, IUnknown)
{
	// IWizardBar methods

	// Methods for determining Wiz Bar status


	// ********* Begin Method*****************************************
	// Basically a subset of GetWizBarFlags. Return S_OK if the CWizBarUniversal
	// object has been created. If fCreateIfNone, force creation of the object
	// if not there already.

	STDMETHOD(FExistWizBar)(BOOL fCreateIfNone = FALSE) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Force creation of the Wizard bar object if not there already.

	STDMETHOD(FCreateWizBar)() PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// more status info than FExistsWizBar. Tells if it exists,or is tracking
	// etc. See mks definitions above. If fCreateIfNone, force creation of the object
	// if not there already.

	// returns ULONG. Use mskWB*  to interpret values
	STDMETHOD(GetWizBarFlags)(ULONG *puFlags, BOOL fCreateIfNone = FALSE) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Called by shell when active view changes. 
	STDMETHOD(ActivateWizBar)(BOOL bActivate) PURE; 
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Called by tracking editor when the current selection changes. 
	// Wizard Bar will update at idle time. Should only call when
	// the selection changes enough to change the Wizard Bar status - in the
	// source editor, call when the line selected has changed, not for changes
	// within the line.

	STDMETHOD(WizBarInvalidate)() PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Enable or disable the WizardBar. Should be used with caution (no current callers 6/96)
	STDMETHOD(EnableWizBar)(BOOL bEnable = TRUE) PURE;

  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// free any interface data. If fNoRefill, leave disabled and empty, otherwise
	// refill with current data.

	STDMETHOD(ResetWizBar)(BOOL fNoRefill = FALSE) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Perform specified action. Will force immediate update of Wizard Bar before action is 
	// performed. If WB does not exist, it will be created.
	// If fOnlyIfTracked (useful for double click in dialog editor), ignore action
	// if not tracking. *pfHandledAction indicates whether action ever happened.

	STDMETHOD(DoWizBarAction)(UINT idAction, BOOL *pfHandledAction, BOOL fOnlyIfTracked, BOOL fReportError = TRUE) PURE;
  	// ********* End Method ******************************************



	// IWizardBar methods for context lists


	// ********* Begin Method*****************************************
    // Returns the position of the current selection (*pdwPos) for the WB list
	// specified by wblType. Useful as input to GetWizBarItem, etc.
	
  	STDMETHOD(GetCurSelList)(WB_LIST_TYPE wblType, DWORD *pdwPos) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Sets the position of the current selection (*pdwPos) for the WB list
	// specified by wblType. 

	STDMETHOD(SetCurSelList)(WB_LIST_TYPE wblType, DWORD *pdwPos) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Find position of a specified item (pwbItem) in a list specified by wblType.
	// Does NOT addref item
	STDMETHOD(FindWizBarItemExact)(WB_LIST_TYPE wblType, void * pWbItem, DWORD * pdwPosItem) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Return pointer (*ppWbItem) to the item at position dwPos in list specified by wblType.
	// note: this will call AddRef on the item returned

	STDMETHOD(GetWizBarItem)(WB_LIST_TYPE wblType, void ** ppWbItem, DWORD dwPos) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************
	// Return pointer (*ppWbItemSel) to the currently selected item in list specified by wblType.
	// note: this will call AddRef on the item returned

	STDMETHOD(GetSelWizBarItem)(WB_LIST_TYPE wblType, void ** ppWbItemSel) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Empties list specified by wblType. Causes items in list to be Released.
	// If fNoDelPosSel, do not rlease the currently selected item (Note:
	// fNoDelPosSel may be obsolete. I recommend only using FALSE)

	STDMETHOD(EmptyWizBarList)(WB_LIST_TYPE wblType, BOOL fNoDelPosSel) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Sorts list specified by wblType. Uses qsort and calls IWizBarLang::CompareWBItems
	// Lists are sorted to make filtering combos faster

	STDMETHOD(SortWizBarList)(WB_LIST_TYPE wblType) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Add item specified by pwbItem to list specified by wblType. Returns
	// location where item was added in *pdwPos

	STDMETHOD(AddTailList)(WB_LIST_TYPE wblType, void * pWbItem, DWORD * pdwPos) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Return in *pdwPos the location of the first item in the list
	// specified by wblType.

	STDMETHOD(GetHeadList)(WB_LIST_TYPE wblType, DWORD *pdwPos) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Analog of CObList::GetNext. Return in *ppWbItem the item in table wblType
	// at position *pdwPos.  Update *pdwPos to point to the next item in the list.
	// note: this will call AddRef on the item returned

	STDMETHOD(GetNextList)(WB_LIST_TYPE wblType, void ** ppWbItem, DWORD *pdwPos) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Return in *pcEntries the number of items in list specified by wblType

	STDMETHOD(GetCountList)(WB_LIST_TYPE wblType, UINT *pcEntries) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Draw the "standard" glyph specified by idGlyphStd and associated with an item (pwbItem, wblType)
	// at rect lpRect in hDC. Usually called by IWizBarClntLang::DrawWBItemGlyph
	// NOTE: if this function succeeds, lpRect will be set to the area remaining in the original
	// rect after the glyph id drawn (usually means left value is increased). It is untouched on failure

	STDMETHOD(DrawWBStdGlyph) (WB_TARGET_TYPE idGlyphStd, HDC hDC, RECT *lpRect) PURE; 
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************
	// Returns current string for a shared wizard bar command control specified by wbcType to use
	// for its command or tooltip text. This lets us specify new tooltips or menu
	// text for commands in the wizard bar core set (defined in ClsView package)

	STDMETHOD(GetDynamicCmdString)(UINT nID, UINT iString, BSTR* pbstrCmd) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************
	// Returns command id for default command based on current Wizard bar context
	// will put up an error message on failure if fReportError.

	STDMETHOD(GetCmdDefault)(UINT *pnIDDefault, BOOL fReportError = TRUE) PURE;

  	// ********* End Method ******************************************

	// IWizardBar methods for action button menu. More can be added if needed, essentially CMenu / CPopupMenu APIs


	// ********* Begin Method*****************************************

	// Fill us action button memu with commands specified in POPDESC structure. See
	// shlmenu.h for POPDESC description

	STDMETHOD(CreateMenuFromPopdesc)(POPDESC *ppop) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Remove command from menu. See CMenu::RemoveMenu for description

	STDMETHOD(RemoveMenu)(UINT nPosition, UINT nFlags, BOOL *pfOK) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Append command to menu. See CMenu::AppendMenu for description

	STDMETHOD(AppendMenu)(UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem, BOOL *pfOK) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Insert command to menu at nPosition. See CMenu::InsertMenu for description

	STDMETHOD(InsertMenu)(UINT nPosition, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem, BOOL *pfOK) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Modify command to menu at nPosition. See CMenu::InsertMenu for description

	STDMETHOD(ModifyMenu)(UINT nPosition, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem, BOOL *pfOK) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Analog of CMenuPopup::AddMenuItem
	// appends command with id id and text lpszText to menu

	STDMETHOD(AddMenuItem)(UINT id, LPCTSTR lpszText) PURE;
  	// ********* End Method ******************************************


	// IWizardBar methods for accessing "current" client interfaces


	// ********* Begin Method*****************************************

	// Returns pointer to currently active IWizBarClntEditor interface or
	// NULL if none

	STDMETHOD(GetCurWizBarClntEditor)(LPWIZBARCLNTEDITOR *ppWBClntEditor) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Returns pointer to currently active IWizBarClntLang interface or
	// NULL if none

	STDMETHOD(GetCurWizBarClntLang)(LPWIZBARCLNTLANG *ppWBClntLang) PURE;
  	// ********* End Method ******************************************


	// IWizardBar methods for accessing *specific* client interfaces


	// ********* Begin Method*****************************************

	// Returns pointer to IWizBarClntEditor interface specified by lpszLang and pguidEditor,  or
	// NULL if none

	STDMETHOD(GetWizBarClntEditor)(LPCTSTR lpszLang, GUID* pguidEditor, LPWIZBARCLNTEDITOR *ppWBClntEditor) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Returns pointer to IWizBarClntLang interface specified by lpszLang or
	// NULL if none

	STDMETHOD(GetWizBarClntLang)(LPCTSTR lpszLang, LPWIZBARCLNTLANG *ppWBClntLang) PURE;
  	// ********* End Method ******************************************

	// Misc functions
	// ********* Begin Method*****************************************

	// Displays error message for specified error (needed for shared
	// errors in langlib only)

	STDMETHOD(ShowWBError)(WB_ERROR_CODE wec) PURE;
  	// ********* End Method ******************************************
};



/////////////////////////////////////////////////////////////////////////////
// IWizBarClntLang
// This interface does most of the work for a Wizard bar client. If not present, these is no
// Wizard Bar support for a particular programming language.

// Methods in this interface create objects that fill the Wizard bar combos. They compare, display and handle
// AddRef and Release on iitems in the list. They fill the lists and perform actions based onthe combo contents.

// This interface may be associated with one or more IWizardBarEditor interfaces, which support
// selection tracking in an editor

// These methods are all called by the Wizard Bar itself. Noone else should be calling them.


#undef  INTERFACE
#define INTERFACE IWizBarClntLang
DECLARE_INTERFACE_(IWizBarClntLang, IUnknown)
{
	// IWizBarClntLang methods

	// ********* Begin Method *****************************************
	// Returns flags through pbfSupportsWB indicating whether this interface supports the current project
	// Typically returns mskSWAProjectSupport if the project can be supported or mskSWANoSupport otherwise. 
	// May use existence of a parser database to determine if it supports this project.
	// If mskSWANoSupport, the Wizard bar will be deactivated.

	STDMETHOD(FSupportsWBActivate)(UINT *pbfSupportsWB) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method *****************************************
	// Initializes any data required by this interface. Called after FSupportsWBActivate succeeds.
	// bfSupportsWB is the value returned by FSupportsWBActivate.

	STDMETHOD(InitWBLangData)(UINT bfSupportsWB) PURE; // argument is what you get back from FSupportsWBActivate
  	// ********* End Method *******************************************

	// ********* Begin Method *****************************************
	// Frees any interface specific data when a project	configuration changes or at shutdown

 	STDMETHOD(ClearWBLangData)() PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method *****************************************
	// Used to fill succeeding context entries with information based on the previous context
	// entries. wbltFirstEmpty indicated which items need to be filled in; any items in pwbcontext
	// >= wbltFirstEmpty will be NULL and need to be filled in.
	// This is a tricky and important method. If someone changes the 2nd combo, this is called to
	// fill up the third. You may not have to refill the entire list to do this; You do not need
	// to empty a list - You need to produce at most 3 items. Typically the Wizard Bar will take
	// what you give it here and empty its lists and refill with the context.
	// This is called directly from the Wizard Bar. GetCurWBCFromTrackData may also find it useful.

	// This method fill up the "surface" of the lists - only 1 item deep. It is called when tracking
	// as part of getting the current context, and also when dropping the first or 2nd combo. The list
	// associated with the item we return here is marked as needing to be filled if dropped.

	STDMETHOD(CompleteWBContext)(WBContext * pwbcontext, WB_LIST_TYPE wbltFirstEmpty) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method *****************************************
	// The wizard Bar has 3 "lists" each associated with one of the combos. When a combo is dropped
	// this method is called to fill the list. wblType indicates which list is to be filled. The
	// data in pwbcontext is often useful for determining what goes in this list.

	// Current implementations save the current context entry to use for reselecting, then empty the
	// list, then create objects appropriate to the list, and call IWizardBar::AddTailList to add them
	// to the list. The Wizard Bar knows nothing of the nature of these objects, except that they
	// support some interfaces (GetWBItemText, DrawWBItemGlyph, CompareWBItems, AddRefWBItem, ReleaseWBItem).
	// It just holds a pointer and calls the interface to do what it needs

	STDMETHOD(FillWizBarList) (WBContext * pwbcontext, WB_LIST_TYPE wblType, BOOL *pfSortedByFiller) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************

	// Called by the Wizard Bar when the action menu is dropped. Fill the menu with commands appropriate
	// to the context in pwbcontext. Indicate the index of the default command in piMenuDefault.
	// Several IWizardBar methods are available for filling the menu (CreateMenuFromPopdesc, RemoveMenu,
	// AppendMenu, InsertMenu, ModifyMenu, AddMenuItem).

	STDMETHOD(CreateWBMenuAction)(WBContext * pwbcontext, int *piMenuDefault) PURE; 
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************

	// Implements action menu commands that are unique to this language, appropriate to the 
	// context in pwbcontext, and which were added to the menu by CreateWBMenuAction.
	// Set *pfHandledAction	TRUE if you have implemented the command idAction.
	// This function is called after the IWizBarClntEditor equivalent.

	STDMETHOD(DoWBAction)(WBContext * pwbcontext, UINT idAction, BOOL *pfHandledAction) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************

	// called by IWizBarClntEditor::GetCurWBContext. Knows how to interpret data in pwbctrack
	// to produce a valid pwbcontext. This lets the IWizBarClntEditor be ignorant of the 
	// details of the objects that fill the lists. This function needs to be in sync with GetCurWBContext.
	// For example, in the C++ SOurce editor, the editor interface gets the name of the class and function
	// from the current line in the editor. This is passed in pwbctrack, and this function knows how to
	// produce appropriate Class, Object and Target objects to fill the context with.

	STDMETHOD(GetCurWBCFromTrackData)(WBContextTrack * pwbctrack, WBContext * pwbcontext) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************

	// Returns ILanguageService associated with this interface, if any. Not currently (6/96) used
	// by the Wizard Bar itself, may be useful in the future. If there is no associated
	// IULanguageService, return something other than S_OK (E_NOTIMPL might be a good choice).

	STDMETHOD(GetLanguageService)(ILanguageService** ppvLangService) PURE;  // associated ILanguageService
  	// ********* End Method *******************************************

	// ********* Begin Method*******************************************
	// Returns language name associated with the IWizBarClntLang interface.
	// The language string is usually the same as those used by the Source editor (ISourceEdit::GetLanguage)
	// or that returned by ILanguageService::get_LanguageName

	STDMETHOD(GetLanguageName)(BSTR* pbstrLang) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************

	// Returns pointer to a collection interface containing IWizBarClntEditor interface
	// pointers for this language. Example, in C++, there are Source and Dialog editor interfaces
	// These interfaces are optional. If you have none return a non S_OK value and set
	// *ppvClntEditors NULL.

	STDMETHOD(GetAllClntEditors)(LPWIZBARCLNTEDITORS* ppvClntEditors) PURE;  // associated IWizBarClntEditors interface
  	// ********* End Method *******************************************



	// IWizBarClntLang methods for list items
	// These methods are called by the Wizard Bar to manipulate individual list objects.



	// ********* Begin Method*****************************************

	// Given an item (pwbItem) and its type (wblType), return its associated text in *ppText.
	// If there should be different text in the edit control of a combo than what appears in the
	// list, use fTextForEdit to determine which to return. 

	STDMETHOD(GetWBItemText) (void * pwbItem, WB_LIST_TYPE wblType, BSTR* pbstrText, BOOL fTextForEdit) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************

	// Draw the glyph associated with an item (pwbItem, wblType) at rect lpRect in hDC.
	// If the glyph should be a "standard" Class View glyph, you can have IWizardBar::DrawWBStdGlyph
	// draw it for you.
	// uDrawStringFlags are hints to the drawing code that the string drawn with the glyph
	// should be drawn in a special way. Basically a hanger on argument to avoid a new member function.
	// see mskWBDrawString* for available options
	// NOTE: if this function succeeds, lpRect will be set to the area remaining in the original
	// rect after the glyph id drawn (usually means left value is increased). It is untouched on failure

	STDMETHOD(DrawWBItemGlyph) (void * pwbItem, WB_LIST_TYPE wblType, HDC hDC, RECT *lpRect, UINT *puDrawStringFlags) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************
	// Compare 2 list object items. Return Zero if the items are identical, < 0 if pwbItem1 is less 
	// than pwbItem2, or > 0 if this pwbItem1 is greater than pwbItem2.  Usually this is a
	// comarison of the text returned by GetWBItemText.You can choose
	// whether to make this case sensitive or not. (For C++ comparisons ARE case sensitive).

	STDMETHOD(CompareWBItems) (const void * pwbItem1, const void * pwbItem2, WB_LIST_TYPE wblType, int *pRetCompare) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************
	// Increases the reference count of the specified item. The Wizard Bar calls this
	// when it gets its current context from its lists so we can have multiple copies and 
	// still delete at the right time. When you create an item, set its ref count to 1. Don;t
	// addref again if adding to a WizBar list.

	STDMETHOD(AddRefWBItem) (void * pwbItem, WB_LIST_TYPE wblType) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************
	// Decreases the reference count on the item. When the count goes to 0
	// the item should be deleted. At item destruction time, the ref count
	// should be 0 or 1 (similar to how CCmdTarget works).
	STDMETHOD(ReleaseWBItem) (void * pwbItem, WB_LIST_TYPE wblType) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************
	// Returns current string for a shared wizard bar command control specified by wbcType to use
	// for its command or tooltip text. This lets us specify new tooltips or menu
	// text for commands in the wizard bar core set (defined in ClsView package)
	// if not implemented, NULL is returned, which means get string from command table

	STDMETHOD(GetDynamicCmdString)(UINT nID, UINT iString, BSTR* pbstrCmd) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************
	// Returns command id for default command based on Wizard bar context
	// if not handled, returns FALSE in *pfHandled.

	STDMETHOD(GetCmdDefault)(WBContext *pwbcontext, UINT *pnIDDefault, BOOL *pfHandled) PURE;
  	// ********* End Method *******************************************

};




/////////////////////////////////////////////////////////////////////////////
// IWizBarClntEditor
// This interface is optional. If not implemented, the user can still use the Wizard Bar
// by changing the combos by hand. It is used primarily to implement tracking of selection (via the
// GetCurWBContext method (which can call GetCurWBCFromTrackData in IWizBarClntLang)). It can also 
// add menu commands that are active only in this editor (AddWBMenuAction, DoWBAction)

// This interface is associated with a single editor and programming language, and is accessed
// through the IWizBarClntLang method GetAllClntEditors. You can have several of these interfaces
// associated with a single programming language.

// These methods are all called by the Wizard Bar itself. Noone else should be calling them.

#undef  INTERFACE
#define INTERFACE IWizBarClntEditor
DECLARE_INTERFACE_(IWizBarClntEditor, IUnknown)
{
	// IWizBarClntEditor methods

	// ********* Begin Method*******************************************

	// Returns flags through pbfSupportsWB indicating whether this interface supports the current active
	// view. Typically returns mskSWATrackingSupport if it can track selection changes, and  mskSWANoSupport
	// otherwise. Typically uses info on the current editor and language of the doc in the view to
	// determine if it has support

	STDMETHOD(FSupportsWBActivate)(UINT *pbfSupportsWB) PURE; 
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************

	// Initializes any data required by this interface. Called after FSupportsWBActivate succeeds.
	// bfSupportsWB is the value returned by FSupportsWBActivate.

	STDMETHOD(InitWBEditorData)(UINT bfSupportsWB) PURE; // argument is what you get back from FSupportsWBActivate
	// ********** End Method ******************************************

	// ********* Begin Method*****************************************

	// Frees any editor specific data when a new editor is activated

	STDMETHOD(ClearWBEditorData)() PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************

	// The main tracking interface. Fills pwbcontext with information appropriate to
	// the current selection. (Note that the editor must call IWizardBar::WizBarInvalidate()
	// when its selection changes to make this work).
	// pwbcontext is empty when this is called. Set *pfUnchanged true if the selection
	// has not changed since the last call. 
	// This method usually has a contract with GetCurWBCFromTrackData in IWizBarClntLang. It
	// analyzes the current selection and gathers data that	GetCurWBCFromTrackData can use to
	// fill pwbcontext. These 2 methods should be written in tamdem.

	STDMETHOD(GetCurWBContext)(WBContext * pwbcontext, BOOL *pfUnchanged) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************
	// Optional. Use to add commands that are unique to this editor, not common to the language
	// implementation. Example, in C++ source editor, GoToNextFunction uses editor tracking
	// and so is implemented in this interface.

	STDMETHOD(AddWBMenuAction)(WBContext * pwbcontext) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************
	// Optional. Implements commands that are unique to this editor, not common to the language,
	// and which were added to the menu by AddWBMenuAction.
	// Set *pfHandledAction	TRUE if you have implemented the command idAction.
	// This function is called before the IWizBarClntLang equivalent, so it has first try.

	STDMETHOD(DoWBAction)(WBContext * pwbcontext, UINT idAction, BOOL *pfHandledAction) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*******************************************
	// Identification of interface. This info is usually hard coded. Editors are identified by
	// a guid, accessible through CPartView::GetEditorID, and we store the approriate value.
	// The language string is usually the same as those used by the Source editor (ISourceEdit::GetLanguage)

	STDMETHOD(GetLangAndEditor)(BSTR* pbstrLang, GUID* pguidEditor) PURE;
  	// ********* End Method ******************************************

	// ********* Begin Method*****************************************
	// A convenience call. Each of these interfaces is associated with an IWizBarClntLang and the
	// pointer to that interface should be returned here.

	STDMETHOD(GetIClntLang)(LPWIZBARCLNTLANG *ppWBClntLang) PURE;  // get associated IWizBarClntLang interface
	// ********* End Method *******************************************

	// ********* Begin Method*****************************************
	// Returns current string for a shared wizard bar command control specified by wbcType to use
	// for its command or tooltip text. This lets us specify new tooltips or menu
	// text for commands in the wizard bar core set (defined in ClsView package)
	// if not implemented, NULL is returned, which means get string from command table

	STDMETHOD(GetDynamicCmdString)(UINT nID, UINT iString, BSTR* pbstrCmd) PURE;
  	// ********* End Method *******************************************

	// ********* Begin Method*****************************************
	// Returns command id for default command based on Wizard bar context
	// if not handled, returns FALSE in *pfHandled.

	STDMETHOD(GetCmdDefault)(WBContext *pwbcontext, UINT *pnIDDefault, BOOL *pfHandled) PURE;
  	// ********* End Method *******************************************

};

/////////////////////////////////////////////////////////////////////////////
// IWizBarClntEditors
// Collections of wiz bar client editors (get from IWizBarClntLang)
// If you derive an object from CWizBarEditorCollection (see langlib package)
// You will get these interfaces for free. You will have to implement the
// CWizBarEditorCollection Init() and destructors, but those are pretty simple.
// See CWizBarEditorCollectionCpp in the langcpp package for an example.

#undef  INTERFACE
#define INTERFACE IWizBarClntEditors
DECLARE_INTERFACE_(IWizBarClntEditors, IUnknown)
{
	STDMETHOD(_NewEnum)(THIS_ IEnumVARIANT **ppenum) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// IWizBarClntLangProvider
// Provides access to IWizBarClntLang interface implemented on object
// in the package. A package that implements IWizBarClntLang have have
// one of these interfaces to give the Wizard Bar access to IWizBarClntLang

#undef  INTERFACE
#define INTERFACE IWizBarClntLangProvider
DECLARE_INTERFACE_(IWizBarClntLangProvider, IUnknown)
{
	// Returns pointer to IWizBarClntLang interface or NULL if none

	STDMETHOD(GetClntLang)(LPWIZBARCLNTLANG *ppWBClntLang) PURE;
};


/////////////////////////////////////////////////////////////////////////////

#endif // __WIZBARAPI_H__