<%@ Language=JavaScript EnableSessionState=False%>
<!--**************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

The contents on this page are MICROSOFT CONFIDENTIAL

Module Name:

    editOwner.asp

Abstract:

    Displays information and test history of a given box

Arguments:

    required:
    owner   The user to assign to an API
    area    API substring to associate a given user to

    optional POST elements:
    del     set to "true" to delete the specified owner/area pair


Author:

    Josh Poley (jpoley)

**************************************************************************-->
<!--#include file ="include/lab.asp"-->
<%
    Response.AddHeader("Cache-Control", "no-cache");
    Response.AddHeader("Pragma", "no-cache");

    // delete an entry
    if(Request("del") == "true")
        {
        var SQLQuery = "DELETE FROM [Owners] WHERE [Area]='" + Request("area") + "' AND [Owner]='" + Request("owner") + "';";
        records.Open(SQLQuery, driver, 1, 2);
        Response.Redirect("areaOwners.asp");
        }

    // sql settings
    var SQLQuery = "SELECT * FROM [Owners];";

    // connect & get records
    records.Open(SQLQuery, driver, 1, 2);

    records.AddNew();
    records("Owner") = Request("Owner");
    records("Area") = Request("Area");
    records.Update();
    records.Close();
    Response.Redirect("areaOwners.asp");
%>
