// LS2Type_Foo.cpp - example from a LavishScript 2.0 module

// include LavishScript 2.0 Module stuff
#include "LS2_sqllite.h"
#include "../isxSQLite.h"

#define g_pLS2Environment LavishScript2::ILS2StandardEnvironment::s_pInstance
LavishScript2::ILS2CodeBoxType *g_pLS2_SQLiteQueryType=0;
// set up shorthand form of the LS2 value class we'll use for our DB class



namespace LavishScript2
{
	namespace Types
	{
		namespace SQLite
		{
			namespace Query
			{

				// helper function to retrieve Query number and/or Query from its presumed LS2CodeBoxValue
				bool SQLiteQueryFromValue(LavishScript2::LS2CodeBoxValue &subject, CppSQLite3Query **ppQuery, int &out_nQuery, LavishScript2::LS2Exception **ppException)
				{
					LS2_SUBJECT_AS_OBJECT(pObjectView);
					if (!pObjectView->m_Type.Is(*g_pLS2_SQLiteQueryType))
					{
						*ppException = new LavishScript2::LS2IllegalSubjectException(L"Unexpected 'this' object type");
						return false;
					}
					LS2CodeBoxValue_SQLiteQuery *pQueryView = (LS2CodeBoxValue_SQLiteQuery *)pObjectView;
					out_nQuery = pQueryView->m_Object;

					std::map<int,CppSQLite3Query*>::iterator It = gQueries.find(out_nQuery);
					if (It == gQueries.end())
						return false;

					CppSQLite3Query *pQuery = It->second;
					if (!pQuery)
						return false;
					if (ppQuery)
						*ppQuery = pQuery;
					return true;
				}


				static bool __stdcall ID_get(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::LS2CodeBoxValue **ppOutput, LavishScript2::LS2Exception **ppException)
				{
					int nQuery = 0;
					if (!SQLiteQueryFromValue(subject,0,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.ID_get");
						return false;
					}

					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewInteger(nQuery,LavishScript2::VT_Int32,ppOutput);
					return true;
				}

				static bool __stdcall NumRows_get(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::LS2CodeBoxValue **ppOutput, LavishScript2::LS2Exception **ppException)
				{					
					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.NumRows_get");
						return false;
					}
					
					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewInteger(pQuery->NumRows(),LavishScript2::VT_Int32,ppOutput);
					return true;
				}

				static bool __stdcall NumFields_get(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::LS2CodeBoxValue **ppOutput, LavishScript2::LS2Exception **ppException)
				{					
					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.NumFields_get");
						return false;
					}
					
					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewInteger(pQuery->numFields(),LavishScript2::VT_Int32,ppOutput);
					return true;
				}

				static bool __stdcall LastRow_get(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::LS2CodeBoxValue **ppOutput, LavishScript2::LS2Exception **ppException)
				{					
					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.LastRow_get");
						return false;
					}
					
					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewBoolean(pQuery->eof(),(LavishScript2::LS2CodeBoxValue_Boolean **)ppOutput);
					return true;
				}

				static bool __stdcall GetFieldName(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_INT32(1,pNumField);

					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.GetFieldName");
						return false;
					}
					
					std::string Buffer;
					try
					{
						Buffer = pQuery->fieldName(pNumField->Value);
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.GetFieldName:: Invalid field index '%d' (Error: %d:%s)", pNumField->Value, e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Query.GetFieldName");
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
						(*ppException)->Tag(L"SQLite.Query.GetFieldName");
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

					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.GetFieldValue");
						return false;
					}
					
					std::string Buffer;
					try
					{
						Buffer = pQuery->fieldValue(pNumField->Value);
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.GetFieldValue:: Invalid field index '%d' (Error: %d:%s)", pNumField->Value, e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Query.GetFieldValue");
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
						(*ppException)->Tag(L"SQLite.Query.GetFieldValue");
						return false;
					}
					pOutRef->SetReference(pOutString);
					return true;
				}

				static bool __stdcall GetFieldDeclType(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_INT32(1,pNumField);

					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.GetFieldDeclType");
						return false;
					}
					
					std::string Buffer;
					try
					{
						Buffer = pQuery->fieldDeclType(pNumField->Value);
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.GetFieldDeclType:: Invalid field index '%d' (Error: %d:%s)", pNumField->Value, e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Query.GetFieldDeclType");
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
						(*ppException)->Tag(L"SQLite.Query.GetFieldDeclType");
						return false;
					}
					pOutRef->SetReference(pOutString);
					return true;
				}

				static bool __stdcall GetFieldDataType(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_INT32(1,pNumField);

					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.GetFieldDataType");
						return false;
					}
					
					int nValue = 0;
					try
					{
						nValue = pQuery->fieldDataType(pNumField->Value);
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.GetFieldDataType:: Invalid field index '%d' (Error: %d:%s)", pNumField->Value, e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Query.GetFieldDataType");
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

					LavishScript2::LS2SmartRef<LavishScript2::LS2CodeBoxValue_Enum> pOutEnum;

					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewEnum(nValue,*g_pLS2_SQLiteDataTypeEnum,pOutEnum);
					pOutRef->SetReference(pOutEnum);
					return true;
				}

				static bool __stdcall GetFieldIndex(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_STRING(1,pFieldName);

					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.GetFieldIndex");
						return false;
					}

					LavishScript2::LS2SmartRef<LavishScript2::ILS2String8> pFieldName8;
					if (!pFieldName->m_pValue->GetLS2String8(pFieldName8,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.GetFieldIndex");
						return false;
					}
					
					int nIndex = 0;
					try
					{
						nIndex = pQuery->fieldIndex(pFieldName8->c_str());
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.GetFieldIndex:: Invalid field '%s' (Error: %d:%s)", pFieldName8->c_str(), e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Query.GetFieldIndex");
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

					LavishScript2::LS2SmartRef<LavishScript2::LS2CodeBoxValue> pOutValue;

					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewInteger(nIndex,LavishScript2::VT_Int32,pOutValue);
					pOutRef->SetReference(pOutValue);
					return true;
				}

				static bool __stdcall FieldIsNull(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_INT32(1,pNumField);

					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.FieldIsNull");
						return false;
					}
					
					bool bIsNull = false;
					try
					{
						bIsNull = pQuery->fieldIsNull(pNumField->Value);
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.FieldIsNull:: Invalid field index '%d' (Error: %d:%s)", pNumField->Value, e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Query.FieldIsNull");
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



				static bool __stdcall Finalize(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.Finalize");
						return false;
					}
					
					try
					{
						pQuery->finalize();
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.Finalize:: Error finalizing Query (Error: %d:%s)", e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Query.Finalize");
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

				static bool __stdcall NextRow(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.NextRow");
						return false;
					}
					
					try
					{
						pQuery->nextRow();
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.NextRow:: Error moving to next row (Error: %d:%s)", e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Query.NextRow");
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

				static bool __stdcall Reset(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{					
					int nQuery = 0;
					CppSQLite3Query *pQuery = 0;
					if (!SQLiteQueryFromValue(subject,&pQuery,nQuery,ppException))
					{
						(*ppException)->Tag(L"SQLite.Query.Reset");
						return false;
					}
					
					try
					{
						pQuery->Reset();
					}
					catch (CppSQLite3Exception& e)
					{
						#pragma region isxSQLite_onErrorMsg
						std::string ErrCode = format("%d",e.errorCode());
						std::string s = format("SQLiteQuery.Reset:: Error resetting query (Error: %d:%s)", e.errorCode(), e.errorMessage());

						{
							// need to convert to UTF-16 for LS2 exception
							LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pString;
							LavishScript2::LS2SmartRef<LavishScript2::LS2Exception> pSubException;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_ACP,s.c_str(),pString,pSubException);
							*ppException = new LavishScript2::LS2StringException(pString->c_str());
							(*ppException)->Tag(L"SQLite.Query.Reset");
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


bool RegisterSQLiteQueryType()
{
	
	if (!g_pLS2Environment->RegisterType(L"SQLite.Query",0,&g_pLS2_SQLiteQueryType))
	{
		if (!gQuietMode)
			printf("\arisxSQLite failed to register LavishScript 2 type SQLite.Query");
		return false;
	}

	LavishScript2::LS2SmartRef<LavishScript2::ILS2CodeBoxType> pIDisposable;
	g_pLS2Environment->ResolveType(L"System.IDisposable",pIDisposable);
	g_pLS2_SQLiteDBType->RegisterInterface(pIDisposable);

	LavishScript2::ILS2CodeBoxType *pType = g_pLS2_SQLiteQueryType;
	pType->RegisterProperty(L"ID",L"System.Int32",0,&LavishScript2::Types::SQLite::Query::ID_get,0);
	pType->RegisterProperty(L"NumRows",L"System.Int32",0,&LavishScript2::Types::SQLite::Query::NumRows_get,0);
	pType->RegisterProperty(L"NumFields",L"System.Int32",0,&LavishScript2::Types::SQLite::Query::NumFields_get,0);	
	pType->RegisterProperty(L"LastRow",L"System.Boolean",0,&LavishScript2::Types::SQLite::Query::LastRow_get,0);

	REGISTER_METHOD(pType,L"GetFieldName{int}",&LavishScript2::Types::SQLite::Query::GetFieldName,INPUTDECLS(RETURNDECL(L"System.String"),INPUTDECL(L"numField",L"System.Int32",L"System.Int32 numField")));
	REGISTER_METHOD(pType,L"GetFieldValue{int}",&LavishScript2::Types::SQLite::Query::GetFieldValue,INPUTDECLS(RETURNDECL(L"System.String"),INPUTDECL(L"numField",L"System.Int32",L"System.Int32 numField")));
	REGISTER_METHOD(pType,L"GetFieldDeclType{int}",&LavishScript2::Types::SQLite::Query::GetFieldDeclType,INPUTDECLS(RETURNDECL(L"System.String"),INPUTDECL(L"numField",L"System.Int32",L"System.Int32 numField")));
	REGISTER_METHOD(pType,L"GetFieldDataType{int}",&LavishScript2::Types::SQLite::Query::GetFieldDataType,INPUTDECLS(RETURNDECL(L"SQLite.DataTypeEnum"),INPUTDECL(L"numField",L"System.Int32",L"System.Int32 numField")));
	REGISTER_METHOD(pType,L"GetFieldIndex{string}",&LavishScript2::Types::SQLite::Query::GetFieldIndex,INPUTDECLS(RETURNDECL(L"System.Int32"),INPUTDECL(L"fieldName",L"System.String",L"System.String fieldName")));
	REGISTER_METHOD(pType,L"FieldIsNull{int}",&LavishScript2::Types::SQLite::Query::FieldIsNull,INPUTDECLS(RETURNDECL(L"System.Boolean"),INPUTDECL(L"numField",L"System.Int32",L"System.Int32 numField")));

	pType->RegisterMethod(L"NextRow{}",0,&LavishScript2::Types::SQLite::Query::NextRow);
	pType->RegisterMethod(L"Reset{}",0,&LavishScript2::Types::SQLite::Query::Reset);

	pType->RegisterMethod(L"Finalize{}",0,&LavishScript2::Types::SQLite::Query::Finalize);
	pType->RegisterMethod(L"Dispose{}",0,&LavishScript2::Types::SQLite::Query::Finalize);

	return true;
}

void UnregisterSQLiteQueryType()
{
	if (!g_pLS2_SQLiteQueryType)
		return;
	g_pLS2_SQLiteQueryType->Delete();
	g_pLS2_SQLiteQueryType=0;
}
