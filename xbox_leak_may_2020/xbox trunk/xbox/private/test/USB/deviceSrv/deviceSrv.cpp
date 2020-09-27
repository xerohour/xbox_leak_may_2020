
#include "deviceSrv.h"
#include "CSockServer.h"
#include "USBManager.h"
#include "iString.h"

void ErrorPage(SOCKET sock, unsigned code);
void GETConnected(SOCKET sock, char *url);

USBManager *usbmanager;
DeviceServer::CSockServer *server;

char *helpText = "The following 'pages' are recognized:<BR>&nbsp;connect.txt<BR><P>";
char *htmlHeader = "<STYLE>BODY { color:000000; background-color:FFFFFF; font:normal 12pt Arial; }</STYLE>";

void __cdecl main(void)
    {
    XInitDevices(0, NULL);

    server = new DeviceServer::CSockServer;
    usbmanager = new USBManager;

    if(!server)
        {
        DebugPrint("ERROR: Out of memory!\n");
        return;
        }

    server->SetPort(80);
    server->SetTimeout(1000);
    server->m_socktype = SOCK_STREAM;
    server->OpenServerSocket(server->m_socktype);
    server->BindSocket();
    server->Run();

    DebugPrint("Main Loop is Running\n");
    usbmanager->CheckForHotplugs();
    while(1)
        {
        // check for new devices
        server->Lock();
        usbmanager->CheckForHotplugs();
        server->Unlock();


        SleepEx(80, true);
        }

    delete server;

    DebugPrint("END\n");
    while(1) {}
    }


void HandleRequest(SOCKET sock)
    {
    char request[1024];
    char method[16];
    char url[512];

    if(server->IsDataAvailable(sock))
        recv(sock, request, 1024, 0);
    else
        return;

    sscanf(request, "%s %s", method, url);

    if(stricmp(method, "GET") != 0)
        {
        ErrorPage(sock, 405); // invalid "Method"
        return;
        }

    if(stristr(url, "connect.") == true)
        {
        GETConnected(sock, url);
        }
    else
        {
        ErrorPage(sock, 404);
        }
    }

void ErrorPage(SOCKET sock, unsigned code)
    {
    iString response;
    iString fullResponse;
    response.sprintf("<HTML><HEAD>\r\n");
    response.scatf("%s\r\n", htmlHeader);
    response.scatf("<TITLE>Device Server Error</TITLE></HEAD><BODY>\r\n");
    response.scatf("<B>%u</B><P>\r\n", code);
    response.scatf("%s\r\n", helpText);
    response.scatf("</BODY></HTML>\r\n");

    fullResponse.sprintf("HTTP/1.1 %u\r\n", code);
    fullResponse.scatf("Content-Length: %u\r\n", response.length());
    fullResponse.scatf("Content-Type: text/html\r\n");
    fullResponse.scatf("Connection: close\r\n");
    fullResponse.scatf("\r\n");
    fullResponse.scatf("%s", response.str);
    send(sock, fullResponse.str, fullResponse.length(), 0);
    }

void GETConnected(SOCKET sock, char *url)
    {
    iString response;
    iString fullResponse;
    XINPUT_CAPABILITIES info;

    const char *xidSubTypes[] = {
        "",
        "GAMEPAD",
        "JOYSTICK",
        "WHEEL",
        "DANCE PAD",
        "FISHING ROD",
        "LIGHT GUN"
        };

    response.sprintf("\r\n");

    response.scatf("Port, Slot, Device, Notes;\r\n");
    for(unsigned i=0; i<XGetPortCount(); i++)
        {
        for(unsigned j=0; j<SLOT_MAX; j++)
            {
            if(usbmanager->devices[i][j] != NULL)
                {
                response.scatf("%u, %u", i, j);
                if(XDEVICE_TYPE_GAMEPAD == usbmanager->devices[i][j]->type)
                    {
                    XInputGetCapabilities(((DeviceDuke*)(usbmanager->devices[i][j]))->duke, &info);
                    response.scatf(", XID");
                    response.scatf(", %s;\r\n", xidSubTypes[info.SubType]);
                    }
                else if(XDEVICE_TYPE_MEMORY_UNIT == usbmanager->devices[i][j]->type)
                    {
                    response.scatf(", MU");
                    response.scatf(", %c;\r\n", (char)((DeviceMU*)(usbmanager->devices[i][j]))->drive);
                    }
                else if(XDEVICE_TYPE_VOICE_MICROPHONE == usbmanager->devices[i][j]->type)
                    {
                    response.scatf(", HAWK");
                    response.scatf(", \"\";\r\n");
                    }
#ifdef DEBUG_KEYBOARD
                else if(XDEVICE_TYPE_DEBUG_KEYBOARD == usbmanager->devices[i][j]->type)
                    {
                    response.scatf(", KEYBOARD");
                    response.scatf(", \"\";\r\n");
                    }
#endif // DEBUG_KEYBOARD
                else
                    {
                    response.scatf(", UNKNOWN");
                    response.scatf(", \"\";\r\n");
                    }
                }
            }
        }

    //DebugPrint("Content-Length: %u\r\n", response.length());

    fullResponse.sprintf("HTTP/1.1 200 OK\r\n");
    fullResponse.scatf("Content-Length: %u\r\n", response.length());
    fullResponse.scatf("Content-Type: text/plain\r\n");
    fullResponse.scatf("Connection: close\r\n");
    fullResponse.scatf("\r\n");
    fullResponse.scatf("%s", response.str);
    send(sock, fullResponse.str, fullResponse.length(), 0);
    }