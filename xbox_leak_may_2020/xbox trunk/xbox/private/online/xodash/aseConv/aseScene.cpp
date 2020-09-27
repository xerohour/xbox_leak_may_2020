#include "std.h"
#include "ASEScene.h"
#include "TGL.h"
#include <stdio.h>
#include "File.h"
#include "XBInput.h"
#include "Globals.h"
#include "utilities.h"
#include "XOConst.h" 
#include "csvfile.h"  
#include <malloc.h>


#define ASE_NODE_PARENT			"*NODE_PARENT"

ASEScene::ASEScene()
{
    m_SceneStringNames = 0; // name in the csv file identifying strings
    m_SceneStringNameCount = 0;

}
ASEScene::~ASEScene()
{
    ClearIntelligentObjects();


    for ( std::list< StringInfo* >::iterator stIter = m_StringList.begin(); stIter != m_StringList.end(); stIter++ )
    {
        delete (*stIter );
    }

    for ( int i = 0; i < m_SceneStringNameCount; i++ )
    {
        if ( m_SceneStringNames[i] )
            delete m_SceneStringNames[i];
    }

    if ( m_SceneStringNames )
        delete m_SceneStringNames;

    m_SceneStringNames = 0;

    m_StringList.clear();
}

void ASEScene::ClearIntelligentObjects()
{
for ( std::list< ButtonInfo*>::iterator bIter = m_ButtonList.begin(); bIter != m_ButtonList.end(); bIter++ )
    {
        for ( int i = 0; i < (*bIter)->m_cStandardMatCount; i++ )
        {
            delete (*bIter)->m_cStandardMaterials[i];
        }
        for ( int i = 0; i < (*bIter)->m_cEgglowMatCount; i++ )
        {
            delete (*bIter)->m_cEgglowMaterials[i];
        }

        delete (*bIter );
    }

    m_ButtonList.clear();
    
    for ( std::list< TextInfo* >::iterator tIter = m_TextList.begin(); tIter != m_TextList.end(); tIter++ )
    {
        if ( (*tIter)->m_pTextID )
        {
            delete (*tIter)->m_pTextID;
        }

        delete (*tIter );
    }

    m_TextList.clear();

    for ( std::list< SpinnerInfo* >::iterator spIter = m_SpinnerList.begin(); spIter != m_SpinnerList.end(); spIter++ )
    {
        delete (*spIter );
    }

    m_SpinnerList.clear();

    for ( std::list< ListBoxInfo* >::iterator lbIter = m_ListBoxList.begin(); lbIter != m_ListBoxList.end(); lbIter++ )
    {
        delete( *lbIter );
    }

    m_ListBoxList.clear();

	m_TextCount = m_ButtonCount = m_SpinnerCount = m_ListBoxCount = 0;
}


//////////////////////////////////////////////////////////////////////////////
// This loads an ASE file, this is only used to create an XBG file
/////////////////////////////////////////////////////////////////////////////
struct GroupInfo
{
    char NodeName[64];
    long BraceCount;
    GroupInfo* pParent;
};

#define GROUP_TAG "*GROUP "

// hacky helper function to create group heirarch 
// since MAX doesn't helpfully spit out names like it
// does for geometry
void SetUpGroups( const char* pBuffer, list< GroupInfo >& groupList )
{
   char* pGroupStr = (char*)pBuffer;
   char* pNextGroupStr = NULL;
   char GroupName[256];
   long curBraceCount = 0;
   GroupInfo* pParent = NULL;

   while( pGroupStr = strstr( pGroupStr, GROUP_TAG  ) )
   {
       GroupInfo groupInfo;
       memset( &groupInfo, 0, sizeof ( GroupInfo ) );
       GetNameData(pGroupStr + strlen( GROUP_TAG ) ,groupInfo.NodeName );
       groupInfo.BraceCount = curBraceCount;
       groupInfo.pParent = pParent;
       long oldBraceCount = curBraceCount;

       CharUpperA( groupInfo.NodeName );

       groupList.push_back( groupInfo );
       
       pNextGroupStr = strstr( pGroupStr + strlen( GROUP_TAG ), GROUP_TAG );
       if ( pNextGroupStr )
        *pNextGroupStr = NULL;

       char* pOpenBrace = pGroupStr;
       while( pOpenBrace = strchr( pOpenBrace, '{' ) )
       {
           pOpenBrace++;
           curBraceCount++;
       }
       char* pCloseBrace = pGroupStr;
       while( pCloseBrace = strchr( pCloseBrace, '}' ) )
       {
           pCloseBrace++;
           curBraceCount--;
       }

       if ( curBraceCount < 1 )
       {
           pParent = NULL;
       }
       else if ( curBraceCount > oldBraceCount )
       {
            pParent = &groupList.back();
       }


       if ( pNextGroupStr )
           *pNextGroupStr = '*';

       if ( !pNextGroupStr )
           break;
       
       pGroupStr = pNextGroupStr;


   }


}

char* FindNextObject( const char* pBuffer )
{
    char* pSeeker = NULL;
    char* pNextObject = strstr( pBuffer, "GEOMOBJECT ");
    char* pNextGroup = strstr( pBuffer, "GROUP ");

    if ( (pNextGroup && pNextGroup < pNextObject) || !pNextObject )
    {
        pSeeker = pNextGroup;
    }
    else
    {
        pSeeker = pNextObject;
    }

    return pSeeker;

}
void ASEScene::Load(const char* pBuffer )
{
    m_ButtonCount = 0;
    m_TextCount = 0;
    m_ListBoxCount = 0;
    m_SpinnerCount = 0;

    
    list< GroupInfo > groupList;
    // Last second hack, it turns out that max doesn't write out the
    // names of the parents if the group isn't a geomobject.  So we need 
    // to figure out the group relationship and factor that in
    SetUpGroups( pBuffer, groupList );
 

    TG_Animation animHeader;
    TG_Animation* pHeader = NULL;
    bool bAnimInfo = false;
    if ( S_OK == TG_Shape::ParseAnimationHeader( (char*)pBuffer, &animHeader ) )
    {
        pHeader = &animHeader;
    }

  	m_Camera.Load( pBuffer, &animHeader );


	memset ( &m_Light, 0x00, sizeof ( D3DLIGHT8 ) );

	
	m_Light.Type = (D3DLIGHTTYPE)0;

	if ( !m_pRoot )
	{
		m_pRoot = new TG_Shape( );

		// load up materials
		OutputDebugStringA("Loading the material library\n");
		g_MaterialLib.LoadFromASE( 0, (BYTE*)pBuffer );

		int numObjects = 0;
        // skip to geometry
		const char* pSeeker = FindNextObject( pBuffer );

        TG_Shape* pKid = NULL;
        OutputDebugStringA("Loading objects from the scene\n");
 
		while( pSeeker )
		{
            char tmp = pSeeker[44];
            ((char*)pSeeker)[44] = NULL;
			OutputDebugStringA("Importing Object");
            OutputDebugStringA(pSeeker + strlen("GEOMOBJECT ") + 1);
            OutputDebugStringA("\n");
            ((char*)pSeeker)[44] = tmp;

			// only parse next object
            char* pNext = FindNextObject( pSeeker + strlen( "GEOMOBJECT " ) );
			if ( pNext )
				*pNext = NULL;
			if ( NULL == pKid )
			{
				pKid = new TG_Shape;
			}

            char* parentName = strstr((char *)pSeeker,ASE_NODE_PARENT);
            TG_Shape* pParent = NULL;
            char pStrParent[64];
            pStrParent[0] = 0;

	        //-------------------------------------------------------------------
	        // Must also check to make sure we HAVE a parent.
	        // We will get the next GeomObject's parent if we don't check length!
	        if ((parentName != NULL))
	        {
		        parentName += strlen(ASE_NODE_PARENT)+1;
                pStrParent[0] = 0;
		        GetNameData(parentName,pStrParent);
		        
                pParent = m_pRoot->FindObject( pStrParent );

	        }
            

            D3DXVECTOR3 parentVec;
            parentVec.x = 0.f;
            parentVec.y = 0.f;
            parentVec.z = 0.f;
            TG_Shape* pTmpParent = pParent;
            while( pTmpParent )
            {
                D3DXVECTOR3 tmpParentVec;
                pTmpParent->GetNodeCenter( tmpParentVec.x, tmpParentVec.y, tmpParentVec.z );
                pTmpParent = pTmpParent->GetParent();
                parentVec += tmpParentVec;
            }
	        

			if ( S_OK == pKid->Load( pSeeker, NULL, parentVec, pHeader) )
			{
                if ( strlen( pStrParent ) )
                {
                    if ( !pParent )
                    {
                       pParent = new TG_Shape();
                       pParent->SetNodeName( pStrParent );

                       TG_Shape* pGrandParent = m_pRoot;

                       // look through group list, and see if we can find a parent
                       // got a group
                        for ( list< GroupInfo >::iterator iter = groupList.begin();
                        iter != groupList.end(); iter++ )
                        {
                            if ( _stricmp( pStrParent, (*iter).NodeName ) == 0 )
                            {
                                if ( (*iter).pParent )
                                {
                                    pGrandParent = m_pRoot->FindObject( (*iter).pParent->NodeName );
                                    if ( !pGrandParent )
                                    {
                                        pGrandParent = new TG_Shape();
                                        pGrandParent->SetNodeName( (*iter).pParent->NodeName );
                                        m_pRoot->AddChild( pGrandParent );
                                    }
                                    break;
                                }
                            }
                        }
                       
                        pGrandParent->AddChild( pParent );                       
                    }


                    pParent->AddChild( pKid ); 
                    pKid = NULL;
                }
                else
                {
				    m_pRoot->AddChild( pKid );
				    pKid = NULL;
                }
			}

			numObjects++;
			if ( pNext )
				*pNext = 'G';

            // increment
		    pSeeker += strlen( "GEOMOBJECT " );
            pSeeker = FindNextObject( pSeeker );


		}
		OutputDebugStringA("Finished loading objects\n");

		if ( pKid )
			delete pKid;
        pKid = NULL;
	}

    const char* pSortItems[] = {
                            "ORB",
                            "CONNECT",
                            "FRAME",
                            "PROGRESS BAR",
                            "LISTBOX",
                          "SPINNER",
                          "BUTTON",
                          "SPINNER_UP",
                          "SPINNER_DOWN",
						  "ICON",
                          "TEXT"
                        };


    m_pRoot->SortShapes( pSortItems, 11 );

    m_pRoot->Dump();


	

}

//////////////////////////////////////////////////////////////////////////////
// Recursivly goes through the tree and makes the appropriate objects
// when it hits an intellignet object, it knows that it's at the end of the
// tree
HRESULT ASEScene::CreateIntelligentObjects( TG_Shape* pShape, const char* pStringName )
{
    const char* pName = pShape->GetNodeName();

    if ( pName )
    {
   
        // DO NOT call find with wild cards, otherwise text belonging to buttons
        // will not resolve properly

        CharUpperA( (char*)pName );
        DbgPrint( (char*)pName );
        DbgPrint( "\n" );
        if ( strstr( pName, "BUTTON_" ) )
        {
            CreateButtonObject( pShape, pStringName );
        }
        else if ( strstr( pName, "TEXT_" ) )
        {
            CreateTextObject( pShape, pStringName );
        }
        else if ( strstr( pName, "LISTBOX" ) )
        {
            CreateListBox( pShape, pStringName );
        }
        else if ( strstr( pName, "SPINNER_" ) )
        {
            CreateSpinner( pShape, pStringName );
        }
    
        else // just background art
        {
            TG_Shape* pChild = pShape->GetFirstChild();
            while( pChild )
            {
                CreateIntelligentObjects( pChild, pStringName );
                pChild = pShape->GetNextChild( pChild );
            }
        }
    }
    else // just background art
    {
        TG_Shape* pChild = pShape->GetFirstChild();
        while( pChild )
        {
            CreateIntelligentObjects( pChild, pStringName );
            pChild = pShape->GetNextChild( pChild );
        }
    }

       return S_OK;


}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Create's a button object and potentially any associated text
HRESULT ASEScene::CreateButtonObject( TG_Shape* pShape, const char* pNameInCSV )
{
    ButtonInfo* pInfo = new ButtonInfo;
    memset( pInfo, 0, sizeof( ButtonInfo ) );

    float z;


    long size = sizeof( ButtonInfo );

    list< TG_Shape* > shapeList;

    FindTG_ShapeWildCards( shapeList, pShape, "STANDARD" );
      
    pInfo->m_cStandardMatCount = (unsigned char)shapeList.size( );
    pInfo->m_cStandardMaterials = new char*[pInfo->m_cStandardMatCount];
    
    pInfo->m_cTextObjectID2 = pInfo->m_cTextObjectID = -1;
    
    int i = 0;
    for( list< TG_Shape* >::iterator iter = shapeList.begin(); 
        iter != shapeList.end(); iter++ )
    {
        
        pInfo->m_cStandardMaterials[i] = new char[strlen((*iter)->GetNodeName())+1];
        strcpy( pInfo->m_cStandardMaterials[i],(*iter)->GetNodeName() );
        size += strlen( pInfo->m_cStandardMaterials[i] );
        i++;
     }

    shapeList.clear();
    i = 0;

    FindTG_ShapeWildCards( shapeList, pShape, "EGGLOW" );
      
    pInfo->m_cEgglowMatCount = (char)shapeList.size( );
    pInfo->m_cEgglowMaterials = new char*[pInfo->m_cEgglowMatCount];
    for( iter = shapeList.begin(); 
        iter != shapeList.end(); iter++ )
    {
        
        pInfo->m_cEgglowMaterials[i] = new char[strlen((*iter)->GetNodeName())+1];
        strcpy( pInfo->m_cEgglowMaterials[i],(*iter)->GetNodeName() );
        size += strlen( pInfo->m_cEgglowMaterials[i] );
    }

    shapeList.clear();

    FindTG_ShapeWildCards( shapeList, pShape, "TEXT" );
    if ( shapeList.size() )
    {
        ASSERT( shapeList.size() == 1 || shapeList.size() == 2); // two texts per button????

      iter = shapeList.begin();
      (*iter)->GetNodeCenter( pInfo->m_fXLoc, z, pInfo->m_fYLoc );

        if ( shapeList.size() == 1 )
        {

            pInfo->m_cTextObjectID = (char)m_TextCount;
            CreateTextObject( (*iter), pNameInCSV );   
        }
        else if ( shapeList.size() == 2 )
        {
            char ID1 = (char)m_TextCount;
            char ID2 = (char)m_TextCount+1;

            float fY1, fY2, fX1, fX2;

            iter = shapeList.begin();

            //pInfo->m_cTextObjectID = (char)m_TextCount;
            float z;
            CreateTextObject( (*iter), pNameInCSV );
            (*iter)->GetNodeCenter( fX1, z, fY1);
            
            iter ++;
            CreateTextObject( (*iter), pNameInCSV );
            (*iter)->GetNodeCenter( fX2, z, fY2 );

            // OK, now figure out which one is on top
            if ( fY1 > fY2 )
            {
                pInfo->m_cTextObjectID = ID1;
                pInfo->m_cTextObjectID2 = ID2;
            }
            else if ( fY2 > fY1 )
            {
                pInfo->m_cTextObjectID = ID2;
                pInfo->m_cTextObjectID2 = ID1;
            }
            else if ( fX1 < fX2 )
            {
              pInfo->m_cTextObjectID = ID1;
              pInfo->m_cTextObjectID2 = ID2;  
            }
            else
            {
                 pInfo->m_cTextObjectID = ID1;
                 pInfo->m_cTextObjectID2 = ID2;   
            }
        }
    }

    

     
    strcpy( pInfo->m_TG_ShapeID, pShape->GetNodeName() );
    m_ButtonCount++;
    pInfo->m_nButtonSize = size;
    m_ButtonList.push_back( pInfo );

    return S_OK;
}

void ASEScene::FindTG_ShapeWildCards( list< TG_Shape* >& shapeList, 
                                     TG_Shape* pShape, const char* pWildCard )
{
    TG_Shape* pChild = pShape->GetFirstChild();
    while( pChild )
    {
        const char* pNodeID = pChild->GetNodeName();
        if ( pNodeID && strstr( pNodeID, pWildCard ) != NULL )
        {
            shapeList.push_back( pChild );
        }

        FindTG_ShapeWildCards( shapeList, pChild, pWildCard );


        pChild = pChild->GetNextChild( pChild );

    }

}

//////////////////////////////////////////////////////////////////////////////
// Create's a text object
HRESULT ASEScene::CreateTextObject( TG_Shape* pShape, const char* pNameInCSV )
{
    TextInfo* pInfo = new TextInfo();
    const char* pName = pShape->GetNodeName();
    char* pAttributeStr = strstr( pName, "TEXT_" );
    pAttributeStr += strlen( "TEXT_" );
  
     pInfo->m_nFontType = 0;

    // OK, here we need to look up the real values somewhere....
    pInfo->m_bIsHelpText = strstr( pName, "HELP" ) || strstr( pName, "META" ) ? 1 : 0;
    pInfo->m_fScaleX = .1f;
    pInfo->m_fScaleY = .1f;
    pInfo->m_fScrollDelay = 0.f;
    pInfo->m_fScrollRate = 0.f;
    pInfo->m_fTimeToScroll = 0.f;
    pInfo->m_nFontType = 0;
    pInfo->m_pTextID = NULL;
    pInfo->m_cHorizontalAlignment = 'C';
    pInfo->m_cVerticalAlignment = 'C';
    pInfo->m_bSingleLine = 1;

    char textPath[256];
    strcpy( textPath, MEDIA_PATH_ANSI );
    strcat( textPath, "text.csv" );
    File csvFile;
    csvFile.open( textPath, MC2_APPEND );




    // now figure out height and width.... these should be in model coords since the
    // text WILL move and stuff
    pShape->GetRectDimsRecenterBuildRotation( pInfo->m_fWidth, pInfo->m_fHeight );

    strcpy( pInfo->m_TG_ShapeID, pShape->GetNodeName() );

    m_TextList.push_back( pInfo );
    m_TextCount++;

    //set up string info...
    bool bFound = false;
    for ( std::list< StringInfo* >::iterator iter = m_StringList.begin();
        iter != m_StringList.end(); iter++ )
        {
            if ( _stricmp( (*iter)->m_szFileName, pNameInCSV ) == 0 )
            {
                if ( _stricmp( pInfo->m_TG_ShapeID, (*iter)->m_TG_ShapeID ) == 0 )
                {
                    pInfo->m_fScaleX = (*iter)->m_fXScale;
                    pInfo->m_fScaleY = (*iter)->m_fYScale;
                    pInfo->m_pTextID = new TCHAR[strlen((*iter)->m_TextID)+1];
                    Unicode( pInfo->m_pTextID, (*iter)->m_TextID, strlen((*iter)->m_TextID)+1 );
                    pInfo->m_bLocalize = (*iter)->m_bLocalize;
                    pInfo->m_fScrollRate = (*iter)->m_fScrollRate;
                    pInfo->m_fScrollDelay = (*iter)->m_fScrollDelay;
                    pInfo->m_cHorizontalAlignment = (*iter)->m_cHorizAlignment;
                    pInfo->m_cVerticalAlignment = (*iter)->m_cVertAlignment;
                    pInfo->m_bSingleLine = !(*iter)->m_bMultiLine;
                    pInfo->m_nFontType = (*iter)->m_Font;
                    pInfo->m_bIsHelpText = (*iter)->m_bHelp;

                    bFound = true;
                }
            }
        }
 
        if ( !bFound )
        {
            char errorStr[256];
            sprintf( errorStr, "Couldn't find string info for %s", pInfo->m_TG_ShapeID );
            ASSERT( !"Missing string info!" );

            ASSERT( "Missing StringInfo" );
            sprintf( errorStr, "%s,%s,%s,1,.1,.1,0,0,C,C,0,0", pNameInCSV, pShape->GetNodeName(), "Done" );
            csvFile.writeLine( errorStr );

            pInfo->m_pTextID = new TCHAR[_tcslen( _T("Missing Text" ) )+1];
            _tcscpy( pInfo->m_pTextID, _T("Missing Text" ) );
        }


     int len = pInfo->m_pTextID ? _tcslen( pInfo->m_pTextID ) : 0;
     pInfo->m_nTextInfoSize = sizeof( TextInfo ) + len * sizeof( TCHAR ) - sizeof( TCHAR*);


     csvFile.close();
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
// Create's a spinner object -- i.e. one button
HRESULT ASEScene::CreateSpinner(TG_Shape* pShape, const char* pNameInCSV )
{
    char cSpinnerButtonID = -1;

    TG_Shape* pChild = pShape->GetFirstChild();
    while( pChild  )
    {
        const char* pChildName = pChild->GetNodeName();
        if ( -1 == cSpinnerButtonID && strstr( pChildName, "BUTTON_" ) )
        {
            CreateButtonObject( pChild, pNameInCSV );
        }

        pChild = pShape->GetNextChild( pChild );

        if ( cSpinnerButtonID != -1 )
            break;
    }

    // we're out of children, make sure we got all three needed items
    ASSERT( cSpinnerButtonID != -1 );

    SpinnerInfo* pSpinner = new SpinnerInfo;

    pSpinner->m_pButton = m_ButtonList.back();

    strcpy( pSpinner->m_TG_ShapeID, pShape->GetNodeName() );

    m_SpinnerList.push_back( pSpinner );
    m_SpinnerCount++;

    return S_OK;

}


//////////////////////////////////////////////////////////////////////////////
// create's a list group, which is a series of buttons 
HRESULT ASEScene::CreateListBox( TG_Shape* pShape, const char* pNameInCSV )
{
    ListBoxInfo* pInfo = new ListBoxInfo;
    strcpy( pInfo->m_TG_ShapeID, pShape->GetNodeName() );
    TG_Shape* pChild = pShape->GetFirstChild();
    
    while( pChild  )
    {
        const char* pChildName = pChild->GetNodeName();
        if ( strstr( pChildName, "BUTTON" ) )
        {
            CreateButtonObject( pChild, pNameInCSV );
            pInfo->m_buttonList.push_back( m_ButtonList.back() );

        }    
        
        pChild = pShape->GetNextChild( pChild );
    }


    m_ListBoxList.push_back(pInfo);
    m_ListBoxCount++;

    return S_OK;


}


//////////////////////////////////////////////////////////////////////////////
HRESULT ASEScene::SaveBinary( const char* strFilename  )
{
   
    // Open the file to write
    File file;
    if ( NO_ERR != file.create( strFilename ) )
    {
        char ErrorStr[256];
        sprintf( ErrorStr, "Couldn't open file %s", strFilename );
        DbgPrint( ErrorStr );
        ASSERT(!"XBG file marked as read-only\n");
        return -1;
    }

    // need to do this first so we recenter text...
    CreateIntelligentObjects( m_pRoot, m_SceneStringNames[0] );


    m_pRoot->SaveBinary( &file );

    m_Camera.SaveBinary( &file );


    file.writeLong( m_SceneStringNameCount );

    long beginningOfSubsets = file.getLogicalPosition();

    long* subSets = (long*)_alloca( sizeof( long ) * m_SceneStringNameCount );

    // pad for header info
    for ( int i = 0; i < m_SceneStringNameCount; i++ )
    {
        
        file.writeString( m_SceneStringNames[i] );
        file.writeLong( 0 );        
        subSets[i] = 0;
    }

    for ( int i = 0; i < m_SceneStringNameCount; i++ )
    {
        subSets[i] = file.getLogicalPosition();

		ClearIntelligentObjects();

        CreateIntelligentObjects( m_pRoot, m_SceneStringNames[i] );
        SortButtons(m_SceneStringNames[i]);

        file.writeLong( m_TextCount );
        file.writeLong( m_ButtonCount );
        file.writeLong( 0 ); // padding
        file.writeLong( 0 );
        file.writeLong( m_ListBoxCount );
        file.writeLong( m_SpinnerCount );

        long nOffsetPos = file.getLogicalPosition();
        // Pad the offsets
        for ( int i = 0; i < 4; i++ )
        {
            file.writeLong( 0 );
        }
    
        long nTextOffset = file.getLogicalPosition();
        WriteTexts( &file );
        long nButtonOffset = file.getLogicalPosition();
        WriteButtons( &file );
        long nListBoxOffset = file.getLogicalPosition();
        WriteListBoxes( &file);
        long nSpinnerOffset = file.getLogicalPosition();
        WriteSpinners( &file );

		long endOfFilePos = file.getLogicalPosition();
        file.seek( nOffsetPos );
        file.writeLong( nTextOffset );
        file.writeLong( nButtonOffset );
        file.writeLong( nListBoxOffset );
        file.writeLong( nSpinnerOffset );

		file.seek( endOfFilePos );
    }

     file.seek(beginningOfSubsets);

    // go back in and fill headers
    for ( int i = 0; i < m_SceneStringNameCount; i++ )
    {
   
        file.writeString( m_SceneStringNames[i] );
        file.writeLong( subSets[i] );        
    }


    return S_OK;

}

/////////////////////////////////////////////////////////////////
// writes all the info for a button object
void ASEScene::WriteButtons( File* pFile )
{
      
    long counter = 0;
    for ( std::list< ButtonInfo*>::iterator iter = m_ButtonList.begin();
        iter != m_ButtonList.end(); iter++, counter++ )
    {
        pFile->write( (BYTE*)(*iter), sizeof ( ButtonInfo ) - 2 * (sizeof ( char** ) ));
        for ( unsigned char i = 0; i < (*iter)->m_cStandardMatCount; i++ )
        {
            pFile->writeString( (*iter)->m_cStandardMaterials[i] );
        }
        for ( i = 0; i < (*iter)->m_cEgglowMatCount; i++ )
        {
            pFile->writeString( (*iter)->m_cEgglowMaterials[i] );
        }
         
         /*pFile->writeLong( (*iter)->m_nButtonSize );       // size of the particular ButtonInfo instance 
        pFile->write( (*iter).m_TG_ShapeID, NODE_NAME_LEN * sizeof( char ) );
        pFile->writeByte( (*iter).m_cTextObjectID );
        pFile->writeString( m_HelpTextID );
        pFile->writeByte( (*iter).m_cStandardMatCount );
        pFile->write( (*iter).m_cstandardMatCount );
        pFile->writeByte( (*iter).m_cEgglowMatCount );
        pFile->write( (*iter).m_cEgglowMaterials );*/
    }


}

////////////////////////////////////////////////////////////////////
// writes all the info for all the text objects
void ASEScene::WriteTexts( File* pFile )
{

    long counter = 0;
    for ( std::list< TextInfo*>::iterator iter = m_TextList.begin(); 
            iter != m_TextList.end(); iter++, counter++ )
    {

        pFile->write( (BYTE*)(*iter), sizeof( TextInfo ) - sizeof( TCHAR*) );
        if ( (*iter)->m_pTextID )
            pFile->write( (BYTE*)(*iter)->m_pTextID, (_tcslen((*iter)->m_pTextID) * sizeof( TCHAR ))  );
     /*   pFile->writeLong( m_nTextInfoSize );
        pFile->write( m_Tg_ShapeID, NODE_NAME_LEN * sizeof( char ) );
        pFile->writeChar( (*iter)->m_cVerticalAlignment );
        pFile->writeChar( (*iter)->m_cHorizontalAlignment );
        pFile->writeBoolean( (*iter)->m_bSingleLine );
        pFile->writeLong( (*iter)->m_nFontType );
        pFile->writeFloat( (*iter)->m_fWidth );
        pFile->writeFloat( (*iter)->m_fHeight );
        pFile->writeByte( (*iter)->m_bIsHelpText );
        pFile->writeFloat( (*iter)->m_fScaleX );
        pFile->writeFloat( (*iter)->m_fScaleY );
        pFile->writeFloat( (*iter)->m_fScaleScrollRate );
        pFile->writeFloat( (*iter)->m_fScaleScrollDelay );
        pFile->writeString( m_pTextID );*/

    }
}

////////////////////////////////////////////////////////////////////
// writes all the info for all the spinner objects
void ASEScene::WriteSpinners( File* pFile )
{
   
    long counter = 0;
    for ( std::list< SpinnerInfo*>::iterator iter = m_SpinnerList.begin(); 
            iter != m_SpinnerList.end(); iter++, counter++ )
    {
       pFile->write( (BYTE*)(*iter)->m_TG_ShapeID, sizeof ( (*iter)->m_TG_ShapeID ) );
      
       unsigned char finder = 0;

       // we have to do this because we sadly sorted all the buttons earlier
       for ( std::list< ButtonInfo*>::iterator buttonIter = m_ButtonList.begin();
            buttonIter != m_ButtonList.end(); buttonIter ++ )
            {
                if ( (*buttonIter == (*iter)->m_pButton ) )
                {
                    pFile->writeByte( finder );
                    break;
                }
                finder++;
            }
     }
}

////////////////////////////////////////////////////////////////////
// writes all the info for all the list box objects
void ASEScene::WriteListBoxes( File* pFile )
{

    long counter = 0;
    for ( std::list< ListBoxInfo*>::iterator iter = m_ListBoxList.begin(); 
        iter != m_ListBoxList.end(); iter++, counter++ )
    {
       pFile->write( (unsigned char*)(*iter)->m_TG_ShapeID, sizeof ( (*iter)->m_TG_ShapeID ) );
       pFile->writeByte( (unsigned char)(*iter)->m_buttonList.size() );

         for ( std::list< ButtonInfo*>::iterator lbIter = (*iter)->m_buttonList.begin();
            lbIter !=  (*iter)->m_buttonList.end(); lbIter ++ )
            {
 
                long finder = 0;

                for ( std::list< ButtonInfo*>::iterator buttonIter = m_ButtonList.begin();
                    buttonIter != m_ButtonList.end(); buttonIter ++ )
                {
                    if ( (*buttonIter == *lbIter ) )
                    {
                        pFile->writeByte( (unsigned char)finder );
                        break;
                    }
                    finder++;
                }
            }   

    }
}

void ASEScene::SortButtons( const char* pNameOfId )
{
 
    float tolerance = .01f;
    float linebreak = .25;

    // HACK -- the keyboard needs a crazy big tolerance to work
    if ( _stricmp( m_SceneName, "Keyboard" ) == 0 )
    {
        tolerance = .025f;
        linebreak = .025f;
    }

     
    // first need to sort vertically
    if ( m_ButtonList.size() )
    {

        for (std::list< ButtonInfo*>::iterator iter = m_ButtonList.begin();
            iter != m_ButtonList.end(); iter++ )
        {
                float curY = (*iter)->m_fYLoc;
                float curX = (*iter)->m_fXLoc;
                for ( std::list< ButtonInfo* >::iterator inner = m_ButtonList.begin();
                    inner != iter; inner++ )
                {
                    float prevX = (*inner)->m_fXLoc;
                    float prevY = (*inner)->m_fYLoc;

                    if ( (prevY < curY) && fabs( prevY - curY ) > tolerance )
                    {
                        m_ButtonList.insert( inner, (*iter) );
                        m_ButtonList.erase( iter );
                        iter = inner;
                        break;

                    }             
                    
                }
                  
         }
//        std::list< ButtonInfo*>::iterator lastIter = m_ButtonList.begin();
        
        for (std::list< ButtonInfo*>::iterator iter = m_ButtonList.begin();
            iter != m_ButtonList.end(); iter++ )
        {
                float curY = (*iter)->m_fYLoc;
                float curX = (*iter)->m_fXLoc;
                for ( std::list< ButtonInfo* >::iterator inner = m_ButtonList.begin();
                    inner != iter; inner++ )
                {
                    float prevX = (*inner)->m_fXLoc;
                    float prevY = (*inner)->m_fYLoc;

                    if ( fabs( prevY - curY ) < linebreak &&  prevX - curX > tolerance  )
                    {
                        m_ButtonList.insert( inner, (*iter) );
                        m_ButtonList.erase( iter );
                        iter = inner;
                        break;
                    }
              
                    
                }
                  
         }

 

        long firstVert = 0;
        long firstHoriz = 0;
        if ( m_ButtonList.size() )
        {
            iter = m_ButtonList.begin();
            float xPrev    =(*iter)->m_fXLoc;
            float yPrev     = (*iter)->m_fYLoc;
            long  count = 0;
            iter++;
            bool bSetPrev = 0;
            // now go back and find the first horizontal and fist vertical
            for (iter = m_ButtonList.begin(); iter != m_ButtonList.end(); iter++, count++ )
            {
                (*iter)->m_buttonEnumID = eNullButtonId;
                bool bFoundId = 0;
                char* pLookupName = strstr( (*iter)->m_TG_ShapeID, "BUTTON_" );

                if ( pLookupName )
                {
                    char finalName[256];
                    sprintf( finalName, "%s%s", pNameOfId, pLookupName + strlen( "BUTTON_" ) );
                    for ( long i =0; i < (long)eLastButtonId; i++ )
                    {
                        if ( _stricmp( ButtonName[i], finalName ) == 0 )
                        {
                            (*iter)->m_buttonEnumID = i;
                            bFoundId = true;
                            break;
                        }
                    }
                }

                ASSERT( bFoundId );
                if ( !bFoundId )
                {
                    char tmpStr[256];
                    sprintf( tmpStr, "Couldn't find ID for %lf, %lf, %s, \n", (*iter)->m_fXLoc, (*iter)->m_fYLoc, (*iter)->m_TG_ShapeID );
                    DbgPrint( tmpStr ); 
                }
                if ( fabs((*iter)->m_fXLoc - xPrev) < .01) 
                {
                    (*iter)->m_orientation = eButtonVerticalOrientation;
                    bSetPrev = false;

                    if ( bSetPrev )
                    {
                       std::list< ButtonInfo*>::iterator prevIter = iter;
                       prevIter --;
                       (*iter)->m_orientation = eButtonVerticalOrientation;
                    }
                }
                else if ( fabs((*iter)->m_fYLoc - yPrev) < .01)
                {
                    (*iter)->m_orientation = eButtonHorizontalOrientation;
                    bSetPrev = false;

                    if ( bSetPrev )
                    {
                       std::list< ButtonInfo*>::iterator prevIter = iter;
                       prevIter --;
                       (*iter)->m_orientation = eButtonHorizontalOrientation;
                    }
                }
                else
                {
                    bSetPrev = true;
                }

               

                yPrev = (*iter)->m_fYLoc;
                xPrev    =(*iter)->m_fXLoc;
            }

            iter = m_ButtonList.begin();
            std::list< ButtonInfo*>::iterator tmpIter = iter;
            iter++;
            if ( iter != m_ButtonList.end() )
            {
                (*tmpIter)->m_orientation = (*iter)->m_orientation;
            }
        }

    }
}

void    ASEScene::LoadStrings( CSVFile* pFile, const char* aseName, const char** pSceneName, long sceneCount )
{
    strcpy( m_SceneName, aseName );

    if ( sceneCount )
    {
        m_SceneStringNameCount = sceneCount;
        m_SceneStringNames = new char*[sceneCount];
        for ( int i = 0; i < sceneCount; i++ )
        {
            m_SceneStringNames[i] = new char[strlen( pSceneName[i] ) + 1];
            strcpy( m_SceneStringNames[i], pSceneName[i] );
        }
    }
    
    for ( int i = 0; i < pFile->getTotalRows(); i++ )
    {
        char sceneName[256];
        pFile->readString( i+1, 1, sceneName, 255 );

        for ( int j = 0; j < sceneCount; j++ )
        {

            if ( _stricmp( sceneName, pSceneName[j] ) == 0 ) // only load ones for this scene
            {
                StringInfo* pInfo = new StringInfo;
                strcpy( pInfo->m_szFileName, pSceneName[j] );
                pFile->readString( i+1, 2, pInfo->m_TG_ShapeID, NODE_NAME_LEN ); 
                pFile->readString( i+1, 3, pInfo->m_TextID, 63 );
                CharUpperA( pInfo->m_TextID );
                pFile->readBoolean( i+1, 4, pInfo->m_bLocalize );
                pFile->readFloat( i+1, 5, pInfo->m_fXScale );
                pFile->readFloat( i+1, 6, pInfo->m_fYScale );
                pFile->readFloat( i+1, 7, pInfo->m_fScrollRate );
                pFile->readFloat( i+1, 8, pInfo->m_fScrollDelay );
                pFile->readChar( i+1, 9,  pInfo->m_cHorizAlignment );
                pFile->readChar( i+1, 10, pInfo->m_cVertAlignment );
                pFile->readBoolean( i+1, 11, pInfo->m_bMultiLine );
                pFile->readLong( i+1, 12, (long&) pInfo->m_Font ); 
                pFile->readBoolean( i+1, 13, pInfo->m_bHelp );

  

                m_StringList.push_back( pInfo );
            }
        }
    }
}


