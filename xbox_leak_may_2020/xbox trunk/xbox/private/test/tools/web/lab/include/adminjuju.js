/*****************************************************************************
Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    adminjuju.js

Abstract:

    Functions used by the web pages

Author:

    Josh Poley (jpoley)

*****************************************************************************/

function ExecuteJob(jobID)
    {
    try
        {
        var shell = new ActiveXObject("WScript.Shell");
        shell.Environment("Process")("Path") = shell.ExpandEnvironmentStrings("%_NTDRIVE%%_NTROOT%/public/idw;%_NTDRIVE%%_NTROOT%/public/mstools;%_NTDRIVE%%_NTROOT%/public/tools;%PATH%");
        var SQLQuery = "SELECT * FROM Scheduler WHERE [Job ID]=" + jobID;
        //var driver = "DSN=LabTest;UID=admin;PWD=;";
        var driver = "DRIVER={SQL Server};SERVER=xdb01;DATABASE=TestLab;UID=TestLabUser;PWD=TestLab1.User";
        var records = new ActiveXObject("ADODB.Recordset");
        records.Open(SQLQuery, driver, 1, 2); // forward, record locking
        if(!records.EOF) 
            {
            var script = "" + records("Script");
            var d = new Date();
            records("Last Run On") = d.getVarDate();
            records("Last Result") = shell.Run(shell.ExpandEnvironmentStrings("%SystemRoot%/system32/cmd.exe /T:17 /C " + script), 0, true);
            records.Update();
            records.Close();
            window.location.reload();
            }
        }
    catch(e)
        {
        }
    }

function XBOXAction(form, list)
    {
    for(i=0; i<form.elements.length; i++)
        {
        if(form.elements.item(i).checked == true)
            {
            if(list.options[list.selectedIndex].value == "Reboot")
                {
                Razzle("%SystemRoot%/system32/cmd.exe /T:17 /C xbreboot -x " + form.elements.item(i).id);
                }
            else if(list.options[list.selectedIndex].value == "CMD")
                {
                Razzle("%SystemRoot%/system32/cmd.exe /T:17 /K xbdir -x " + form.elements.item(i).id + " xc:\\devkit\\*.*");
                }
            else
                {
                //DEBUG.innerHTML += form.elements.item(i).id + "<BR>";
                }
            }
        }
    }

