/*-----------------------------------------------------------------------------
Microsoft VSEE

Microsoft Confidential
Copyright 1995-2000 Microsoft Corporation. All Rights Reserved.

@doc
@module VseeGuids.h - Guids for VSEE services/interfaces |
This is included by .idl files to define guids for VSEE services/interfaces.
These are the non-shell-SDK interfaces, in idl/vsee

@owner Source Control Integration Team
-----------------------------------------------------------------------------*/
#pragma once

#ifdef _WIN64

#define uuid_IVsSccManager							53474C4D-0F05-4735-8AAC-264109CF68AC
#define uuid_IVsSccProject							53474C4D-CD19-4928-A834-AFCD8A966C36
#define uuid_IVsQueryEditQuerySave					53474C4D-7E28-4d0c-A00F-3446801350CE
#define uuid_IVsTrackProjectDocuments				53474C4D-449A-4487-A56F-740CF8130032
#define uuid_IVsTrackProjectDocumentsEvents			53474C4D-A98B-4fd3-AA79-B182EE26185B
#define uuid_IVsSccEngine							53474C4D-F82C-11d0-8D84-00AA00A3F593
#define uuid_IVsSccPopulateList						53474C4D-F8CF-11d0-8D84-00AA00A3F593
#define uuid_IVsSccToolsOptions						53474C4D-304B-4D82-AD93-074816C1A0E5
#define uuid_IVsSccManagerTooltip					53474C4D-DF28-406D-81DA-96DEEB800B64
#define uuid_IVsExternalCommandTarget				53474C4D-AB21-4A15-BD22-28A9B35DD89E
#define uuid_IVsSccItem								53474C4D-AAF8-11D0-8E5E-00A0C911005A
#define uuid_IVsSccMergeConflictsUIFactory			53474C4D-00FA-4dcb-BBBF-F0F1CD8C62FC
#define uuid_IVsSccMergeConflictsUIEvents			53474C4D-03FA-4dcb-BBBF-F0F1CD8C62FC
#define uuid_IVsMergeUIFactory						53474C4D-18AC-4227-A60F-4A50AFD0A89B
#define uuid_IVsSccMergeConflictsUI					53474C4D-01FA-4dcb-BBBF-F0F1CD8C62FC
#define uuid_IVsDiffUIFactory						53474C4D-48DB-488E-95C9-1F0461970402
#define uuid_IVsDiffMergeUIClientCallback			53474C4D-6B1F-476A-B423-4BCD87DAAD03
#define uuid_IVsDiffMergeUIBroker					53474C4D-0BA6-49E0-B9B3-3D90E34C0BBF
#define uuid_IVsSccMergeConflictsUIFileInto			53474C4D-02FA-4dcb-BBBF-F0F1CD8C62FC


#else

#define uuid_IVsSccManager							53544C4D-0F05-4735-8AAC-264109CF68AC
#define uuid_IVsSccProject							53544C4D-CD19-4928-A834-AFCD8A966C36
#define uuid_IVsQueryEditQuerySave					53544C4D-7E28-4d0c-A00F-3446801350CE
#define uuid_IVsTrackProjectDocuments				53544C4D-449A-4487-A56F-740CF8130032
#define uuid_IVsTrackProjectDocumentsEvents			53544C4D-A98B-4fd3-AA79-B182EE26185B
#define uuid_IVsSccEngine							53544C4D-F82C-11d0-8D84-00AA00A3F593
#define uuid_IVsSccPopulateList						53544C4D-F8CF-11d0-8D84-00AA00A3F593
#define uuid_IVsSccToolsOptions						53544C4D-304B-4D82-AD93-074816C1A0E5
#define uuid_IVsSccManagerTooltip					53544C4D-DF28-406D-81DA-96DEEB800B64
#define uuid_IVsExternalCommandTarget				53544C4D-AB21-4A15-BD22-28A9B35DD89E
#define uuid_IVsSccItem								53544C4D-AAF8-11D0-8E5E-00A0C911005A
#define uuid_IVsSccMergeConflictsUIFactory			53544c4d-00FA-4dcb-BBBF-F0F1CD8C62FC
#define uuid_IVsSccMergeConflictsUIEvents			53544c4d-03FA-4dcb-BBBF-F0F1CD8C62FC
#define uuid_IVsMergeUIFactory						53544C4D-18AC-4227-A60F-4A50AFD0A89B
#define uuid_IVsSccMergeConflictsUI					53544c4d-01FA-4dcb-BBBF-F0F1CD8C62FC
#define uuid_IVsDiffUIFactory						53544C4D-48DB-488E-95C9-1F0461970402
#define uuid_IVsDiffMergeUIClientCallback			53544C4D-6B1F-476A-B423-4BCD87DAAD03
#define uuid_IVsDiffMergeUIBroker					53544C4D-0BA6-49E0-B9B3-3D90E34C0BBF
#define uuid_IVsSccMergeConflictsUIFileInto			53544c4d-02FA-4dcb-BBBF-F0F1CD8C62FC

#endif

#define uuid_SVsExternalCommandTarget				53544C4D-1B21-4A15-BD22-28A9B35DD89E
#define uuid_SVsDiffMergeUIBroker					53544C4D-1BA6-49E0-B9B3-3D90E34C0BBF
#define uuid_SVsSccMergeConflicts					53544c4D-10FA-4dcb-BBBF-F0F1CD8C62FC
#define uuid_SVsSccToolsOptions						53544C4D-104B-4D82-AD93-074816C1A0E5

