/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#ifndef TOPLEVELOBJECT
#define TOPLEVELOBJECT_SELF
#define TOPLEVELOBJECT(name,funcname) extern bool funcname(int argc, char *argv[], LSTYPEVAR &Ret);
#endif
/////////
// Top Level Objects used in isxSQLite are initialized below.
/////////


// These Top Level Objects are registered after all other initialization routines have completed.  
// (Note:  Most TLOs fall into this category.)
TOPLEVELOBJECT("SQLite",TLO_SQLite);


// ----------------------------------------------------
#ifdef TOPLEVELOBJECT_SELF
#undef TOPLEVELOBJECT_SELF
#undef TOPLEVELOBJECT
#endif