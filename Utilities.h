/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
//--------------------------------
// All utility functions are declared here.
//--------------------------------
#pragma once

// SQLite Related
extern CppSQLite3DB* OpenDatabase(std::string Name, std::string FileName);
extern void CloseAllDatabases();
extern void FinalizeAllQueries();
extern void FinalizeAllTables();

// isxSQLite Related
extern void ProcessMainXMLSettings();

// Misc. Utility Functions
extern std::string format_arg_list(const char *fmt, va_list args);
extern std::string format(const char *fmt, ...);