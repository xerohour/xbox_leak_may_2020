//-----------------------------------------------------------------------------
// Name: PerformanceCounters Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The PerformanceCounters sample demonstrates how to create and query Xbox
performance counters on the XDK.  The DmWalkPerformanceCounters method
enumerates all available performance counters.  The counter can then be queried
to return a DM_COUNTDATA structure, the interpretation of which depends on the
type of counter.  Some counters return instantaneous values, some return raw 
counts, and some return percentages.
   It also demonstrates how to add application-defined performance counters,
which can then be queried by the application or queried remotely from the
development system.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================

