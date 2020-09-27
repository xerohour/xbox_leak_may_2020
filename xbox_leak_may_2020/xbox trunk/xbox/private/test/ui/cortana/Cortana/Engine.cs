using System;

namespace Cortana
{
	/// <summary>
	/// Summary description for Engine.
	/// </summary>
	public class CEngine
	{
		private bool		m_Initialized = false;
		private string		m_ModelFilename = "";
		private string		m_InitRunFilename = "";
		private string		m_DRIVER_TYPE = "";
		private string		m_SIMULATOR_IP = "";
		private string		m_SIMULATOR_PORT = "";
		private string		m_BUTTON_DOWN_DELAY = "";
		private string		m_BUTTON_UP_DELAY = "";
		private string		m_XDASH_TRANSITION_DELAY = "";
		private string		m_INIT_TEST_FILENAME = "";
		


		public string ModelFilename()
		{
			string s = m_ModelFilename;
			return s;	// return copy of private data
		}

		public string InitRunFilename()
		{
			string s = m_InitRunFilename;
			return s;	// return copy of private data
		}

		public string INIT_TEST_FILENAME()
		{
			string s = m_INIT_TEST_FILENAME;
			return s;	// return copy of private data
		}

		public string SIMULATOR_IP()
		{
			string s = m_SIMULATOR_IP;
			return s;	// return copy of private data
		}

		public ushort BUTTON_DOWN_DELAY()
		{
			string s = m_BUTTON_DOWN_DELAY;
			ushort u = UInt16.Parse(s);
			return u;	// return copy of private data
		}

		public ushort BUTTON_UP_DELAY()
		{
			string s = m_BUTTON_UP_DELAY;
			ushort u = UInt16.Parse(s);
			return u;	// return copy of private data
		}

		public int XDASH_TRANSITION_DELAY()
		{
			string s = m_XDASH_TRANSITION_DELAY;
			int i = Int32.Parse(s);
			return i;	// return copy of private data
		}

		public short SIMULATOR_PORT()
		{
			string s = m_SIMULATOR_PORT;
			short x = Int16.Parse(s);
			return x;	// return copy of private data
		}

		public void ShowUsage()
		{
			Console.WriteLine("Launches the XBOX UI Automation Framework.\n");
			Console.WriteLine("Cortana.exe	/M:modelname	/IR:initrunname\n");
			Console.WriteLine("/M	specifies the TMT model file to execute");
			Console.WriteLine("		/M:netconfig.xtc");
			Console.WriteLine("		/M:\"c:\\My Tests\\netconfig.xtc\"\n");
			Console.WriteLine("/IR	specifies the InitRun file used to preconfig the XTC run");
			Console.WriteLine("		/IR:InitRun.xml");
			Console.WriteLine("		/IR:\"c:\\My Tests\\InitRun.xml\"");
			Console.WriteLine();
		}

		public bool Init(string[] args)
		{
			// init's the engine
			// returns:
			//		false on failure to init
			//		true on success

			ClearVars();
			if (!ParseArgs(args))
				return false;
			if (!RequiredArgsProvided())
				return false;			

			m_Initialized = true;
			return true;
		}

		private void ClearVars()
		{
			m_Initialized = false;
			m_ModelFilename = "";
			m_InitRunFilename = "";
			m_DRIVER_TYPE = "";
			m_SIMULATOR_IP = "";
			m_SIMULATOR_PORT = "";
			m_BUTTON_DOWN_DELAY = "";
			m_BUTTON_UP_DELAY = "";
			m_XDASH_TRANSITION_DELAY = "";
			m_INIT_TEST_FILENAME = "";
		}

		private bool ParseArgs(string[] args)
		{
			string temp;

			foreach (string s in args)
			{
				temp = s.ToUpper();
				//Console.WriteLine("{0}",temp);
				if (temp.StartsWith("/M:"))
				{
					if ("" != m_ModelFilename)
					{
						return false; // switch already used
					}
					temp = temp.Substring(3);
					temp = temp.Trim();
					m_ModelFilename = temp;
				}
				else if (temp.StartsWith("/IR:"))
				{
					if ("" != m_InitRunFilename)
					{
						return false; // switch already used
					}
					temp = temp.Substring(4);
					temp = temp.Trim();
					m_InitRunFilename = temp;
				}
				else
				{
					return false;	//invalid arg found
				}

			}
			return true;
		}

		private bool RequiredArgsProvided()
		{
			if ("" != m_ModelFilename)
				return true;
			else
				return false; // requirements not met
		}


		public bool InitRun()
		{
			if (!m_Initialized)
			{
				Console.WriteLine("Engine::InitRun() - Engine Not Initialized!");
				return false;
			}
			CInitRun InitRun = new CInitRun();
			if (!InitRun.ProcessInitRunFile(this.InitRunFilename()))
			{
				return false;
			}

			m_INIT_TEST_FILENAME		= InitRun.INIT_TEST_FILENAME();
			m_DRIVER_TYPE				= InitRun.DRIVER_TYPE();
			m_SIMULATOR_IP				= InitRun.SIMULATOR_IP();
			m_SIMULATOR_PORT			= InitRun.SIMULATOR_PORT();
			m_BUTTON_DOWN_DELAY			= InitRun.BUTTON_DOWN_DELAY();
			m_BUTTON_UP_DELAY			= InitRun.BUTTON_UP_DELAY();
			m_XDASH_TRANSITION_DELAY	= InitRun.XDASH_TRANSITION_DELAY();


			return true;
		}
	}


}
