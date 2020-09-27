#ifndef _XDASHTESTSTEPS_H_
#define _XDASHTESTSTEPS_H_

const WCHAR STEP_ACTION_WAIT_FOR_DST[]                         = L"Wait until the DST rollover time has passed";

const WCHAR STEP_NOTE_CONFIG_SETTINGS[]                        = L"Note the various configuration settings: Clock, Language, Video, Audio, and Parental Control";

const WCHAR STEP_INITIATE_ACTION_ACCORD_TO_VAR[]               = L"Initiate the action according to the test variation";

const WCHAR STEP_INSERT_DVD_GAME_MEM_CLEANUP[]                 = L"Insert a disk that will allow you to boot to the XDash Memory Management Clean-Up Mode";
const WCHAR STEP_INSERT_HARDWARE_ACCORD_TO_VAR[]               = L"Insert the hardware according to the test variation";
const WCHAR STEP_INSERT_MEM_UNIT_EMPTY[]                       = L"Insert a Memory Unit that is EMPTY";
const WCHAR STEP_INSERT_MEM_UNIT_FULL[]                        = L"Insert a Memory Unit that is FULL";
const WCHAR STEP_INSERT_MEM_UNIT_1_SAVED_GAME_1_BLOCK[]        = L"Insert a Memory Unit that contains 1 saved game that is 1 block in size";
const WCHAR STEP_INSERT_PLUG_IN_TO_XBOX[]                      = L"Insert the plug in to the XBox console";

const WCHAR STEP_INVOKE_SCREEN_ACCORD_TO_VAR[]                 = L"Invoke the screen according to the test variation";

const WCHAR STEP_REMOVE_AVPACK_FROM_XBOX[]                     = L"Remove the A/V pack from the XBox console";
const WCHAR STEP_REMOVE_BB_PLUG_FROM_XBOX[]                    = L"Remove the broadband plug from the XBox console";
const WCHAR STEP_REMOVE_HARDWARE_ACCORD_TO_VAR[]               = L"Remove the hardware according to the test variation";
const WCHAR STEP_REMOVE_POWER_PLUG_FROM_XBOX[]                 = L"Remove the power plug from the XBox console";

const WCHAR STEP_SKIP_VARS_BASED_ON_DEFAULT_DUKE[]             = L"Skip variations based on the Default Duke/Port";

const WCHAR STEP_PERFORM_FILE_COMPARE_ACCORD_TO_VAR[]          = L"Perform a file compare to ensure the action was a successful according to the test variation";

const WCHAR STEP_PRESS_POWER[]                                 = L"Press Power";
const WCHAR STEP_PRESS_ACCEPT[]                                = L"Press 'A'";
const WCHAR STEP_PRESS_BACK[]	                               = L"Press 'B'";

const WCHAR STEP_TITLE_SHOULD_REQUEST_MAX_MINUS_2_BLOCKS[]     = L"The Title should request MAX - 2 blocks of space needed";
const WCHAR STEP_TITLE_SHOULD_REQUEST_MAXBLOCKS[]              = L"The Title should request MAX blocks of space needed";

const WCHAR STEP_SELECT_AUDIO[]                                = L"Select the 'Audio' menu option";
const WCHAR STEP_SELECT_AUDIO_OPTION_ACCORD_TO_VAR[]           = L"Select the audio option according to the test variation";
const WCHAR STEP_SELECT_AUDIO_OPTION_NEW[]                     = L"Select an audio option that is different from the current option";
const WCHAR STEP_SELECT_AUDIO_OPTION_TO_BE[]                   = L"Select the audio option to be:";
const WCHAR STEP_SELECT_CANCEL[]                               = L"Select the 'Cancel' menu option";
const WCHAR STEP_SELECT_CLOCK[]                                = L"Select the 'Clock' menu option";
const WCHAR STEP_SELECT_CLOCK_EACH_MONTH[]                     = L"Select each month";
const WCHAR STEP_SELECT_CLOCK_INFO_ACCORD_TO_VAR[]             = L"Select the 'Clock' date and time according to the test variation";
const WCHAR STEP_SELECT_CLOCK_LEAP_YEAR[]                      = L"Select the 'Year' to be 2004, or 2008, a leap year";
const WCHAR STEP_SELECT_CLOCK_MONTH_FEB[]                      = L"Select the 'Month' to be 'February'";
const WCHAR STEP_SELECT_CLOCK_NON_LEAP_YEAR[]                  = L"Select the 'Year' to be 2001, a non leap year";
const WCHAR STEP_SELECT_CLOCK_NEW[]                            = L"Select a different date or time";
const WCHAR STEP_SELECT_CONROL_ACCORD_TO_VAR[]                 = L"Select the control according to the test variation";
const WCHAR STEP_SELECT_COPY[]                                 = L"Select the 'Copy' menu option";
const WCHAR STEP_SELECT_COUNTRY_CURRENT[]                      = L"Select the 'Country' information for your current country";
const WCHAR STEP_SELECT_COUNTRY_WITH_DST[]                     = L"Select the 'Country' information for a country that has daylight savings time";
const WCHAR STEP_SELECT_DELETE[]                               = L"Select the 'Delete' menu option";
const WCHAR STEP_SELECT_DEST_MEM_AREA_ACCORD_TO_VAR[]          = L"Select the Memory Area to be copied TO according to the test variation";
const WCHAR STEP_SELECT_DEVICE_ACCORD_TO_VAR[]                 = L"Select the device according to the test variation";
const WCHAR STEP_SELECT_LANGUAGE[]                             = L"Select the 'Language' menu option";
const WCHAR STEP_SELECT_LANGUAGE_ACCORD_TO_VAR[]               = L"Select the language according to the test variation";
const WCHAR STEP_SELECT_LANGUAGE_NEW[]                         = L"Select a language that is different from the current language";
const WCHAR STEP_SELECT_MEM_AREA_BLOCK_CONFIG_ACCORD_TO_VAR[]  = L"Select the Memory Area block configuration according to the test variation";
const WCHAR STEP_SELECT_MEM_AREA_DEFAULT_DUKE[]                = L"Select the Memory Area in the 'default Duke'";
const WCHAR STEP_SELECT_MEM_AREA_EACH_MU[]                     = L"Select each Memory Unit Memory Area";
const WCHAR STEP_SELECT_MEM_AREA_EMPTY[]                       = L"Select the Memory Area that is empty";
const WCHAR STEP_SELECT_MEMORY_MANAGEMENT[]                    = L"Select Memory Management";
const WCHAR STEP_SELECT_SAVED_GAME[]                           = L"Select a saved game";
const WCHAR STEP_SELECT_SAVED_GAME_ACCORD_TO_VAR[]             = L"Select the saved game(s) according to the test variation";
const WCHAR STEP_SELECT_SAVED_GAME_ALL[]                       = L"Select ALL saved games";
const WCHAR STEP_SELECT_SAVED_GAMES_MARKED_BAD[]               = L"Select any saved games that are marked as 'invalid', or 'bad'";
const WCHAR STEP_SELECT_SETTINGS[]                             = L"Select Settings";
const WCHAR STEP_SELECT_SOURCE_MEM_AREA_ACCORD_TO_VAR[]        = L"Select the Memory Area to be copied FROM according to the test variation";
const WCHAR STEP_SELECT_SOURCE_MEM_AREA_XDISK[]                = L"Select the XDisk as the 'SOURCE' Memory Area";
const WCHAR STEP_SELECT_TITLE_ACCORD_TO_VAR[]                  = L"Select the Title (game) according to the test variation";
const WCHAR STEP_SELECT_VIDEO[]                                = L"Select the 'Video' menu option";
const WCHAR STEP_SELECT_VIDEO_ACCORD_TO_VAR[]                  = L"Select the video option according to the test variation";
const WCHAR STEP_SELECT_VIDEO_NEW[]                            = L"Select a video option that is different from the current option";
const WCHAR STEP_SELECT_XZONE[]                                = L"Select XZone";
const WCHAR STEP_SELECT_XZONE_SET_CLOCK[]                      = L"Select the 'Set Clock' option";

const WCHAR STEP_SETUP_XBOX_AVPACK_ACCORD_TO_VAR[]             = L"Setup the XBox to have the A/V pack installed according to the test variation";
const WCHAR STEP_SETUP_XBOX_AVPACK_ANY[]                       = L"Setup the XBox to have an A/V pack installed";
const WCHAR STEP_SETUP_XBOX_AVPACK_NONE[]                      = L"Setup the XBox to have NO A/V packs installed";
const WCHAR STEP_SETUP_XBOX_CLOCK_INCORRECT[]                  = L"Setup the XBox clock to be incorrect according to your current time zone / country";
const WCHAR STEP_SETUP_XBOX_DATE_JUST_BEFORE_DST[]             = L"Setup the XBox clock to be just before DST rollover - 'Month, Day, Year XX:XX:XX'";
const WCHAR STEP_SETUP_XBOX_FACTORY_SETTINGS_GOOD_BATTERY[]    = L"Setup the XBox with Factory Default Settings with a GOOD 20 day battery";
const WCHAR STEP_SETUP_XBOX_FACTORY_SETTINGS_BAD_BATTERY[]     = L"Setup the XBox with Factory Default Settings with an EXPIRED 20 day battery";
const WCHAR STEP_SETUP_XBOX_FROM_REGION_ACCORD_TO_VAR[]        = L"Setup the XBox to be from the region according to the test variation";
const WCHAR STEP_SETUP_XBOX_PORTS_DEVICE_ACCORD_TO_VAR[]       = L"Setup the XBox ports, and devices according to the test variation";
const WCHAR STEP_SETUP_XBOX_PORTS_DUKE_E_E[]                   = L"Setup the available XBox ports with a Duke (E, E)";
const WCHAR STEP_SETUP_XBOX_PORTS_DUKE_M_M[]                   = L"Setup the available XBox ports with a Duke (M, M)";
const WCHAR STEP_SETUP_XDISK_SOME_DATA[]                       = L"Setup the XDisk Memory Area in this test variation with at least 1 saved game";
const WCHAR STEP_SETUP_ALL_MEM_UNITS_FULL[]                    = L"Setup all the memory units used in this test case to be FULL (no space free)";
const WCHAR STEP_SETUP_ALL_MEM_AREAS_EMPTY[]                   = L"Setup all the memory areas used in this test case to be EMPTY (no space used)";
const WCHAR STEP_SETUP_ALL_MEM_AREAS_FULL[]                    = L"Setup all the memory areas used in this test case to be FULL (no space free)";
const WCHAR STEP_SETUP_ALL_MEM_AREAS_SAME_SAVED_GAME[]         = L"Setup all the memory areas with the EXACT SAME saved game.";
const WCHAR STEP_SETUP_MEM_AREA_DEFAULT_DUKE_SOME_DATA[]       = L"Setup the Memory Area in the 'default Duke' with at least 1 saved game";
const WCHAR STEP_SETUP_MEM_AREA_DEFAULT_DUKE_ACCORD_TO_VAR[]   = L"Setup the Memory Area in the 'default Duke' according to the test variation";
const WCHAR STEP_SETUP_MEM_AREA_DEFAULT_DUKE_TWO_SAVED_GAMES[] = L"Setup the Memory Area in the 'default Duke' with at least 2 saved games";
const WCHAR STEP_SETUP_MEM_AREA_IN_VAR_TWO_SAVED_GAMES[]       = L"Setup the Memory Area in the test variation with at least 2 saved games";
const WCHAR STEP_SETUP_MEM_AREA_IN_VAR_MAX_SAVED_GAMES[]       = L"Setup the Memory Area in the test variation with the MAX number of saved games";
const WCHAR STEP_SETUP_MEM_AREA_MAX_SAVED_GAMES_1_BLOCK_EACH[] = L"Setup the Memory Area in the test variation with the MAX number of saved games (1 block each)";
const WCHAR STEP_SETUP_MEM_AREA_EMPTY[]                        = L"Setup the Memory Area in this test variation to be empty (no saved games, etc)";
const WCHAR STEP_SETUP_MEM_AREA_SOME_DATA[]                    = L"Setup the Memory Area in this test variation with at least 1 saved game";
const WCHAR STEP_SETUP_MEM_AREA_ONE_SAVED_GAME[]               = L"Setup the Memory Area in this test variation with ONLY 1 saved game";
const WCHAR STEP_SETUP_MEM_AREA_TWO_SAVED_GAMES[]              = L"Setup the Memory Area in this test variation with ONLY 2 saved games";
const WCHAR STEP_SETUP_MEM_AREA_ACCORD_TO_VAR[]                = L"Setup the Memory Area in this test according to the test variation";
const WCHAR STEP_SETUP_MEM_AREA_TITLES_ACCORD_TO_VAR[]         = L"Setup the Memory Area, both with TITLES according to the test variation";
const WCHAR STEP_SETUP_MEM_AREA_GAMES_ACCORD_TO_VAR[]          = L"Setup the Memory Area, both with GAMES according to the test variation";
const WCHAR STEP_SETUP_SOURCE_MEM_AREA_ACCORD_TO_VAR[]         = L"Setup the 'SOURCE' Memory Area in this test according to the test variation";
const WCHAR STEP_SETUP_DEST_MEM_AREA_ENOUGH_SPACE[]            = L"Setup the 'DESTINATION' Memory Area in this test to be empty, or to contain enough space for the test variation";
const WCHAR STEP_SETUP_DEST_MEM_AREA_NOT_ENOUGH_SPACE[]        = L"Setup the 'DESTINATION' Memory Area in this test to be full, or to NOT contain enough space for the test variation";

const WCHAR STEP_VERIFY_DEST_MEMORY_DEVICE_SCREEN[]            = L"Verify you are at the 'DESTINATION' Memory Area screen";
const WCHAR STEP_VERIFY_GAME_DELETE_DIALOG[]                   = L"Verify that the delete SAVED GAME verification dialog appears";
const WCHAR STEP_VERIFY_GAME_OPTIONS_DIALOG[]                  = L"Verify that the game options dialog appears";
const WCHAR STEP_VERIFY_MEMORY_MANAGEMENT_SCREEN[]             = L"Verify you are at the Memory Management screen";
const WCHAR STEP_VERIFY_MEMORY_DEVICE_SCREEN[]                 = L"Verify you are at the Memory Area screen";
const WCHAR STEP_VERIFY_ROOT_MENU_SCREEN[]                     = L"Verify you are at the Default Title Root Menu";
const WCHAR STEP_VERIFY_SAVED_GAME_COPY_SCREEN[]               = L"Verify you are at the Saved Game Copy screen";
const WCHAR STEP_VERIFY_SETTINGS_AUDIO_SCREEN[]                = L"Verify you are at the Settings 'Audio' screen";
const WCHAR STEP_VERIFY_SETTINGS_CLOCK_SCREEN[]                = L"Verify you are at the Settings 'Clock' screen";
const WCHAR STEP_VERIFY_SETTINGS_LANGUAGE_SCREEN[]             = L"Verify you are at the Settings 'Language' screen";
const WCHAR STEP_VERIFY_SETTINGS_SCREEN[]                      = L"Verify you are at the Settings screen";
const WCHAR STEP_VERIFY_SETTINGS_VIDEO_SCREEN[]                = L"Verify you are at the Settings 'Video' screen";
const WCHAR STEP_VERIFY_SOURCE_MEMORY_DEVICE_SCREEN[]          = L"Verify you are at the 'SOURCE' Memory Area screen";
const WCHAR STEP_VERIFY_TITLE_DELETE_DIALOG[]                  = L"Verify that the delete TITLE verification dialog appears";
const WCHAR STEP_VERIFY_XZONE_SCREEN[]                         = L"Verify you are at the XZone screen";

const WCHAR STEP_VERIFY_ACTION_COMPLETED[]                     = L"Verify the action being performed completed successfully";
const WCHAR STEP_VERIFY_ACTION_NOT_ALLOWED[]                   = L"Verify that you are not allowed to perform that action";
const WCHAR STEP_VERIFY_ACTION_NOTIFICATION[]                  = L"Verify that the user is notified that the action completed, or is finished";
const WCHAR STEP_VERIFY_ACTION_STARTED[]                       = L"Verify that the action has begun.";
const WCHAR STEP_VERIFY_ALL_MEM_AREAS_PROPER_FILES[]           = L"Verify that all Memory Areas have the files expected";
const WCHAR STEP_VERIFY_AUDIO_CHOICES_AVAILABLE_ARE[]          = L"Verify the audio choices that are available are:";
const WCHAR STEP_VERIFY_AUDIO_SET_ACCORD_TO_VAR[]              = L"Verify the audio display has been set according to the test variation";
const WCHAR STEP_VERIFY_AUDIO_SET_CORRECTLY[]                  = L"Verify the audio display has been set correctly";
const WCHAR STEP_VERIFY_AUDIO_SET_TO[]                         = L"Verify that the audio is set to:";
const WCHAR STEP_VERIFY_CLOCK_CANT_SET_AFTER[]                 = L"Verify that you CANNOT set the clock to a date after:";
const WCHAR STEP_VERIFY_CLOCK_CANT_SET_PRIOR_TO[]              = L"Verify that you CANNOT set the clock to a date prior to:";
const WCHAR STEP_VERIFY_CLOCK_DST_RESET[]                      = L"Verify that the clock DST information has been reset (disabled)";
const WCHAR STEP_VERIFY_CLOCK_INFO_ACCORD_TO_VAR[]             = L"Verify that the clock is set according to the test variation";
const WCHAR STEP_VERIFY_CLOCK_INITIAL_BOOT_GOOD_BATTERY[]      = L"Verify that the clock is set to 'Month, Day, Year', 'XX:XX:XX'";
const WCHAR STEP_VERIFY_CLOCK_INITIAL_BOOT_BAD_BATTERY[]       = L"Verify that the clock is set to 'Month, Day, Year', 'XX:XX:XX'";
const WCHAR STEP_VERIFY_CLOCK_NOT_UPDATED[]                    = L"Verify that the clock has NOT been updated";
const WCHAR STEP_VERIFY_CLOCK_NUM_DAYS_FOR_CURRENT_MONTH[]     = L"Verify that you can only select a valid day for the current month";
const WCHAR STEP_VERIFY_CLOCK_SET_CORRECTLY[]                  = L"Verify that the clock has been set correctly";
const WCHAR STEP_VERIFY_CLOCK_SET_FOR_DST_CORRECTLY[]          = L"Verify that the clock has been set for daylight savings time correctly";
const WCHAR STEP_VERIFY_CLOCK_UPDATED[]                        = L"Verify that the clock has been updated";
const WCHAR STEP_VERIFY_CORRECT_FILES_EXIST[]                  = L"Verify the correct files exist";
const WCHAR STEP_VERIFY_CORRECT_LANGUAGE[]                     = L"Verify the correct language is displayed";
const WCHAR STEP_VERIFY_CORRECT_LANGUAGE_ACCORD_TO_VAR[]       = L"Verify the correct language is displayed according to the test variation";
const WCHAR STEP_VERIFY_CORRECT_SETTINGS_ACCORD_TO_NOTE[]      = L"Verify the settings are correct as noted";
const WCHAR STEP_VERIFY_CORRECT_SIZE_ACCORD_TO_VAR[]           = L"Verify the correct size is displayed according to the test variation";
const WCHAR STEP_VERIFY_CORRECT_TITLES_EXIST[]                 = L"Verify the correct Titles (games) exist";
const WCHAR STEP_VERIFY_CORRECT_TITLES_DELETED[]               = L"Verify the correct Titles (games) have been deleted";
const WCHAR STEP_VERIFY_DEST_MEM_AREA_FILES_OK[]               = L"Verify that the files on the Memory Area are either NOT there, or MARKED bad";
const WCHAR STEP_VERIFY_DEST_MEM_AREA_MARKED_INVALID[]         = L"Verify that the 'DESTINATION' memory area is marked that it does not contain enough space";
const WCHAR STEP_VERIFY_DEST_SCREEN_ACCORD_TO_VAR[]            = L"Verify you are on the correct destination according to the test variation";
const WCHAR STEP_VERIFY_DISPLAY_PROPERLY_WRAPPING[]            = L"Verify the display is wrapping in the proper locations, and looks good";
const WCHAR STEP_VERIFY_MD_TEXT_REFLECTS_VARIATION[]           = L"Verify any references to the Memory Device (area) reflects the test variation";
const WCHAR STEP_VERIFY_MD_SPACE_REFLECTS_VARIATION[]          = L"Verify any references to space available on the Memory Device (area) reflects the test variation";
const WCHAR STEP_VERIFY_MEMORY_MANAGEMENT_CLEANUP[]            = L"Verify that the XDash is in Memory Managment clean-up mode";
const WCHAR STEP_VERIFY_MEMORY_MANAGEMENT_NOT_CLEANUP[]        = L"Verify that the XDash is NOT in Memory Managment clean-up mode";
const WCHAR STEP_VERIFY_SAVED_GAMES_SORTED_ALPHABETICALLY[]    = L"Verify that the saved games for each title are sorted alphabetically";
const WCHAR STEP_VERIFY_SCREEN_UPDATED[]                       = L"Verify the screen is updated as expected";
const WCHAR STEP_VERIFY_SCREEN_ACCORD_TO_VAR[]                 = L"Verify you are on the correct screen according to the test variation";
const WCHAR STEP_VERIFY_SELECTION[]                            = L"Verify the items you selected previously are still selected";
const WCHAR STEP_VERIFY_TEXT_NOT_CLIPPED[]                     = L"Verify that not text is being clipped (unless it's supposed to be)";
const WCHAR STEP_VERIFY_TITLE_FROM_PARENT[]                    = L"Verify that the title that is displayed is from the saved game's parent";
const WCHAR STEP_VERIFY_TITLE_CORRECT_NUM_GAMES_SELECTED[]     = L"Verify that the title reflects the correct number of saved games that were selected";
const WCHAR STEP_VERIFY_TITLES_SORTED_ALPHABETICALLY[]         = L"Verify that the game titles are sorted alphabetically";
const WCHAR STEP_VERIFY_VIDEO_SET_ACCORD_TO_VAR[]              = L"Verify the video display has been set according to the test variation";
const WCHAR STEP_VERIFY_VIDEO_SET_CORRECTLY[]                  = L"Verify the video display has been set correctly";
const WCHAR STEP_VERIFY_VIDEO_SET_TO[]                         = L"Verify that the video is set to:";
const WCHAR STEP_VERIFY_XBOX_POWERED_OFF[]                     = L"Verify the XBox has been powered off";

#endif // _XDASHTESTSTEPS_H_