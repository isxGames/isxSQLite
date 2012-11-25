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

namespace LavishScript2
{
	class ILS2Array;
	class LS2Exception;
};

// SQLite Related
extern CppSQLite3DB* OpenDatabase(std::string Name, std::string FileName, LavishScript2::LS2Exception **ppException=0);
extern int OpenTable(CppSQLite3DB *pDB, const char *name, LavishScript2::LS2Exception **ppException=0);
extern int ExecQuery(CppSQLite3DB *pDB, const char *sql, LavishScript2::LS2Exception **ppException___no_exception_for_empty_result_set=0);
extern bool CloseDatabase(const char *name, CppSQLite3DB* pDB, LavishScript2::LS2Exception **ppException=0);
extern bool ExecDML(CppSQLite3DB *pDB, const char *dml, LavishScript2::LS2Exception **ppException=0);
extern bool ExecDMLTransaction(CppSQLite3DB *pDB, LavishScript2::ILS2Array *pArray, LavishScript2::LS2Exception **ppException=0);

extern void CloseAllDatabases();
extern void FinalizeAllQueries();
extern void FinalizeAllTables();

// isxSQLite Related
extern void ProcessMainXMLSettings();

// Misc. Utility Functions
extern std::string format_arg_list(const char *fmt, va_list args);
extern std::string format(const char *fmt, ...);