//
//
//

var surge = WScript.CreateObject("Surge.Engine");
var filesystem = WScript.CreateObject("Scripting.FileSystemObject");
var file = filesystem.OpenTextFile("debug.txt", 8, true);

main();

function main()
    {
    surge.getFullPage = true;
    surge.useHead = false;
    surge.SetReceiveTimeout(10, 0);

    surge.AddHeader("Content-Type", "application/x-www-form-urlencoded")
    surge.POST("http://172.26.160.46/lab/submit/ping.asp", "machine=jpoleyxbx&dbgIP=157.56.10.128&hwver=DVT4,%20SMC%3DB3F&vmode=AVPACK%3DSDTV%20Analog,%20LANG%3DEnglish,%20STD%3DNTSC_M,%20MODE%3D()&amode=&periph=Duke(0);%20MU(0,%201);%20MU(0,%202);%20");

    file.Write(surge.GetHeader() + surge.GetBody());
    }

