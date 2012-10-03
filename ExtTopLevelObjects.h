/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
/////////
// Top Level Objects used in isxSQLite are initialized below.
/////////
#ifndef EXTTOPLEVELOBJECT
#define EXTTOPLEVELOBJECT_SELF
#define EXTTOPLEVELOBJECT(name,funcname) extern bool funcname(int argc, char *argv[], LSTYPEVAR &Ret);
#endif

// These Top Level Objects are registered immediately upon loading the extension.
// (Note:  Most Top Level Objects should not be initialized here.  See TopLevelObjects.h insetad.)
EXTTOPLEVELOBJECT("ISXSQLite",TLO_isxSQLite);

// ----------------------------------------------------
#ifdef EXTTOPLEVELOBJECT_SELF
#undef EXTTOPLEVELOBJECT_SELF
#undef EXTTOPLEVELOBJECT
#endif