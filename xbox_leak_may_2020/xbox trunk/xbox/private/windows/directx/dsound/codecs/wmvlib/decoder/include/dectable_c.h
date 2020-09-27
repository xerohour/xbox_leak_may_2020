    //  tables for decoding.
     extern U8_WMV sm_rgRunAtIndx_HghMt [168];
     extern I8_WMV sm_rgLevelAtIndx_HghMt [168];
     extern U8_WMV sm_rgRunAtIndxOfIntraY_HghMt [185];
     extern I8_WMV sm_rgLevelAtIndxOfIntraY_HghMt [185];

     extern U8_WMV sm_rgRunAtIndx_Talking [148];
     extern I8_WMV sm_rgLevelAtIndx_Talking [148];
     extern U8_WMV sm_rgRunAtIndxOfIntraY_Talking [132];
     extern I8_WMV sm_rgLevelAtIndxOfIntraY_Talking [132];
     extern tPackHuffmanCode_WMV sm_HufMVTable_Talking [1101];
     extern U8_WMV sm_uXMvFromIndex_Talking [1099];
     extern U8_WMV sm_uYMvFromIndex_Talking [1099];
     extern tPackHuffmanCode_WMV sm_HufMVTable_HghMt [1101];
     extern U8_WMV sm_uXMvFromIndex_HghMt [1099];
     extern U8_WMV sm_uYMvFromIndex_HghMt [1099];
    // Header
     extern tPackHuffmanCode_WMV sm_HufPCBPCYTable [129];
     extern tPackHuffmanCode_WMV sm_HufICBPCYTable [65];
    
    //#ifdef NEW_PCBPCY_TABLE
     extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_HighRate [129];
     extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_MidRate [129];
     extern tPackHuffmanCode_WMV sm_HufPCBPCYTable_LowRate [129];
    //#endif
    
    //_DCTDC
     extern tPackHuffmanCode_WMV sm_HufDCTDCyTable_Talking[121];
     extern tPackHuffmanCode_WMV sm_HufDCTDCcTable_Talking[121];
     extern tPackHuffmanCode_WMV sm_HufDCTDCyTable_HghMt[121];
     extern tPackHuffmanCode_WMV sm_HufDCTDCcTable_HghMt[121];
     extern tPackHuffmanCode_WMV sm_HufDCTACInterTable_HghMt[170];
     extern tPackHuffmanCode_WMV sm_HufDCTACIntraTable_HghMt[187];
     extern tPackHuffmanCode_WMV sm_HufDCTACInterTable_Talking[150];
     extern tPackHuffmanCode_WMV sm_HufDCTACIntraTable_Talking[134];
     extern tPackHuffmanCode_WMV sm_HufDCTACInterTable_MPEG4 [104];
     extern tPackHuffmanCode_WMV sm_HufDCTACIntraTable_MPEG4 [104];
     extern U8_WMV sm_rgRunAtIndx_MPEG4 [102];
     extern I8_WMV sm_rgLevelAtIndx_MPEG4 [102];
     extern U8_WMV sm_rgRunAtIndxOfIntraY_MPEG4 [102];
     extern I8_WMV sm_rgLevelAtIndxOfIntraY_MPEG4 [102];

     // ---------------------------------------------------

    extern HuffDecInfo m_hufMVDec_TalkingDecInfo[1806]; 
    extern TableInfo   m_hufMVDec_TalkingTableInfo[90];

    extern HuffDecInfo m_hufMVDec_HghMtDecInfo[1894]; 
    extern TableInfo   m_hufMVDec_HghMtTableInfo[151];
    
    extern HuffDecInfo m_hufICBPCYDecDecInfo[162]; 
    extern TableInfo   m_hufICBPCYDecTableInfo[15];
    
    extern HuffDecInfo m_hufICBPCYDecDecInfo[162]; 
    extern TableInfo   m_hufICBPCYDecTableInfo[15];
    
    extern HuffDecInfo m_hufPCBPCYDecDecInfo[756]; 
    extern TableInfo   m_hufPCBPCYDecTableInfo[13];
    
    extern HuffDecInfo m_hufPCBPCYDec_HighRateDecInfo[1112];
    extern TableInfo   m_hufPCBPCYDec_HighRateTableInfo[19];
    
    extern HuffDecInfo m_hufPCBPCYDec_MidRateDecInfo[650];
    extern TableInfo   m_hufPCBPCYDec_MidRateTableInfo[17];
    
    extern HuffDecInfo m_hufPCBPCYDec_LowRateDecInfo[408];
    extern TableInfo   m_hufPCBPCYDec_LowRateTableInfo[21];
    
    extern HuffDecInfo m_hufDCTDCyDec_TalkingDecInfo[838]; 
    extern TableInfo   m_hufDCTDCyDec_TalkingTableInfo[6];
    
    extern HuffDecInfo m_hufDCTDCcDec_TalkingDecInfo[668]; 
    extern TableInfo   m_hufDCTDCcDec_TalkingTableInfo[7];
    
    extern HuffDecInfo m_hufDCTDCyDec_HghMtDecInfo[1476]; 
    extern TableInfo   m_hufDCTDCyDec_HghMtTableInfo[7];
    
    extern HuffDecInfo m_hufDCTDCcDec_HghMtDecInfo[1088]; 
    extern TableInfo   m_hufDCTDCcDec_HghMtTableInfo[5];
    
    extern HuffDecInfo m_hufDCTACInterDec_HghMtDecInfo[464]; 
    extern TableInfo   m_hufDCTACInterDec_HghMtTableInfo[30];
    
    extern HuffDecInfo m_hufDCTACIntraDec_HghMtDecInfo[504]; 
    extern TableInfo   m_hufDCTACIntraDec_HghMtTableInfo[42];
    
    extern HuffDecInfo m_hufDCTACInterDec_TalkingDecInfo[404]; 
    extern TableInfo   m_hufDCTACInterDec_TalkingTableInfo[32];
    
    extern HuffDecInfo m_hufDCTACIntraDec_TalkingDecInfo[286];
    extern TableInfo   m_hufDCTACIntraDec_TalkingTableInfo[28];
    
    extern HuffDecInfo m_hufDCTACInterDec_MPEG4DecInfo[144]; 
    extern TableInfo   m_hufDCTACInterDec_MPEG4TableInfo[18];
    
    extern HuffDecInfo m_hufDCTACIntraDec_MPEG4DecInfo[144]; 
    extern TableInfo   m_hufDCTACIntraDec_MPEG4TableInfo[18];