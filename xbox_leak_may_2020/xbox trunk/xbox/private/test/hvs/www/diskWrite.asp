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
    var TITLE = "<CENTER><SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Disk Write Documentation</SPAN></CENTER>";
%>
<HTML>
<HEAD>
    <TITLE>HVS: Disk Write Documentation</TITLE>
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
        <TD WIDTH="*" COLSPAN="2" STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)">Low level and filesystem writing test</TD></TR>
    <TR>
        <TD WIDTH="100">Filename</TD>
        <TD WIDTH="*">diskWrite.xbe (0x48570004)</TD></TR>
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
            Name of the saved game of where the results are stored, (default = diskWrite)
            <P><B>Example</B><DIV CLASS=EXAMPLE>SaveName = MyResults</DIV>
            </TD></TR>

    <TR HEIGHT=10><TD COLSPAN=2></TD></TR>
    <TR>
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>Configuration</H4></TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">Test</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Sets the test mode, (default = 0). Available options are: 
            <UL>
            <LI>0 - Read Write Read test
            <LI>1 - Create files
            </UL>
            <P><B>Example</B><DIV CLASS=EXAMPLE>Test = 0</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">WriteSize</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Sets the size of the writes.
            <P><B>Example</B><DIV CLASS=EXAMPLE>WriteSize = 32</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">PauseOnExit</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Pauses before rebooting (default = 0).
            <P><B>Example</B><DIV CLASS=EXAMPLE>PauseOnExit = 1</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">Seed</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Sets the seed for the random number generator (default = 0)
            <P><B>Example</B><DIV CLASS=EXAMPLE>Seed = 1587</DIV>
            </TD></TR>


    <TR HEIGHT=10><TD COLSPAN=2></TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>Read Write Read Test</H4> (Test == 0)</TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">StartLBA</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Starting logical block address (default = 0)
            <P><B>Example</B><DIV CLASS=EXAMPLE>StartLBA = 11111<BR>StartLBA = 0x3300</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">EndLBA</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Ending logical block address (default = 0). If the value is zero, then the maximum size of the disk is used.
            <P><B>Example</B><DIV CLASS=EXAMPLE>EndLBA = 22222<BR>EndLBA = 0x33FF</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">SequentialReads</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            If set (1) then the disk will be read/write sequentially (default). Otherwise the application will read/write from a random location each time. Note that "StopAfter = x iteration(s)" is not valid in random mode, you must specify a time based StopAfter.
            <P><B>Example</B><DIV CLASS=EXAMPLE>SequentialReads = 0</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">Partition</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Specifies the hard drive's partition to perform the test on, valid values are 0 (default) through 5.
            <P><B>Example</B><DIV CLASS=EXAMPLE>Partition = 0</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">PauseOnMismatch</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Pauses the test when a buffer mismatch error occurs (default = 0).
            <P><B>Example</B><DIV CLASS=EXAMPLE>PauseOnMismatch = 1</DIV>
            </TD></TR>


    <TR HEIGHT=10><TD COLSPAN=2></TD></TR>
    <TR>
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>Create Files Test</H4> (Test == 1)</TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">Drive</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Specifies the Drive to perform the test on.
            <P><B>Example</B><DIV CLASS=EXAMPLE>Drive = T:</DIV>
            </TD></TR>

    </TABLE>




<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
