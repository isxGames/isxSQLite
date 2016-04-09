/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
//--------------------------------
// All isxSQLite globals are defined in this file.
//--------------------------------
#include "isxSQLite.h"

// Events
unsigned int isxSQLite_onUpdateComplete = 0;
unsigned int isxSQLite_onDoShutdown = 0;
unsigned int onFrameEnds = 0;
unsigned int isxSQLite_onErrorMsg = 0;
unsigned int isxSQLite_onStatusMsg = 0;

// XML Files and Settings
unsigned int MainXMLFileID = 0;	

// Arrays
std::tr1::unordered_map<std::string,CppSQLite3DB*> gDatabases;
int gQueriesCounter = 0;
std::map<int,CppSQLite3Query*> gQueries;
int gTablesCounter = 0;
std::map<int,CppSQLite3Table*> gTables;

// isxSQLite Specific Globals
bool gDetoursActive = false;
unsigned int gWaitPulses = 0;
bool gQuietMode = false;

// .NET
unsigned int isxSQLiteClass = 0;

// Datatypes
#define DATATYPE(_class_,_variable_,_inherits_) class _class_ *_variable_=0
#include "DataTypeList.h"
#undef DATATYPE
