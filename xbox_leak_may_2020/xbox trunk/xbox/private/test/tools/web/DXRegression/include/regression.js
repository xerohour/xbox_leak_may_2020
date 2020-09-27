/*
Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    regression.js

Abstract:

    Functions used by the regression web pages

Author:

    Josh Poley (jpoley)

Revision History:
    04-04-2000  Created

*/

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
    items[i++] = new MenuItem("default.asp", "Regression Home", "Nav", "include/page.gif");
    items[i++] = new MenuItem("-");
    items[i++] = new MenuItem("http://xbox", "xBox Home", "NavNewWindow", "include/page.gif");
    items[i++] = new MenuItem("http://xbox/specs", "Specs", "NavNewWindow", "include/page.gif");
    items[i++] = new MenuItem("http://xbox", "Test Plans", "NavNewWindow", "include/page.gif");
    items[i++] = new MenuItem("-");
    items[i++] = new MenuItem("file://cpitgcfs01/xboxro", "Source", "NavNewWindow", "include/folder.gif");
    items[i++] = new MenuItem("XBoxRW.rdq", "RAID", "NavNewWindow", "include/raid.gif");
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
            display += "<TD WIDTH=16 STYLE='font:normal 1pt Arial; border-bottom:2px groove #00FF00;'>&nbsp;</TD>";
            display += "<TD STYLE='font:normal 1pt Arial; border-bottom:2px groove #00FF00;'>&nbsp;</TD>";
            display += "<TD WIDTH=16 STYLE='font:normal 1pt Arial; border-bottom:2px groove #00FF00;'>&nbsp;</TD>";
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
    COOL.style.color = "#AA0000";
    COOL.innerHTML = "<B>Fuck SONY</B>";
    }

function m()
    {
    COOL.style.pixelTop = (Math.sin(coolDirection)+1) * document.body.clientHeight/2.1;
    COOL.style.pixelLeft = (Math.cos(coolDirection)+1) * document.body.clientWidth/2.4;
    coolDirection += .05;
    setTimeout("m();", 75);
    }

