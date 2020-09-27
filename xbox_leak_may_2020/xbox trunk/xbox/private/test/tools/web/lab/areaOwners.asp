<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    areaOwners.asp

Abstract:

    Displays who owns which component

Arguments:

    none

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var TITLE = "<TABLE><TR><TD><IMG SRC='images/xtextbig.gif' HEIGHT=36></TD><TD> <SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Area Owners</SPAN></TD></TABLE>";
    var row = 0;
    var html = "";

    var states = new Array;
    states[0] = "Crashed";
    states[1] = "Not Responding";
    states[2] = "Idle";
    states[3] = "Running";

    // sql settings
    var SQLQuery;
    SQLQuery = "SELECT * FROM [Owners] ORDER BY [Area], [Owner];";

    // connect & get records
    try 
        {
        records.Open(SQLQuery, driver, 0, 1); // forward, read only
        }
    catch(e)
        {
        Response.Write("Error Communicating with the database<P>" + e.description + " (" + e.number +")");
        Response.End();
        }
%>
<HTML>
<HEAD>
    <TITLE>XBOX Area Owners</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/lab.css">
</HEAD>

<SCRIPT LANGUAGE="javascript" SRC="include/lab.js"></SCRIPT>
<BODY TOPMARGIN=0>


<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->

<CENTER>

To assign an owner to an API, add the function name (or substring) to the below list.<P>

<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">

<!-- Column header Row -->
<TR>
<TD WIDTH=15><IMG SRC="images\circle.gif"></TD>
<TH CLASS=ROWHEAD WIDTH=200>Area</TH>
<TH CLASS=ROWHEAD WIDTH=100>Owner</TH>
<TH CLASS=ROWHEAD WIDTH=28>Del</TH>
<TD WIDTH=15 STYLE="margin-left:0;padding-left: 0; margin-right:0;padding-right: 0;"><IMG SRC="images\circle2.gif"></TD>
</TR>

<!-- Data Rows -->
<%
while(!records.EOF)
    {
    html = "<TR><TD></TD>";
    html += "<TD CLASS=ROW" + row + ">" + records("Area") + "</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + "><A HREF='mailto:" + records("Owner") + "&subject=Area Ownership: " + records("Area") + "'>" + records("Owner") + "</A></TD>\r\n";
    html += "    <TD CLASS=ROW" + row + " ALIGN=CENTER><A HREF='editOwner.asp?del=true&area=" +records("Area")+ "&owner="+records("Owner")+"'><IMG ALT='Delete' BORDER=0 SRC='images/trash.gif'></A></TD>\r\n";
    html += "    <TD></TD></TR>\r\n\r\n";

    Response.Write(html);
    records.MoveNext();
    row == 0 ? row = 1 : row = 0;
    }
records.Close();
%>
<!-- Column footer Row -->
<FORM METHOD=POST ACTION="editOwner.asp?add=true">
<TR>
<TD></TD>
<TD><INPUT STYLE='width:148;height:18' NAME='Area' MAXLENGTH=50 TYPE=TEXT VALUE=''></TD>
<TD><INPUT STYLE='width:58;height:18' NAME='Owner' MAXLENGTH=12 TYPE=TEXT VALUE=''></TD>
<TD ALIGN=CENTER><INPUT TYPE=IMAGE NAME='Save' ALT='Add New' BORDER=0 SRC='images/edit.gif' VALUE='OK'></TD>
<TD></TD>
</TR>
</FORM>
</TABLE>
</CENTER>

<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
