<%@ Language=JavaScript %>
<!--

Copyright (C) 1999-2000 Microsoft Corporation

Module Name:

    edit.asp

Abstract:

    Allows the user to edit a test variation

Arguments:

    testID          [form, qs]  keys used to identify a single test variation
    component       [form, qs]  keys used to identify a single test variation
    subcomponent    [form, qs]  keys used to identify a single test variation
    function        [form, qs]  keys used to identify a single test variation
    variation       [form, qs]  keys used to identify a single test variation
    logString       [form, qs]  keys used to identify a single test variation
    newcomponent    [form]      [optional] new value for the component
    newsubcomponent [form]      [optional] new value for the newsubcomponent
    newfunction     [form]      [optional] new value for the newfunction
    newvariation    [form]      [optional] new value for the newvariation
    newlogString    [form]      [optional] new value for the newlogString
    newlogLevel     [form]      [optional] new value for the newlogLevel
    newboxID        [form]      [optional] new value for the newboxID
    newthreadID     [form]      [optional] new value for the newthreadID
    newtester       [form]      [optional] new value for the newtester
    newbugID        [form]      [optional] new value for the newbugID

Author:

    Josh Poley (jpoley)

Revision History:
    04-05-2000  Created

-->

<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");
    var row = 0;

    var editlink = "";

    // sql settings
    var SQLQuery = "SELECT * FROM Log WHERE (Component='" + Request("component") + "' AND Subcomponent='"+Request("subcomponent")+"' AND [function]='"+Request("function")+"' AND Variation='"+Request("variation")+"' AND logString='"+Request("logString")+"' AND testID='"+Request("testID")+"');";

    // connect & get records
    var records = Server.CreateObject("ADODB.Recordset");
    records.Open(SQLQuery, "DRIVER={SQL Server};SERVER=xdb01;DATABASE=regression;UID=regressionuser;PWD=regressionuser", 2, 3);

    if(Request("Submit").Count >= 1 && !records.EOF)
        {
        records("component") = Request.Form("newcomponent");
        records("subcomponent") = Request.Form("newsubcomponent");
        records("function") = Request.Form("newfunction");
        records("variation") = Request.Form("newvariation");
        records("logString") = Request.Form("newlogString");
        records("logLevel") = Request.Form("newlogLevel");
        //records("boxID") = Request.Form("newboxID");
        records("threadID") = Request.Form("newthreadID");
        records("tester") = Request.Form("newtester");
        records("bugID") = Request.Form("newbugID");
        records.Update();
        }

    if(!records.EOF)
        {
        comp = (""+records("component")).replace(/\s*$/, "");
        subcomp = (""+records("subcomponent")).replace(/\s*$/, "");
        funct = (""+records("function")).replace(/\s*$/, "");
        varn = (""+records("variation")).replace(/\s*$/, "");
        comment = (""+records("logString")).replace(/\s*$/, "");
        editlink = "edit.asp?testid="+records("testID")+"&component="+Server.URLEncode(comp)+"&subcomponent="+Server.URLEncode(subcomp)+"&function="+Server.URLEncode(funct)+"&variation="+Server.URLEncode(varn)+"&logString="+Server.URLEncode(comment);
        }
%>
<HTML>
<HEAD>
    <LINK REL="SHORTCUT ICON" HREF="xicon.ico">
    <TITLE>Edit Variation</TITLE>
</HEAD>

<LINK REL="stylesheet" TYPE="text/css" HREF="include/regression.css">
<SCRIPT LANGUAGE="javascript" SRC="include/regression.js"></SCRIPT>

<BODY>
<IMG SRC="include/xicon.gif" oncontextmenu='ContextMenu(this);' STYLE="position:absolute;bottom:4;right:4">
<DIV ID=MENU CLASS=MENUSTYLE STYLE="display:none;position:absolute;bottom:14;right:20"></DIV>
<CENTER><H2>Edit Variation</H2>

<%=Date()%> <P>&nbsp;<P>

<FORM METHOD=POST ACTION="<%=editlink%>">
<TABLE CELLPADDING=2 CELLSPACING=0 BORDER=0>

<!-- Column header Row -->
<TR><TH CLASS=ROWHEAD>Field</TH><TH CLASS=ROWHEAD>Value</TH></TR>

<!-- Data Rows -->
<TR><TH>component</TH><TD><INPUT NAME=newcomponent VALUE="<%=(""+records("component")).replace(/\s*$/, "")%>" SIZE=30 MAXLENGTH=32></TD></TR>
<TR><TH>subcomponent</TH><TD><INPUT NAME=newsubcomponent VALUE="<%=(""+records("subcomponent")).replace(/\s*$/, "")%>" SIZE=30 MAXLENGTH=32></TD></TR>
<TR><TH>function</TH><TD><INPUT NAME=newfunction VALUE="<%=(""+records("function")).replace(/\s*$/, "")%>" SIZE=30 MAXLENGTH=64></TD></TR>
<TR><TH>variation</TH><TD><INPUT NAME=newvariation VALUE="<%=(""+records("variation")).replace(/\s*$/, "")%>" SIZE=30 MAXLENGTH=32></TD></TR>
<TR><TH>logString</TH><TD><TEXTAREA NAME=newlogString COLS=32 ROWS=5 MAXLENGTH=255><%=(""+records("logString")).replace(/\s*$/, "")%></TEXTAREA></TD></TR>
<TR><TH>logLevel</TH><TD><INPUT NAME=newlogLevel VALUE="<%=(""+records("logLevel")).replace(/\s*$/, "")%>" SIZE=30 MAXLENGTH=3></TD></TR>
<!-- <TR><TH>boxID</TH><TD><INPUT NAME=newboxID VALUE="<%=(""+records("boxID")).replace(/\s*$/, "")%>" SIZE=30 MAXLENGTH=10></TD></TR> -->
<TR><TH>threadID</TH><TD><INPUT NAME=newthreadID VALUE="<%=(""+records("threadID")).replace(/\s*$/, "")%>" SIZE=30 MAXLENGTH=5></TD></TR>
<TR><TH>tester</TH><TD><INPUT NAME=newtester VALUE="<%=(""+records("tester")).replace(/\s*$/, "")%>" SIZE=30 MAXLENGTH=16></TD></TR>
<TR><TH>bugID</TH><TD><INPUT NAME=newbugID VALUE="<%=(""+records("bugID")).replace(/\s*$/, "")%>" SIZE=30 MAXLENGTH=5></TD></TR>

<%
records.Close();
%>

<!-- Column Footer Row -->

</TABLE>
<BR>
<INPUT TYPE=SUBMIT NAME="Submit" VALUE="Save" STYLE="width:60"> <INPUT TYPE=BUTTON VALUE="Cancel" onClick="window.close();" STYLE="width:60">

</FORM>

</CENTER>

<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->

</BODY>
</HTML>
