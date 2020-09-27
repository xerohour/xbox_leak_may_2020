<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    default.asp

Abstract:

    Main page

Arguments:

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    var TITLE = "<CENTER><SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Read File Documentation</SPAN></CENTER>";
%>
<HTML>
<HEAD>
    <TITLE>HVS: Read File Documentation</TITLE>
    <LINK REL="SHORTCUT ICON" HREF="xbox.ico">
    <LINK REL="stylesheet" TYPE="text/css" HREF="include/hvs.css">
</HEAD>
<STYLE>
    .EXAMPLE { font:normal 9pt Courier; background-color:EEEEEE; }
    UL { margin-bottom:0;margin-top: 0; }
    TD { font:normal 9pt Arial; padding-top:4; }
</STYLE>
<BODY>

<!-- Begin Header Content -->
<!--#include file ="include/header.asp"-->
<!-- End Header Content -->

<TABLE WIDTH="100%" STYLE="border: 1px solid #000000;">
    <TR>
        <TD WIDTH="*" COLSPAN="2" STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)">Reads files as found by the filesystem</TD></TR>
    <TR>
        <TD WIDTH="100">Filename</TD>
        <TD WIDTH="*">readFile.xbe (0x48570003)</TD></TR>
    <TR>
        <TD WIDTH="100">Author</TD>
        <TD WIDTH="*"><A HREF="mailto:jpoley">jpoley</A></TD></TR>
    <TR>
        <TD WIDTH="100">Description</TD>
        <TD WIDTH="*"></TD></TR>
    </TABLE>

<P>
<HR>

<TABLE WIDTH=100%>
    <TR>
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>Core Settings</H4></TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">StopAfter</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            x "minute(s)"|"hour(s)"|"iteration(s)"  (default = 0 = infinite)
            <P><B>Example</B><DIV CLASS=EXAMPLE>StopAfter = 1 iteration(s)</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">ResultsServer</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Server that results are posted to, URL must contain a trailing forward slash. If this value is empty or does not contain a valid URL, then no results will be posted to the web.
            <P><B>Example</B><DIV CLASS=EXAMPLE>ResultsServer = http://172.26.172.13/hvs/</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">SaveName</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Name of the saved game of where the results are stored, (default = readFile)
            <P><B>Example</B><DIV CLASS=EXAMPLE>SaveName = MyResults</DIV>
            </TD></TR>

    <TR HEIGHT=10><TD COLSPAN=2></TD></TR>
    <TR>
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>Configuration</H4></TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">Drive</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            The starting drive.
            <P><B>Example</B><DIV CLASS=EXAMPLE>Drive = A:</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">ReadSize</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Number of bytes to read at a time (default = 1024). Use 131072 for best performance on a DVD.
            <P><B>Example</B><DIV CLASS=EXAMPLE>ReadSize = 131072</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">NumberOfReads</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Specifies if an area of the disk will be read once (1) or twice (2) (default = 2). If 2 is used, then the app will do a comparison of the two reads to ensure data integrity. See additional related options under the "Buffer Mismatch" category.
            <P><B>Example</B><DIV CLASS=EXAMPLE>NumberOfReads = 1</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">Dir</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            If set (1) log out the directory structure while counting the number of files (default = 0).
            <P><B>Example</B><DIV CLASS=EXAMPLE>Dir = 1</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">PauseOnExit</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Pauses before rebooting (default = 0).
            <P><B>Example</B><DIV CLASS=EXAMPLE>PauseOnExit = 1</DIV>
            </TD></TR>


    <TR HEIGHT=10><TD COLSPAN=2></TD></TR>
    <TR>
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>Buffer Mismatch</H4> (when NumberOfReads == 2)</TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">CompareBuffers</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            If set to 1 then any differences in the two buffers will be logged to the results file. Otherwise differences will not be written out (default) though will still be logged as an error.
            <P><B>Example</B><DIV CLASS=EXAMPLE>CompareBuffers = 1</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">PauseOnMismatch</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Pauses the test when a buffer mismatch error occurs (default = 0).
            <P><B>Example</B><DIV CLASS=EXAMPLE>PauseOnMismatch = 1</DIV>
            </TD></TR>
    </TABLE>




<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
