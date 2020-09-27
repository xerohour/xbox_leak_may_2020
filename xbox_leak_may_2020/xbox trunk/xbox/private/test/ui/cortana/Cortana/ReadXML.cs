using System;
using System.IO;
using System.Xml;

namespace Cortana
{
   public class ProcessXTC
   {
     
	   
	  public void ProcessXTC_Main(ref CEngine Engine, ref Simulator	INPUT)
      {

         XmlTextReader reader = null;
		 CInitTest m_InitTest = new CInitTest();

         reader = new XmlTextReader(Engine.ModelFilename());
		  try
		  {
			  reader.Read();
			  reader.Close();
			  reader = new XmlTextReader(Engine.ModelFilename());
		  }
		  catch
		  {
			  Console.WriteLine("Failure to read file: {0}",Engine.ModelFilename());
			  return;
		  }

		 Console.WriteLine("Processing of stream started...");
         Console.WriteLine();

		  int testcount = 0;
		  bool bTestInitialized = false;

		  if (null == reader)
		  {
			  Console.WriteLine("Unable to open file.");
		  }
		  else
		  {

			  while (reader.Read())
			  {
				  switch (reader.NodeType)
				  {
					  case XmlNodeType.ProcessingInstruction:
						  //OutputXML (reader, "ProcessingInstruction");
						  break;
					  case XmlNodeType.DocumentType:
						  //OutputXML (reader, "DocumentType");
						  break;
					  case XmlNodeType.Comment:
						  //OutputXML (reader, "Comment");
						  break;
					  case XmlNodeType.Element:
						  if ("TEST" == reader.Name)
						  {
							  testcount++;
							  if (testcount > 1)
								  Console.WriteLine();
							  bTestInitialized = false; // this gets set on first TRANSITION
							  Console.Write("Test {0}: ",testcount);						
							  reader.MoveToAttribute("Title");
							  Console.WriteLine(reader.Value);
						  }
				  
						  if ("TRANSITION" == reader.Name)
						  {
							  if (! bTestInitialized)
							  {
								  reader.MoveToAttribute("ModelName");
								  Console.WriteLine("Call InitTest(\"{0}\")",reader.Value);
								  if (!m_InitTest.ProcessInitTestCommands(ref Engine, ref INPUT,reader.Value))
								  {
								  	  Console.WriteLine("InitTest failed! Aborting Test");
								  	  return;
								  }
								  bTestInitialized = true;
							  }
						  }

						  if ("ACTION" == reader.Name)
						  {
							  if (0 == reader.AttributeCount)
							  {
								  Console.WriteLine("Empty ACTION detected. No action taken.");
							  }
							  else
							  {
								  reader.MoveToAttribute("Name");
								  Console.WriteLine("Performing ACTION: {0}",reader.Value);
								  // note - do no work here! the PARAM element is what we need to parse!
							  }
						  }

						  if ("PARAM" == reader.Name)
						  {
                              reader.MoveToAttribute("Value");
							  Console.WriteLine("Interpretting PARAM: {0}",reader.Value);
							  string s = reader.Value.ToUpper();
							  Console.WriteLine("Calling {0}",s);
							  
							  bool result = false;
							  switch (s)
							  {
								// deliberate fallthrough here
								  case "PRESS_A":
								  case "PRESS_START": 
								  case "PRESS_X": 
								  case "PRESS_Y": 
								  case "PRESS_BLACK": 
								  case "PRESS_WHITE":
								  case "PRESS_LEFT_STICK":
								  case "PRESS_RIGHT_STICK":
								  case "PRESS_RIGHT_TRIGGER":
								  case "PRESS_LEFT_TRIGGER":
								  case "DPAD_UP":
								  case "LEFT_STICK_UP":
								  case "DPAD_NW":
								  case "DPAD_NE":
								  case "LEFT_STICK_NW":
								  case "LEFT_STICK_NE":
								  case "DPAD_DOWN":
								  case "LEFT_STICK_DOWN":
								  case "DPAD_SW":
								  case "DPAD_SE":
								  case "LEFT_STICK_SW":
								  case "LEFT_STICK_SE": 
								  case "LEFT_STICK_LEFT": 
								  case "LEFT_STICK_RIGHT": 
								  case "DPAD_LEFT": 
								  case "DPAD_RIGHT": 
								  case "RIGHT_STICK_LEFT":
								  case "RIGHT_STICK_RIGHT": 
								  case "PRESS_B":
								  case "PRESS_BACK":
									  result = INPUT.PressButtonFrom_XTC_Command(s);
									  break;
								  default:
									  Console.WriteLine("Unknown PARAM command: {0}",s);
									  result = false;
									  break;
							  }
							  if (!result)
							  {
								  Console.WriteLine("Failed to press the button. Quitting.");
								  return;
							  }
						  }

						  if ("STATEVAR" == reader.Name)
						  {
							  reader.MoveToAttribute("Name");
							  string s = reader.Value.ToUpper();
							  switch (s)
							  {
									  // deliberate fallthrough here
								  case "SCREENNAME":
								  case "BUTTONFOCUS": 
									  Console.Write("Verifying STATEVAR [{0}] == ",s);
									  reader.MoveToAttribute("Value");
									  Console.WriteLine("\"{0}\"",reader.Value.ToUpper());
									  //check it
									  break;
								  default:
									  Console.WriteLine("Unknown STATEVAR: {0}",s);
									  break;
							  }
							  
						  }

						  break;
					  case XmlNodeType.Text:
						  //OutputXML (reader, "Text");
						  break;
					  case XmlNodeType.Whitespace:
						  break;
				  }
			  }
		  }

			  Console.WriteLine();
			  Console.WriteLine("Processing of stream complete.");
			  Console.WriteLine("Total Tests: {0}",testcount);

			  if (reader != null){reader.Close();}
		  }

   }

   public class CInitRun
	{
	   private bool		m_bInitialized = false;
	   private string	m_DRIVER_TYPE = "";
	   private string	m_SIMULATOR_IP = "";
	   private string	m_SIMULATOR_PORT = "";
	   private string	m_BUTTON_DOWN_DELAY = "";
	   private string	m_BUTTON_UP_DELAY = "";
	   private string	m_XDASH_TRANSITION_DELAY = "";
	   private string	m_INIT_TEST_FILENAME = "";
	   



		public bool ProcessInitRunFile(string initrunfilename)
		{

			XmlTextReader reader = null;

			reader = new XmlTextReader (initrunfilename);
			try
			{
				reader.Read();
				reader.Close();
				reader = new XmlTextReader (initrunfilename);
			}
			catch
			{
				Console.WriteLine("Failure to read file: {0}",initrunfilename);
				return false;
			}

			Console.WriteLine("Processing InitRun file...");
			Console.WriteLine();

			if (null == reader)
			{
				Console.WriteLine("Unable to open file.");
				return false;
			}
			else
			{

				while (reader.Read())
				{
					switch (reader.NodeType)
					{
						case XmlNodeType.ProcessingInstruction:
							//OutputXML (reader, "ProcessingInstruction");
							break;
						case XmlNodeType.DocumentType:
							//OutputXML (reader, "DocumentType");
							break;
						case XmlNodeType.Comment:
							//OutputXML (reader, "Comment");
							break;
						case XmlNodeType.Element:
							
							if ("DRIVER_TYPE" == reader.Name)
							{
								Console.Write("DRIVER_TYPE: ");						
								reader.MoveToAttribute("Value");
								Console.WriteLine(reader.Value);
								m_DRIVER_TYPE = reader.Value;
							}

							if ("SIMULATOR_IP" == reader.Name)
							{
								Console.Write("SIMULATOR_IP: ");						
								reader.MoveToAttribute("Value");
								Console.WriteLine(reader.Value);
								m_SIMULATOR_IP = reader.Value;
							}

							if ("BUTTON_DOWN_DELAY" == reader.Name)
							{
								Console.Write("BUTTON_DOWN_DELAY: ");						
								reader.MoveToAttribute("Value");
								Console.WriteLine(reader.Value);
								m_BUTTON_DOWN_DELAY = reader.Value;
							}

							if ("BUTTON_UP_DELAY" == reader.Name)
							{
								Console.Write("BUTTON_UP_DELAY: ");						
								reader.MoveToAttribute("Value");
								Console.WriteLine(reader.Value);
								m_BUTTON_UP_DELAY = reader.Value;
							}

							if ("XDASH_TRANSITION_DELAY" == reader.Name)
							{
								Console.Write("XDASH_TRANSITION_DELAY");						
								reader.MoveToAttribute("Value");
								Console.WriteLine(reader.Value);
								m_XDASH_TRANSITION_DELAY = reader.Value;
							}

							if ("SIMULATOR_PORT" == reader.Name)
							{
								Console.Write("SIMULATOR_PORT");						
								reader.MoveToAttribute("Value");
								Console.WriteLine(reader.Value);
								m_SIMULATOR_PORT = reader.Value;
							}

							if ("INIT_TEST_FILENAME" == reader.Name)
							{
								Console.Write("INIT_TEST_FILENAME");						
								reader.MoveToAttribute("Value");
								Console.WriteLine(reader.Value);
								m_INIT_TEST_FILENAME = reader.Value;
							}

							break;
						case XmlNodeType.Text:
							//OutputXML (reader, "Text");
							break;
						case XmlNodeType.Whitespace:
							break;
					}
				}
			}

			Console.WriteLine();
			Console.WriteLine("Processing InitRun file complete.");
			m_bInitialized = true;
			if (reader != null){reader.Close();}
			return true;
		}

	   public bool Initialized()
	   {
		   bool retval = m_bInitialized;
		   return retval;
	   }

	   public string INIT_TEST_FILENAME()
	   {
		   string retval = "uninitialized";
		   if (m_bInitialized)
			   retval = m_INIT_TEST_FILENAME;
		   return retval;
	   }

	   public string DRIVER_TYPE()
	   {
		   string retval = "uninitialized";
		   if (m_bInitialized)
			   retval = m_DRIVER_TYPE;
		   return retval;
	   }

	   public string SIMULATOR_IP()
	   {
		   string retval = "uninitialized";
		   if (m_bInitialized)
			   retval = m_SIMULATOR_IP;
		   return retval;
	   }

	   public string BUTTON_DOWN_DELAY()
	   {
		   string retval = "uninitialized";
		   if (m_bInitialized)
			   retval = m_BUTTON_DOWN_DELAY;
		   return retval;
	   }

	   public string BUTTON_UP_DELAY()
	   {
		   string retval = "uninitialized";
		   if (m_bInitialized)
			   retval = m_BUTTON_UP_DELAY;
		   return retval;
	   }

	   public string XDASH_TRANSITION_DELAY()
	   {
		   string retval = "uninitialized";
		   if (m_bInitialized)
			   retval = m_XDASH_TRANSITION_DELAY;
		   return retval;
	   }

	   public string SIMULATOR_PORT()
	   {
		   string retval = "uninitialized";
		   if (m_bInitialized)
			   retval = m_SIMULATOR_PORT;
		   return retval;
	   }

	}
   public class CInitTest
	{
	   
		public bool ProcessInitTestCommands(ref CEngine Engine, ref Simulator INPUT, string MODELNAME)
		{

			XmlTextReader reader = null;

			reader = new XmlTextReader(Engine.INIT_TEST_FILENAME());

			try
			{
				reader.Read();
				reader.Close();
				reader = new XmlTextReader(Engine.INIT_TEST_FILENAME());
			}
			catch
			{
				Console.WriteLine("Failure to read file: {0}",Engine.INIT_TEST_FILENAME());
				return false;
			}

			Console.WriteLine("Processing InitTest stream started...");
			Console.WriteLine();

			bool bModelNamefound = false;

			if (null == reader)
			{
				Console.WriteLine("Unable to open file.");
				return false;
			}
			else
			{
				
				while (reader.Read())
				{
					switch (reader.NodeType)
					{
						case XmlNodeType.ProcessingInstruction:
							//OutputXML (reader, "ProcessingInstruction");
							break;
						case XmlNodeType.DocumentType:
							//OutputXML (reader, "DocumentType");
							break;
						case XmlNodeType.Comment:
							//OutputXML (reader, "Comment");
							break;
						case XmlNodeType.Element:
							if ("ModelName" == reader.Name)
							{
								Console.Write("Found InitTest ModelName: ");
								reader.MoveToAttribute("Value");
								Console.WriteLine(reader.Value);
								if (reader.Value == MODELNAME)
								{
									bModelNamefound = true;
									Console.WriteLine("Executing: {0}",MODELNAME);
									while (reader.Read())
									{
										if (reader.NodeType == XmlNodeType.Element)
										{
											if ("Command" != reader.Name)
											{
												Console.WriteLine("End of Commands for {0}",MODELNAME);
												return true;
											}
											else
											{
												reader.MoveToAttribute("Value");
												Console.WriteLine("Executing Command: {0}",reader.Value);
												if (reader.Value == "INITTEST")
												{	// read ModelName attribute, then execute as a new InitTest
													reader.MoveToAttribute("ModelName");
													if (!ProcessInitTestCommands(ref Engine,ref INPUT,reader.Value))
													{
														return false;
													}
													else
													{
														// successful interpret, keep going...
														// do nothing in this else
													}
												}
												else
												{	// just execute it
													if (!INPUT.PressButtonFrom_XTC_Command(reader.Value))
													{
														Console.WriteLine("Failed InitTest command aborting.");
														return false;
													}
													else
													{
														// success on the command, keep processing
														// and do nothing in this else
														Console.WriteLine("InitTest command succeeded.");
													}
												}
											}
										}
										else
										{
											//Console.WriteLine(".");
										}

									}
								}
							}

							break;
						case XmlNodeType.Text:
							//OutputXML (reader, "Text");
							break;
						case XmlNodeType.Whitespace:
							break;
					}
				}
			}

			Console.WriteLine();
			Console.WriteLine("Processing of InitTest stream complete.");

			if (reader != null){reader.Close();}
			if (!bModelNamefound)
			{
				Console.WriteLine("InitTest for {0} not found!",MODELNAME);
				return false;
			}
			else
			{
				return true; // ModelName was processed
			}
		}

	}

} 
