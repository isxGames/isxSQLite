/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
//--------------------------------
// All isxSQLite global variables are declared in this file.
//--------------------------------
#pragma once

// Events
extern unsigned int isxSQLite_onUpdateComplete;
extern unsigned int isxSQLite_onDoShutdown;
extern unsigned int onFrameEnds;
extern unsigned int isxSQLite_onErrorMsg;
extern unsigned int isxSQLite_onStatusMsg;

// XML Files
extern char XMLFileName[MAX_PATH];
extern unsigned int MainXMLFileID;

// Arrays
extern std::tr1::unordered_map<std::string,CppSQLite3DB*> gDatabases;
extern int gQueriesCounter;
extern std::map<int,CppSQLite3Query*> gQueries;
extern int gTablesCounter;
extern std::map<int,CppSQLite3Table*> gTables;

// isxSQLite Specific Globals
extern bool gDetoursActive;
extern unsigned int gWaitPulses;
extern bool gQuietMode;

// Internal isxSQLite Functions
extern void __cdecl isxSQLitePostInitialize(int argc, char *argv[], PLSOBJECT);
extern void __cdecl isxSQLiteSpawnApplication(int argc, char *argv[], PLSOBJECT);
extern void __cdecl isxSQLiteonDoShutdown(int argc, char *argv[], PLSOBJECT);

// .NET
extern unsigned int isxSQLiteClass;