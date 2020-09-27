// This script generates a configuration file using files from the
// latest build and then launches the LabPusher app.

// create new .INI file and overwrite the old one
var filesystem = WScript.CreateObject("Scripting.FileSystemObject");
var inifile = filesystem.CreateTextFile("labpusher.ini", true);

// specify settings
inifile.WriteLine("[Settings]");
inifile.WriteLine("QueryTime=30");
inifile.WriteLine("FileSleep=5");
inifile.WriteLine("PacketSleep=2");
inifile.WriteLine("XbAppSrc=\\\\xstress\\stress\\LabPusher\\XbServer.xbe");
inifile.WriteLine("XbAppDst=xE:\\XbServer\\XbServer.xbe");
inifile.WriteLine("RebootApp=xE:\\harness.xbe");
inifile.WriteLine("XboxList=xlab081");
inifile.WriteLine("xlab082");
inifile.WriteLine("xlab083");
inifile.WriteLine("xlab084");
inifile.WriteLine("xlab085");
inifile.WriteLine("xlab086");
inifile.WriteLine("xlab087");
inifile.WriteLine("xlab088");
inifile.WriteLine("xlab089");
inifile.WriteLine("xlab0810");
inifile.WriteLine("xlab0811");
inifile.WriteLine("xlab0812");
inifile.WriteLine("xlab0813");
inifile.WriteLine("xlab0814");
inifile.WriteLine("xlab0815");
inifile.WriteLine("xlab0816");
inifile.WriteLine("xlab0817");
inifile.WriteLine("xlab0818");
inifile.WriteLine("xlab0819");
inifile.WriteLine("xlab0820");
inifile.WriteLine("xlab0821");
inifile.WriteLine("xlab0822");
inifile.WriteLine("xlab0823");
inifile.WriteLine("xlab0824");
inifile.WriteLine("xlab0825");
inifile.WriteLine("xlab091");
inifile.WriteLine("xlab092");
inifile.WriteLine("xlab093");
inifile.WriteLine("xlab094");
inifile.WriteLine("xlab095");
inifile.WriteLine("xlab096");
inifile.WriteLine("xlab097");
inifile.WriteLine("xlab098");
inifile.WriteLine("xlab099");
inifile.WriteLine("xlab0910");
inifile.WriteLine("xlab0911");
inifile.WriteLine("xlab0912");
inifile.WriteLine("xlab0913");
inifile.WriteLine("xlab0914");
inifile.WriteLine("xlab0915");
inifile.WriteLine("xlab0916");
inifile.WriteLine("xlab0917");
inifile.WriteLine("xlab0918");
inifile.WriteLine("xlab0919");
inifile.WriteLine("xlab0920");
inifile.WriteLine("xlab0921");
inifile.WriteLine("xlab0922");
inifile.WriteLine("xlab0923");
inifile.WriteLine("xlab0924");
inifile.WriteLine("xlab0925");
inifile.WriteLine("xlab101");
inifile.WriteLine("xlab102");
inifile.WriteLine("xlab103");
inifile.WriteLine("xlab104");
inifile.WriteLine("xlab105");
inifile.WriteLine("xlab106");
inifile.WriteLine("xlab107");
inifile.WriteLine("xlab108");
inifile.WriteLine("xlab112");
inifile.WriteLine("xlab1010");
inifile.WriteLine("xlab1011");
inifile.WriteLine("xlab1012");
inifile.WriteLine("xlab1013");
inifile.WriteLine("xlab1014");
inifile.WriteLine("xlab1015");
inifile.WriteLine("xlab1016");
inifile.WriteLine("xlab1017");
inifile.WriteLine("xlab1018");
inifile.WriteLine("xlab1019");
inifile.WriteLine("xlab1020");
inifile.WriteLine("xlab1021");
inifile.WriteLine("xlab1022");
inifile.WriteLine("xlab1023");
inifile.WriteLine("xlab1024");
inifile.WriteLine("xlab1025");
inifile.WriteLine("xlab111");
inifile.WriteLine("xlab112");
inifile.WriteLine("xlab113");
inifile.WriteLine("xlab114");
inifile.WriteLine("xlab115");
inifile.WriteLine("xlab116");
inifile.WriteLine("xlab117");
inifile.WriteLine("xlab118");
inifile.WriteLine("xlab119");
inifile.WriteLine("xlab1110");
inifile.WriteLine("xlab1111");
inifile.WriteLine("xlab1112");
inifile.WriteLine("xlab1113");
inifile.WriteLine("xlab1114");
inifile.WriteLine("xlab1115");
inifile.WriteLine("xlab1116");
inifile.WriteLine("xlab1117");
inifile.WriteLine("xlab1118");
inifile.WriteLine("xlab1119");
inifile.WriteLine("xlab1120");
inifile.WriteLine("xlab1121");
inifile.WriteLine("xlab1122");
inifile.WriteLine("xlab1123");
inifile.WriteLine("xlab1124");
inifile.WriteLine("xlab1125");
inifile.WriteLine("xlab121");
inifile.WriteLine("xlab122");
inifile.WriteLine("xlab123");
inifile.WriteLine("xlab124");
inifile.WriteLine("xlab125");
inifile.WriteLine("xlab126");
inifile.WriteLine("xlab127");
inifile.WriteLine("xlab128");
inifile.WriteLine("xlab129");
inifile.WriteLine("xlab1210");
inifile.WriteLine("xlab1211");
inifile.WriteLine("xlab1212");
inifile.WriteLine("xlab1213");
inifile.WriteLine("xlab1214");
inifile.WriteLine("xlab1215");
inifile.WriteLine("xlab1216");
inifile.WriteLine("xlab1217");
inifile.WriteLine("xlab1218");
inifile.WriteLine("xlab1219");
inifile.WriteLine("xlab1220");
inifile.WriteLine("xlab1221");
inifile.WriteLine("xlab1222");
inifile.WriteLine("xlab1223");
inifile.WriteLine("xlab1224");
inifile.WriteLine("xlab1225");
inifile.WriteLine("xlab131");
inifile.WriteLine("xlab132");
inifile.WriteLine("xlab133");
inifile.WriteLine("xlab134");
inifile.WriteLine("xlab135");
inifile.WriteLine("xlab136");
inifile.WriteLine("xlab137");
inifile.WriteLine("xlab138");
inifile.WriteLine("xlab139");
inifile.WriteLine("xlab1310");
inifile.WriteLine("xlab1311");
inifile.WriteLine("xlab1312");
inifile.WriteLine("xlab1313");
inifile.WriteLine("xlab1314");
inifile.WriteLine("xlab1315");
inifile.WriteLine("xlab1316");
inifile.WriteLine("xlab1317");
inifile.WriteLine("xlab1318");
inifile.WriteLine("xlab1319");
inifile.WriteLine("xlab1320");
inifile.WriteLine("xlab1321");
inifile.WriteLine("xlab1322");
inifile.WriteLine("xlab1323");
inifile.WriteLine("xlab1324");
inifile.WriteLine("xlab1325");
inifile.WriteLine("xlab141");
inifile.WriteLine("xlab142");
inifile.WriteLine("xlab143");
inifile.WriteLine("xlab144");
inifile.WriteLine("xlab145");
inifile.WriteLine("xlab146");
inifile.WriteLine("xlab147");
inifile.WriteLine("xlab148");
inifile.WriteLine("xlab149");
inifile.WriteLine("xlab1410");
inifile.WriteLine("xlab1411");
inifile.WriteLine("xlab1412");
inifile.WriteLine("xlab1413");
inifile.WriteLine("xlab1414");
inifile.WriteLine("xlab1415");
inifile.WriteLine("xlab1416");
inifile.WriteLine("xlab1417");
inifile.WriteLine("xlab1418");
inifile.WriteLine("xlab1419");
inifile.WriteLine("xlab1420");
inifile.WriteLine("xlab1421");
inifile.WriteLine("xlab1422");
inifile.WriteLine("xlab1423");
inifile.WriteLine("xlab1424");
inifile.WriteLine("xlab1425");
inifile.WriteLine("xlab151");
inifile.WriteLine("xlab152");
inifile.WriteLine("xlab153");
inifile.WriteLine("xlab154");
inifile.WriteLine("xlab155");
inifile.WriteLine("xlab156");
inifile.WriteLine("xlab157");
inifile.WriteLine("xlab158");
inifile.WriteLine("xlab159");
inifile.WriteLine("xlab1510");
inifile.WriteLine("xlab1511");
inifile.WriteLine("xlab1512");
inifile.WriteLine("xlab1513");
inifile.WriteLine("xlab1514");
inifile.WriteLine("xlab1515");
inifile.WriteLine("xlab1516");
inifile.WriteLine("xlab1517");
inifile.WriteLine("xlab1518");
inifile.WriteLine("xlab1519");
inifile.WriteLine("xlab1520");
inifile.WriteLine("xlab1521");
inifile.WriteLine("xlab1522");
inifile.WriteLine("xlab1523");
inifile.WriteLine("xlab1524");
inifile.WriteLine("xlab1525");
inifile.WriteLine("xlab161");
inifile.WriteLine("xlab162");
inifile.WriteLine("xlab163");
inifile.WriteLine("xlab164");
inifile.WriteLine("xlab165");
inifile.WriteLine("xlab166");
inifile.WriteLine("xlab167");
inifile.WriteLine("xlab168");
inifile.WriteLine("xlab169");
inifile.WriteLine("xlab1610");
inifile.WriteLine("xlab1611");
inifile.WriteLine("xlab1612");
inifile.WriteLine("xlab1613");
inifile.WriteLine("xlab1614");
inifile.WriteLine("xlab1615");
inifile.WriteLine("xlab1616");
inifile.WriteLine("xlab1617");
inifile.WriteLine("xlab1618");
inifile.WriteLine("xlab1619");
inifile.WriteLine("xlab1620");
inifile.WriteLine("xlab1621");
inifile.WriteLine("xlab1622");
inifile.WriteLine("xlab1623");
inifile.WriteLine("xlab1624");
inifile.WriteLine("xlab1625");
inifile.WriteLine("xlab171");
inifile.WriteLine("xlab172");
inifile.WriteLine("xlab173");
inifile.WriteLine("xlab174");
inifile.WriteLine("xlab175");
inifile.WriteLine("xlab176");
inifile.WriteLine("xlab177");
inifile.WriteLine("xlab178");
inifile.WriteLine("xlab179");
inifile.WriteLine("xlab1710");
inifile.WriteLine("xlab1711");
inifile.WriteLine("xlab1712");
inifile.WriteLine("xlab1713");
inifile.WriteLine("xlab1714");
inifile.WriteLine("xlab1715");
inifile.WriteLine("xlab1716");
inifile.WriteLine("xlab1717");
inifile.WriteLine("xlab1718");
inifile.WriteLine("xlab1719");
inifile.WriteLine("xlab1720");
inifile.WriteLine("xlab1721");
inifile.WriteLine("xlab1722");
inifile.WriteLine("xlab1723");
inifile.WriteLine("xlab1724");
inifile.WriteLine("xlab1725");
inifile.WriteLine("t-msanto-x1");
inifile.WriteLine("t-msanto-x2");
//inifile.WriteLine("t-msanto-x3");
inifile.Write("FileList=");

// get C and Y drive files from today's build 
var WshShell = WScript.CreateObject("WScript.Shell");
WshShell.Run("%comspec% /c dir \/A-D \/B \/S \\\\xbuilds\\recovery\\XC \\\\xbuilds\\recovery\\XY >> temp.txt", 0, true);

// sleep for 1 sec to make sure temp.txt exists before we try to open it
WScript.Sleep(1000);
var tempfile = filesystem.OpenTextFile("temp.txt", 1);


// specify Xbox destination path for each file
var cdir = /\\XC(.*)/ 
var ydir = /\\XY(.*)/

while(!tempfile.AtEndOfStream)
{
    var line = tempfile.ReadLine();

    var result = line.match(cdir);
    if(result)
    {
        inifile.WriteLine( line + ",C:" + result[1]);
    }
    else
    {
        result = line.match(ydir);
	if(result)
        {
            inifile.WriteLine( line + ",Y:" + result[1]);
        }
    }
}

// close and delete temp file
tempfile.close();
filesystem.DeleteFile("temp.txt");

// get latest build version
var verfile = filesystem.OpenTextFile("\\\\xbuilds\\release\\usa\\latest.txt");
var version = verfile.Read(4);

// specify settings that depend on latest build version
inifile.WriteLine("\\\\xbuilds\\release\\usa\\" + version + "\\checked\\dump\\harness.xbe,E:\\harness.xbe");
inifile.WriteLine("\\\\xbuilds\\release\\usa\\" + version + "\\checked\\xboxtest\\stress.ini,S:\\a7049955\\testini.ini");
inifile.WriteLine("KernelSrc=\\\\xbuilds\\release\\usa\\" + version + "\\checked\\boot\\xboxrom_dvt4.bin");

// close .INI file
inifile.close();

// launch LabPusher
WshShell.Run("%comspec% /c \\\\xstress\\stress\\LabPusher\\PcClient.exe");
