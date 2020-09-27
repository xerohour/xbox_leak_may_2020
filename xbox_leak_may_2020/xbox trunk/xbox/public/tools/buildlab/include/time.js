/*****************************************************************************
Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    time.js

Abstract:

    Common Time and Date functions

Functions:

    FormatShortDate()   Returns the textual short form of the date and time
    FormatTime()        Returns the textual short form of just the time

Author:

    Josh Poley (jpoley)

*****************************************************************************/


/*****************************************************************************

Routine Description:

    FormatShortDate

    Takes a date object and returns the textual short form of the date and time

Arguments:

    d       date object

Return Value:

    new string

Notes:

    Format: "MM/DD/YYYY HH:MM PM"

*****************************************************************************/
function FormatShortDate(d)
    {
    var date = new Date(d);
    if(isNaN(date.getMonth())) return "Never";
    return (date.getMonth()+1) + "/" + date.getDate() + "/" + date.getYear() + " " + FormatTime(date);
    }


/*****************************************************************************

Routine Description:

    FormatTime

    Takes a date object and returns the textual short form of just the time

Arguments:

    d       date object

Return Value:

    new string

Notes:

    Format: "HH:MM PM"

*****************************************************************************/
function FormatTime(d)
    {
    var am = " AM";
    var date = new Date(d);
    var hours = date.getHours();
    if(isNaN(date.getMonth())) return "Never";
    if(hours > 12)
        {
        am = " PM";
        hours-=12;
        }
    else if(hours == 0)
        {
        hours = 12;
        }
    return hours + ":" + (date.getMinutes()<10?"0":"") + date.getMinutes() + am;
    }
