/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#include "../isxSQLite.h"

bool SQLiteDBType::ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen)
{
	TESTCALLS_LOG_TOTEXT;
	if (ObjectData.CharPtr == nullptr)
		return false;

	std::tr1::unordered_map<std::string,CppSQLite3DB*>::iterator It = gDatabases.find(ObjectData.CharPtr);
	if (It == gDatabases.end())
		return false;
	CppSQLite3DB *pDB = It->second;
	if (!pDB)
		return false;

	strcpy(buf,It->first.c_str());
	return true;
}

bool SQLiteDBType::GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char *argv[], LSOBJECT &Dest)
{
	TESTCALLS_LOG_GETMEMBER;
	if (ObjectData.CharPtr == nullptr)
		return false;

	std::tr1::unordered_map<std::string,CppSQLite3DB*>::iterator It = gDatabases.find(ObjectData.CharPtr);
	if (It == gDatabases.end())
		return false;
	CppSQLite3DB *pDB = It->second;
	if (!pDB)
		return false;

	switch((SQLiteDBTypeMembers)pMember->ID)
	{
		case ID:
		{
			Dest.ConstCharPtr = ISXGames::GetTempBuffer.Convert(It->first);
			Dest.Type = pStringType;
			return true;
		}
		case ExecQuery:
		{
			if (argc != 1)
				return false;
			
			if (Dest.Int = ::ExecQuery(pDB,argv[0],0))
			{
				Dest.Type = pSQLiteQueryType;
				return true;
			}
			return false;
		}
		case TableExists:
		{
			if (argc != 1)
				return false;

			Dest.Int = pDB->tableExists(argv[0]);
			Dest.Type = pBoolType;
			return true;
		}
		case GetTable:
		{
			if (argc != 1)
				return false;
			
			if (Dest.Int = OpenTable(pDB,argv[0],0))
			{
				Dest.Type = pSQLiteTableType;
				return true;
			}
			
			return false;
		}
	}


	return false;
}

bool SQLiteDBType::GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMETHOD pMethod, int argc, char *argv[])
{
	TESTCALLS_LOG_GETMETHOD;
	if ((SQLiteDBTypeMethods)pMethod->ID == Set && argc > 0)											
		return this->FromText(ObjectData,argc,argv);
	if (ObjectData.CharPtr == nullptr)
		return false;

	std::tr1::unordered_map<std::string,CppSQLite3DB*>::iterator It = gDatabases.find(ObjectData.CharPtr);
	if (It == gDatabases.end())
		return false;
	CppSQLite3DB *pDB = It->second;
	if (!pDB)
		return false;

	switch((SQLiteDBTypeMethods)pMethod->ID)
	{
		case Close:
		{
			return CloseDatabase(It->first.c_str(),pDB);
		}
		case ExecDML:
		{
			if (argc != 1)
			{
				#pragma region isxSQLite_onErrorMsg
				if (!gQuietMode)
					printf("SQLiteDB.ExecDML:: This method requires exactly one argument.");
				
				char *argv[] = {
					(char*)"-1",
					(char*)"SQLiteDB.ExecDML:: This method requires exactly one argument."
				};
				pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
				#pragma endregion
				return false;
			}
			return ::ExecDML(pDB,argv[0],0);
		}
		case ExecDMLTransaction:
		{
			if (argc != 1)
			{
				#pragma region isxSQLite_onErrorMsg
				if (!gQuietMode)
					printf("SQLiteDB:ExecDMLTransaction:: This method requires exactly one argument.");
				
				char *argv[] = {
					(char*)"-1",
					(char*)"SQLiteDB:ExecDMLTransaction:: This method requires exactly one argument."
				};
				pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
				#pragma endregion
				return false;
			}
			try 
			{
				LSOBJECT Object;

				if (!pISInterface->DataParse(argv[0],Object))
				{
					#pragma region isxSQLite_onErrorMsg
					if (!gQuietMode)
						printf("SQLiteDB:ExecDMLTransaction:: Invalid arguments.");
				
					char *argv[] = {
						(char*)"-1",
						(char*)"SQLiteDB:ExecDMLTransaction:: Invalid arguments."
					};
					pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
					#pragma endregion
					return false;
				}
                if (Object.Type != pIndexType)
				{
					#pragma region isxSQLite_onErrorMsg
					std::string s = format("SQLiteDB:ExecDMLTransaction:: Invalid object type (%s)",Object.Type->GetName());
					if (!gQuietMode)
						printf(s.c_str());
				
					char *argv[] = {
						(char*)"-1",
						(char*)s.c_str()
					};
					pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
					#pragma endregion
					return false;
				}
                
				LSIndex *pIndex=(LSIndex*)Object.Ptr;
                // ensure that the index subtype (i.e. index of what) is what we're expecting
                if (pIndex && pIndex->GetType() != pMutableStringType)
				{
					#pragma region isxSQLite_onErrorMsg
					std::string s = format("SQLiteDB:ExecDMLTransaction:: Invalid object subtype (%s)",pIndex->GetType()->GetName());
					if (!gQuietMode)
						printf(s.c_str());
				
					char *argv[] = {
						(char*)"-1",
						(char*)s.c_str()
					};
					pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
					#pragma endregion
					return false;
				}

				try 
				{
					pDB->execDML( "BEGIN TRANSACTION;" );
				}
				catch (CppSQLite3Exception& e)
				{
					#pragma region isxSQLite_onErrorMsg
					std::string ErrCode = format("%d",e.errorCode());
					std::string s = format("SQLiteDB:ExecDMLTransaction:: Error beginning SQLite Transaction. (Error: %d:%s)",e.errorCode(),e.errorMessage());
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
				for (unsigned int i = 1; i <= pIndex->GetUsed(); i++)
				{
					PLSOBJECT pDML = pIndex->GetItem(i);
					utf8string *pString;
					const char *pDMLString;
					if (pDML)
					{
						pString = (utf8string*)pDML->Ptr;
						if (pString)
							pDMLString = pString->c_str();
					}
					
					if (pDMLString)
					{
						/*#pragma region isxSQLite_onStatusMsg
						sprintf(gBuf, "SQLiteDB.ExecDMLTransaction:: Executing %d. %s",i,pDMLString); 
						if (!gQuietMode)
							printf(gBuf);
				
						char *argv[] = {
							(char*)gBuf
						};
						pISInterface->ExecuteEvent(isxSQLite_onStatusMsg,0,1,argv,0);
						#pragma endregion*/

						try 
						{
							pDB->execDML( pDMLString );
						}
						catch (CppSQLite3Exception& e)
						{
							#pragma region isxSQLite_onErrorMsg
							std::string ErrCode = format("%d",e.errorCode());
							std::string s = format("SQLiteDB:ExecDMLTransaction:: Error executing DML statement \"%s\". (Error: %d:%s)",pDMLString, e.errorCode(),e.errorMessage());
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
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteDB.ExecDMLTransaction:: (Error: %d:%s)",e.errorCode(),e.errorMessage());
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
	}

	return false;
}