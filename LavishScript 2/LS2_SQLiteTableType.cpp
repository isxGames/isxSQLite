// LS2Type_Foo.cpp - example from a LavishScript 2.0 module
#ifdef USE_LAVISHSCRIPT2
// include LavishScript 2.0 Module stuff
#include "LS2_sqllite.h"
#include "../isxSQLite.h"

#define g_pLS2Environment LavishScript2::ILS2StandardEnvironment::s_pInstance
LavishScript2::ILS2CodeBoxType *g_pLS2_SQLiteTableType=0;
// set up shorthand form of the LS2 value class we'll use for our DB class



namespace LavishScript2
{
	namespace Types
	{
		namespace SQLite
		{
			namespace Table
			{

				// helper function to retrieve Table number and/or Table from its presumed LS2CodeBoxValue
				bool SQLiteTableFromValue(LavishScript2::LS2CodeBoxValue &subject, CppSQLite3Table **ppTable, int &out_nTable, LavishScript2::LS2Exception **ppException)
				{
					LS2_SUBJECT_AS_OBJECT(pObjectView);
					if (!pObjectView->m_Type.Is(*g_pLS2_SQLiteTableType))
					{
						*ppException = new LavishScript2::LS2IllegalSubjectException(L"Unexpected 'this' object type");
						return false;
					}
					LS2CodeBoxValue_SQLiteTable *pTableView = (LS2CodeBoxValue_SQLiteTable *)pObjectView;
					out_nTable = pTableView->m_Object;

					std::map<int,CppSQLite3Table*>::iterator It = gTables.find(out_nTable);
					if (It == gTables.end())
						return false;

					CppSQLite3Table *pTable = It->second;
					if (!pTable)
						return false;
					if (ppTable)
						*ppTable = pTable;
					return true;
				}


				static bool __stdcall ID_get(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::LS2CodeBoxValue **ppOutput, LavishScript2::LS2Exception **ppException)
				{
					int nTable = 0;
					if (!SQLiteTableFromValue(subject,0,nTable,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.ID_get");
						return false;
					}

					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewInteger(nTable,LavishScript2::VT_Int32,ppOutput);
					return true;
				}

				static bool __stdcall NumRows_get(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::LS2CodeBoxValue **ppOutput, LavishScript2::LS2Exception **ppException)
				{					
					int nTable = 0;
					CppSQLite3Table *pTable = 0;
					if (!SQLiteTableFromValue(subject,&pTable,nTable,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.NumRows_get");
						return false;
					}
					
					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewInteger(pTable->numRows(),LavishScript2::VT_Int32,ppOutput);
					return true;
				}

				static bool __stdcall NumFields_get(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::LS2CodeBoxValue **ppOutput, LavishScript2::LS2Exception **ppException)
				{					
					int nTable = 0;
					CppSQLite3Table *pTable = 0;
					if (!SQLiteTableFromValue(subject,&pTable,nTable,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.NumFields_get");
						return false;
					}
					
					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewInteger(pTable->numFields(),LavishScript2::VT_Int32,ppOutput);
					return true;
				}

				static bool __stdcall GetFieldName(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_INT32(1,pNumField);

					int nTable = 0;
					CppSQLite3Table *pTable = 0;
					if (!SQLiteTableFromValue(subject,&pTable,nTable,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.GetFieldName");
						return false;
					}
					
					std::string Buffer;
					try
					{
						Buffer = pTable->fieldName(pNumField->Value);
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteTable.GetFieldName:: Invalid field index '%d' (Error: %d:%s)", pNumField->Value, e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Table.GetFieldName");
						}

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

					LavishScript2::LS2SmartRef<LavishScript2::LS2CodeBoxValue_String> pOutString;

					if (!LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_UTF8,Buffer.c_str(),pOutString,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.GetFieldName");
						return false;
					}
					pOutRef->SetReference(pOutString);
					return true;
				}

				static bool __stdcall GetFieldValue(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_INT32(1,pNumField);

					int nTable = 0;
					CppSQLite3Table *pTable = 0;
					if (!SQLiteTableFromValue(subject,&pTable,nTable,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.GetFieldValue");
						return false;
					}
					
					std::string Buffer;
					try
					{
						Buffer = pTable->fieldValue(pNumField->Value);
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteTable.GetFieldValue:: Invalid field index '%d' (Error: %d:%s)", pNumField->Value, e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Table.GetFieldValue");
						}

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

					LavishScript2::LS2SmartRef<LavishScript2::LS2CodeBoxValue_String> pOutString;

					if (!LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_UTF8,Buffer.c_str(),pOutString,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.GetFieldValue");
						return false;
					}
					pOutRef->SetReference(pOutString);
					return true;
				}

				static bool __stdcall FieldIsNull(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_INT32(1,pNumField);

					int nTable = 0;
					CppSQLite3Table *pTable = 0;
					if (!SQLiteTableFromValue(subject,&pTable,nTable,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.FieldIsNull");
						return false;
					}
					
					bool bIsNull = false;
					try
					{
						bIsNull = pTable->fieldIsNull(pNumField->Value);
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteTable.FieldIsNull:: Invalid field index '%d' (Error: %d:%s)", pNumField->Value, e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Table.FieldIsNull");
						}

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

					LavishScript2::LS2SmartRef<LavishScript2::LS2CodeBoxValue_Boolean> pOutBool;

					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewBoolean(bIsNull,pOutBool);
					pOutRef->SetReference(pOutBool);
					return true;
				}


				static bool __stdcall SetRow(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					LS2_REQUIRE_INPUTS(==1);
					LS2_INPUT_AS_INT32(0,pNumRow);

					int nTable = 0;
					CppSQLite3Table *pTable = 0;
					if (!SQLiteTableFromValue(subject,&pTable,nTable,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.SetRow");
						return false;
					}
					
					try
					{
						pTable->setRow(pNumRow->Value);
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.SetRow:: Error setting to row %d (Error: %d:%s)", pNumRow->Value, e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Table.SetRow");
						}

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

				static bool __stdcall Finalize(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					int nTable = 0;
					CppSQLite3Table *pTable = 0;
					if (!SQLiteTableFromValue(subject,&pTable,nTable,ppException))
					{
						(*ppException)->Tag(L"SQLite.Table.Finalize");
						return false;
					}
					
					try
					{
						pTable->finalize();
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteTable.Finalize:: Error finalizing table (Error: %d:%s)", e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Table.Finalize");
						}

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
			};
		};
	};
};


bool RegisterSQLiteTableType()
{
	
	if (!g_pLS2Environment->RegisterType(L"SQLite.Table",0,&g_pLS2_SQLiteTableType))
	{
		if (!gQuietMode)
			printf("\arisxSQLite failed to register LavishScript 2 type SQLite.Table");
		return false;
	}

	LavishScript2::LS2SmartRef<LavishScript2::ILS2CodeBoxType> pIDisposable;
	g_pLS2Environment->ResolveType(L"System.IDisposable",pIDisposable);
	g_pLS2_SQLiteDBType->RegisterInterface(pIDisposable);

	LavishScript2::ILS2CodeBoxType *pType = g_pLS2_SQLiteTableType;
	pType->RegisterProperty(L"ID",L"System.Int32",0,&LavishScript2::Types::SQLite::Table::ID_get,0);
	pType->RegisterProperty(L"NumRows",L"System.Int32",0,&LavishScript2::Types::SQLite::Table::NumRows_get,0);
	pType->RegisterProperty(L"NumFields",L"System.Int32",0,&LavishScript2::Types::SQLite::Table::NumFields_get,0);

	REGISTER_METHOD(pType,L"GetFieldName{int}",&LavishScript2::Types::SQLite::Table::GetFieldName,INPUTDECLS(RETURNDECL(L"System.String"),INPUTDECL(L"numField",L"System.Int32",L"System.Int32 numField")));
	REGISTER_METHOD(pType,L"GetFieldValue{int}",&LavishScript2::Types::SQLite::Table::GetFieldValue,INPUTDECLS(RETURNDECL(L"System.String"),INPUTDECL(L"numField",L"System.Int32",L"System.Int32 numField")));
	REGISTER_METHOD(pType,L"FieldIsNull{int}",&LavishScript2::Types::SQLite::Table::FieldIsNull,INPUTDECLS(RETURNDECL(L"System.Boolean"),INPUTDECL(L"numField",L"System.Int32",L"System.Int32 numField")));

	REGISTER_METHOD(pType,L"SetRow{int}",&LavishScript2::Types::SQLite::Table::SetRow,INPUTDECLS(INPUTDECL(L"numRow",L"System.Int32",L"System.Int32 numRow")));
	pType->RegisterMethod(L"Finalize{}",0,&LavishScript2::Types::SQLite::Table::Finalize);
	pType->RegisterMethod(L"Dispose{}",0,&LavishScript2::Types::SQLite::Table::Finalize);

	return true;
}

void UnregisterSQLiteTableType()
{
	if (!g_pLS2_SQLiteTableType)
		return;
	g_pLS2_SQLiteTableType->Delete();
	g_pLS2_SQLiteTableType=0;
}
#endif USE_LAVISHSCRIPT2