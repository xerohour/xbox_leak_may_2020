using System;


namespace Cortana
{

	public class Simulator
	{
		//
		//PRIVATE DECLARATIONS
		//
		//declarations of the available objects
		private USBSIMULATORLib.SimulatorClass simulator = new USBSIMULATORLib.SimulatorClass();
		private USBSIMULATORLib.DukeClass duke = new USBSIMULATORLib.DukeClass();
		private USBSIMULATORLib.MUClass mu = new USBSIMULATORLib.MUClass();
		private USBSIMULATORLib.HawkClass hawk = new USBSIMULATORLib.HawkClass();
		
		//Private Methods
		private bool InsertDevice(USBSIMULATORLib.DukeClass dev)
		{
			//Plugs in a duke and verifies it's connection
			int retry = 3;
			int time;
			do  
			{
				dev.Plug();
				for(time=32; time<1000; time*=2)
				{
					System.Threading.Thread.Sleep(time);
					if(dev.enumerated == 1) return true;
				}
				dev.Unplug();
				System.Threading.Thread.Sleep(250);
				--retry;
			} while(retry != 0);
			
			Console.WriteLine("[InsertDevice]:Failed to enumerate Duke");
			return false;
		}

		private string VerifySimulator(string address) 
		{
			//Verifies the simulator is connected to the address that is given
			
			string version = simulator.GetVersion(address);
			if(version == "")
			{
				return "";
			}

			short boards = simulator.GetUSBPorts(address);
			if(boards == 0)
			{
				return "no USB";
			}
			
			string simulatorStats = "Simulator (" + address + "):\n";
			simulatorStats += "  Version:    " + version + "\n";
			simulatorStats += "  MacAddr:    " + simulator.GetMACAddr(address) + "\n";
			simulatorStats += "  USB Boards: NOT SURE\n";

			return simulatorStats;
		}

		
			
		//PUBLIC PROPERTIES
		public string SIMULATOR_IP;
		public ushort BUTTON_DOWN_DELAY
		{
			// The USB simulator will hold the button down this long before releasing
			get 
			{ 
				return duke.buttonDownDelay; 
			}
			set 
			{
				duke.buttonDownDelay = value;
 			}
		}

		public ushort BUTTON_UP_DELAY
		{
			// The USB simulator will wait this long before proceeding after a button press
			get
			{
				return duke.buttonUpDelay;
			}
			set
			{
				duke.buttonUpDelay = value;
			}
		}

		public int XDASH_TRANSITION_DELAY;  // Seconds it takes to transition between menus
		public short DUKE_PORT
		{
			// Port on USBsim that will drive the duke
			get
			{
				return duke.port;
			}
			set
			{
				duke.port = value;
			}
		}


		//METHODS
		public Simulator()
		{
			//Default Constructor
			SIMULATOR_IP = "";
			XDASH_TRANSITION_DELAY = 0;

		}

		~Simulator()
		{
			duke.Unplug();
			mu.Unplug();
			hawk.Unplug();

			duke.Free();
			mu.Free();
			hawk.Free();
		}

		public bool PlugDuke()
		{
			//Checks to see if we set the simulator_IP
			if (SIMULATOR_IP == "")
			{
				Console.WriteLine("[PlugDuke]:SIMULATOR_IP not set");
				return false;
			}

			//Checks to see if we set the DUKE_PORT
			if (duke.port == 0)
			{
				Console.WriteLine("[PlugDuke]:DUKE_PORT not set");
				return false;
			}

			// This Verifies that the Simulator is connected
			// before trying to connect a duke
			string SimStatus = VerifySimulator(SIMULATOR_IP);
			if (SimStatus == "")
			{
				Console.WriteLine("[PlugDuke]:The Simulator is not connected");
			}
			else
			{
				Console.WriteLine("[PlugDuke]:" + SimStatus);
			}

			duke.IP = SIMULATOR_IP;
			return InsertDevice(duke);
		}
        	
		public bool Button(string ButtonName)
		{
			//This enumeration check will have to be removed later
			//we shouldn't be going it before every press
			if (duke.enumerated == 1)
			{
				duke.Button(ButtonName);
				return true;
			}
			else
			{
				Console.WriteLine("[PlugDuke]:Duke not enumerated");
				return false;
			}

			//This is where we verify that the button press was recieved
			/*
			if (VerifyButton(ButtonName)==false)
			{
				//check enumeration and try again if not enumerated
			}
			else
			{
				return true;
			}
			*/

		}

		public bool SetButtonState(int digital,
			short a,
			short b,
			short x,
			short y,
			short black,
			short white,
			short left_t,
			short right_t,
			short thumbLX,
			short thumbLY,
			short thumbRX,
			short thumbRY)
		{
			duke.SetButtonState(digital, a,b,x,y,black,white,left_t,right_t,thumbLX,thumbLY,thumbRX,thumbRY);
		
			//This is where we verify the button press took place
			//
			// return VerifyButtons(all buttons)
			return true;
		
		}

		public bool PressButtonFrom_XTC_Command(string command)
		{
			bool result = false;
			string s = command.ToUpper();
			switch (s)
			{
					// deliberate fallthrough here
				case "PRESS_A":
					result = Button("A");
					break;
				case "PRESS_START": 
					result = Button("START");
					break;
				case "PRESS_X": 
					result = Button("X");
					break;
				case "PRESS_Y": 
					result = Button("Y");
					break;
				case "PRESS_BLACK": 
					result = Button("BLACK");
					break;
				case "PRESS_WHITE":
					result = Button("WHITE");
					break;
				case "PRESS_LEFT_STICK":
					result = Button("LEFTTHUMB");
					break;
				case "PRESS_RIGHT_STICK":
					result = Button("RIGHTTHUMB");
					break;
				case "PRESS_RIGHT_TRIGGER":
					result = Button("RIGHTTRIGGER");
					break;
				case "PRESS_LEFT_TRIGGER":
					result = Button("LEFTTRIGGER");
					break;
				case "DPAD_UP":
					result = Button("DPADN");
					break;
				case "LEFT_STICK_UP":
					result = Button("UP");
					break;
				case "DPAD_NW":
					result = Button("DPADNW");
					break;
				case "DPAD_NE":
					result = Button("DPADNE");
					break;
				case "LEFT_STICK_NW":
					result = Button("A");
					break;
				case "LEFT_STICK_NE":
					result = Button("A");
					break;
				case "DPAD_DOWN":
					result = Button("DPADS");
					break;
				case "LEFT_STICK_DOWN":
					result = Button("DOWN");
					break;
				case "DPAD_SW":
					result = Button("DPADSW");
					break;
				case "DPAD_SE":
					result = Button("DPADSE");
					break;
				case "LEFT_STICK_SW":
					result = Button("A");
					break;
				case "LEFT_STICK_SE": 
					result = Button("A");
					break;
				case "LEFT_STICK_LEFT": 
					result = Button("LEFT");
					break;
				case "LEFT_STICK_RIGHT": 
					result = Button("RIGHT");
					break;
				case "DPAD_LEFT": 
					result = Button("DPADW");
					break;
				case "DPAD_RIGHT": 
					result = Button("DPADE");
					break;
				case "RIGHT_STICK_LEFT":
					result = Button("A");
					break;
				case "RIGHT_STICK_RIGHT": 
					result = Button("A");
					break;
				case "PRESS_B":
					result = Button("B");
					break;
				case "PRESS_BACK":
					result = Button("BACK");
					break;
				default:
					Console.WriteLine("Unknown PARAM command: {0}",s);
					result = false;
					break;
			}
			
			return result;
		}
		
		
	}
}

