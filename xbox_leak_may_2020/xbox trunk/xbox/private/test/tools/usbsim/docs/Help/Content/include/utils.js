//////////////////////////////////////////////////////////////////////////////
// 
// Microsoft Confidential
// Copyright (C) Microsoft Corporation.  All rights reserved.
//
//     Josh Poley (jpoley)
// 
//////////////////////////////////////////////////////////////////////////////

hexvalues = new Array;
for(i=0; i<=9; i++) hexvalues[i]="" + i;
hexvalues[10]="A"; hexvalues[11]="B"; hexvalues[12]="C"; hexvalues[13]="D"; hexvalues[14]="E"; hexvalues[15]="F";

var HEADER_NONE = 0;
var HEADER_OVERWRITE = 1;
var HEADER_APPEND = 2;

function xtoa(val)
    {
    return hexvalues[val >> 4] + hexvalues[val & 0xF];
    }

function Gradient(r0, g0, b0, r1, g1, b1, ns, th, tw)
    {
    hr = (r1-r0)/ns;
    hg = (g1-g0)/ns;
    hb = (b1-b0)/ns;

    document.write("<TABLE CELLPADDING=0 CELLSPACING=0 HEIGHT=" + th + " WIDTH="+tw+">");
    for(i=0; i<ns; i++)
        {
        document.write("<TD BGCOLOR=#"+xtoa(r0)+xtoa(g0)+xtoa(b0)+"></TD>");
        r0+=hr;g0+=hg;b0+=hb;
        }
    document.write("</TABLE>");
    }

function ToolTipClickHandler()
    {
    if(event.srcElement != TIP)
        {
        document.onclick="";
        ToolTip();
        }
    }

function ToolTip(msg, left, persist)
    {
    if(msg == null)
        {
        TIP.style.display="none";
        }
    else
        {
        TIP.innerHTML = msg;
        TIP.style.display = "block";
        if(left != null) TIP.style.left = left;
        else TIP.style.pixelLeft = event.x;
        TIP.style.pixelTop = event.y + 18 + document.body.scrollTop;
        if(persist != null)
            {
            event.cancelBubble = true;
            document.onclick = ToolTipClickHandler;
            }
        }
    }

function WriteFooter()
    {
    document.write("<P><HR SIZE=1>");
    document.write("<SPAN STYLE='font: normal 8pt Arial;color:#555555;'>Copyright &#169; Microsoft Corporation. All rights reserved.<BR>Microsoft Confidential</SPAN>");
    }

function WriteHeader(set, str)
    {
    if(set == HEADER_NONE) return;
    if(set == HEADER_OVERWRITE)
        {
        document.write("<DIV CLASS=PAGEHEADER>"+ str +"</DIV>");
        return;
        }

    header = "<TABLE WIDTH=100% BORDER=0 CELLSPACING=0 CELLPADDING=0><TR HEIGHT=30><TD STYLE='background:#9CCFFF'><B>USB Simulator</B>";
    contents = " ";

    // generic headers
    if(document.URL.match("Content.USBSimulator.Simulator*") != null)
        {
        header += ": Simulator COM Interface";
        contents = "<A HREF='contents.htm'><IMG SRC='../include/contents.gif' BORDER=0></A>";
        }
    else if(document.URL.match("Content.USBSimulator.Duke*") != null)
        {
        header += ": Duke COM Interface";
        contents = "<A HREF='contents.htm'><IMG SRC='../include/contents.gif' BORDER=0></A>";
        }
    else if(document.URL.match("Content.USBSimulator.MU*") != null)
        {
        header += ": MU COM Interface";
        contents = "<A HREF='contents.htm'><IMG SRC='../include/contents.gif' BORDER=0></A>";
        }
    else if(document.URL.match("Content.USBSimulator.Hawk*") != null)
        {
        header += ": Hawk COM Interface";
        contents = "<A HREF='contents.htm'><IMG SRC='../include/contents.gif' BORDER=0></A>";
        }
    else if(document.URL.match("Content.USBSimulator.Debugger*") != null)
        {
        header += ": Debugger COM Interface";
        contents = "<A HREF='contents.htm'><IMG SRC='../include/contents.gif' BORDER=0></A>";
        }
    else if(document.URL.match("Content.Guide*") != null)
        {
        header += ": User's Guide";
        }
    else if(document.URL.match("Content.References*") != null)
        {
        header += ": References";
        }
    else if(document.URL.match("Content.Samples*") != null)
        {
        header += ": Sample Code";
        }
    else if(document.URL.match("Content.TroubleShoot*") != null)
        {
        header += ": Trouble Shooting";
        }

    // dont display the contents link if this page is the contents
    if(document.URL.match("contents.htm") != null) contents = " ";

    if(set == HEADER_APPEND) header += str;

    header += "</TD><TD WIDTH=80 ALIGN=RIGHT STYLE='background:#9CCFFF'>" + contents + "</TD></TABLE>"

    document.write("<DIV CLASS=PAGEHEADER>"+ header +"</DIV>");
    }

var ColorIndex = 0;

function PassiveBlueCheck(obj)
    {
    var color = "BLUE";
    if(obj.currentStyle.color=="green") color = "green";

    if(obj.id.indexOf("FOO") == -1)
        {
        obj.id = "FOO" + ColorIndex ;
        obj.name = "FOO" + ColorIndex ;
        setTimeout("ClearExpr('FOO"+ColorIndex +"', '"+ color +"');", 10);
        ++ColorIndex ;
        }
    return color;
    }

function ClearExpr(id, color)
    {
    eval(id + ".style.removeExpression('color')");
    eval(id + ".style.color='" + color + "';");
    }

function WriteCode(code)
    {
    code = code.replace(/(\/\/|REM)(.*)$/gm, "<SPAN STYLE='color:green'>$1$2</SPAN>");
    code = code.replace(/\b(void|bool|char|short|int|long|float|double|unsigned|BSTR|DWORD|BOOL|__int8|__int16|__int32|__int64|break|case|class|continue|do|else|false|for|function|goto|if|in|null|return|struct|switch|template|true|var|while|try|catch)\b/g, "<SPAN STYLE='color:expression(PassiveBlueCheck(this))'>$1</SPAN>");
    code = code.replace(/\b(JOB|SCRIPT|PACKAGE|REFERENCE)\b/g, "<SPAN STYLE='color:purple'>$1</SPAN>");
    code = code.replace(/\b(LANGUAGE=|SRC=|OBJECT=|ID=)/g, "<SPAN STYLE='color:brown'>$1</SPAN>");
    code = code.replace(/\"([^\"]*)\"/g, "<SPAN STYLE='color:darkblue'>\"$1\"</SPAN>");

    document.write("<PRE>" + code + "</PRE>");
    }

function WriteSyntax(code)
    {
    code = code.replace(/(\(|\))/g, " $1 ");
    code = code.replace(/\b(\S*)\s*(\)|\,)/g, "<I>$1</I> $2");
    code = code.replace(/(\/\/)(.*)$/gim, "<SPAN STYLE='color:green;font-weight:normal'>$1$2</SPAN>");
    code = code.replace(/\b(void|bool|char|short|int|long|float|double|unsigned|BSTR|DWORD|BOOL|IDispatch|IUnknown)\b/g, "<SPAN STYLE='font-weight:normal;'>$1</SPAN>");
    code = code.replace(/\,/g, ",<BR>&nbsp;&nbsp;&nbsp;&nbsp;");

    document.write("<B>" + code + "</B>");
    }

function GetScriptEngineInfo()
    {
    var s = "";
    s += ScriptEngine() + " ";
    s += ScriptEngineMajorVersion() + ".";
    s += ScriptEngineMinorVersion() + ".";
    s += ScriptEngineBuildVersion();
    return s;
    }

document.write("<DIV ID=DEBUG></DIV>");
document.write("<DIV ID=TIP CLASS=TOOLTIP STYLE='display:none;'></DIV>");
