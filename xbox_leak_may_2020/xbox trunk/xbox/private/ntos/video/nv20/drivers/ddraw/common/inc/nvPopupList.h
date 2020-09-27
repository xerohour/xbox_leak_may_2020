
#ifndef STEREO_POPUP_LIST_CLASS_DECL
#define STEREO_POPUP_LIST_CLASS_DECL

#include "PopupAgent.h"
#include "duma.h"

///////////////////////////////////////////////////////////
//
// popup flavours
//
///////////////////////////////////////////////////////////
class CPerfStatPopup: public CPopupAgent
{
public:
	CPerfStatPopup();
	virtual ~CPerfStatPopup();
	
};

class CStereoStatPopup: public CPopupAgent, protected STEREOSTATS  
{
public:
	CStereoStatPopup();
	virtual ~CStereoStatPopup();

	void			setContext(NVD3DCONTEXT* aContext); //overloaded: set context and update stereo stats from context
};

///////////////////////////////////////////////////////////
//
// popup controller
//
///////////////////////////////////////////////////////////
class CPopupAgentList
{
public:
//types
    struct PopupHotkeys
    {
        HOTKEY  PopupToggle;
        HOTKEY  FPStoggle;
        HOTKEY  StatsToggle;
    };

protected:
//attributes
	char			strModule[MAX_PATH];
	HMODULE			hMod;
	CPopupAgent*	popupAgentList; //root of the list
    bool            bPerfStat;      //FPS popup enabled/disabled
    bool            bStereoStat;    //Stereo statistics popup enabled/disabled
    bool            isPopupEnabled;
    PopupHotkeys    hotkeys;
//members
    void            add(CPopupAgent* next);
public:
	CPopupAgentList();
	~CPopupAgentList();
	HRESULT			create();
	HRESULT			create(const char* aModule);
	HRESULT			destroy();
    HRESULT         initHotKeys();
    HRESULT         finiHotKeys();
	HRESULT			render(NVD3DCONTEXT *pContext);
    bool&           enabled() {return isPopupEnabled;};
    void            toggle()   {isPopupEnabled = isPopupEnabled?false:true;};
    void            getOptions();
};

extern CPopupAgentList popupAgents;

#endif //STEREO_POPUP_LIST_CLASS_DECL