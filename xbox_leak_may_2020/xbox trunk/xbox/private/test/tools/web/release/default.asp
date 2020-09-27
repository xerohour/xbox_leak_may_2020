<%@ Language=JavaScript %>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    ReleaseCheckList.asp

Abstract:

    Displays the current status of the ship process

Arguments:

    none

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 0;
    var html = "";
    var description;
    var disabled;
    var admin = false;

    var currentUser = ""+Request.ServerVariables("AUTH_USER");
    currentUser = currentUser.replace(/[^\\]*\\(.*)/g, "$1");
    currentUser = currentUser.toUpperCase();

    if(currentUser == "JOHNDALY" || currentUser == "WINTHC" || currentUser == "MARKMC" || currentUser == "JPOLEY") admin = true;

    var currentState = 0xFF;
    var steps = new Array;
    steps[0] = "";
    steps[1] = "Doc review/draft";
    steps[2] = "Setup";
    steps[3] = "Test verification";
    steps[4] = "Staging";
    steps[5] = "Staging Verification";
    steps[6] = "Posting";
    steps[7] = "Posting verification";
    steps[8] = "War announcement mail";
    steps[9] = "Announcement mails";
    steps[10] = "Manufacturing: docs and media";
    steps[11] = "Finish";

    // sql settings
    var SQLQuery = "SELECT * FROM [Checklist] ORDER BY [Step];";
    var driver = "DRIVER={SQL Server};SERVER=xdb01;DATABASE=ReleaseChecklist;UID=ReleaseChecklistUser;PWD=ReleaseChecklist1.User";

    if(Request("SUBMIT").Count >= 1)
        {
        records.Open(SQLQuery, driver, 1, 2);
        while(!records.EOF)
            {
            if(admin || RTrim(records("Owner")).toUpperCase() == currentUser)
                {
                if(""+Request("step" + records("Step")) == "on")
                    records("State") = "CHECKED";
                else
                    records("State") = "off";
                }
            records.MoveNext();
            }

        records.Close();
        Response.Redirect("default.asp"); // clean the post data
        }

    records.Open(SQLQuery, driver, 0, 1); // forward, read only
%>
<HTML>
<HEAD>
    <TITLE>XDK Web Release Check List</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/lab.css">
</HEAD>

<BODY background="http://xbox/specs/pm/AprilXDKShipChecklist_files/image001.gif">

<h1>XDK Web Release Check List</h1>

<p><span style='font-size:11.0pt;font-family:"Comic Sans MS"'>Checklist: (Owner), please check off each item as it is accomplished so we can look at this page and see the current release status</span></p>

<span style='font-size:11.0pt;font-family:"Comic Sans MS"'>For a possibly more detailed explanation of each task, please refer to:</span><BR>
<span style='font-size:11.0pt;font-family:"Comic Sans MS"'>&nbsp;&nbsp;&nbsp;&nbsp;
<a href="http://xbox/specs/pm/XDK Monthly Release Plan.doc">http://xbox/specs/pm/XDK Monthly Release Plan.doc</a></span><P>

<span style='font-size:11.0pt;font-family:"Comic Sans MS"'>Can't find someone? Look in the <a href="http://xbox/Team/contacts/">contact list</a></span><BR>


<p></p>

<FORM METHOD=POST ACTION="default.asp">
<CENTER><INPUT TYPE=SUBMIT VALUE=Save NAME=SUBMIT ID=SUBMIT></CENTER>

<%
while(!records.EOF)
    {
    row = 0;
    currentState = parseInt(""+records("Major Step"));
    %>

<P><B>Step <%=records("Major Step")%>: <%=steps[records("Major Step")]%></B>
<TABLE WIDTH=100% CELLPADDING=3 CELLSPACING=0 BORDER=0>
<%
        while(!records.EOF)
            {
            if(records("Major Step") != currentState) break;

            disabled = "DISABLED";
            description = ""+records("Description");
            description = description.replace(/\[([^\]]*)\]/g, "<A HREF='mailto:$1&subject=Release Checklist'>$1</A>");
            description = description.replace(/\{([^\}]*)\}/g, "<A HREF='file:$1'>$1</A>");
            if(description == null) description = ""+records("Description")
            if(admin || RTrim(records("Owner")).toUpperCase() == currentUser) disabled = "";
            html = "<TR><TD WIDTH=20 VALIGN=TOP><INPUT TYPE=CHECKBOX " + records("State") + " " + disabled + " NAME=step" +records("Step")+ "></TD>";
            html += "<TD VALIGN=TOP>(<A HREF='mailto:" + RTrim(records("Owner")) + "&subject=Release Checklist'>" + RTrim(records("Owner")) + "</A>) " + description + "</TD>\n";
            html += "</TR>\r\n";

            Response.Write(html);
            records.MoveNext();
            row == 0 ? row = 1 : row = 0;
            }
        Response.Write("</TABLE>");
        Response.Flush();
        }
records.Close();
%>

<P>
<CENTER><INPUT TYPE=SUBMIT VALUE=Save NAME=SUBMIT ID=SUBMIT></CENTER>
</FORM>


</BODY>
</HTML>
