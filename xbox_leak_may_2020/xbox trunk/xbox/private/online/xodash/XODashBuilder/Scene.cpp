#include "stdafx.h"
#include "scene.h"

// VButton Class Member Functions

VButton::VButton()
{
}

VButton::~VButton()
{
    m_pVButtonName.Empty();
    m_pVButtonID.Empty();
    m_pVHelpText.Empty();
}

// VButton Class Member Functions

VButtonScene::VButtonScene()
{
    m_pVButton = NULL;
}

VButtonScene::~VButtonScene()
{
    m_pVButton = NULL;
    m_pVButtonLink.Empty();
}

// VScene Class Member Functions

VScene::VScene()
{
    m_VButtonList.clear();
}

VScene::~VScene()
{
    m_pzVSceneName.Empty();
    m_pzVSceneID.Empty();
    m_pzVScenePath.Empty();
    m_pzVSceneTextName.Empty();
    m_pzVSceneShowTop.Empty();
    m_pzVButtonFocus.Empty();

	// Clean up the buttons
	VButtonSceneList::iterator i;
	for(i = m_VButtonList.begin(); i != m_VButtonList.end(); i++)
	{
        delete (*i);
	}
    m_VButtonList.clear();
}

// VSceneCollection Member Functions

VSceneCollection::VSceneCollection()
{
    m_gVSceneList.clear();
}

VSceneCollection::~VSceneCollection()
{

	// Clean up the Global Button list
	VButtonList::iterator i;
	for(i = m_gVButtonList.begin(); i != m_gVButtonList.end(); i++)
	{
	    delete (*i);
	}
    m_gVButtonList.clear();

	// Clean up the Scenes
	VSceneList::iterator j;
	for(j = m_gVSceneList.begin(); j != m_gVSceneList.end(); j++)
    {
		delete (*j);
	}
    m_gVSceneList.clear();
}

HRESULT ReadList(char *fileName, VStringList *buttonList)
{
    HRESULT retVal = S_OK;

    // Load the button name structure
    HANDLE nameFile = CreateFile(fileName,
                                 GENERIC_READ,
                                 FILE_SHARE_READ,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    DWORD dwSize = GetFileSize(nameFile, NULL);

    char *pInputBuffer = new char[dwSize + 1];
    DWORD numRead = 0;

    ReadFile(nameFile, pInputBuffer, dwSize, &numRead, NULL);
    pInputBuffer[dwSize] = '\0';

    // Now that we have the file loaded into inputBuffer, remove everything
    // before and after the { }
    int nBeginStringPos = strcspn( pInputBuffer, "{" );

    // Get a pointer to the beginning of the text we care about
    char *pCleanString = pInputBuffer + nBeginStringPos + 1;

    char seps[] = ",\t\n\r\" ;}";
    char *token;

    token = strtok( pCleanString, seps );
    while( token != NULL )
    {
        // Add the token to the linked list
        if((strncmp("/*", token, 2) != 0) &&
           (strcmp("_T(", token) != 0) &&
           (strcmp(")", token) != 0) &&
           (strcmp("eNullButtonId", token) != 0) &&
           (strcmp("eNullSceneId", token) != 0) &&
           (strcmp("=", token) != 0) &&
           (strcmp("-1", token) != 0) &&
           (strcmp("eLastButtonId", token) != 0) &&
           (strcmp("eLastSceneId", token) != 0))
        { 
            CString newToken;
            newToken = token;
            buttonList->push_back(newToken);
        }
        
        // Get the next token
        token = strtok( NULL, seps );
    }

    delete [] pInputBuffer;
    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::initalizeButtons(void)
{
    HRESULT retVal = S_OK;

    VStringList VButtonNames;
    VStringList VButtonIds;
    VStringList VButtonHelp;

    // Load the button names from the include files
    retVal = ReadList("..\\xodashlib\\ButtonName.h", &VButtonNames);
    retVal = ReadList("..\\xodashlib\\ButtonId.h", &VButtonIds);
    retVal = ReadList("..\\xodashlib\\ButtonHelp.h", &VButtonHelp);

    // Populate the Button Structure
    VStringList::iterator i;
    VStringList::iterator j;
    VStringList::iterator k;

	i = VButtonNames.begin();
	j = VButtonIds.begin();
	k = VButtonHelp.begin();

    for(int x = 0; x < VButtonNames.size(); x++)
    {
        VButton *newButton = new VButton();

        newButton->m_pVButtonName = (*i);
        newButton->m_pVButtonID   = (*j);
        newButton->m_pVHelpText   = (*k);
        (*i).Empty();
        (*j).Empty();
        (*k).Empty();
        i++; j++; k++;

        m_gVButtonList.push_back(newButton);
    }

	VButtonNames.clear();
    VButtonIds.clear();
    VButtonHelp.clear();

    return retVal;
}

HRESULT VSceneCollection::initalizeScenes(void)
{
    HRESULT retVal = S_OK;

    VStringList VSceneNames;
    VStringList VSceneIds;

    // Load the button names from the include files
    retVal = ReadList("..\\xodashlib\\SceneName.h", &VSceneNames);
    retVal = ReadList("..\\xodashlib\\SceneId.h", &VSceneIds);

    // Populate the Button Structure
    VStringList::iterator i;
    VStringList::iterator j;

	i = VSceneNames.begin();
	j = VSceneIds.begin();

    for(int x = 0; x < VSceneNames.size(); x++)
    {
        VScene *newScene = new VScene();

        newScene->m_pzVSceneName = (*i);
        newScene->m_pzVSceneID   = (*j);
        (*i).Empty();
        (*j).Empty();
        i++; j++;

        m_gVSceneList.push_back(newScene);
    }

	VSceneNames.clear();
    VSceneIds.clear();

    return retVal;
}

HRESULT VSceneCollection::readNavMap(void)
{
    HRESULT retVal = S_OK;

    // Load the button name structure
    HANDLE nameFile = CreateFile("..\\XODashMain\\NavigationMap.cpp",
                                 GENERIC_READ,
                                 FILE_SHARE_READ,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    DWORD dwSize = GetFileSize(nameFile, NULL);

    char *pInputBuffer = new char[dwSize + 1];
    DWORD numRead = 0;

    ReadFile(nameFile, pInputBuffer, dwSize, &numRead, NULL);
    pInputBuffer[dwSize] = '\0';

    // Now that we have the file loaded into inputBuffer, remove everything
    // before the first { 
    int nBeginStringPos = strcspn( pInputBuffer, "{" );

    // Get a pointer to the beginning of the text we care about
    char *pCleanString = pInputBuffer + nBeginStringPos + 1;

    char seps[] = "{,\t\n\r ;";
    char *token;
    int  currentState = 0;    // current state tracks the naviation map data
    VSceneList::iterator i;  // our current scene
    i = m_gVSceneList.begin();

    VButtonSceneList::iterator j;  // our current button in the scene

    token = strtok( pCleanString, seps );
    while( token != NULL )
    {
        if(strcmp(token, "}") == 0)
        {
            currentState++;
            if(currentState > 2)
            {
                currentState = 0;
                i++; // next scene
                token = strtok( NULL, seps );
            }
            
            token = strtok( NULL, seps );

            if( strcmp("eLastSceneId", token) == 0)  // we are at the end of the navigation links
            {
                break;
            }

            j = (*i)->m_VButtonList.begin();
        }

        // Reading out the comments
        if(strcmp(token, "/*") == 0)
        {
            while(strcmp(token, "*/") != 0)
            {
                token = strtok( NULL, seps );
            }      
            // Get the next appropriate token
            token = strtok( NULL, seps );
        }

        if(currentState == 0)  // we are reading the links
        {
            if(strcmp("eLastSceneId", token) == 0)
            {
                break;  // we are finished reading the navigation links
            }

            // Add the button to the scene and save the link
            VButtonScene *newButton = new VButtonScene();
            newButton->m_pVButtonLink = token;
        
            (*i)->m_VButtonList.push_back(newButton);
        }

        if(currentState == 1) // we are reading in the buttons for the scene
        {
            // find the cooresponding button in our global list
            if( strcmp("eNullButtonId", token) == 0)
            {
				// we are at the last button, so remove it's reference
				delete (*j);
				(*i)->m_VButtonList.remove(*j);
            }
            else
            {
                VButtonList::iterator x;
                for(x = m_gVButtonList.begin(); x != m_gVButtonList.end(); x++)
                {
                    if((*x)->m_pVButtonID.Compare(token) == 0)
                    {
                        // found the button reference in our global list, point to it!
                        (*j)->m_pVButton = (*x);

                        break;
                    }
                }
                j++; // next button
            }
        }   

        // Get the next token
        token = strtok( NULL, seps );
    }

    delete [] pInputBuffer;
    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::readNavDesc(void)
{
    HRESULT retVal = S_OK;

    // Load the button name structure
    HANDLE nameFile = CreateFile("..\\XODashMain\\NavigationDesc.cpp",
                                 GENERIC_READ,
                                 FILE_SHARE_READ,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    DWORD dwSize = GetFileSize(nameFile, NULL);

    char *pInputBuffer = new char[dwSize + 1];
    DWORD numRead = 0;

    ReadFile(nameFile, pInputBuffer, dwSize, &numRead, NULL);

    // Now that we have the file loaded into inputBuffer, remove everything
    // before the first { 
    int nBeginStringPos = strcspn( pInputBuffer, "{" );

    // Get a pointer to the beginning of the text we care about
    char *pCleanString = pInputBuffer + nBeginStringPos + 1;

    char seps[] = "{,\t\n\r\" ;}";
    char *token;

    VSceneList::iterator i;  // our current scene
    i = m_gVSceneList.begin();

    token = strtok( pCleanString, seps );
    while( token != NULL )
    {

        // Reading out the comments
        if(strcmp(token, "/*") == 0)
        {
            while(strcmp(token, "*/") != 0)
            {
                token = strtok( NULL, seps );
            }      
            // Get the next appropriate token
            token = strtok( NULL, seps );
        }

        // Ignore the ANSI header
        if(strcmp("NULL", token) == 0)  // The end of the scenes
        {
            break;
        }

        // Read in the scene path
        token = strtok( NULL, seps );
        (*i)->m_pzVScenePath = token;

        // Read in the text name
        token = strtok( NULL, seps );
        (*i)->m_pzVSceneTextName = token;

        // Read in the top value
        token = strtok( NULL, seps );
        (*i)->m_pzVSceneShowTop = token;

        // Pass the NULL token
        token = strtok( NULL, seps );

        // Get the Button Focus
        token = strtok( NULL, seps );
        (*i)->m_pzVButtonFocus = token;

        i++;

        // Get the next token
        token = strtok( NULL, seps );
    }

    delete [] pInputBuffer;
    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::initalize(void)
{
    HRESULT retVal = S_OK;
    
    initalizeButtons();
    initalizeScenes();
    readNavMap();
    readNavDesc();
    
    return retVal;
}

void WriteFilePragma(HANDLE nameFile)
{
    // Write file Pragma Header
    unsigned long numWrite;
    CString outputBuffer = "//Copyright (c) Microsoft Corporation.  All rights reserved.\n\n#pragma once\n\n";
    WriteFile(nameFile, outputBuffer, strlen(outputBuffer), &numWrite, NULL);
}

void WriteCopyRight(HANDLE nameFile)
{
    // Write file Pragma Header
    unsigned long numWrite;
    CString outputBuffer = "//Copyright (c) Microsoft Corporation.  All rights reserved.\n\n";
    WriteFile(nameFile, outputBuffer, strlen(outputBuffer), &numWrite, NULL);
}

HRESULT VSceneCollection::writeButtonNames(char *fileName)
{
    HRESULT retVal = S_OK;
    unsigned long numWrite = 0;

    // Load the button name structure
    HANDLE nameFile = CreateFile(fileName,
                                 GENERIC_WRITE,
                                 FILE_SHARE_WRITE,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    WriteFilePragma(nameFile);

    // Write file Header
    CString outputBuffer = "static char* ButtonName[] = \n{\n";
    WriteFile(nameFile, outputBuffer, strlen(outputBuffer), &numWrite, NULL);

    VButtonList::iterator j;
    for(j = m_gVButtonList.begin(); j != m_gVButtonList.end(); j++)
    {
        // for every scenes button, print out the name into a file
        CString outputStr;
        outputStr.Format("\t\"%s\",\n", (*j)->m_pVButtonName);
        WriteFile(nameFile, outputStr, outputStr.GetLength(), &numWrite, NULL);
    }

    WriteFile(nameFile, "};", strlen("};"), &numWrite, NULL);

    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::writeButtonIds(char *fileName)
{
    HRESULT retVal = S_OK;
    unsigned long numWrite = 0;

    // Load the button name structure
    HANDLE nameFile = CreateFile(fileName,
                                 GENERIC_WRITE,
                                 FILE_SHARE_WRITE,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    WriteFilePragma(nameFile);

    // Write file Header
    CString outputBuffer = "enum eButtonId\n{\n\teNullButtonId = -1,\n";
    WriteFile(nameFile, outputBuffer, strlen(outputBuffer), &numWrite, NULL);

    VButtonList::iterator j;
    for(j = m_gVButtonList.begin(); j != m_gVButtonList.end(); j++)
    {
        // for every scenes button, print out the name into a file
        char outputStr[255];
        sprintf(outputStr, "\t%s,\n", (*j)->m_pVButtonID);
        WriteFile(nameFile, outputStr, strlen(outputStr), &numWrite, NULL);
    }

    WriteFile(nameFile, "\teLastButtonId\n};", strlen("\teLastButtonId\n};"), &numWrite, NULL);

    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::writeButtonHelp(char *fileName)
{
    HRESULT retVal = S_OK;
    unsigned long numWrite = 0;

    // Load the button name structure
    HANDLE nameFile = CreateFile(fileName,
                                 GENERIC_WRITE,
                                 FILE_SHARE_WRITE,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    WriteFilePragma(nameFile);

    // Write file Header
    CString outputBuffer = "static TCHAR* ButtonHelpText[] =\n{\n";
    WriteFile(nameFile, outputBuffer, strlen(outputBuffer), &numWrite, NULL);

    VButtonList::iterator j;
    for(j = m_gVButtonList.begin(); j != m_gVButtonList.end(); j++)
    {
        // for every scenes button, print out the name into a file
        CString outputStr;
        outputStr.Format("\t/*%s*/\n\t_T(\"%s\"),\n", (*j)->m_pVButtonName, (*j)->m_pVHelpText);
        WriteFile(nameFile, outputStr, outputStr.GetLength(), &numWrite, NULL);
    }

    WriteFile(nameFile, "};", strlen("};"), &numWrite, NULL);

    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::writeNavDesc(char *fileName)
{
    HRESULT retVal = S_OK;
    unsigned long numWrite = 0;

    // Load the button name structure
    HANDLE nameFile = CreateFile(fileName,
                                 GENERIC_WRITE,
                                 FILE_SHARE_WRITE,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    WriteCopyRight(nameFile);

    // Write file Header
    CString outputBuffer = "CSceneNavigator* CSceneNavigator::pinstance = 0;\nSceneDescription CSceneNavigator::m_SceneDataMap[] = \n{\n";
    WriteFile(nameFile, outputBuffer, strlen(outputBuffer), &numWrite, NULL);

    VSceneList::iterator j;
    for(j = m_gVSceneList.begin(); j != m_gVSceneList.end(); j++)
    {
        // Dump out some comments
        CString szSceneComments;
        szSceneComments.Format("\n\t/* %s Scene */\n", (*j)->m_pzVSceneName);
        WriteFile(nameFile, szSceneComments, strlen(szSceneComments), &numWrite, NULL);

        WriteFile(nameFile, "\t{\n\t\t", strlen("\t{\n\t\t"), &numWrite, NULL);

        // for every scenes button, print out the name into a file
        char outputStr[255];
        sprintf(outputStr, "XBG_PATH_ANSI\"%s\", \"%s\", %s, NULL, %s\n", (*j)->m_pzVScenePath, (*j)->m_pzVSceneTextName, (*j)->m_pzVSceneShowTop, (*j)->m_pzVButtonFocus );
        WriteFile(nameFile, outputStr, strlen(outputStr), &numWrite, NULL);

        WriteFile(nameFile, "\t},\n", strlen("\t},\n"), &numWrite, NULL);
    }

    CString closeBuffer = "\t{ NULL, false, NULL },\n};";
    WriteFile(nameFile, closeBuffer, strlen(closeBuffer), &numWrite, NULL);

    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::writeNavMap(char *fileName)
{
    HRESULT retVal = S_OK;
    unsigned long numWrite = 0;

    // Load the button name structure
    HANDLE nameFile = CreateFile(fileName,
                                 GENERIC_WRITE,
                                 FILE_SHARE_WRITE,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    WriteCopyRight(nameFile);

    // Write file Header
    CString outputBuffer = "SceneChild CSceneNavigator::m_SceneHierarchy[] =\n{\n";
    WriteFile(nameFile, outputBuffer, strlen(outputBuffer), &numWrite, NULL);

    VSceneList::iterator j;
    for(j = m_gVSceneList.begin(); j != m_gVSceneList.end(); j++)
    {
        // Dump out some comments
        CString szSceneComments;
        szSceneComments.Format("\n\t/* %s Scene */\n", (*j)->m_pzVSceneName);
        WriteFile(nameFile, szSceneComments, strlen(szSceneComments), &numWrite, NULL);

        WriteFile(nameFile, "\t{\n\t\t", strlen("\t{\n\t\t"), &numWrite, NULL);

        // Write out the links
        WriteFile(nameFile, "{ ", strlen("{ "), &numWrite, NULL);
        VButtonSceneList::iterator i;
        for(i = (*j)->m_VButtonList.begin(); i != (*j)->m_VButtonList.end(); i++)
        {
            char outputStr[255];
            sprintf(outputStr, "%s, ", (*i)->m_pVButtonLink);
            WriteFile(nameFile, outputStr, strlen(outputStr), &numWrite, NULL);
        }
        WriteFile(nameFile, "eNullSceneId },\n", strlen("eNullSceneId },\n"), &numWrite, NULL);

        // Write out the buttons
        WriteFile(nameFile, "\t\t{ ", strlen("\t\t{ "), &numWrite, NULL);
        for(i = (*j)->m_VButtonList.begin(); i != (*j)->m_VButtonList.end(); i++)
        {
            char outputStr[255];
            sprintf(outputStr, "%s, ", (*i)->m_pVButton->m_pVButtonID);
            WriteFile(nameFile, outputStr, strlen(outputStr), &numWrite, NULL);
        }
        WriteFile(nameFile, "eNullButtonId },\n", strlen("eNullButtonId },\n"), &numWrite, NULL);

        // Write out the callbacks
        WriteFile(nameFile, "\t\t{ ", strlen("\t\t{ "), &numWrite, NULL);
        for(int x = 0; x < (*j)->m_VButtonList.size(); x++)
        {
            WriteFile(nameFile, "NULL, ", strlen("NULL, "), &numWrite, NULL);
        }
        WriteFile(nameFile, "NULL }\n\t},\n", strlen("NULL }\n\t},\n"), &numWrite, NULL);
    }

    CString closeBuffer = "\t{\n\t\t{ eLastSceneId },\n\t\t{ eNullButtonId },\n\t\t{ NULL }\n\t}\n};";
    WriteFile(nameFile, closeBuffer, strlen(closeBuffer), &numWrite, NULL);

    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::writeSceneNames(char *fileName)
{
    HRESULT retVal = S_OK;
    unsigned long numWrite = 0;

    // Load the button name structure
    HANDLE nameFile = CreateFile(fileName,
                                 GENERIC_WRITE,
                                 FILE_SHARE_WRITE,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    WriteFilePragma(nameFile);

    // Write file Header
    CString outputBuffer = "static char* SceneName[] = \n{\n";
    WriteFile(nameFile, outputBuffer, strlen(outputBuffer), &numWrite, NULL);

    VSceneList::iterator j;
    for(j = m_gVSceneList.begin(); j != m_gVSceneList.end(); j++)
    {
        // for every scenes button, print out the name into a file
        char outputStr[255];
        sprintf(outputStr, "\t\"%s\",\n", (*j)->m_pzVSceneName);
        WriteFile(nameFile, outputStr, strlen(outputStr), &numWrite, NULL);
    }

    WriteFile(nameFile, "};", strlen("};"), &numWrite, NULL);

    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::writeSceneIds(char *fileName)
{
    HRESULT retVal = S_OK;
    unsigned long numWrite = 0;

    // Load the button name structure
    HANDLE nameFile = CreateFile(fileName,
                                 GENERIC_WRITE,
                                 FILE_SHARE_WRITE,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    WriteFilePragma(nameFile);

    // Write file Header
    CString outputBuffer = "enum eSceneId\n{\n\teNullSceneId = -1,\n";
    WriteFile(nameFile, outputBuffer, strlen(outputBuffer), &numWrite, NULL);

    VSceneList::iterator j;
    for(j = m_gVSceneList.begin(); j != m_gVSceneList.end(); j++)
    {
        // for every scenes button, print out the name into a file
        char outputStr[255];
        sprintf(outputStr, "\t%s,\n", (*j)->m_pzVSceneID);
        WriteFile(nameFile, outputStr, strlen(outputStr), &numWrite, NULL);
    }

    WriteFile(nameFile, "\teLastSceneId\n};", strlen("\teLastButtonId\n};"), &numWrite, NULL);

    CloseHandle(nameFile);

    return retVal;
}

HRESULT VSceneCollection::writeScenesToLoad(char *fileName)
{
    HRESULT retVal = S_OK;
    unsigned long numWrite = 0;

    // Load the button name structure
    HANDLE nameFile = CreateFile(fileName,
                                 GENERIC_WRITE,
                                 FILE_SHARE_WRITE,
                                 NULL,
                                 CREATE_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);

    // Create an array to see if the scene has been written out yet
    bool *bWriteCheck;
    bWriteCheck = new bool[m_gVSceneList.size()];

    for(int i = 0; i < m_gVSceneList.size(); i++)
    {
        bWriteCheck[i] = true;
    }

    int nCurrentCheck = 0;
    VSceneList::iterator j;
    for(j = m_gVSceneList.begin(); j != m_gVSceneList.end(); j++)
    {
        // for every scenes button, print out the name into a file
        if(bWriteCheck[nCurrentCheck])
        {
            char outputStr[255];
            CString szScenePath = (*j)->m_pzVScenePath;
            sprintf(outputStr, "%s", szScenePath.Left(szScenePath.GetLength() - 4));
            WriteFile(nameFile, outputStr, strlen(outputStr), &numWrite, NULL);

            // Now write out all scenes that share that filename
            int nCheckFile = 0;
            VSceneList::iterator k;
            for(k = m_gVSceneList.begin(); k != m_gVSceneList.end(); k++)
            {
                if(strcmp((*k)->m_pzVScenePath, (*j)->m_pzVScenePath) == 0)
                {
                    CString szOutputStr;
                    szOutputStr.Format(",%s", (*k)->m_pzVSceneTextName);
                    WriteFile(nameFile, szOutputStr, strlen(szOutputStr), &numWrite, NULL);
                    bWriteCheck[nCheckFile] = false;
                }
                nCheckFile++;
            }
            WriteFile(nameFile, "\n", strlen("\n"), &numWrite, NULL);
        }
        
        nCurrentCheck++;
    }
    
    CloseHandle(nameFile);

    delete [] bWriteCheck;

    return retVal;
}

