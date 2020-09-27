<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    labView.asp

Abstract:

    Displays the current status of the lab machines

Arguments:

    none

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    if((""+Request.ServerVariables("QUERY_STRING")).length >= 2)
        Response.AddHeader("ReturnAddress", "default.asp?" + Request.ServerVariables("QUERY_STRING"));
    else
        Response.AddHeader("ReturnAddress", Request.ServerVariables("URL"));
    var row = 0;
    var html = "";
    var tip = "";

    var states = new Array("Crashed", "Not Responding", "Idle", "Running");
    var imageList = new Array("xboxred.jpg", "xboxoff.jpg", "xboxyellow.jpg", "xboxgreen.jpg");
    var currentState = 0xFF;

    // sql settings
    var SQLQuery;
    if(Request("where").Count >= 1) SQLQuery = "SELECT * FROM BoxList WHERE " +Request("where")+ " ORDER BY [Current State], [Location], [Machine Name];";
    else SQLQuery = "SELECT * FROM BoxList ORDER BY [Machine Name];";

    // connect & get records
    try {
        records.Open(SQLQuery, driver, 0, 1); // forward, read only
        }
    catch(e)
        {
        Response.Write("Error Communicating with the database<P>" + e.description + " (" + e.number +")");
        Response.End();
        }

    var TITLE = "<TABLE><TR><TD><IMG SRC='images/xtextbig.gif' HEIGHT=36></TD><TD> <SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Lab</SPAN></TD></TABLE>";
%>
<HTML>
<HEAD>
    <TITLE>XBOX Lab</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/lab.css">
</HEAD>

<SCRIPT LANGUAGE="javascript" SRC="include/lab.js"></SCRIPT>
<BODY TOPMARGIN=0>

<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->

<%
var currentBench = 0;
var bench;
boxList = new Array;

function BoxInfo(s, t)
    {
    this.state = parseInt(s);
    this.tip = ""+t;
    }

while(!records.EOF)
    {
    currentState = parseInt(records("Current State"));
    if(!(currentState >=0 && currentState <=3))
        {
        records.MoveNext();
        continue;
        }

    if((""+records("Machine Name")).indexOf("xlab") == -1) 
        {
        records.MoveNext();
        continue;
        }

    currentBench = (""+records("Machine Name")).substr(4,2);
    for(box=1; box<=25; box++)
        boxList[box] = new BoxInfo(1, "No Info");

    while(!records.EOF)
        {
        currentState = parseInt(records("Current State"));
        if(!(currentState >=0 && currentState <=3))
            {
            records.MoveNext();
            continue;
            }

        if((""+records("Machine Name")).indexOf("xlab") == -1) 
            {
            records.MoveNext();
            continue;
            }

        bench = (""+records("Machine Name")).substr(4,2);
        if(bench != currentBench) break;

        tip = "<B>HW:</B> " + RTrim(records("Configuration Hardware")) + "<BR>";
        tip += "<B>VER:</B> " + RTrim(records("Current HW Version")) + "<BR>";
        if(currentState == 0)
            {
            tip += "<B>Stack:</B><BR>" + (""+records("Triage Notes")).replace(/\'|\"/g, "`").replace(/\r\n/g, "<BR>").replace(/\\/g, "/");
            }

        box = parseInt((""+records("Machine Name")).substr(6,2));
        boxList[box] = new BoxInfo(currentState, tip);

        records.MoveNext();
        }

    Response.Write("\r\n\r\n<!-- BENCH "+currentBench+"-->\r\n<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING=0 STYLE='border-top:1px solid #7CB021'><TD STYLE='cursor:hand' onClick='ExpandCollapse(BLOCK" + currentBench + ");'><H3>Bench " + currentBench + "</H3></TD><TD STYLE='text-align:right'></TD></TABLE>");

    Response.Write("<BLOCKQUOTE ID=BLOCK"+currentBench+"><TABLE BORDER=0 CELLPADDING=0 CELLSPACING=0>");
    for(row=0; row<5; row++)
        {
        Response.Write("<TR>");
        for(col=0; col<5; col++)
            {
            box = (col*5+row+1);
            Response.Write("<TD ALIGN=CENTER onMouseOver='ToolTip(\""+boxList[box].tip+"\");' onMouseOut='ToolTip();'>");
            if(boxList[box].tip == "No Info")
                Response.Write("<IMG BORDER=0 SRC='images/"+imageList[boxList[box].state]+"'>");
            else
                Response.Write("<A HREF='editBox.asp?mac=xlab"+currentBench+box+"'><IMG BORDER=0 SRC='images/"+imageList[boxList[box].state]+"'></A>");
            Response.Write("</TD>");
            }
        Response.Write("</TR>\r\n");
        }
    Response.Write("</TABLE></BLOCKQUOTE>\r\n");
    }

records.Close();
%>




<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
