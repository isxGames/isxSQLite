/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2012 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#include "../isxSQLite.h"

bool SQLiteQueryType::ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen)
{
	TESTCALLS_LOG_TOTEXT;

	std::map<int,CppSQLite3Query*>::iterator It = gQueries.find(ObjectData.Int);
	if (It == gQueries.end())
		return false;
	CppSQLite3Query *pQuery = It->second;
	if (!pQuery)
		return false;

	sprintf(buf,"%d",It->first);
	return true;
}

bool SQLiteQueryType::GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char *argv[], LSOBJECT &Dest)
{
	TESTCALLS_LOG_GETMEMBER;

	std::map<int,CppSQLite3Query*>::iterator It = gQueries.find(ObjectData.Int);
	if (It == gQueries.end())
		return false;
	CppSQLite3Query *pQuery = It->second;
	if (!pQuery)
		return false;

	switch((SQLiteQueryTypeMembers)pMember->ID)
	{
		case ID:
		{
			Dest.Int = It->first;
			Dest.Type = pIntType;
			return true;
		}
		case NumRows:
		{
			Dest.Int = pQuery->NumRows();
			Dest.Type = pIntType;
			return true;
		}
		case NumFields:
		{
			Dest.Int = pQuery->numFields();
			Dest.Type = pIntType;
			return true;
		}
		case GetFieldName:
		{
			if (argc != 1)
				return false;
			if (!IsNumber(argv[0]))
				return false;

			std::string Buffer;
			try
			{
				Buffer = pQuery->fieldName(atoi(argv[0]));
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.GetFieldName:: Invalid field index '%d' (Error: %d:%s)", atoi(argv[0]), e.errorCode(), e.errorMessage());
				if (!gQuietMode)
					printf(s.c_str());
				
				char *argv[] = {
					(char*)ErrCode.c_str(),
					(char*)s.c_str()
				};
				pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
				#pragma endregion
			}
			Dest.ConstCharPtr = ISXGames::GetTempBuffer.Convert(Buffer);
			Dest.Type = pStringType;
			return true;
		}
		case GetFieldIndex:
		{
			if (argc != 1)
				return false;

			Dest.Int = -1;
			try
			{
				Dest.Int = pQuery->fieldIndex(argv[0]);
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.GetFieldIndex:: Invalid field '%s' (Error: %d:%s)", argv[0], e.errorCode(), e.errorMessage());
				if (!gQuietMode)
					printf(s.c_str());
				
				char *argv[] = {
					(char*)ErrCode.c_str(),
					(char*)s.c_str()
				};
				pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
				#pragma endregion
			}
			Dest.Type = pIntType;
			return true;
		}
		case GetFieldDeclType:
		{
			if (argc != 1)
				return false;
			if (!IsNumber(argv[0]))
				return false;

			std::string Buffer;
			try
			{
				Buffer = pQuery->fieldDeclType(atoi(argv[0]));
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.GetFieldDeclType:: Invalid field index '%d' (Error: %d:%s)", atoi(argv[0]), e.errorCode(), e.errorMessage());
				if (!gQuietMode)
					printf(s.c_str());
				
				char *argv[] = {
					(char*)ErrCode.c_str(),
					(char*)s.c_str()
				};
				pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
				#pragma endregion
			}
			Dest.ConstCharPtr = ISXGames::GetTempBuffer.Convert(Buffer);
			Dest.Type = pStringType;
			return true;
		}
		case GetFieldType:
		{
			if (argc != 1)
				return false;
			if (!IsNumber(argv[0]))
				return false;

			Dest.Int = -1;
			try
			{
				Dest.Int = pQuery->fieldDataType(atoi(argv[0]));
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.GetFieldType:: Invalid field index '%d' (Error: %d:%s)", atoi(argv[0]), e.errorCode(), e.errorMessage());
				if (!gQuietMode)
					printf(s.c_str());
				
				char *argv[] = {
					(char*)ErrCode.c_str(),
					(char*)s.c_str()
				};
				pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
				#pragma endregion
			}
			Dest.Type = pIntType;
			return true;
		}
		case GetFieldValue:
		{
			int ReturnDT = RDT_STRING;
			if (argc == 2)
			{
				if (!stricmp(argv[1],"int"))			ReturnDT = RDT_INT;
				else if (!stricmp(argv[1],"float"))		ReturnDT = RDT_DOUBLE;
				else if (!stricmp(argv[1],"double"))	ReturnDT = RDT_DOUBLE;
				else if (!stricmp(argv[1],"int64"))		ReturnDT = RDT_INT64;
			}
			if (argc < 1)
				return false;

			try
			{
				switch (ReturnDT)
				{
					case RDT_DOUBLE:
					{
						if (IsNumber(argv[0]))
							Dest.Float64Ptr = ISXGames::GetTempBuffer.Convert(pQuery->getFloatField(atoi(argv[0])));
						else
							Dest.Float64Ptr = ISXGames::GetTempBuffer.Convert(pQuery->getFloatField(argv[0]));

						Dest.Type = pFloat64PtrType;
						break;
					}
					case RDT_INT64:
					{
						if (IsNumber(argv[0]))
							Dest.Int64Ptr = ISXGames::GetTempBuffer.Convert(pQuery->getInt64Field(atoi(argv[0])));
						else
							Dest.Int64Ptr = ISXGames::GetTempBuffer.Convert(pQuery->getInt64Field(argv[0]));

						Dest.Type = pInt64PtrType;
						break;
					}
					case RDT_INT:
					{
						if (IsNumber(argv[0]))
							Dest.Int = pQuery->getIntField(atoi(argv[0]));
						else
							Dest.Int = pQuery->getIntField(argv[0]);

						Dest.Type = pIntType;
					}
					default:
					{
						std::string Buffer;
						if (IsNumber(argv[0]))
							Buffer = (char*)pQuery->fieldValue(atoi(argv[0]));
						else
							Buffer = (char*)pQuery->fieldValue(argv[0]);

						Dest.ConstCharPtr = ISXGames::GetTempBuffer.Convert(Buffer);
						Dest.Type = pStringType;
						break;
					}
				}
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.GetFieldValue:: Invalid field '%s' (Error: %d:%s)", argv[0], e.errorCode(), e.errorMessage());
				if (!gQuietMode)
					printf(s.c_str());
				
				char *argv[] = {
					(char*)ErrCode.c_str(),
					(char*)s.c_str()
				};
				pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
				#pragma endregion
			}
			return true;
		}
		case FieldIsNULL:
		{
			if (argc != 1)
				return false;
			
			Dest.Int = -1;
			try
			{
				if (IsNumber(argv[0]))
					Dest.Int = pQuery->fieldIsNull(atoi(argv[0]));
				else
					Dest.Int = pQuery->fieldIsNull(argv[0]);
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.FieldIsNULL:: Invalid field '%s' (Error: %d:%s)", argv[0], e.errorCode(), e.errorMessage());
				if (!gQuietMode)
					printf(s.c_str());
				
				char *argv[] = {
					(char*)ErrCode.c_str(),
					(char*)s.c_str()
				};
				pISInterface->ExecuteEvent(isxSQLite_onErrorMsg,0,2,argv,0);
				#pragma endregion
			}
			Dest.Type = pIntType;
			return true;
		}
		case LastRow:
		{
			Dest.Int = pQuery->eof();
			Dest.Type = pBoolType;
			return true;
		}
	}


	return false;
}

bool SQLiteQueryType::GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMETHOD pMethod, int argc, char *argv[])
{
	TESTCALLS_LOG_GETMETHOD;

	if ((SQLiteQueryTypeMethods)pMethod->ID == Set)
		return this->FromText(ObjectData,argc,argv);

	std::map<int,CppSQLite3Query*>::iterator It = gQueries.find(ObjectData.Int);
	if (It == gQueries.end())
		return false;
	CppSQLite3Query *pQuery = It->second;
	if (!pQuery)
		return false;

	switch((SQLiteQueryTypeMethods)pMethod->ID)
	{
		case NextRow:
		{
			try
			{
				pQuery->nextRow();
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.NextRow:: Error moving to next row (Error: %d:%s)", e.errorCode(), e.errorMessage());
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
		case Reset:
		{
			try
			{
				pQuery->Reset();
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.Reset:: Error resetting query (Error: %d:%s)", e.errorCode(), e.errorMessage());
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
		case Finalize:
		{
			try
			{
				pQuery->finalize();
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.Finalize:: Error finalizing query (Error: %d:%s)", e.errorCode(), e.errorMessage());
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
			int QueryID = It->first;
			delete pQuery;
			gQueries.erase(It);
			#pragma region isxSQLite_onStatusMsg
			std::string s = format("SQLiteQuery.Finalize:: Query %d finalized.",QueryID);
			if (!gQuietMode)
				printf(s.c_str());
				
			char *argv[] = {
				(char*)s.c_str()
			};
			pISInterface->ExecuteEvent(isxSQLite_onStatusMsg,0,1,argv,0);
			#pragma endregion

			return true;
		}
	}

	return false;
}