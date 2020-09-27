/*****************************************************************************
Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    lab.js

Abstract:

    Functions used by the web pages

Author:

    Josh Poley (jpoley)

*****************************************************************************/

var reloadTimer = 0;
var coolDirection = 1;

function Nav(o)
    {
    document.location = o;
    }

function NavNewWindow(o)
    {
    window.open(o);
    }

function ContextMenu(obj)
    {
    var i = 0;
    items = new Array;
    items[i++] = new MenuItem("default.asp", "Lab Home", "Nav", "images/page.gif");
    items[i++] = new MenuItem("-");
    items[i++] = new MenuItem("http://xbox", "XBox Home", "NavNewWindow", "images/page.gif");
    items[i++] = new MenuItem("http://xbox/specs", "Specs", "NavNewWindow", "images/page.gif");
    MenuShow(items);
    event.returnValue = false;
    }

function MenuDocClickHandler()
    {
    if(event.srcElement != MENU)
        {
        document.onclick="";
        MENU.style.display = "none";
        }
    }

function MenuItem(c, t, a, s)
    {
    this.context = c;
    this.text = t;
    this.action = a;
    this.src = s;

    return this;
    };

function MenuShow(items)
    {
    document.onclick = MenuDocClickHandler;

    display = "<TABLE BORDER=0 CELLSPACING=0>";

    var i;

    for(i=0; i < items.length; i++)
        {
        display += "<TR>";
        
        if(items[i].text == "-" || items[i].text == null)
            {
            display += "<TR>";
            display += "<TD WIDTH=16 STYLE='font:normal 1pt Arial; border-bottom:2px groove #777777;'>&nbsp;</TD>";
            display += "<TD STYLE='font:normal 1pt Arial; border-bottom:2px groove #777777;'>&nbsp;</TD>";
            display += "<TD WIDTH=16 STYLE='font:normal 1pt Arial; border-bottom:2px groove #777777;'>&nbsp;</TD>";
            display += "</TR>";
            }
        else
            {
            display += "<TR onClick='"+items[i].action+"(\""+items[i].context+"\");' onMouseOut='this.style.textDecoration=\"none\";' onMouseOver='this.style.textDecoration=\"underline\";'>";
            display += "<TD WIDTH=16>";
            if(items[i].src != null) display += "<IMG SRC='"+items[i].src+"'>";
            display += "</TD>";
            display += "<TD VALIGN=MIDDLE CLASS=MENUTEXT>"+items[i].text+"</TD>";
            display += "<TD WIDTH=16></TD>";
            display += "</TR>";
            }
        }

    display += "</TABLE>";

    MENU.innerHTML = display;

    //MENU.style.pixelLeft = event.x + document.body.scrollLeft;
    //MENU.style.pixelTop = event.y + document.body.scrollTop;
    MENU.style.display = "block";
    }

function c()
    {
    if(event.shiftKey != true) return;
    if(event.altKey != true) return;

    setTimeout("m();", 0);
    COOL.style.color = "7CB021";
    COOL.innerHTML = "<B>Fuck SONY</B>";
    }

function m()
    {
    COOL.style.pixelTop = (Math.sin(coolDirection)+1) * document.body.clientHeight/2.1;
    COOL.style.pixelLeft = (Math.cos(coolDirection)+1) * document.body.clientWidth/2.4;
    coolDirection += .05;
    setTimeout("m();", 75);
    }

/*
function WriteTempFile(script, ext)
    {
    var filesystem = new ActiveXObject("Scripting.FileSystemObject");
    var filename = filesystem.GetSpecialFolder(2) + filesystem.GetTempName() + ext;
    var re = new RegExp("\\\\", "g");
    filename = filename.replace(re, "/");

    var file = filesystem.CreateTextFile(filename, true);
    file.Write(script);
    file.Close();

    setTimeout("DeleteFile('"+filename+"');", 5000);
    setTimeout("DeleteFile('"+filename+"');", 10000);

    return filename;
    }

function DeleteFile(file)
    {
    try
        {
        var filesystem = new ActiveXObject("Scripting.FileSystemObject");
        filesystem.DeleteFile(file);
        }
    catch(e)
        {
        }
    }
*/
function Run(command)
    {
    try
        {
        var shell = new ActiveXObject("WScript.Shell");
        shell.Environment("Process")("Path") = shell.ExpandEnvironmentStrings("%_NTDRIVE%%_NTROOT%/public/idw;%_NTDRIVE%%_NTROOT%/public/mstools;%_NTDRIVE%%_NTROOT%/public/tools;%PATH%");
        shell.Run(shell.ExpandEnvironmentStrings( command ), 9, false);
        }
    catch(e)
        {
        }
    }

function Razzle(command)
    {
    try
        {
    var shell = new ActiveXObject("WScript.Shell");
    shell.Environment("Process")("Path") = shell.ExpandEnvironmentStrings("%_NTDRIVE%%_NTROOT%/public/idw;%_NTDRIVE%%_NTROOT%/public/mstools;%_NTDRIVE%%_NTROOT%/public/tools;%PATH%");
    shell.Run(shell.ExpandEnvironmentStrings( command ), 9, false);
        }
    catch(e)
        {
        }
    }

function AutoUpdate()
    {
    var state = "CHECKED";

    if(AUTOUPDATE.checked == false)
        {
        state = "";
        clearTimeout(reloadTimer);
        }
    else
        {
        var querystring = SetParam(document.location.search, "refresh", state);
        reloadTimer = setTimeout("document.location.search = '" + querystring + "'", 60000);
        }
    }

function SetParam(str, name, value)
    {
    var argument;

    if(value == "" || value == null || value == undefined) argument = name;
    else argument = name + "=" + value;

    if(str.indexOf("?") == -1) 
        {
        return "?" + argument;
        }
    
    // strip off the ?
    str = str.substr(1);

    var args = str.split("&");
    for(i in args)
        {
        var pair = args[i].split("=");
        if(pair[0] == name)
            {
            args[i] = argument;
            return "?" + args.join("&");
            }
        }

    args[args.length] = argument;

    return "?" + args.join("&");
    }

function ExpandCollapse(o)
    {
    if(o.style.display == "none")
        {
        o.style.display = "block";
        }
    else
        {
        o.style.display = "none";
        }
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
