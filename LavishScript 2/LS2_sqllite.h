//
#include <ls2module.h>

extern LavishScript2::ILS2CodeBoxType *g_pLS2_SQLiteDBType;
extern LavishScript2::ILS2CodeBoxType *g_pLS2_SQLiteTableType;
extern LavishScript2::ILS2CodeBoxType *g_pLS2_SQLiteQueryType;
extern LavishScript2::ILS2CodeBoxType *g_pLS2_SQLiteDataTypeEnum;

typedef LavishScript2::LS2CodeBoxValue_ObjectT<std::string> LS2CodeBoxValue_SQLiteDB;
typedef LavishScript2::LS2CodeBoxValue_ObjectT<int> LS2CodeBoxValue_SQLiteTable;
typedef LavishScript2::LS2CodeBoxValue_ObjectT<int> LS2CodeBoxValue_SQLiteQuery;
typedef LavishScript2::LS2CodeBoxValue_Enum LS2CodeBoxValue_SQLiteDataTypeEnum;

bool RegisterSQLiteDataTypeEnum();
bool RegisterSQLiteDBType();
bool RegisterSQLiteQueryType();
bool RegisterSQLiteTableType();
void UnregisterSQLiteDataTypeEnum();
void UnregisterSQLiteDBType();
void UnregisterSQLiteQueryType();
void UnregisterSQLiteTableType();
