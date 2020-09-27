using System;

using Modeling;
using Modeling.Events;

//TODO: Rename the Model Namespace to reflect your area
namespace CustomModels {

	/****************  StartMenu Model *****************
	 *	Description: 
	 *	Area: 
	 *	Author: 
	 *	Dependencies: Modeling.dll
	 *	Revision History:
	 **********************************************************/
	/// <summary>
	/// StartMenu Model class
	/// </summary>
	public class StartMenu : Model 
	{

		//Custom Properties
		/* TODO: Add any custom Properties here that you want to make available to your Model handlers
		 * Example:
		 *	private int myVariable = 5;
		 */
		
		/// <summary>
		/// Creates a StartMenu Model instance
		/// </summary>
		public StartMenu()
		{
			Description = "StartMenu Model";
			ModelPath = "StartMenu.csv";

			//Attach Event Handlers
			OnInitialize += new EventHandler(OnInitialize_Handler);
			OnCleanUp += new EventHandler(OnCleanUp_Handler);
			OnGetCurrentState += new StateEventHandler(OnGetCurrentState_Handler);

			//Add StateVariables
			AddStateVariable("ScreenName");
			AddStateVariable("ButtonFocus");			
			//Add Action Handlers
			AddAction("StartButtonPress", new ActionHandler(StartButtonPress));
			AddAction("OtherButtonPress", new ActionHandler(OtherButtonPress));
			AddAction("StandardNavigationUpPress", new ActionHandler(StandardNavigationUpPress));
			AddAction("StandardNavigationDownPress", new ActionHandler(StandardNavigationDownPress));
			AddAction("NavigateLeftRightPress", new ActionHandler(NavigateLeftRightPress));
			AddAction("BackButtonPress", new ActionHandler(BackButtonPress));
		}


		/// <summary>
		/// Initializes the Model
		/// </summary>
		/// <remarks>
		/// Attached to OnInitialize event which is fired when your model is
		/// created and added to a Traversal
		/// </remarks>
		/// <param name="sender">The model that fired the event</param>
		/// <param name="e">Contains no information (EventArgs.Empty)</param>
		private void OnInitialize_Handler(object sender, EventArgs e)
		{
			//TODO: Put code here that prepares the interfaces that your model will test.
			
		}


		/// <summary>
		/// Cleans up the Model
		/// </summary>
		/// <remarks>
		/// Attached to OnCleanUp event which is fired when your model is
		/// removed from the Traversal
		/// </remarks>
		/// <param name="sender">The model that fired the event</param>
		/// <param name="e">Contains no information (EventArgs.Empty)</param>
		private void OnCleanUp_Handler(object sender, EventArgs e) 
		{
			//TODO: Put code here that releases any interfaces that your model used during the test.
			
		}


		/// <summary>
		/// Gets the current State of the Model
		/// </summary>
		/// <remarks>
		/// Attached to OnGetCurrentState event which is fired after
		/// each action to validate
		/// </remarks>
		/// <param name="sender">Model that fired the event</param>
		/// <param name="e">The current State in a StateEventArgs</param>
		private void OnGetCurrentState_Handler(object sender, StateEventArgs e)
		{
			// The state values set here will be compared to the expected state by the Model's default ValidateState function.
			// Only put code here that sets the State object to represent the current State of the Model.
			
			//TODO: Modify the sample code below to set the value of each state variable
			e.State["ScreenName"] = null;	//This screen is open
			e.State["ButtonFocus"] = null;	//This button in main has focus
		}

		

		/// <summary>
		/// 
		/// </summary>
		/// <remarks>Handler for StartButtonPress</remarks>
		/// <param name="endState">Expected end State object</param>
		/// <param name="inParams">Input action parameters object</param>
		/// <param name="outParams">Output action parameters object</param>
		/// <returns>false if errors</returns>
		private bool StartButtonPress(State endState, State inParams, State outParams)
		{
			//Action Params (listed here for convienence during coding)
			//inParams["StartButtons"] - Press a start button

			/* You should perform your action based on the Action Parameters passed to you in the Params Object.
			 * Use the ParamsOut object to validate the result of the action
			 * If the action fails you should call RaiseError and return false.
			 */
			//TODO: Add necessary action specific functionality here.

			return true;
		}


		/// <summary>
		/// 
		/// </summary>
		/// <remarks>Handler for OtherButtonPress</remarks>
		/// <param name="endState">Expected end State object</param>
		/// <param name="inParams">Input action parameters object</param>
		/// <param name="outParams">Output action parameters object</param>
		/// <returns>false if errors</returns>
		private bool OtherButtonPress(State endState, State inParams, State outParams)
		{
			//Action Params (listed here for convienence during coding)
			//inParams["OtherButtons"] - press another button

			/* You should perform your action based on the Action Parameters passed to you in the Params Object.
			 * Use the ParamsOut object to validate the result of the action
			 * If the action fails you should call RaiseError and return false.
			 */
			//TODO: Add necessary action specific functionality here.

			return true;
		}


		/// <summary>
		/// 
		/// </summary>
		/// <remarks>Handler for StandardNavigationUpPress</remarks>
		/// <param name="endState">Expected end State object</param>
		/// <param name="inParams">Input action parameters object</param>
		/// <param name="outParams">Output action parameters object</param>
		/// <returns>false if errors</returns>
		private bool StandardNavigationUpPress(State endState, State inParams, State outParams)
		{
			//Action Params (listed here for convienence during coding)
			//inParams["StandardNavigationUp"] - Navigate the dash using standard means

			/* You should perform your action based on the Action Parameters passed to you in the Params Object.
			 * Use the ParamsOut object to validate the result of the action
			 * If the action fails you should call RaiseError and return false.
			 */
			//TODO: Add necessary action specific functionality here.

			return true;
		}


		/// <summary>
		/// 
		/// </summary>
		/// <remarks>Handler for StandardNavigationDownPress</remarks>
		/// <param name="endState">Expected end State object</param>
		/// <param name="inParams">Input action parameters object</param>
		/// <param name="outParams">Output action parameters object</param>
		/// <returns>false if errors</returns>
		private bool StandardNavigationDownPress(State endState, State inParams, State outParams)
		{
			//Action Params (listed here for convienence during coding)
			//inParams["StandardNavigationDown"] - Navigate the dash using standard means 

			/* You should perform your action based on the Action Parameters passed to you in the Params Object.
			 * Use the ParamsOut object to validate the result of the action
			 * If the action fails you should call RaiseError and return false.
			 */
			//TODO: Add necessary action specific functionality here.

			return true;
		}


		/// <summary>
		/// 
		/// </summary>
		/// <remarks>Handler for NavigateLeftRightPress</remarks>
		/// <param name="endState">Expected end State object</param>
		/// <param name="inParams">Input action parameters object</param>
		/// <param name="outParams">Output action parameters object</param>
		/// <returns>false if errors</returns>
		private bool NavigateLeftRightPress(State endState, State inParams, State outParams)
		{
			//Action Params (listed here for convienence during coding)
			//inParams["NavigateLeftRight"] - Navigate the dash using standard means 

			/* You should perform your action based on the Action Parameters passed to you in the Params Object.
			 * Use the ParamsOut object to validate the result of the action
			 * If the action fails you should call RaiseError and return false.
			 */
			//TODO: Add necessary action specific functionality here.

			return true;
		}


		/// <summary>
		/// 
		/// </summary>
		/// <remarks>Handler for BackButtonPress</remarks>
		/// <param name="endState">Expected end State object</param>
		/// <param name="inParams">Input action parameters object</param>
		/// <param name="outParams">Output action parameters object</param>
		/// <returns>false if errors</returns>
		private bool BackButtonPress(State endState, State inParams, State outParams)
		{
			//Action Params (listed here for convienence during coding)
			//inParams["BackButton"] - Press a back button

			/* You should perform your action based on the Action Parameters passed to you in the Params Object.
			 * Use the ParamsOut object to validate the result of the action
			 * If the action fails you should call RaiseError and return false.
			 */
			//TODO: Add necessary action specific functionality here.

			return true;
		}

	
	}

}

//This script file was generated using TMT on: Monday, January 28, 2002 15:27:26
