#include "std.h"
#include "scene.h"
#include "globals.h"
#include "NetConfig.h"

const char cszHelpNodeName[] = "TEXT_PANEL";
const char cszTitleNodeName[] = "TEXT_TITLE";

HRESULT CNTSHelpScene::Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest)
{
	HRESULT hr = CCellWallScene::Initialize(pUrl, pButtonTextName, eCurSceneId, bAutoTest);
	if(FAILED(hr))
	{
		return hr;
	}
	
	int nTextIndex = CPrimitiveScene::FindTextObjForShape(cszHelpNodeName);
	int nTitleTextIndex = CPrimitiveScene::FindTextObjForShape(cszTitleNodeName);

	ASSERT(nTitleTextIndex != -1 && nTextIndex != -1);
	if(nTitleTextIndex == -1 || nTextIndex == -1)
	{
		DbgPrint("CNTSHelpScene::Initialize - fail to find texts\n");
		return XBAPPERR_INTERNAL_ERROR;
	}
	eNetworkState eCurState = g_NetConfig.GetNetworkStatus();
	// set the proper text based on the current network status
	switch (m_eSceneId)
	{
		case eNTS_CableHelpId:
//			eCurState == eCableFail ? m_pTexts[nTextIndex]->SetTextId(				
			break;
		case eNTS_PPPoEHelpId:
			break;
		case eNTS_IPHelpId:
			break;
		case eNTS_DNSHelpId:
			break;
		case eNTS_ServicesHelpId:
			break;
		default:
			// not supposed to be here, unknown screen
			ASSERT(false);
	};

	return hr;
}