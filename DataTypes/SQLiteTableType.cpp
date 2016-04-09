/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#include "../isxSQLite.h"

bool SQLiteTableType::ToText(LSOBJECTDATA ObjectData, char *buf, unsigned int buflen)
{
	TESTCALLS_LOG_TOTEXT;

	std::map<int,CppSQLite3Table*>::iterator It = gTables.find(ObjectData.Int);
	if (It == gTables.end())
		return false;
	CppSQLite3Table *pTable = It->second;
	if (!pTable)
		return false;

	sprintf(buf,"%d",It->first);
	return true;
}

bool SQLiteTableType::GetMember(LSOBJECTDATA ObjectData, PLSTYPEMEMBER pMember, int argc, char *argv[], LSOBJECT &Dest)
{
	TESTCALLS_LOG_GETMEMBER;

	std::map<int,CppSQLite3Table*>::iterator It = gTables.find(ObjectData.Int);
	if (It == gTables.end())
		return false;
	CppSQLite3Table *pTable = It->second;
	if (!pTable)
		return false;

	switch((SQLiteTableTypeMembers)pMember->ID)
	{
		case ID:
		{
			Dest.Int = It->first;
			Dest.Type = pIntType;
			return true;
		}
		case NumRows:
		{
			Dest.Int = pTable->numRows();
			Dest.Type = pIntType;
			return true;
		}
		case NumFields:
		{
			Dest.Int = pTable->numFields();
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
				Buffer = pTable->fieldName(atoi(argv[0]));
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteTable.GetFieldName:: Invalid field index '%d' (Error: %d:%s)", atoi(argv[0]), e.errorCode(), e.errorMessage());
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
		case GetFieldValue:
		{
			int ReturnDT = RDT_STRING;
			if (argc == 2)
			{
				if (!stricmp(argv[0],"int"))			ReturnDT = RDT_INT;
				else if (!stricmp(argv[0],"float"))		ReturnDT = RDT_DOUBLE;
				else if (!stricmp(argv[0],"double"))	ReturnDT = RDT_DOUBLE;
				else if (!stricmp(argv[0],"int64"))		ReturnDT = RDT_INT64;
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
							Dest.Float64 = pTable->getFloatField(atoi(argv[0]));
						else
							Dest.Float64 = pTable->getFloatField(argv[0]);

						Dest.Type = pFloat64PtrType;
					}
					case RDT_INT64:
					{
						if (IsNumber(argv[0]))
							Dest.Int64 = pTable->getInt64Field(atoi(argv[0]));
						else
							Dest.Int64 = pTable->getInt64Field(argv[0]);

						Dest.Type = pInt64Type;
					}
					case RDT_INT:
					{
						if (IsNumber(argv[0]))
							Dest.Int = pTable->getIntField(atoi(argv[0]));
						else
							Dest.Int = pTable->getIntField(argv[0]);

						Dest.Type = pIntType;
					}
					default:
					{
						std::string Buffer;
						if (IsNumber(argv[0]))
							Buffer = (char*)pTable->fieldValue(atoi(argv[0]));
						else
							Buffer = (char*)pTable->fieldValue(argv[0]);
						Dest.ConstCharPtr = ISXGames::GetTempBuffer.Convert(Buffer);
						Dest.Type = pStringType;
					}
				}
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s;
				if (IsNumber(argv[0]))
					s = format("SQLiteTable.GetFieldValue:: Invalid field index '%d' (Error: %d:%s)", atoi(argv[0]), e.errorCode(), e.errorMessage());
				else
					s = format("SQLiteTable.GetFieldValue:: Invalid field '%s' (Error: %d:%s)", argv[0], e.errorCode(), e.errorMessage());
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
					Dest.Int = pTable->fieldIsNull(atoi(argv[0]));
				else
					Dest.Int = pTable->fieldIsNull(argv[0]);
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s;
				if (IsNumber(argv[0]))
					s = format("SQLiteTable.FieldIsNULL:: Invalid field index '%d' (Error: %d:%s)", atoi(argv[0]), e.errorCode(), e.errorMessage());
				else
					s = format("SQLiteTable.FieldIsNULL:: Invalid field '%s' (Error: %d:%s)", argv[0], e.errorCode(), e.errorMessage());
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
	}


	return false;
}

bool SQLiteTableType::GetMethod(LSOBJECTDATA &ObjectData, PLSTYPEMETHOD pMethod, int argc, char *argv[])
{
	TESTCALLS_LOG_GETMETHOD;

	if ((SQLiteTableTypeMethods)pMethod->ID == Set)
		return this->FromText(ObjectData,argc,argv);

	std::map<int,CppSQLite3Table*>::iterator It = gTables.find(ObjectData.Int);
	if (It == gTables.end())
		return false;
	CppSQLite3Table *pTable = It->second;
	if (!pTable)
		return false;

	switch((SQLiteTableTypeMethods)pMethod->ID)
	{
		case SetRow:
		{
			if (argc != 1)
				return false;
			if (!IsNumber(argv[0]))
				return false;

			try
			{
				pTable->setRow(atoi(argv[0]));
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteQuery.SetRow:: Error setting to row %d (Error: %d:%s)", atoi(argv[0]), e.errorCode(), e.errorMessage());
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
				pTable->finalize();
			}
			catch (CppSQLite3Exception& e)
			{
				#pragma region isxSQLite_onErrorMsg
				std::string ErrCode = format("%d",e.errorCode());
				std::string s = format("SQLiteTable.Finalize:: Error finalizing table (Error: %d:%s)", e.errorCode(), e.errorMessage());
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
			int TableID = It->first;
			delete pTable;
			gTables.erase(It);
			#pragma region isxSQLite_onStatusMsg
			std::string s = format("SQLiteTable.Finalize:: Table %d finalized.",TableID);
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