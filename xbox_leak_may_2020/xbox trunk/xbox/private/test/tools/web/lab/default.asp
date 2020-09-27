<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    default.asp

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

    var currentState = 0xFF;
    var states = new Array;
    states[0] = "Crashed";
    states[1] = "Not Responding";
    states[2] = "Idle";
    states[3] = "Running";

    var counts = new Array(0, 0, 0, 0, 0);
    var uptime = new Array(0, 0, 0, 0, 0);

    // sql settings
    var SQLQuery;
    var where = "";
    var orderBy = "ORDER BY [Current State], [Machine Name]";

    if(Request("where").Count >= 1) where = " WHERE " + Request("where");
    if(Request("order").Count >= 1) orderBy = " ORDER BY " + Request("order");

    SQLQuery = "SELECT * FROM BoxList "+where+" " + orderBy + ";";

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
while(!records.EOF)
    {
    row = 0;
    currentState = parseInt(records("Current State"));
    if(!(currentState >=0 && currentState <=3))
        {
        records.MoveNext();
        continue;
        }
    Response.Write("\n\n\n\n<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING=0 STYLE='border-top:1px solid #7CB021'><TD STYLE='cursor:hand' onClick='ExpandCollapse(BLOCK" + currentState + ");'><H3>" + states[ records("Current State") ] + "</H3></TD><TD STYLE='text-align:right'></TD></TABLE>");
    %>

<FORM ID=ACTIONFORM<%=currentState%>>
<BLOCKQUOTE ID=BLOCK<%=currentState%>>
<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">

<!-- Column header Row -->
<TR>
<TD WIDTH=15><IMG SRC="images\circle.gif"></TD>
<TH CLASS=ROWHEAD WIDTH=90>Machine</TH>
<TH CLASS=ROWHEAD WIDTH=90>Name</TH>
<TH CLASS=ROWHEAD WIDTH=100>Owner</TH>
<% if(currentState == 0)
        Response.Write("<TH CLASS=ROWHEAD WIDTH=100>Assigned To</TH>");
%>
<TH CLASS=ROWHEAD WIDTH=90>SW</TH>
<TH CLASS=ROWHEAD WIDTH=90>ROM</TH>
<TH CLASS=ROWHEAD WIDTH=28>Info</TH>
<TH CLASS=ROWHEAD WIDTH=20>&nbsp;</TH>
<TD WIDTH=15 STYLE="margin-left:0;padding-left: 0; margin-right:0;padding-right: 0;"><IMG SRC="images\circle2.gif"></TD>
</TR>

<!-- Data Rows -->
<%
        while(!records.EOF)
            {
            if(records("Current State") != currentState) break;

            ++counts[currentState];
            ++counts[4]; // total count
            var d = new Date(records("Last Ping") - records("Last Reboot"));
            if(d.getTime() >= 0) uptime[currentState] += d.getTime();
            tip = "<B>HW:</B> " + RTrim(records("Configuration Hardware")) + "<BR>";
            tip += "<B>VER:</B> " + RTrim(records("Current HW Version")) + "<BR>";
            if(currentState == 0)
                {
                tip += "<B>Stack:</B><BR>" + (""+records("Triage Notes")).replace(/\'|\"/g, "`").replace(/\r\n/g, "<BR>").replace(/\\/g, "/");
                }

            html = "<TR><TD></TD>";
            html += "<TD CLASS=ROW" + row + " onMouseOver='ToolTip(\""+tip+"\");' onMouseOut='ToolTip();'><A TITLE='Open Debugger' HREF=\"javascript:Run('%SystemRoot%/system32/cmd.exe /T:0A /K %_NTDRIVE%%_NTROOT%/public/tools/razzle.cmd REMOTE /C " + RTrim(records("Debugger")) + " " + RTrim(records("Machine Name")) + "')\">" + RTrim(records("Machine Name")) + "</A></TD>\n";
            html += "    <TD CLASS=ROW" + row + ">" + RTrim(records("Friendly Name")) + "</TD>\n";
            html += "    <TD CLASS=ROW" + row + "><A HREF='mailto:" + RTrim(records("Owner")) + "'>"+RTrim(records("Owner"))+"</A></TD>\n";
            if(currentState == 0)
                html += "    <TD CLASS=ROW" + row + "><A HREF='mailto:" + RTrim(records("Crash Owner")) + "'>"+RTrim(records("Crash Owner"))+"</A></TD>\n";
            html += "    <TD CLASS=ROW" + row + ">" + RTrim(records("Current SW Version")) + "</TD>\n";
            html += "    <TD CLASS=ROW" + row + ">" + RTrim(records("Current ROM Version")) + "</TD>\n";
            html += "    <TD CLASS=ROW" + row + " ALIGN=CENTER><A HREF='editBox.asp?mac=" + RTrim(records("MAC Address")) + "'><IMG ALT='Edit / Details' SRC='images/edit.gif' BORDER=0></A></TD>\n";
            if(admin)
                html += "    <TD CLASS=ROW" + row + "><INPUT TYPE=checkbox ID=" + RTrim(records("Machine Name")) + "></TD>\n";
            else
                html += "    <TD CLASS=ROW" + row + "></TD>\n";
            html += "    <TD></TD></TR>\n\n";

            Response.Write(html);
            records.MoveNext();
            row == 0 ? row = 1 : row = 0;
            }
%>
<!-- Column footer Row -->
<TR><TD></TD>
<TD ALIGN=RIGHT><%=counts[currentState]%> Box<%=(counts[currentState]>1?"es":"&nbsp;&nbsp;")%></TD>
<TD CLASS=ROWFOOT COLSPAN=6 ALIGN=RIGHT>
<% if(admin) { %>
Action for checked XBOXs 
<SELECT ID=ACTIONLIST<%=currentState%> SIZE=1>
    <OPTION VALUE='Reboot'>Reboot
    <OPTION VALUE='CMD'>CMD Prompt
    </SELECT>
<INPUT TYPE=button VALUE='GO' onClick="XBOXAction(ACTIONFORM<%=currentState%>, ACTIONLIST<%=currentState%>)";>
<% } %>
</TD><TD></TD></TR>
</TABLE>

</BLOCKQUOTE></FORM>
<%
        Response.Flush();
        }

records.Close();
%>


<!-- Summary -->
<TABLE WIDTH=100% CELLPADDING=0 CELLSPACING=0 STYLE='border-top:1px solid #7CB021'><TD STYLE='cursor:hand' onClick='ExpandCollapse(BLOCKSUM);'><H3>Summary</H3></TD><TD STYLE='text-align:right'></TD></TABLE>
<BLOCKQUOTE ID=BLOCKSUM><TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0  STYLE="table-layout:fixed">
<TR><TD WIDTH=15><IMG SRC="images\circle.gif"></TD>
    <TH WIDTH=130 CLASS=ROWHEAD>State</TH>
    <TH WIDTH=120 CLASS=ROWHEAD COLSPAN=2>Count</TH>
    <TH WIDTH=120 CLASS=ROWHEAD COLSPAN=2>Uptime (hr)</TH>
    <TD WIDTH=15 STYLE="margin-left:0;padding-left: 0; margin-right:0;padding-right: 0;"><IMG SRC="images\circle2.gif"></TD></TR>
<TR><TD></TD><TD><%=states[3]%></TD><TD ALIGN=RIGHT><%=counts[3]%></TD><TD ALIGN=RIGHT> (<%=counts[4]?Format2Places(100*counts[3]/counts[4]):0%>%)</TD><TD ALIGN=RIGHT><%=counts[3]?Format2Places(uptime[3]/1000/3600/counts[3]):0%></TD><TD></TD></TR>
<TR><TD></TD><TD><%=states[0]%></TD><TD ALIGN=RIGHT><%=counts[0]%></TD><TD ALIGN=RIGHT> (<%=counts[4]?Format2Places(100*counts[0]/counts[4]):0%>%)</TD><TD ALIGN=RIGHT><%=counts[0]?Format2Places(uptime[0]/1000/3600/counts[0]):0%></TD><TD></TD></TR>
<TR><TD></TD><TD><%=states[1]%></TD><TD ALIGN=RIGHT><%=counts[1]%></TD><TD ALIGN=RIGHT> (<%=counts[4]?Format2Places(100*counts[1]/counts[4]):0%>%)</TD><TD ALIGN=RIGHT></TD><TD></TD></TR>
<TR><TD></TD><TD><%=states[2]%></TD><TD ALIGN=RIGHT><%=counts[2]%></TD><TD ALIGN=RIGHT> (<%=counts[4]?Format2Places(100*counts[2]/counts[4]):0%>%)</TD><TD ALIGN=RIGHT></TD><TD></TD></TR>
<TR><TD></TD><TD><B>Total</B></TD><TD ALIGN=RIGHT><B><%=counts[4]%></B></TD><TD ALIGN=RIGHT> </TD><TD></TD><TD></TD></TR>
</TABLE>
</BLOCKQUOTE>


<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
