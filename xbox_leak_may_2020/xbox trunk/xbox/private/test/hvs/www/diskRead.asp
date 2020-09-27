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
    var TITLE = "<CENTER><SPAN STYLE='color:80C040;font:italic bold 30pt Arial'>Disk Read Documentation</SPAN></CENTER>";
%>
<HTML>
<HEAD>
    <TITLE>HVS: Disk Read Documentation</TITLE>
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
        <TD WIDTH="*" COLSPAN="2" STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)">Low level reading from the dvd or hard disk</TD></TR>
    <TR>
        <TD WIDTH="100">Filename</TD>
        <TD WIDTH="*">diskRead.xbe (0x48570002)</TD></TR>
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
            Name of the saved game of where the results are stored, (default = diskRead)
            <P><B>Example</B><DIV CLASS=EXAMPLE>SaveName = MyResults</DIV>
            </TD></TR>

    <TR HEIGHT=10><TD COLSPAN=2></TD></TR>
    <TR>
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>Configuration</H4></TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">Device</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            The device path, (default = \Device\cdrom0). Available options are: 
            <UL>
            <LI>\Device\cdrom0
            <LI>\Device\Harddisk0\Partition0 - 19541088 bytes
            <LI>\Device\Harddisk0\Partition1 - 9997568 bytes
            <LI>\Device\Harddisk0\Partition2 - 1023840 bytes
            <LI>\Device\Harddisk0\Partition3 - 1535776 bytes
            <LI>\Device\Harddisk0\Partition4 - 1535776 bytes
            <LI>\Device\Harddisk0\Partition5 - 1535776 bytes
            </UL>
            <P><B>Example</B><DIV CLASS=EXAMPLE>Device = \Device\Harddisk0\Partition0</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">ReadSizeMin</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Num blocks to read at a time (default = 32), the actual read size will be a random number between ReadSizeMin and ReadSizeMax. If ReadMode equals 1 (raw DVD) then ReadSizeMax cannot be bigger than 64.
            <P><B>Example</B><DIV CLASS=EXAMPLE>ReadSizeMin = 16</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">ReadSizeMax</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Num blocks to read at a time (default = 32), the actual read size will be a random number between ReadSizeMin and ReadSizeMax. If ReadMode equals 1 (raw DVD) then ReadSizeMax cannot be bigger than 64.
            <P><B>Example</B><DIV CLASS=EXAMPLE>ReadSizeMax = 64</DIV>
            </TD></TR>
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
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">ReadMode</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Selects how data will be read from the device (default = 0).  Available options are: 
            <UL>
            <LI>0 - Detects best mode based on the media.
            <LI>1 - Raw DVD (DeviceIoControl call)
            <LI>2 - CD Audio (DeviceIoControl call)
            <LI>3 - Low level NtReadFile call
            </UL>
            If your Device is the hard drive, then only option 3 is available. If reading data (no audio) from a CD or DVD, then options 1 and 3 are available.
            <P><B>Example</B><DIV CLASS=EXAMPLE>ReadMode = 1</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">NumberOfReads</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Specifies if an area of the disk will be read once (1) or twice (2) (default = 2). If 2 is used, then the app will do a comparison of the two reads to ensure data integrity. See additional related options under the "Buffer Mismatch" category.
            <P><B>Example</B><DIV CLASS=EXAMPLE>NumberOfReads = 1</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">SequentialReads</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            If set (1) then the disk will be read sequentially (default). Otherwise the application will read from a random location each time. Note that "StopAfter = x iteration(s)" is not valid in random mode, you must specify a time based StopAfter.
            <P><B>Example</B><DIV CLASS=EXAMPLE>SequentialReads = 0</DIV>
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
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>DVD Settings</H4> (when ReadMode == 1)</TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">RequestSense</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            0=No extra requests (default = 0), 1=Request only after buff mismatch, 2=Always request sense
            <P><B>Example</B><DIV CLASS=EXAMPLE>RequestSense = 2</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">EnableRetries</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            1=Retries (and speed control enabled). (default = 0)
            <P><B>Example</B><DIV CLASS=EXAMPLE>EnableRetries = 1</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">DefaultSpeed</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">  
            Sets the initial rotational speed of the DVD drive (default = 2). Available options are: 
            <UL>
            <LI>2 - ~3000rpm, 2x-5x
            <LI>1 - ~2000rpm, 4/3x-10/3x
            <LI>0 - ~1000rpm, 2/3x-5/3x
            </UL>
            <P><B>Example</B><DIV CLASS=EXAMPLE>DefaultSpeed = 1</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">DelayOn062E</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Delay on 06/2E errors (in ms) (default = 300)
            <P><B>Example</B><DIV CLASS=EXAMPLE>DelayOn062E = 600</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">SeekOn062E</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Seek to LBA 0 on 6/2E errors (default = 0)
            <P><B>Example</B><DIV CLASS=EXAMPLE>SeekOn062E = 1</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">SpeedDelay0to1<BR>SpeedDelay1to2<BR>SpeedDelay2to1<BR>SpeedDelay1to0<BR>SpeedDelay0to2<BR>SpeedDelay2to0</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Sets the sleep time (in ms) that is enforced when the drive is asked to speed up or slow down based on read successes / errors. Default for all six options is 0 ms.
            <P><B>Example</B><DIV CLASS=EXAMPLE>
                SpeedDelay0to1 = 300<BR>
                SpeedDelay1to2 = 300<BR>
                SpeedDelay2to1 = 300<BR>
                SpeedDelay1to0 = 300<BR>
                SpeedDelay0to2 = 600<BR>
                SpeedDelay2to0 = 600</DIV>
            </TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">SummaryInternalErrs</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Show internal errors in final summary distribution (default = 0)
            <P><B>Example</B><DIV CLASS=EXAMPLE>SummaryInternalErrs = 1</DIV>
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


    <TR HEIGHT=10><TD COLSPAN=2></TD></TR>
    <TR>
        <TD WIDTH=* COLSPAN=2 STYLE="FILTER: progid:DXImageTransform.Microsoft.gradient(GradientType=1, startColorstr=#E0FFB0, endColorstr=#FFFFFF)"><H4>Exclude Blocks</H4></TD></TR>
    <TR VALIGN=TOP>
        <TD WIDTH=100 STYLE="font-weight:bold;border-top: 1px solid #000000;">Exclude</TD>
        <TD WIDTH=* STYLE="border-top: 1px solid #000000;">
            Specifies a LBA range that will be skipped (e.g. not read). Multiple Exclude settings are allowed.
            <P><B>Example</B><DIV CLASS=EXAMPLE>
                ;Exclude security blocks for ZZ01001W-R3<BR>
                Exclude = 91524-95620<BR>
                Exclude = 249324-253420<BR>
                Exclude = 480122-484218<BR>
                Exclude = 632996-637092<BR>
                Exclude = 794424-798520<BR>
                Exclude = 941640-945736<BR>
                Exclude = 1100202-1104298<BR>
                Exclude = 1258118-1262214<BR>
                Exclude = 1783554-1787650<BR>
                Exclude = 2095900-2099996<BR>
                Exclude = 2248718-2252814<BR>
                Exclude = 2557052-2561148<BR>
                Exclude = 2716512-2720608<BR>
                Exclude = 2866000-2870096<BR>
                Exclude = 3103876-3107972<BR>
                Exclude = 3256974-3261070<BR>
            </DIV>
            </TD></TR>
    </TABLE>




<!-- Begin Footer Content -->
<!--#include file ="include/footer.asp"-->
<!-- End Footer Content -->
</BODY>
</HTML>
