using System;



namespace Cortana
{

	public class State
	{		
		//PUBLIC METHODS
		public bool Connect(string xboxname)
		{
			debug.Connect(xboxname);
			// Check Connection
			return true;
		}

		public bool disconnect()
		{
			debug.Disconnect();	
			return true;
		}

		public bool Start()
		{
			debug.Start();	
			return true;
		}

		public bool Stop()
		{
			debug.Stop();
			return true;
		}

		public bool StartPolling()
		{
			return true;
		}
		
		public string GiveButtonPress()
		{
			return LAST_BUTTON;
		}

		public string GiveCurrentScreen()
		{
			return CURRENT_SCREEN;
		}	
	
		public string GiveCurrentFocus()
		{
			return CURRENT_FOCUS;
		}

		public string GiveHelpText()
		{
			return HELP_TEXT;
		}




		//PRIVATE PROPERTIES
		private XBDEBUGLib.NetDebug debug = new XBDEBUGLib.NetDebug();
		
		private string HELP_TEXT;
		private string CURRENT_SCREEN;
		private string CURRENT_FOCUS;
		private string LAST_BUTTON;

	}

}


