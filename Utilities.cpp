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
#include <fstream>
#include "isxSQLite.h"
#ifdef USE_LAVISHSCRIPT2
#include <ls2module.h>
#endif

#pragma region SQLite Related
CppSQLite3DB* OpenDatabase(std::string Name, std::string FileName, LavishScript2::LS2Exception **ppException)
{
	// NOTE:  Calling function should be responsible for determining if the database is already opened and located
	//        in gDatabases.

	CppSQLite3DB *pDatabase = new CppSQLite3DB();
	char FullPathAndFileName[MAX_PATH] = {0};
	bool bFileExists;

	// Note: FileName is UTF-8, while ModulePath is ANSI
	// And this should technically be using LavishScript->SetRoot, to adhere to LavishScript the filesystem... but... okay.
	if (!IsAbsolutePath(FileName.c_str()))
		_snprintf_s(FullPathAndFileName,MAX_PATH,"%s\\%s",ModulePath,FileName.c_str());
	else
		strncpy(FullPathAndFileName,FileName.c_str(),MAX_PATH);
	FullPathAndFileName[MAX_PATH-1]=0;

	if (FileName == ":Memory:" || FileName == ":memory:")
		bFileExists = false;
	else
	{
		std::ifstream infile(FullPathAndFileName);
		bFileExists = infile.good();
	}
	
	try 
	{
		pDatabase->open(FullPathAndFileName);
	}
	catch (CppSQLite3Exception& e)
	{
		#pragma region isxSQLite_onErrorMsg
		std::string ErrCode = format("%d",e.errorCode());
		std::string s = format("OpenDB:: Error Opening %s (Error: %s)",FullPathAndFileName,e.errorMessage());

		#ifdef USE_LAVISHSCRIPT2
		if (ppException)
		{
			// need to convert to UTF-16 for LS2 exception
			LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
			LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
			LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
			*ppException = new LavishScript2::LS2Exception(pString->c_str());
		}
		#endif

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

			#ifdef USE_LAVISHSCRIPT2
			if (ppException)
			{
				// need to convert to UTF-16 for LS2 exception
				LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
				LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
				LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
				*ppException = new LavishScript2::LS2StringException(pString->c_str());
			}
			#endif

			if (!gQuietMode)
				printf(s.c_str());
				
			char *argv[] = {
				(char*)ErrCode.c_str(),
				(char*)s.c_str()
			};
			pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
			#pragma endregion
			return nullptr;
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

int OpenTable(CppSQLite3DB *pDB, const char *name, LavishScript2::LS2Exception **ppException)
{
	CppSQLite3Buffer bufSQL;
	CppSQLite3Table *pTable = new CppSQLite3Table();
	try 
	{
		if (pDB->tableExists(name))
		{
			bufSQL.format("select * from %s order by 1;", name);
			*pTable = pDB->getTable(bufSQL);
		}
		else
			*pTable = pDB->getTable(name);
	}
	catch (CppSQLite3Exception& e)
	{
		#pragma region isxSQLite_onErrorMsg
		std::string ErrCode = format("%d",e.errorCode());
		std::string s;
		if (pDB->tableExists(name))
			s = format("SQLiteDBType.GetTable:: Error getting table '%s'. (Error: %d:%s)",name, e.errorCode(),e.errorMessage());
		else
			s = format("SQLiteDBType.GetTable:: Error getting table using custom DML Statment '%s'.  (Error: %d:%s)",name, e.errorCode(),e.errorMessage());

		#ifdef USE_LAVISHSCRIPT2
		if (ppException)
		{
			// need to convert to UTF-16 for LS2 exception
			LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
			LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
			LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
			*ppException = new LavishScript2::LS2StringException(pString->c_str());
		}
		#endif

		if (!gQuietMode)
			printf(s.c_str());
				
		char *argv[] = {
			(char*)ErrCode.c_str(),
			(char*)s.c_str()
		};
		pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
		#pragma endregion
		pTable->finalize();
		delete pTable;
		return false;
	}

	gTablesCounter++;
	gTables.insert(make_pair(gTablesCounter,pTable));
	return gTablesCounter;
}

int ExecQuery(CppSQLite3DB *pDB, const char *sql, LavishScript2::LS2Exception **ppException)
{
	CppSQLite3Query *q = new CppSQLite3Query();
	try 
	{
		*q = pDB->execQuery(sql);
	}
	catch (CppSQLite3Exception& e)
	{
		#pragma region isxSQLite_onErrorMsg
		std::string ErrCode = format("%d",e.errorCode());
		std::string s = format("SQLiteDBType.ExecQuery:: Error executing DML. (Error: %d:%s)",e.errorCode(),e.errorMessage());
		#ifdef USE_LAVISHSCRIPT2
		if (ppException)
		{
			// need to convert to UTF-16 for LS2 exception
			LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
			LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
			LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
			*ppException = new LavishScript2::LS2StringException(pString->c_str());
		}
		#endif
		if (!gQuietMode)
			printf(s.c_str());
				
		char *argv[] = {
			(char*)ErrCode.c_str(),
			(char*)s.c_str()
		};
		pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
		#pragma endregion
		delete q;
		return false;
	}

	if (q->eof())
	{
		#pragma region isxSQLite_onErrorMsg
		if (!gQuietMode)
			printf("SQLiteDBType.ExecQuery:: Query returned no results.");
				
		char *argv[] = {
			(char*)"-1",
			(char*)"SQLiteDBType.ExecQuery:: Query returned no results."
		};
		pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
		#pragma endregion 
		q->finalize();
		delete q;
		return false;
	}

	gQueriesCounter++;
	gQueries.insert(make_pair(gQueriesCounter,q));

	return gQueriesCounter;
}

bool CloseDatabase(const char *name, CppSQLite3DB* pDB, LavishScript2::LS2Exception **ppException)
{
	try
	{
		pDB->close();
	}
	catch (CppSQLite3Exception& e)
	{
		#pragma region isxSQLite_onErrorMsg
		std::string ErrCode = format("%d",e.errorCode());
		std::string s = format("SQLiteDB.Close:: Error Closing '%s' (Error: %s)",name,e.errorMessage());

		#ifdef USE_LAVISHSCRIPT2
		if (ppException)
		{
			// need to convert to UTF-16 for LS2 exception
			LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
			LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
			LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
			*ppException = new LavishScript2::LS2StringException(pString->c_str());
		}
		#endif
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

	delete pDB;
	gDatabases.erase(name);
	#pragma region isxSQLite_onStatusMsg
	std::string s = format("SQLiteDB.Close:: Database '%s' closed.",name);
	if (!gQuietMode)
		printf(s.c_str());
				
	char *argv[] = {
		(char*)s.c_str()
	};
	pISInterface->ExecuteEvent(isxSQLite_onStatusMsg,0,1,argv,0);
	#pragma endregion
	return true;
}

bool ExecDML(CppSQLite3DB *pDB, const char *dml, LavishScript2::LS2Exception **ppException)
{
	try 
	{
		pDB->execDML(dml);
	}
	catch (CppSQLite3Exception& e)
	{
		#pragma region isxSQLite_onErrorMsg
		std::string ErrCode = format("%d",e.errorCode());
		std::string s = format("SQLiteDB.ExecDML:: Error executing SQLite DML statement \"%s\". (Error: %d:%s)", dml, e.errorCode(),e.errorMessage());
		#ifdef USE_LAVISHSCRIPT2
		if (ppException)
		{
			// need to convert to UTF-16 for LS2 exception
			LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
			LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
			LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
			*ppException = new LavishScript2::LS2StringException(pString->c_str());
		}
		#endif
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

	return true;
}
#ifdef USE_LAVISHSCRIPT2
bool ExecDMLTransaction(CppSQLite3DB *pDB, LavishScript2::ILS2Array *pArray, LavishScript2::LS2Exception **ppException)
{
	try 
	{
		pDB->execDML( "BEGIN TRANSACTION;" );
	}
	catch (CppSQLite3Exception& e)
	{
		#pragma region isxSQLite_onErrorMsg
		std::string ErrCode = format("%d",e.errorCode());
		std::string s = format("SQLiteDB:ExecDMLTransaction:: Error beginning SQLite Transaction. (Error: %d:%s)",e.errorCode(),e.errorMessage());
		#ifdef USE_LAVISHSCRIPT2
		if (ppException)
		{
			// need to convert to UTF-16 for LS2 exception
			LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
			LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
			LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
			*ppException = new LavishScript2::LS2StringException(pString->c_str());
		}
		#endif

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
	
	for (unsigned int i = 0; i <= pArray->GetSize(); i++)
	{
		LavishScript2::LS2SmartRef<LavishScript2::LS2CodeBoxValue_String> pStringVal;
		if (!pArray->GetAt<LavishScript2::LS2CodeBoxValue_String>(i,LavishScript2::VT_String,pStringVal,ppException))
		{
			return false;
		}
		LavishScript2::LS2SmartRef<LavishScript2::ILS2String8> pDMLString;
		if (!pStringVal->m_pValue->GetLS2String8(pDMLString,ppException))
		{
			return false;
		}
					
		try 
		{
			pDB->execDML( pDMLString->c_str() );
		}
		catch (CppSQLite3Exception& e)
		{
			#pragma region isxSQLite_onErrorMsg
			std::string ErrCode = format("%d",e.errorCode());
			std::string s = format("SQLiteDB:ExecDMLTransaction:: Error executing DML statement \"%s\". (Error: %d:%s)",pDMLString->c_str(), e.errorCode(),e.errorMessage());

			#ifdef USE_LAVISHSCRIPT2
			if (ppException)
			{
				// need to convert to UTF-16 for LS2 exception
				LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
				LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
				LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
				*ppException = new LavishScript2::LS2StringException(pString->c_str());
			}
			#endif

			if (!gQuietMode)
				printf(s.c_str());
				
			char *argv[] = {
				(char*)ErrCode.c_str(),
				(char*)s.c_str()
			};
			pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
			#pragma endregion
			try 
			{
				pDB->execDML( "ROLLBACK;" );
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteDB.ExecDMLTransaction:: Error during SQLite Transaction Rollback. (Error: %d:%s)",e.errorCode(),e.errorMessage());

				#ifdef USE_LAVISHSCRIPT2
				if (ppException)
				{
					// need to convert to UTF-16 for LS2 exception
					LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
					LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
					*ppException = new LavishScript2::LS2StringException(pString->c_str());
				}
				#endif

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

			return false;
		}
	}
	try 
	{
		pDB->execDML( "END TRANSACTION;" );
	}
	catch (CppSQLite3Exception& e)
	{
		#pragma region isxSQLite_onErrorMsg
		std::string ErrCode = format("%d",e.errorCode());
		std::string s = format("SQLiteDB.ExecDMLTransaction:: Error ending SQLite Transaction. (Error: %d:%s)",e.errorCode(),e.errorMessage());

		#ifdef USE_LAVISHSCRIPT2
		if (ppException)
		{
			// need to convert to UTF-16 for LS2 exception
			LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
			LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
			LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
			*ppException = new LavishScript2::LS2StringException(pString->c_str());
		}
		#endif

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

	return true;	
}
#endif // USE_LAVISHSCRIPT2
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
	int   result = -1, length = _vscprintf(fmt,args);
	char *buffer = new char [length + 1];
		result = _vsnprintf_s(buffer, length, _TRUNCATE, fmt, args); 
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