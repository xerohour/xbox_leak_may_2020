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
    var TITLE = "<CENTER><SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Reboot Documentation</SPAN></CENTER>";
%>
<HTML>
<HEAD>
    <TITLE>HVS: Reboot Documentation</TITLE>
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
        <TD WIDTH="*" COLSPAN="2" STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)">Media detection test</TD></TR>
    <TR>
        <TD WIDTH="100">Filename</TD>
        <TD WIDTH="*">reboot.xbe (0x48570005)</TD></TR>
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
            Name of the saved game of where the results are stored, (default = reboot)
            <P><B>Example</B><DIV CLASS=EXAMPLE>SaveName = MyResults</DIV>
            </TD></TR>

    <TR HEIGHT=10><TD COLSPAN=2></TD></TR>
    <TR>
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>Configuration</H4></TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">RebootType</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Specifies how the reboots are accomplished, (default = 0). Available options are: 
            <UL>
            <LI>0 - Soft reboots (HalReturnToFirmware(HalRebootRoutine))
            <LI>1 - Hard reboots (HalWriteSMBusByte(SMC_RESET_ASSERT_POWERCYCLE))
            <LI>2 - Quick reboots (XLaunchNewImage)
            <LI>3 - SMC resets (HalWriteSMBusByte(SMC_RESET_ASSERT_RESET))
            <LI>4 - Random
            </UL>
            <P><B>Example</B><DIV CLASS=EXAMPLE>RebootType = 2</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">Seed</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Sets the seed for the random number generator (default = 0)
            <P><B>Example</B><DIV CLASS=EXAMPLE>Seed = 1587</DIV>
            </TD></TR>
    </TABLE>




<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
