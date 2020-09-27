/*
  
  For PowerPC native Shared Libraries and Applications, make a cfrg resource.
  For Applications be sure to use kIsApp and not kIsLib.
  For Shared Libraries be sure to use kIsLib not kIsApp.
  
  For application plug ins, see the conventions established by the application vendor.
  
  Making a shared library:
  Rez -i : Types.r CodeFragmentTypes.r LibIcon.r {Active} -a -o MathLib -t shlb -c cfmg
  SetFile MathLib -a iB

  Making an application:
  Rez Types.r CodeFragmentTypes.r {Active} -a -o Application -t APPL

  This example is customized for building an application "MyApp"

  Change all occurences of  MyApp  to  YourApp   or YourLib
  NOTE: ID must be zero.  
        (Sysheap & Locked are no longer required, and not recommended.)

*/

#include "Types.r"
#include "CodeFrag.r"

resource 'cfrg' (0) {
  {
    kPowerPC,
    kFullLib,
    kNoVersionNum,kNoVersionNum,
    0, 0,
    kIsApp,kOnDiskFlat,kZeroOffset,kWholeFork,
    "testdll.dll"
  }
};

resource 'SIZE' (-1) {
  reserved,
  ignoreSuspendResumeEvents,	/* suspend-resume    */
  reserved,
  canBackground,		/* Can properly use back-ground null events  */
  needsActivateOnFGSwitch,	/* activate/deactivate  */
  backgroundAndForeground,	/* Application does not	have a user interface*/

  dontGetFrontClicks,		/* Get mouse down/up  */
  ignoreAppDiedEvents,		/* Apps use this.    */
  not32BitCompatible,		/* Works with 24bit addr*/
				/* bit addresses    */
				/* next four bits are new for system 7.0 */
  notHighLevelEventAware,
  onlyLocalHLEvents,		/* paranoid users' flag */
  notStationeryAware,		/* checks stationery bit */
  dontUseTextEditServices,	/* can use text services */

  reserved,			/* These 3 bits are   */
  reserved,			/* reserved.  Set them  */
  reserved,			/* to "reserved". When  */
				/* we decide to define  */
				/* a new flag, your    */
				/* old resource will   */
				/* still compile.    */

				/* Memory sizes are in bytes */
  1200 * 1024,			/* preferred mem size  */
  1200 * 1024;			/* minimum mem size    */
};


