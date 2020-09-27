using System;


namespace Cortana
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	/// 

	class CMain
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		/// 

		[STAThread]
		static void Main(string[] args)
		{
			CEngine		m_Engine	= new CEngine();
			ProcessXTC	m_XTC_Obj	= new ProcessXTC();
			Simulator	m_INPUT		= new Simulator();

			if (-1 == args.GetUpperBound(0))
			{
				// Console.WriteLine("UI not yet implemented.\nDefaulting to console mode.\n");
				m_Engine.ShowUsage();
				return;
			}
			else
			{
				if (!m_Engine.Init(args))
				{
					m_Engine.ShowUsage();
					return;
				}
				Console.WriteLine("Start Processing...");
				Console.WriteLine("Model File: {0}",m_Engine.ModelFilename());
				Console.WriteLine("InitRun File: {0}",m_Engine.InitRunFilename());
				if (!m_Engine.InitRun())
				{
					Console.WriteLine("InitRun failed!");
					return;
				}
				else
				{
					m_INPUT.SIMULATOR_IP				= m_Engine.SIMULATOR_IP(); 
					m_INPUT.DUKE_PORT					= m_Engine.SIMULATOR_PORT();
					m_INPUT.BUTTON_DOWN_DELAY			= m_Engine.BUTTON_DOWN_DELAY();
					m_INPUT.BUTTON_UP_DELAY			= m_Engine.BUTTON_UP_DELAY();
					m_INPUT.XDASH_TRANSITION_DELAY	= m_Engine.XDASH_TRANSITION_DELAY();
					
					if (!m_INPUT.PlugDuke())
					{
						return;
					}

					Console.WriteLine("InitRun succeeded.");
					
				}

				m_XTC_Obj.ProcessXTC_Main(ref m_Engine, ref m_INPUT);
				
				Console.WriteLine("Done.");
			}
			return;
		}
	}


}
