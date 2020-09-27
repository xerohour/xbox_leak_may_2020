<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    ping.asp

Abstract:

    Updates box information (in database) from HTTP POST

Arguments:

    machine - machine name of the xbox
    dbgIP   - IP address of the debug channel

Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="../include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");

    var mailTo = "jpoley";
    var sendErrorMail = true;

    // sql settings
    var SQLQuery = "SELECT * FROM BoxList WHERE [Machine Name]='" +Request("machine")+ "';";

    //
    // connect & get records
    //
    try {
        records.Open(SQLQuery, driver, 1, 2);
        }
    catch(e)
        {
        if(sendErrorMail) SendMail(mailTo, "", "xgirl@microsoft.com", "StressMON Ping Error", "Error: " + e.description + " (" + e.number + ")\r\n\r\nPost: \r\n" + Request.Form);
        Response.End();
        }

    //
    // try the debug IP as the machine name if we couldnt find the given name
    //
    if(records.EOF)
        {
        records.Close();

        SQLQuery = "SELECT * FROM BoxList WHERE [Machine Name]='" +Request("dbgIP")+ "';";
        try {
            records.Open(SQLQuery, driver, 1, 2);
            }
        catch(e)
            {
            if(sendErrorMail) SendMail(mailTo, "", "xgirl@microsoft.com", "StressMON Ping Error", "Error: " + e.description + " (" + e.number + ")\r\n\r\nPost: \r\n" + Request.Form);
            Response.End();
            }
        }

    //
    // Update the record
    //
    if(!records.EOF)
        {
        try
            {
            var d = new Date();
            records("Last Ping") = d.getVarDate();
            records("Friendly Name") = RTrim(Request("machine"));
            records("Configuration Video") = RTrim(Request("vmode"));
            records("Configuration Audio") = RTrim(Request("amode"));
            records("Configuration Peripherals") = RTrim(Request("periph"));
            records("Current HW Version") = RTrim(Request("hwver"));

            if(Request("romver").Count >= 1) records("Current ROM Version") = RTrim(Request("romver"));
            if(Request("swver").Count >= 1) records("Current SW Version") = RTrim(Request("swver"));
            if(Request("hwcfg").Count >= 1) records("Configuration Hardware") = RTrim(Request("hwcfg"));
            if(Request("speed").Count >= 1) records("Hardware Speed") = RTrim(Request("speed"));
            if(Request("smc").Count >= 1) records("SMC State") = RTrim(Request("smc"));

            // set state back to running
            records("Current State") = 3;
            records("Triage") = 0;
            records("Triage Notes") = "";
            records("Crash Owner") = "";
            records("Location") = RTrim(Request("dbgIP"));

            // save the data
            records.Update();

            Response.Write("DB Updated");
            }
        catch(e)
            {
            if(sendErrorMail) SendMail(mailTo, "", "xgirl@microsoft.com", "StressMON Ping Error", "Error: " + e.description + " (" + e.number + ")\r\n\r\nPost: \r\n" + Request.Form);
            }
        }
    else
        {
        Response.Write("Error - Could not find a match in the DB for the supplied name or IP<P>Post: <BR>" + Request.Form);
        }

    records.Close();

%>
