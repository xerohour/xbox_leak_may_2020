/*
Copyright (C) Microsoft Corporation

Module Name:

    utils.js

Abstract:

    JScript helper functions + logging object

Author:

    Josh Poley (jpoley)

Revision History:

*/




/*

Routine Description:

    GetComputername

    Returns the Name of this computer

Arguments:

    none

Return Value:

    the computer's name

Notes:


*/
function GetComputername()
    {
    var objNet = WScript.CreateObject("WScript.Network");
    return ""+objNet.ComputerName;
    }

/*

Routine Description:

    GetArgument

    Returns the value of a command line argument passed to the script

Arguments:

    the name of the value to retrieve

Return Value:

    the argument passed in

Notes:

    script.js /arg=bla

    GetArgument("arg") == "bla"

*/
function GetArgument(name)
    {
    name = "/" + name + "=";
    for(var each = new Enumerator(WScript.Arguments); !each.atEnd(); each.moveNext())
        {
        var item = ""+each.item();
        var start = item.search(new RegExp(name, "i"));
        if(start >= 0)
            {
            return item.substr(start+name.length);
            }
        }

    return null;
    }

