<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    editBox.asp

Abstract:

    Displays information and test history of a given box

Arguments:

    required:
    mac     MAC address of a box

    optional POST elements:
    del     set to "true" to delete the given box
    xyz     modifies the entry in the DB where the column name == xyz
            Only available in admin mode


Author:

    Josh Poley (jpoley)

Notes:

    triage states:
        0   no problem
        1   problem identified
        2   notified (email sent)

    Current State
        0   "Crashed";
        1   "Not Responding";
        2   "Idle";
        3   "Running";

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var TITLE = "<TABLE><TR><TD><IMG SRC='images/xtextbig.gif' HEIGHT=36></TD><TD> <SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Details</SPAN></TD></TABLE>";
    var row = 0;
    var html = "";

    var states = new Array;
    states[0] = "Crashed";
    states[1] = "Not Responding";
    states[2] = "Idle";
    states[3] = "Running";

    var currentState;

    var returnAddress = "default.asp";

    Response.Write("<!-- '" +Request("ReturnAddress")+ "'-->");

    if(Request("ReturnAddress").Count>=1 && ""+Request("ReturnAddress") != "")
        returnAddress = ""+Request("ReturnAddress");
    else if(""+Request.ServerVariables("HTTP_REFERER") != "undefined")
        returnAddress = ""+Request.ServerVariables("HTTP_REFERER");

    returnAddress = returnAddress.replace(/\'/g, "%27");

    //
    // user clicked the Resolve button
    //
    if(Request("TestResults").Count >= 1)
        {
        var emailbody = "";
        var emailto = "";
        var cc = "";

        emailbody += "<HTML>\r\n<HEAD>\r\n";
        emailbody += "    <TITLE>Your box has been resolved</TITLE>\r\n";
        emailbody += "    <LINK REL='stylesheet' TYPE='text/css' HREF='http://xboxlab/lab/include/lab.css'>\r\n";
        emailbody += "</HEAD>\r\n\r\n<BODY RIGHTMARGIN=0 LEFTMARGIN=0><CENTER><H3>Your box has been resolved</H3><P>\r\n";
        emailbody += "<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE='table-layout:fixed'>\r\n";
        emailbody += "    <TR><TD WIDTH=90></TD><TD WIDTH=180></TD></TR>\r\n";

        // connect & get records
        var SQLQuery = "SELECT * FROM TestHistory WHERE [MAC Address]='" + Request("mac") + "' ORDER BY [Timestamp] DESC;";
        records.Open(SQLQuery, driver, 1, 2);
        if(!records.EOF)
            {
            records("Test Results") = ""+Request("TestResults");
            records.Update();
            }
        records.Close();

        // connect & get records
        SQLQuery = "SELECT * FROM BoxList WHERE [MAC Address]='" + Request("mac") + "';";
        records.Open(SQLQuery, driver, 1, 2);

        cc = RTrim(records("Crash Owner"));
        emailto = RTrim(records("Owner"));

        emailbody += "    <TR><TH STYLE='cursor:default;'>Box</TH><TD><A HREF='http://xboxlab/lab/editBox.asp?mac=" +records("MAC Address")+"'>" +records("MAC Address")+"</A></TD></TR>\r\n";
        emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";
        emailbody += "    <TR><TH STYLE='cursor:default;'>Resolved By</TH><TD>" +GetUsername()+"</TD></TR>\r\n";
        emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";
        emailbody += "    <TR><TH STYLE='cursor:default;'>Resolution</TH><TD>" +Request("TestResults")+"</TD></TR>\r\n";
        emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";
        emailbody += "    <TR><TD COLSPAN=2 ALIGN=CENTER>You are now free to reboot your XBOX</TD></TR>\r\n";
        emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";

        // update box state
        records("Current State") = 2;
        records("Triage") = 2;
        records.Update();
        records.Close();

        emailbody += "</TABLE>\r\n\r\n";
        emailbody += "</BODY></HTML>";

        cc = cc.replace(/xstress/g, "");

        SendMail(emailto, cc, GetUsername(), "Your box has been resolved", emailbody);
        Response.Redirect(returnAddress);
        }

    // sql settings
    var SQLQuery = "SELECT * FROM [BoxList] WHERE [MAC Address]='" + Request("mac") + "';";

    // connect & get records
    if(admin || Request("Submit")=="Assign") records.Open(SQLQuery, driver, 1, 2);
    else records.Open(SQLQuery, driver, 0, 1);

    if(admin && records.EOF)
        {
        records.AddNew();
        records("MAC Address") = Request("mac");
        records.Update();
        }

    if(!records.EOF)
        {
        //
        // Re assign the crash to someone else
        //
        if(Request("Submit") == "Assign")
            {
            var emailbody = "";
            emailbody += "<HTML>\r\n<HEAD>\r\n";
            emailbody += "    <TITLE>A Crashed XBOX has been Assigned to You</TITLE>\r\n";
            emailbody += "    <LINK REL='stylesheet' TYPE='text/css' HREF='http://xboxlab/lab/include/lab.css'>\r\n";
            emailbody += "</HEAD>\r\n\r\n<BODY RIGHTMARGIN=0 LEFTMARGIN=0><CENTER><H3>A Crashed XBOX has been Assigned to You</H3><P>\r\n";
            emailbody += "<TABLE WIDTH=95% CELLPADDING=3 CELLSPACING=0 BORDER=0>\r\n";

            emailbody += "    <TR><TH STYLE='cursor:default;'>Box Status Page</TH><TD><A HREF='http://xboxlab/lab/editBox.asp?mac=" +Request("mac")+"'>" +Request("mac")+"</A></TD></TR>\r\n";
            emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";
            emailbody += "    <TR><TH STYLE='cursor:default;'>Box Owner</TH><TD><A HREF='mailto:"+RTrim(Request("Owner"))+"'>" +RTrim(Request("Owner"))+"</A></TD></TR>\r\n";
            emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";
            emailbody += "    <TR><TH STYLE='cursor:default;'>Re Assigned By</TH><TD><A HREF='mailto:"+GetUsername()+"'>" +GetUsername()+"</A></TD></TR>\r\n";
            emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";
            emailbody += "    <TR><TH STYLE='cursor:default;'>Remote</TH><TD NOWRAP>REMOTE /C " +RTrim(Request("Debugger"))+" \""+RTrim(Request("Machine Name"))+"\"</TD></TR>\r\n";
            emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";
            emailbody += "    <TR><TH STYLE='cursor:default;'>Notes</TH><TD STYLE='line-break:strict;white-space:nowrap'>" + (""+Request("Notes")).replace(/\r\n/g,"<BR>")+"</TD></TR>\r\n";
            emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";
            emailbody += "    <TR><TH STYLE='cursor:default;'>Triage Notes</TH><TD STYLE='font:normal 6pt Courier; line-break:strict;white-space:nowrap'>" + (""+Request("Triage Notes")).replace(/\r\n/g,"<BR>")+"</TD></TR>\r\n";
            emailbody += "    <TR HEIGHT=2><TD></TD><TD></TD></TR>\r\n";

            emailbody += "</TABLE>\r\n\r\n";
            emailbody += "</BODY></HTML>";

            records("Crash Owner") = RTrim(Request("Crash Owner"));
            records("Notes") = RTrim(Request("Notes"));
            records.Update();
            records.Close();

            SendMail(RTrim(Request("Crash Owner")), "", GetUsername(), "A Crashed XBOX has been Assigned to You", emailbody);
            Response.Redirect(returnAddress);
            }

        if(admin && Request.Form.Count>1)
            {
            // -2 for the submit button and returnAddress field
            for(i=0; i<Request.Form.Count-2; i++)
                {
                if(Request.Form(i+1) == "null")
                    ;
                else if(records.Fields(i).Type == 135)
                    {
                    var d = new Date(Request.Form(i+1));
                    records(i) = d.getVarDate();
                    }
                else
                    {
                    records(i) = Request.Form(i+1);
                    }
                }
            records.Update();

            if(Request("Submit") == "OK")
                {
                records.Close();
                Response.Redirect(returnAddress);
                }

            }
        }
%>
<HTML>
<HEAD>
    <TITLE>XBOX Details</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/lab.css">
    <!-- <%=returnAddress%> -->
</HEAD>

<SCRIPT LANGUAGE="javascript" SRC="include/lab.js"></SCRIPT>
<BODY TOPMARGIN=0>


<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->
<FORM METHOD=POST ACTION="editBox.asp?mac=<%=Request("mac")%>">
<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">
<%
if(records.EOF)
    {
    Response.Write("No Entry with this MAC Address");
    }
else
    {
    currentState = parseInt(records("Current State"));
    for(i=0; i<records.Fields.Count; i++)
        {
        var rdonly = "";
        if(row) img = "in";
        else img = "out"
        html = "<TR><TD WIDTH=8></TD><TH WIDTH=180 STYLE='cursor:default;'>" + records.Fields(i).Name + "</TH><TD WIDTH=300>";

        if(i==0) rdonly = "READONLY";
        else rdonly = "";

        if(records.Fields(i).Type == 201) // memo
            html += "<TEXTAREA COLS=50 ROWS=7 NAME='" + records.Fields(i).Name + "'>";
        else
            html += "<INPUT STYLE='width:270' "+rdonly+" NAME='" + records.Fields(i).Name + "' TYPE=TEXT VALUE='";

        if(records.Fields(i).Type == 135)
            html += FormatShortDate(records(i));
        else
            html += RTrim(records(i));

        if(records.Fields(i).Type == 201) // memo
            html += "</TEXTAREA>";
        else
            html += "'>";

        if(records.Fields(i).Name == "Owner" || records.Fields(i).Name == "Crash Owner")
            html += "<A HREF='mailto:"+RTrim(records(i))+"'><IMG SRC='images/mail.gif' BORDER=0></A>";

        html += "</TD></TR>\r\n";

        html += "<TR HEIGHT=2><TD></TD><TD></TD><TD></TD></TR>\r\n"
        Response.Write(html);
        row == 0 ? row = 1 : row = 0;
        }
    }

html = "<TR><TD WIDTH=8></TD><TH WIDTH=180 STYLE='cursor:default;'></TH><TD ALIGN=CENTER>";
if(currentState == 0)
    {
    html += "<INPUT STYLE='width:50' ID=SUBMIT NAME=SUBMIT TYPE=SUBMIT VALUE='Assign' TITLE='Reassign the crash to someone else'>";
    if(!admin)
        html += "<DIV ALIGN=LEFT>Enter a new alias in the Crash Owner field and click the Assign button to re-assign a crash to someone else. You can also add information to the Notes field.</DIV>";
    }
if(admin)
    html += " <INPUT STYLE='width:50' ID=SUBMIT NAME=SUBMIT TYPE=SUBMIT VALUE='OK'> <INPUT STYLE='width:50' TYPE=BUTTON onClick='document.location=\""+returnAddress+"\"' VALUE='Cancel'> <INPUT STYLE='width:50' ID=SUBMIT NAME=SUBMIT TYPE=SUBMIT VALUE='Apply'>";
Response.Write(html + "</TD></TR>");

%>
</TABLE>
<INPUT TYPE=HIDDEN NAME="ReturnAddress" VALUE="<%=returnAddress%>">
</FORM>

&nbsp;<P>
<A TITLE='Open Debugger' HREF="javascript:Run('%SystemRoot%/system32/cmd.exe /T:0A /K %_NTDRIVE%%_NTROOT%/public/tools/razzle.cmd REMOTE /C <%=""+RTrim(records("Debugger")) + " " + RTrim(records("Machine Name"))%>')">
REMOTE /C <%=RTrim(records("Debugger"))%> "<%=RTrim(records("Machine Name"))%>"
</A>
<P>&nbsp;

<%
    row = 0;
    records.Close();

    var SQLQuery = "SELECT * FROM TestHistory WHERE [MAC Address]='" + Request("mac") + "' ORDER BY [Timestamp] DESC;";

    // connect & get records
    records.Open(SQLQuery, driver, 0, 1); // forward, read only
%>

<FORM METHOD=POST ACTION="editBox.asp?mac=<%=Request("mac")%>">
<TABLE CELLPADDING=3 CELLSPACING=0 BORDER=0 STYLE="table-layout:fixed">

<!-- Column header Row -->
<TR>
<TD WIDTH=15><IMG SRC="images\circle.gif"></TD>
<TH CLASS=ROWHEAD WIDTH=120>Time</TH>
<TH CLASS=ROWHEAD WIDTH=50>Test</TH>
<TH CLASS=ROWHEAD WIDTH=90>SW</TH>
<TH CLASS=ROWHEAD WIDTH=95>State</TH>
<TH CLASS=ROWHEAD WIDTH=270>Result</TH>
<TD WIDTH=15 STYLE="margin-left:0;padding-left: 0; margin-right:0;padding-right: 0;"><IMG SRC="images\circle2.gif"></TD>
</TR>

<!-- Data Rows -->
<%
var firstEntry = true;
while(!records.EOF)
    {
    html = "<TR><TD></TD>";
    html += "<TD CLASS=ROW" + row + ">" + FormatShortDate(records("Timestamp")) + "</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + ">" + records("Test Name") + "&nbsp;</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + ">" + records("SW Version") + "</TD>\r\n";
    html += "    <TD CLASS=ROW" + row + ">" + states[records("State")] + "</TD>\r\n";
    if(firstEntry && currentState == 0)
        {
        firstEntry = false;
        html += "    <TD CLASS=ROW" + row + "><INPUT STYLE='width:130' NAME='TestResults' TYPE=TEXT VALUE='" + RTrim(records("Test Results")) + "'>";
        html += "    <INPUT TYPE=SUBMIT NAME=SUBMIT Value='Resolve'></TD>\r\n";
        }
    else
        {
        html += "    <TD CLASS=ROW" + row + ">" + records("Test Results") + "</TD>\r\n";
        }
    html += "    <TD></TD></TR>\r\n\r\n";

    Response.Write(html);
    records.MoveNext();
    row == 0 ? row = 1 : row = 0;
    }
records.Close();
%>
<!-- Column footer Row -->
</TABLE>
<INPUT TYPE=HIDDEN NAME='ReturnAddress' VALUE='<%=returnAddress%>'>
</FORM>


<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
