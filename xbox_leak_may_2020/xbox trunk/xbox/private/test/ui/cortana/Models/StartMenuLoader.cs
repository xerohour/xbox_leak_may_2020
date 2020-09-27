using System;

using Modeling;
using Modeling.Events;
using Modeling.TMT;

//TODO: Change the using statment below to reflect the actual namespace of your models
using CustomModels;

namespace Loaders{

	/***************  StartMenu Loader ****************
	 *	Description: Custom exe loader for StartMenu TMT project
	 *	Area: 
	 *	Author: 
	 *	Dependencies: Modeling.dll, Modeling.TMT.dll
	 *	Revision History:
	 **********************************************************/

	class StartMenuLoader{

		static void Main(string[] args){
		
			//Show Usage information
			if (args.Length == 0 || GetArgument("?", args) != null)
			{
				ShowUsage();
				return;
			}

			//Get the Command line arguments
			string XTCPath = GetArgument(null, args);
			if (XTCPath == null){
				string exeName = System.IO.Path.GetFileName(System.Reflection.Assembly.GetExecutingAssembly().Location);
				Console.WriteLine("You must specify a XTC file to load at command line.\n Type " + exeName + " /? for syntax information.");
				return;
			}
			int StartCase = -1;
			int EndCase = -1;
			if (GetArgument("StartCase", args) != null)
				StartCase = int.Parse(GetArgument("StartCase", args));
			if (GetArgument("EndCase", args) != null)
				EndCase = int.Parse(GetArgument("EndCase", args));

			//Specifies whether to throw an Exception when an object RaiseError()
			Automation.BreakOnError = false;
			//Specifies whether to allow Exceptions to be thrown
			Automation.Debug = (GetArgument("Debug", args) != null);
			
			//Create and open the TextLog object
			TextLog myLog = new TextLog("StartMenu.log", true);
			if (!myLog.Open())
			{
				Console.WriteLine("Errors occured while opening the Log file StartMenu.log.\n" + myLog.Errors);
				return;
			}

			//Create the Automation Object
			Automation myAutomation = new Automation();
			myAutomation.Logger = myLog;
			
			//Create the XTCTraversal and add it to the Automation object
			XTCTraversal myTraversal = new XTCTraversal(XTCPath, StartCase, EndCase);
			myAutomation.AddTraversal(myTraversal);
			if (myTraversal.Errors.Count > 0)
			{
				Console.WriteLine("Errors occured while loading the XTC file.\n" + myTraversal.Errors);
				myLog.Close();
				return;
			}
			
			bool testResult = false;
			
			//Initialize the Automation Object
			bool initResult = myAutomation.Initialize();
			if (initResult)
			{
				//Create the Models
				myTraversal.AddModel( new StartMenu() );
				if (myTraversal.Models[myTraversal.Models.Length-1].Errors.Count > 0)
				{
					Console.WriteLine("Errors occured during StartMenu Initialize.");
					Console.WriteLine(myTraversal.Models[myTraversal.Models.Length-1].Errors);
					goto CleanUp;
				}
				//Perform Automation Test
				testResult = myAutomation.Run();
			}

			CleanUp:
			//Clean Up the Automation
			myAutomation.CleanUp();
			
			//Close the TextLog
			myLog.Close();
			
			if (initResult && testResult)
				Console.WriteLine("\n**** PASS ****\n\n Automation results were successful.");
			else
				Console.WriteLine("\n**** FAIL ****\n\n" + myAutomation.Errors.ToString());
		}


		/// <summary>
		/// Shows command line syntax for this loader
		/// </summary>
		private static void ShowUsage(){
			string exeName = System.IO.Path.GetFileName(System.Reflection.Assembly.GetExecutingAssembly().Location);
			Console.WriteLine("Custom exe loader for StartMenu TMT project");
			Console.WriteLine("");
			Console.WriteLine("Usage:");
			Console.WriteLine("    " + exeName + " XTC_Filename [/StartCase:value] [/EndCase:value] [/Debug]");
			Console.WriteLine("");
			Console.WriteLine("XTC_Filename : Path to the TMT .xtc file.");
			Console.WriteLine("StartCase    : Test Case to start Traversing.");
			Console.WriteLine("EndCase      : Test Case to end Traversing.");
			Console.WriteLine("Debug        : Allow exceptions to be thrown and caught by a debugger.");
			Console.WriteLine("");
			Console.WriteLine("Example: " + exeName + " \"RandomQuery.xtc\" /StartCase:10 /EndCase:12");
		}

		/// <summary>
		/// Retrieves the value of a command line argument
		/// </summary>
		/// <param name="name">The name of the argument to retrieve</param>
		/// <param name="args">The arguments to retrieve from</param>
		/// <returns>The value of the specified argument (as a string), null if not found</returns>
		private static string GetArgument(string name, string[] args){
			foreach (string arg in args){
				if (arg.StartsWith("/")){
					int pos = arg.IndexOf(":");
					if (pos > 0){
						string argName = arg.Substring(1, pos-1);
						if (argName.ToLower() == name.ToLower())
							return arg.Substring(pos+1).Trim();
					}
					else if ("/" + name == arg)
						return "true";
				}
				else if (name == null)
					return arg;
			}
			return null;
		}

	}

}
