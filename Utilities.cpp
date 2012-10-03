/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
//--------------------------------
// This file contains "utility" functions.  See Utilities.h for 
// declarations
//--------------------------------
#include "isxSQLite.h"


#pragma region SQLite Related
CppSQLite3DB* OpenDatabase(std::string Name, std::string FileName)
{
	// NOTE:  Calling function should be responsible for determining if the database is already opened and located
	//        in gDatabases.

	CppSQLite3DB *pDatabase = new CppSQLite3DB();
	char FullPathAndFileName[MAX_PATH] = {0};
	bool bFileExists;

	sprintf(FullPathAndFileName,"%s\\%s",ModulePath,FileName.c_str());

	if (FileName == ":Memory:" || FileName == ":memory:")
		bFileExists = false;
	else
		bFileExists = FileExists((char*)FullPathAndFileName);
	
	try 
	{
		pDatabase->open(FullPathAndFileName);
	}
    catch (CppSQLite3Exception& e)
    {
		#pragma region isxSQLite_onErrorMsg
		std::string ErrCode = format("%d",e.errorCode());
		std::string s = format("OpenDB:: Error Opening %s (Error: %s)",FullPathAndFileName,e.errorMessage());
		if (!gQuietMode)
			printf(s.c_str());
				
		char *argv[] = {
			(char*)ErrCode.c_str(),
			(char*)s.c_str()
		};
		pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
		#pragma endregion
		delete pDatabase;
		return nullptr;
    }

	if (!bFileExists)
	{
		try 
		{
			pDatabase->execDML( "PRAGMA encoding = 'UTF-8';"
								"PRAGMA auto_vacuum = 1;"
								"PRAGMA cache_size = 2048;"
								"PRAGMA page_size = 4096;"
								"PRAGMA synchronous = NORMAL;"
								"PRAGMA journal_mode = OFF;"
								"PRAGMA temp_store = MEMORY;"
								"PRAGMA synchronous = OFF;" );
		}
		catch (CppSQLite3Exception& e)
		{
			#pragma region isxSQLite_onErrorMsg
			std::string ErrCode = format("%d",e.errorCode());
			std::string s = format("OpenDatabase:: Error setting defaults on new database file (Error: \"%s\")",e.errorMessage());
			if (!gQuietMode)
				printf(s.c_str());
				
			char *argv[] = {
				(char*)ErrCode.c_str(),
				(char*)s.c_str()
			};
			pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
			#pragma endregion
			return false;
		}
	}
	
	gDatabases.insert(make_pair(Name,pDatabase));
	return pDatabase;
}

void CloseAllDatabases()
{
	for (std::tr1::unordered_map<std::string,CppSQLite3DB*>::iterator It = gDatabases.begin(); It != gDatabases.end(); ++It)
	{
		CppSQLite3DB *pDB = It->second;

		if (pDB && pDB->IsOpen())
			pDB->close();
	}
}

void FinalizeAllQueries()
{
	for (std::map<int,CppSQLite3Query*>::iterator It = gQueries.begin(); It != gQueries.end(); ++It)
	{
		CppSQLite3Query *pQuery = It->second;

		if (pQuery)
			pQuery->finalize();
	}
}

void FinalizeAllTables()
{
	for (std::map<int,CppSQLite3Table*>::iterator It = gTables.begin(); It != gTables.end(); ++It)
	{
		CppSQLite3Table *pTable = It->second;

		if (pTable)
			pTable->finalize();
	}
}
#pragma endregion

#pragma region isxSQLite related
void ProcessMainXMLSettings()
{
	unsigned int GeneralSetID = pISInterface->FindSet(MainXMLFileID,"General");

	////////
	// Process Main Section
	pISInterface->GetSetting(MainXMLFileID,"Use Test Version",gUseTestVersion);
	////////

	pISInterface->ExportSet(MainXMLFileID,XMLFileName);
	return;
}
#pragma endregion

#pragma region Misc. Utility Functions
// Honestly, I would prefer to use boost for formatting with std::string; however, the goal is for isxSQLite to compile
// "out of the box" without any additional packages.   For now anyway....
std::string format_arg_list(const char *fmt, va_list args)
{
	if (!fmt) return "";
	int   result = -1, length = 256;
	char *buffer = 0;
	while (result == -1)
	{
		if (buffer) delete [] buffer;
		buffer = new char [length + 1];
		memset(buffer, 0, length + 1);
		result = _vsnprintf_s(buffer, length, _TRUNCATE, fmt, args); 
		length *= 2;
	}
	std::string s(buffer);
	delete [] buffer;
	return s;
}

std::string format(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	std::string s = format_arg_list(fmt, args);
	va_end(args);
	return s;
}
#pragma endregion