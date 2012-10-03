/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
//--------------------------------
// Top Level Objects (which are available once the extension is fully loaded) 
// are defined in this file.  See TopLevelObjects.h for declarations.
//--------------------------------
#include "isxSQLite.h"


bool __cdecl TLO_SQLite(int argc, char *argv[], LSTYPEVAR &Dest)
{
	#if TESTCALLS
	#if TESTCALLS_TOFILE
	DebugSpew(TESTCALLS_TOFILE "%d\t%s(%s)",pISInterface->GetRuntime(),__FUNCTION__,GetArgvStringForTestingCalls(argc,argv).c_str());
	#else
	DebugSpew("%d\t%s(%s)",pISInterface->GetRuntime(),__FUNCTION__,GetArgvStringForTestingCalls(argc,argv).c_str());
	#endif
	#endif

	Dest.DWord=1;
	Dest.Type=pSQLiteType;
	return true;
}