// (c) 2001 Microsoft Corporation

// Copied from vs\src\vc\ide\wizards\1033\common.js SetFilters

/******************************************************************************
 Description: Adds source, include and resource filters for project folders.
              The symbols contain the file extensions to be used for filtering.
       oProj: Project object
******************************************************************************/
function XboxSetFilters(oProj)
{
	try
	{
		var strSrcFilter = wizard.FindSymbol("SOURCE_FILTER");
		var L_strSource_Text = "Source Files";
		var group = oProj.Object.AddFilter(L_strSource_Text);
		group.Filter = strSrcFilter;

		var strIncFilter = wizard.FindSymbol("INCLUDE_FILTER");
		var L_strHeader_Text = "Header Files";
		group = oProj.Object.AddFilter(L_strHeader_Text);
		group.Filter = strIncFilter;
	}
	catch(e)
	{   
		throw e;
	}
}

/******************************************************************************
    Description: Creates a C++ project
 strProjectName: Project Name
 strProjectPath: The path that the project will be created in

 Modeled on CreateProject in wizards\1033\common.js

 This version uses a differend default.vcproj file

******************************************************************************/
function XboxCreateProject(strProjectName, strProjectPath)
{
	try
	{
		var strProjTemplatePath = wizard.FindSymbol("PROJECT_TEMPLATE_PATH");
		var strProjTemplate = strProjTemplatePath + "\\xboxwiz\\default.vcproj"; 

		var Solution = dte.Solution;
		var strSolutionName = "";
		if (wizard.FindSymbol("CLOSE_SOLUTION"))
		{
			Solution.Close();
			strSolutionName = wizard.FindSymbol("VS_SOLUTION_NAME");
			if (strSolutionName.length)
			{
				var strSolutionPath = strProjectPath.substr(0, strProjectPath.length - strProjectName.length);
				Solution.Create(strSolutionPath, strSolutionName);
			}
		}

		var strProjectNameWithExt = strProjectName + ".vcproj";
		var oTarget = wizard.FindSymbol("TARGET");
		var oProj;
		if (wizard.FindSymbol("WIZARD_TYPE") == vsWizardAddSubProject)
		{
			var prjItem = oTarget.AddFromTemplate(strProjTemplate, strProjectPath + "\\" + strProjectNameWithExt);
			oProj = prjItem.SubProject;
		}
		else
		{
			oProj = oTarget.AddFromTemplate(strProjTemplate, strProjectPath, strProjectNameWithExt);
		}

		return oProj;
	}
	catch(e)
	{   
		throw e;
	}
}

function OnFinish(selProj, selObj)
{
	try
	{
		var strProjectPath = wizard.FindSymbol("PROJECT_PATH");
		var strProjectName = wizard.FindSymbol("PROJECT_NAME");

		var bEmptyProject = wizard.FindSymbol("EMPTY_PROJECT");

		selProj = XboxCreateProject(strProjectName, strProjectPath);
		selProj.Object.Keyword = "XboxProj";

		AddCommonConfig(selProj, strProjectName);

		// Override some of the common config settings 
		{
			var config = selProj.Object.Configurations("Debug");
			var CLTool = config.Tools("VCCLCompilerTool");
			CLTool.Detect64BitPortabilityProblems = false;

			config = selProj.Object.Configurations("Release");
			CLTool = config.Tools("VCCLCompilerTool");
			CLTool.Detect64BitPortabilityProblems = false;
		}


		var strAppType = GetAppType();
		AddSpecificConfig(selProj, strProjectName, bEmptyProject, strAppType);

		XboxSetFilters(selProj);

		if (!bEmptyProject)
		{
			var strCodePage = wizard.FindSymbol("CODE_PAGE");
			if (strCodePage == "1252")
				wizard.AddSymbol("ABOUTBOX_FONT_SIZE", "8");
			else
				wizard.AddSymbol("ABOUTBOX_FONT_SIZE", "9");
			
			var InfFile = CreateInfFile();
			var Pch = wizard.FindSymbol("PRE_COMPILED_HEADER");

			AddFilesToProject(selProj, strProjectName, InfFile);
			SetCommonPchSettings(selProj);	

			InfFile.Delete();
		}
		selProj.Object.Save();
	}
	catch(e)
	{
		if (e.description.length != 0)
			SetErrorInfo(e);
		return e.number
	}
}

function SetFileProperties(projfile, strName)
{
	return false;
}

function DoOpenFile(strTarget)
{
	return false;
}

function GetTargetName(strName, strProjectName, strResPath, strHelpPath)
{
	try
	{
		var strTarget = strName;

		if (strName == "readme.txt")
			strTarget = "ReadMe.txt";
		if (strName.substr(0, 4) == "root")
		{
			var strlen = strName.length;
			strTarget = strProjectName + strName.substr(4, strlen - 4);

		}
		return strTarget; 
	}
	catch(e)
	{
		throw e;
	}
}

function GetAppType()
{
	try
	{
		var strAppType = "";
		if (wizard.FindSymbol("XBOX_APP"))
			strAppType = "XBOX";
		else
		{
			if (wizard.FindSymbol("LIB_APP"))
				strAppType = "LIB";
		}
		return strAppType;
	}
	catch(e)
	{
		throw e;
	}
}

function AddSpecificConfig(proj, strProjectName, bEmptyProject, strAppType)
{
	try
	{
		var config = proj.Object.Configurations("Debug");
		config.CharacterSet = charSetMBCS;

		if (strAppType == "LIB")
			config.ConfigurationType = typeStaticLibrary;

		var CLTool = config.Tools("VCCLCompilerTool");
		CLTool.PrecompiledHeaderFile = "$(OutDir)/$(ProjectName).pch";
		CLTool.RuntimeLibrary = rtSingleThreadedDebug;

		var strDefines = GetPlatformDefine(config);
		strDefines += "_DEBUG";

		switch(strAppType)
		{
			case "LIB":
				strDefines += ";_LIB;_XBOX";
				break;
			case "XBOX":
				strDefines += ";_XBOX";
				break;
			default:
				break;
		}

		CLTool.PreprocessorDefinitions = strDefines;
		if (bEmptyProject)
			CLTool.UsePrecompiledHeader = pchNone;

		CLTool.DebugInformationFormat = debugEditAndContinue;

		if (strAppType == "LIB")
		{
			var LibTool = config.Tools("VCLibrarianTool");
			LibTool.OutputFile = "$(OutDir)/$(ProjectName).lib";
		}
		else
		{
			var LinkTool = config.Tools("VCLinkerTool");
			LinkTool.ProgramDatabaseFile = "$(OutDir)/$(ProjectName).pdb";
			LinkTool.GenerateDebugInformation = true;
			LinkTool.LinkIncremental = linkIncrementalYes;
			LinkTool.AdditionalDependencies = "xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xboxkrnl.lib";
			LinkTool.OutputFile = "$(OutDir)/$(ProjectName).exe";
		}

		config = proj.Object.Configurations.Item("Release");
		config.CharacterSet = charSetMBCS;

		if (strAppType == "LIB")
			config.ConfigurationType = typeStaticLibrary;

		var CLTool = config.Tools("VCCLCompilerTool");
		if ((strAppType == "LIB") || (strAppType == "XBOX" && bEmptyProject))
		{
			CLTool.UsePrecompiledHeader = pchGenerateAuto;
		}

		CLTool.RuntimeLibrary = rtSingleThreaded;
		CLTool.InlineFunctionExpansion = expandOnlyInline;

		var strDefines = GetPlatformDefine(config);
		strDefines += "NDEBUG";
		if (bEmptyProject)
			CLTool.UsePrecompiledHeader = pchNone;

		CLTool.DebugInformationFormat = debugEnabled;

		switch(strAppType)
		{
			case "LIB":
				strDefines += ";_LIB";
				break;
			case "XBOX":
				strDefines += ";_XBOX";
				break;
			default:
				break;
		}

		CLTool.PreprocessorDefinitions = strDefines;

		if (strAppType == "LIB")
		{
			var LibTool = config.Tools("VCLibrarianTool");
			LibTool.OutputFile = "$(OutDir)/$(ProjectName).lib";
		}
		else
		{
			var LinkTool = config.Tools("VCLinkerTool");
			LinkTool.GenerateDebugInformation = true;
			LinkTool.LinkIncremental = linkIncrementalNo;
			LinkTool.AdditionalDependencies = "xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xboxkrnl.lib";
			LinkTool.OutputFile = "$(OutDir)/$(ProjectName).exe";
		}
	}
	catch(e)
	{
		throw e;
	}
}

