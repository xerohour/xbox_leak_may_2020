using System;

namespace Cortana
{

	public class Verify
	{
		
		private State MyState = new State();

		extern public bool VerifyButton(string ButtonName);
		extern public bool VerifyScreen(string ScreenName);
		extern public bool VerifyFocus(string Focus);
		extern public bool VerifyHelpText(string HelpText);


	/*	{
			//This is where we check the debug spew for the last button
			return true;
		}*/
		
	}
}
