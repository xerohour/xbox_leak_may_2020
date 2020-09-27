#include <xtl.h>
#include "UserInterface.h"
#include "UserInterfaceScreen.h"
#include "UserInterfaceText.h"

class CLinkScreen : public CUserInterfaceScreen {

public:

    // Constructor / Destructor
    CLinkScreen(void);
    ~CLinkScreen(void);

    // Overloaded user input
    virtual CUserInterfaceScreen* Update(CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio);

private:

    // Update UI text
    void UpdateText(void);

    // UI text
    CUserInterfaceText m_DisconnectedText;
    CUserInterfaceText m_ConnectedText;

    CUserInterfaceText m_Speed100MbpsText;
    CUserInterfaceText m_Speed10MbpsText;
    CUserInterfaceText m_SpeedUnknownText;

    CUserInterfaceText m_ModeFullDuplexText;
    CUserInterfaceText m_ModeHalfDuplexText;
    CUserInterfaceText m_ModeUnknownText;

    // Link status
    DWORD  m_dwLinkStatus;
};



CLinkScreen::CLinkScreen(void)

    // Link screen
    : CUserInterfaceScreen(NULL, 0xFFFFFFFF, 0x00000000, 320, 60),

    // UI text
    m_DisconnectedText("Disconnected", 320, 180, 0xFFFF0000, 0xFF000000),
    m_ConnectedText("Connected", 320, 180, 0xFF00FF00, 0xFF000000),

    m_Speed100MbpsText("Speed: 100 Mbps", 320, 240, 0xFFFFFF00, 0xFF000000),
    m_Speed10MbpsText("Speed: 10 Mbps", 320, 240, 0xFFFFFF00, 0xFF000000),
    m_SpeedUnknownText("Speed: Unknown", 320, 240, 0xFFFF0000, 0xFF000000),

    m_ModeFullDuplexText("Mode: Full Duplex", 320, 270, 0xFFFFFF00, 0xFF000000),
    m_ModeHalfDuplexText("Mode: Half Duplex", 320, 270, 0xFFFFFF00, 0xFF000000),
    m_ModeUnknownText("Mode: Unknown", 320, 270, 0xFFFF0000, 0xFF000000)

{
    // Set the link screen text
    SetText("Ethernet Link Status");

    // Add the UI text
    AddUIText(&m_DisconnectedText);
    AddUIText(&m_ConnectedText);

    AddUIText(&m_Speed100MbpsText);
    AddUIText(&m_Speed10MbpsText);
    AddUIText(&m_SpeedUnknownText);

    AddUIText(&m_ModeFullDuplexText);
    AddUIText(&m_ModeHalfDuplexText);
    AddUIText(&m_ModeUnknownText);

    UpdateText();
}



CLinkScreen::~CLinkScreen(void)
{
}

void
CLinkScreen::UpdateText(void)
{
    m_dwLinkStatus = XNetGetEthernetLinkStatus();

    if (XNET_ETHERNET_LINK_ACTIVE & m_dwLinkStatus) {
        m_ConnectedText.Hide(FALSE);
        m_DisconnectedText.Hide(TRUE);

        if (XNET_ETHERNET_LINK_100MBPS & m_dwLinkStatus) {
            m_Speed100MbpsText.Hide(FALSE);
            m_Speed10MbpsText.Hide(TRUE);
            m_SpeedUnknownText.Hide(TRUE);
        }
        else if (XNET_ETHERNET_LINK_10MBPS & m_dwLinkStatus) {
            m_Speed10MbpsText.Hide(FALSE);
            m_Speed100MbpsText.Hide(TRUE);
            m_SpeedUnknownText.Hide(TRUE);
        }
        else {
            m_SpeedUnknownText.Hide(FALSE);
            m_Speed100MbpsText.Hide(TRUE);
            m_Speed10MbpsText.Hide(TRUE);
        }

        if (XNET_ETHERNET_LINK_FULL_DUPLEX & m_dwLinkStatus) {
            m_ModeFullDuplexText.Hide(FALSE);
            m_ModeHalfDuplexText.Hide(TRUE);
            m_ModeUnknownText.Hide(TRUE);
        }
        else if (XNET_ETHERNET_LINK_HALF_DUPLEX & m_dwLinkStatus) {
            m_ModeHalfDuplexText.Hide(FALSE);
            m_ModeFullDuplexText.Hide(TRUE);
            m_ModeUnknownText.Hide(TRUE);
        }
        else {
            m_ModeUnknownText.Hide(FALSE);
            m_ModeFullDuplexText.Hide(TRUE);
            m_ModeHalfDuplexText.Hide(TRUE);
        }
    }
    else {
        m_DisconnectedText.Hide(FALSE);
        m_ConnectedText.Hide(TRUE);

        m_Speed100MbpsText.Hide(TRUE);
        m_Speed10MbpsText.Hide(TRUE);
        m_SpeedUnknownText.Hide(TRUE);

        m_ModeFullDuplexText.Hide(TRUE);
        m_ModeHalfDuplexText.Hide(TRUE);
        m_ModeUnknownText.Hide(TRUE);
    }
}



CUserInterfaceScreen* 
CLinkScreen::Update(CUserInterfaceInput* pInput, CUserInterfaceAudio* pAudio)
{
    UpdateText();

    return NULL;
}



void __cdecl main(void)
{
    // hResult is the result of the user interface update
    HRESULT  hResult = S_OK;



    // Construct the link screen
    CLinkScreen linkScreen;

    // Setup the user interface
    hResult = CUserInterface::Instance()->Initialize(&linkScreen, 640, 480, 0, NULL);

    while (SUCCEEDED(hResult)) {
        hResult = CUserInterface::Instance()->Update();
    }

    XLaunchNewImage(NULL, NULL);

    __asm int 3;

    return;
}
