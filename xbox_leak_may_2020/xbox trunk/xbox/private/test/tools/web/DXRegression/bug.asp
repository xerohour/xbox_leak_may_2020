<%@ Language=JavaScript %>
<!--

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    bug.asp

Abstract:

    Display bug information from the raid db

Arguments:

    bug [form, qs]      the bug ID in the raid db

Author:

    Josh Poley (jpoley)

Revision History:
    04-04-2000  Created


Raid Field Names:
    Accessibility  
    AssignedTo  
    BetaID  
    BugGUID  
    BugID  
    BugTimestamp  
    Cause  
    ChangedBy  
    ChangedDate  
    ChildLinkCount  
    ClosedBy  
    ClosedDate  
    CodeChange  
    Component  
    DependentDate  
    DependentLinkStatus  
    Description  
    Environment  
    FixedFWRev
    FixedHWRev  
    FixedRev  
    HowFound  
    IssueType  
    KBArticle  
    Lang  
    LinkCount  
    MigrationStatus  
    OpenedBy  
    OpenedDate  
    OpenedFWRev  
    OpenedHWRev  
    OpenedRev  
    ParentBugDB  
    ParentBugID  
    ParentChangedDate  
    ParentResolution  
    ParentStatus  
    PendingDate  
    Priority  
    Project  
    RegressCount  
    Resolution  
    ResolvedBy  
    ResolvedDate  
    Rev  
    Severity  
    Source  
    SourceID  
    Status  
    SubComp  
    Title  

-->

<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 0;

    // sql settings
    var SQLQuery = "SELECT * FROM Bugs WHERE BugID = " + Request("bug");

    // connect & get records
    var records = Server.CreateObject("ADODB.Recordset");
    records.Open(SQLQuery, "DRIVER={SQL Server};SERVER=raid45;DATABASE=xbox;UID=xbox Read Only;PWD=QF4cSH7yr2");
%>
<HTML>
<HEAD>
    <LINK REL="SHORTCUT ICON" HREF="xicon.ico">
    <TITLE>xBox Bug Info</TITLE>
</HEAD>

<LINK REL="stylesheet" TYPE="text/css" HREF="include/regression.css">
<SCRIPT LANGUAGE="javascript" SRC="include/regression.js"></SCRIPT>

<BODY>
<IMG SRC="include/xicon.gif" oncontextmenu='ContextMenu(this);' STYLE="position:absolute;bottom:4;right:4">
<DIV ID=MENU CLASS=MENUSTYLE STYLE="display:none;position:absolute;bottom:14;right:20"></DIV>
<CENTER><H2>xBox Bug Info</H2>

<%=Date()%> <P>&nbsp;<P>

<TABLE CELLPADDING=4 CELLSPACING=0 BORDER=0 WIDTH=640>

<!-- Column header Row -->
<TR><TH CLASS=ROWHEAD WIDTH=100>Field</TH><TH CLASS=ROWHEAD>Value</TH></TR>

<!-- Data Rows -->
<%
    if(records.EOF)
        {
        Response.Write("<TR><TH>Bug ID</TH><TD>'" + Request("bug") + "' does not exist</TD></TR>");
        }
    else
        {
        %>
<TR><TH>Bug ID</TH><TD><%= records("BugID")%></TD></TR>
<TR><TH>IssueType</TH><TD><%= records("IssueType")%></TD></TR>
<TR><TH>Project</TH><TD><%= records("Project")%></TD></TR>
<TR><TH>Assigned To</TH><TD><A HREF="mailto:<%= records("AssignedTo")%>"><%= records("AssignedTo")%></A></TD></TR>
<TR><TH>Status</TH><TD><%= records("Status")%></TD></TR>
<TR><TH>Pri</TH><TD><%= records("Priority")%></TD></TR>
<TR><TH>Sev</TH><TD><%= records("Severity")%></TD></TR>
<TR><TH>Milestone</TH><TD>...</TD></TR>
<TR><TH>Title</TH><TD WIDTH=540><%= records("Title")%></TD></TR>
        <%
        } // end if()
    records.Close();
%>

<!-- Column Footer Row -->

</TABLE>
</CENTER>

<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->

</BODY>
</HTML>
