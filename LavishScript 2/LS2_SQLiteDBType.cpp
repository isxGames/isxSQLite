#ifdef USE_LAVISHSCRIPT2
// LS2Type_Foo.cpp - example from a LavishScript 2.0 module

// include LavishScript 2.0 Module stuff
#include "LS2_sqllite.h"
#include "../isxSQLite.h"

#define g_pLS2Environment LavishScript2::ILS2StandardEnvironment::s_pInstance
LavishScript2::ILS2CodeBoxType *g_pLS2_SQLiteDBType=0;
// set up shorthand form of the LS2 value class we'll use for our DB class



namespace LavishScript2
{
	namespace Types
	{
		namespace SQLite
		{

			namespace DB
			{

				// helper function to retrieve DB name and/or DB from its presumed LS2CodeBoxValue
				bool SQLiteDBFromValue(LavishScript2::LS2CodeBoxValue &subject, CppSQLite3DB **ppDB, LavishScript2::ILS2String **ppName, LavishScript2::LS2Exception **ppException)
				{
					LS2_SUBJECT_AS_OBJECT(pObjectView);
					if (!pObjectView->m_Type.Is(*g_pLS2_SQLiteDBType))
					{
						*ppException = new LavishScript2::LS2IllegalSubjectException(L"Unexpected 'this' object type");
						return false;
					}
					LS2CodeBoxValue_SQLiteDB *pDBView = (LS2CodeBoxValue_SQLiteDB *)pObjectView;

					if (ppName)
					{
						if (!LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(CP_UTF8,pDBView->m_Object.c_str(),ppName,ppException))
						{
							return false;
						}
					}

					std::tr1::unordered_map<std::string,CppSQLite3DB*>::iterator It = gDatabases.find(pDBView->m_Object.c_str());
					if (It == gDatabases.end())
					{
						*ppException = new LavishScript2::LS2NotFoundException(L"No database");
						return false;
					}
					CppSQLite3DB *pDB = It->second;
					if (!pDB)
					{
						*ppException = new LavishScript2::LS2ObjectDisposedException(L"Database has been disposed");
						return false;
					}
					
					if (ppDB)
						*ppDB = pDB;
					return true;
				}
				
				static bool __stdcall Open(LavishScript2::ILS2Array *pInputs, class LavishScript2::LS2Exception **ppException)
				{
					LS2_REQUIRE_INPUTS(==3);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_STRING(1,pName);
					LS2_INPUT_AS_STRING(2,pFilename);

					LavishScript2::LS2SmartRef<LavishScript2::ILS2String8> pName8;
					if (!pName->m_pValue->GetLS2String8(pName8,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.Open");
						return false;
					}

					LavishScript2::LS2SmartRef<LavishScript2::ILS2String8> pFilename8;
					if (!pFilename->m_pValue->GetLS2String8(pFilename8,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.Open");
						return false;
					}

					CppSQLite3DB *pDB = OpenDatabase(pName8->c_str(),pFilename8->c_str(),ppException);			// if everything goes well, this opens the DB and adds it to gDatabases
					if (!pDB)
					{
						(*ppException)->Tag(L"SQLite.DB.Open");
						return false;	
					}

					pOutRef->SetReference(new LS2CodeBoxValue_SQLiteDB(*g_pLS2_SQLiteDBType,pName8->c_str()),false);
					return true;
				}

				static bool __stdcall ID_get(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::LS2CodeBoxValue **ppOutput, LavishScript2::LS2Exception **ppException)
				{
					LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pName;
					if (!SQLiteDBFromValue(subject,0,pName,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.ID_get");
						return false;
					}

					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewString(pName,(LavishScript2::LS2CodeBoxValue_String **)ppOutput);
					return true;
				}

				static bool __stdcall Close(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{
					CppSQLite3DB *pDB = 0;
					LavishScript2::LS2SmartRef<LavishScript2::ILS2String> pName;
					if (!SQLiteDBFromValue(subject,&pDB,pName,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.Close");
						return false;
					}

					LavishScript2::LS2SmartRef<LavishScript2::ILS2String8> pName8;
					if (!pName->GetLS2String8(pName8,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.Close");
						return false;
					}
					return CloseDatabase(pName8->c_str(),pDB,ppException);
				}

				static bool __stdcall TableExists(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_STRING(1,pName);

					CppSQLite3DB *pDB = 0;
					if (!SQLiteDBFromValue(subject,&pDB,0,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.TableExists");
						return false;
					}

					LavishScript2::LS2SmartRef<LavishScript2::ILS2String8> pName8;
					if (!pName->m_pValue->GetLS2String8(pName8,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.TableExists");
						return false;
					}

					LavishScript2::LS2SmartRef<LavishScript2::LS2CodeBoxValue_Boolean> pBoolean;
					LavishScript2::ILS2StandardEnvironment::s_pInstance->NewBoolean(pDB->tableExists(pName8->c_str()),pBoolean);
					pOutRef->SetReference(pBoolean);
					return true;
				}

				static bool __stdcall GetTable(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_STRING(1,pName);

					CppSQLite3DB *pDB = 0;
					if (!SQLiteDBFromValue(subject,&pDB,0,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.GetTable");
						return false;
					}

					LavishScript2::LS2SmartRef<LavishScript2::ILS2String8> pName8;
					if (!pName->m_pValue->GetLS2String8(pName8,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.GetTable");
						return false;
					}

					int nTable = ::OpenTable(pDB,pName8->c_str(),ppException);
					if (!nTable)
					{
						(*ppException)->Tag(L"SQLite.DB.GetTable");
						return false;
					}

					pOutRef->SetReference(new LS2CodeBoxValue_SQLiteTable(*g_pLS2_SQLiteTableType,nTable),false);
					return true;
				}

				static bool __stdcall ExecQuery(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{
					LS2_REQUIRE_INPUTS(==2);
					LS2_INPUT_AS_REFERENCE(0,pOutRef);
					LS2_INPUT_AS_STRING(1,pSQL);

					CppSQLite3DB *pDB = 0;
					if (!SQLiteDBFromValue(subject,&pDB,0,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.ExecQuery");
						return false;
					}

					LavishScript2::LS2SmartRef<LavishScript2::ILS2String8> pSQL8;
					if (!pSQL->m_pValue->GetLS2String8(pSQL8,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.ExecQuery");
						return false;
					}

					int nQuery = ::ExecQuery(pDB,pSQL8->c_str(),ppException);
					if (!nQuery)
					{
						if (!*ppException)
						{
							// no exception given, it is an empty result set. just give null.
							LavishScript2::LS2SmartRef<LavishScript2::LS2CodeBoxValue_Null> pNull;
							LavishScript2::ILS2StandardEnvironment::s_pInstance->NewNull(pNull);
							pOutRef->SetReference(pNull);
							return true;
						}

						// exception given
						(*ppException)->Tag(L"SQLite.DB.ExecQuery");
						return false;
					}

					pOutRef->SetReference(new LS2CodeBoxValue_SQLiteQuery(*g_pLS2_SQLiteTableType,nQuery),false);
					return true;
				}

				static bool __stdcall ExecDML(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{
					LS2_REQUIRE_INPUTS(==1);
					LS2_INPUT_AS_STRING(0,pSQL);

					CppSQLite3DB *pDB = 0;
					if (!SQLiteDBFromValue(subject,&pDB,0,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.ExecDML");
						return false;
					}

					LavishScript2::LS2SmartRef<LavishScript2::ILS2String8> pSQL8;
					if (!pSQL->m_pValue->GetLS2String8(pSQL8,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.ExecDML");
						return false;
					}

					return ::ExecDML(pDB,pSQL8->c_str(),ppException);
				}

				static bool __stdcall ExecDMLTransaction(LavishScript2::LS2CodeBoxValue &subject, LavishScript2::ILS2Array *pInputs, LavishScript2::LS2Exception **ppException)
				{
					LS2_REQUIRE_INPUTS(==1);
					LS2_INPUT_AS_ARRAY(0,pSQL);

					CppSQLite3DB *pDB = 0;
					if (!SQLiteDBFromValue(subject,&pDB,0,ppException))
					{
						(*ppException)->Tag(L"SQLite.DB.ExecDMLTransaction");
						return false;
					}
					
					return ::ExecDMLTransaction(pDB,pSQL->m_pArray,ppException);
				}

			};
		};
	};
};

bool RegisterSQLiteDBType()
{
	
	if (!g_pLS2Environment->RegisterType(L"SQLite.DB",0,&g_pLS2_SQLiteDBType))
	{
		if (!gQuietMode)
			printf("\arisxSQLite failed to register LavishScript 2 type SQLite.DB");
		return false;
	}
	LavishScript2::ILS2CodeBoxType *pType = g_pLS2_SQLiteDBType;

	LavishScript2::LS2SmartRef<LavishScript2::ILS2CodeBoxType> pIDisposable;
	g_pLS2Environment->ResolveType(L"System.IDisposable",pIDisposable);
	g_pLS2_SQLiteDBType->RegisterInterface(pIDisposable);

	REGISTER_STATIC_METHOD(pType,L"Open{string,string}",&LavishScript2::Types::SQLite::DB::Open,INPUTDECLS(RETURNDECL(L"SQLite.DB"),INPUTDECL(L"dbName",L"System.String",L"System.String dbName"),INPUTDECL(L"filename",L"System.String",L"System.String filename")));
	REGISTER_METHOD(pType,L"TableExists{string}",&LavishScript2::Types::SQLite::DB::TableExists,INPUTDECLS(RETURNDECL(L"System.Boolean"),INPUTDECL(L"tableName",L"System.String",L"System.String tableName")));
	REGISTER_METHOD(pType,L"GetTable{string}",&LavishScript2::Types::SQLite::DB::GetTable,INPUTDECLS(RETURNDECL(L"SQLite.Table"),INPUTDECL(L"tableName",L"System.String",L"System.String tableName")));
	REGISTER_METHOD(pType,L"ExecQuery{string}",&LavishScript2::Types::SQLite::DB::ExecQuery,INPUTDECLS(RETURNDECL(L"SQLite.Query"),INPUTDECL(L"sql",L"System.String",L"System.String sql")));

	REGISTER_METHOD(pType,L"ExecDML{string}",&LavishScript2::Types::SQLite::DB::ExecDML,INPUTDECLS(INPUTDECL(L"dml",L"System.String",L"System.String dml")));
	REGISTER_METHOD(pType,L"ExecDMLTransaction{string[]}",&LavishScript2::Types::SQLite::DB::ExecDMLTransaction,INPUTDECLS(INPUTDECL(L"dml",L"System.Array",L"System.String[] dml")));

	pType->RegisterMethod(L"Close{}",0,&LavishScript2::Types::SQLite::DB::Close);
	pType->RegisterMethod(L"Dispose{}",0,&LavishScript2::Types::SQLite::DB::Close);


	pType->RegisterProperty(L"ID",L"System.String",0,&LavishScript2::Types::SQLite::DB::ID_get,0);

	return true;
}

void UnregisterSQLiteDBType()
{
	if (!g_pLS2_SQLiteDBType)
		return;
	g_pLS2_SQLiteDBType->Delete();
	g_pLS2_SQLiteDBType=0;
}
#endif //USE_LAVISHSCRIPT2