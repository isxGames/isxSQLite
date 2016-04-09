/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#pragma once
//------------------------------------------------------------------
// This file contains code which was extracted from libisxgames.  
// Libisxgames is a closed source product written by Amadeus and used
// in all other isxGames extensions.
//------------------------------------------------------------------


////////////////////////////////////////////////////////////////////
//// Includes and pragma settings
////////////////////////////////////////////////////////////////////
#include <codeanalysis\warnings.h>

#pragma warning(disable:4505) // unreferenced local function has been removed

#pragma warning(push,3) // Reset warning level to /W3 during 3rd party includes
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS) // See http://msdn.microsoft.com/en-us/library/zyhb0b82.aspx
#pragma warning(disable:6246) // Local declaration of <x> hides declaration of the same name in outer scope (massive in boost)

#include <isxdk.h>
#include <unordered_map>
#include <windows.h>
#include <psapi.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <winver.h>
#include "..\SQLite\sqlite3.h"
#include "..\SQLite\CppSQLite3.h"

////////////////////////////////////////////////////////////////////
// The isxGamesExtension class (functions are defined in isxSQLite.cpp)
////////////////////////////////////////////////////////////////////
class isxGamesExtension :
	public ISXInterface
{
public:
	isxGamesExtension() {}
	~isxGamesExtension() {}

	virtual bool Initialize(ISInterface *p_ISInterface);
	virtual bool Initialize2();
	virtual void Shutdown();
	
	virtual void RegisterExtension();

	virtual void ConnectPulseService();
	virtual void ConnectServices();
	virtual void RegisterCommands();
	virtual void RegisterAliases();
	virtual void RegisterDataTypes();
	virtual void RegisterExtDataTypes();
	virtual void RegisterTopLevelObjects();
	virtual void RegisterExtTopLevelObjects();
	virtual void RegisterServices();
	virtual void RegisterTriggers();
	virtual void RegisterDetours();

	virtual void DisconnectServices();
	virtual void UnRegisterCommands();
	virtual void UnRegisterAliases();
	virtual void UnRegisterDataTypes();
	virtual void UnRegisterExtDataTypes();
	virtual void UnRegisterTopLevelObjects();
	virtual void UnRegisterExtTopLevelObjects();
	virtual void UnRegisterServices();
	virtual void UnRegisterTriggers();
	virtual void UnRegisterDetours();

	// Access to InnerSpace TLO
	fLSTopLevelObject pUIElementTLO;	    // the "UIElement" TLO

	// InnerSpace Datatypes
	LSType *pLGUIElementDT;					// the "lguielement" datatype
};
extern isxGamesExtension *pExtension;


////////////////////////////////////////////////////////////////////
// Macros
////////////////////////////////////////////////////////////////////
#define MAX_STRING				2048
#ifdef USE_LIBISXGAMES
// openssl uses an IS_SET macro
#ifndef IS_SET
#define IS_SET(flag, bit)		((flag) & (bit))
#endif
#else
#define IS_SET(flag, bit)		((flag) & (bit))
#endif
#define IS_FLAG_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)		((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))

#define printf pISInterface->Printf

#define EzDetour(Address, Detour, Trampoline) IS_Detour(pExtension,pISInterface,hMemoryService,(unsigned int)Address,Detour,Trampoline)
#define EzUnDetour(Address) IS_UnDetour(pExtension,pISInterface,hMemoryService,(unsigned int)Address)
#define EzDetourAPI(_Detour_,_DLLName_,_FunctionName_,_FunctionOrdinal_) IS_DetourAPI(pExtension,pISInterface,hMemoryService,_Detour_,_DLLName_,_FunctionName_,_FunctionOrdinal_)
#define EzUnDetourAPI(Address) IS_UnDetourAPI(pExtension,pISInterface,hMemoryService,(unsigned int)Address)

#define EzModify(Address,NewData,Length,Reverse) Memory_Modify(pExtension,pISInterface,hMemoryService,(unsigned int)Address,NewData,Length,Reverse)
#define EzUnModify(Address) Memory_UnModify(pExtension,pISInterface,hMemoryService,(unsigned int)Address)

#define EzHttpRequest(_URL_,_pData_) IS_HttpRequest(pExtension,pISInterface,hHTTPService,_URL_,_pData_)

#define EzAddTrigger(Text,Callback,pUserData) IS_AddTrigger(pExtension,pISInterface,hTriggerService,Text,Callback,pUserData)
#define EzRemoveTrigger(ID) IS_RemoveTrigger(pExtension,pISInterface,hTriggerService,ID)
#define EzCheckTriggers(Text) IS_CheckTriggers(pExtension,pISInterface,hTriggerService,Text)

#define TOFILE							1
#define TOFILEOVERWRITE					2

#define PIPE_NONE						0
#define PIPE_TOCONSOLE					1
#define PIPE_TOSCREEN					2
#define PIPE_TOFILE_OVERWRITE_RONLY		3
#define PIPE_TOFILE_OVERWRITE			4
#define PIPE_TOFILE_APPEND				5
#define PIPE_TODEBUGGER					6

////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////
extern bool FileExists(const char *strFilename);
extern bool IsConsoleOpen();
extern void DebugSpew(const PCHAR szFormat,...);
extern void DebugSpew(int PipeToLoc, const PCHAR szFormat,...);
extern void WriteToFile(const char *FileName, int Type, const char* Output);
extern std::string GetArgvStringForTestingCalls(int argc, char *argv[]);

////////////////////////////////////////////////////////////////////
// ISXDK/InnerSpace related Globals
// (NOTE:  These variables are set in isxSQLite.cpp, not libisxgames.cpp)
////////////////////////////////////////////////////////////////////
extern HISXSERVICE hPulseService;
extern HISXSERVICE hMemoryService;
extern HISXSERVICE hHTTPService;
extern HISXSERVICE hTriggerService;
extern LSType *pStringType;
extern LSType *pMutableStringType;
extern LSType *pWStringType;
extern LSType *pUniStringType;
extern LSType *pIntType;
extern LSType *pUIntType;
extern LSType *pInt64Type;
extern LSType *pInt64PtrType;
extern LSType *pBoolType;
extern LSType *pFloatType;
extern LSType *pTimeType;
extern LSType *pByteType;
extern LSType *pIntPtrType;
extern LSType *pBoolPtrType;
extern LSType *pFloatPtrType;
extern LSType *pFloat64Type;
extern LSType *pFloat64PtrType;
extern LSType *pBytePtrType;
extern LSType *pPoint3fType;
extern LSType *pIndexType;
extern LSType *pVectorType;
extern LSType *pMapType;
extern LSType *pSetType;
extern ISInterface *pISInterface;

////////////////////////////////////////////////////////////////////
// Extension globals
////////////////////////////////////////////////////////////////////
extern bool gDoPreUnloadExtension;
extern bool gDoUnloadExtension;
extern bool gUnloadingExtension;
extern bool gDidPreUnload;
extern bool gExtensionLoadDone;
extern bool gExtensionLoading;
extern bool gRunPatcherNow;
extern bool gUseTestVersion;
extern unsigned int isxGames_onHTTPResponse;



////////////////////////////////////////////////////////////////////
// Temporary Buffer class written by CyberTech
////////////////////////////////////////////////////////////////////
namespace ISXGames
{
	/*
	This class is designed to convert various datatypes into an Innerspace GetTempBuffer, for use
	as a return value in GetMemberEx Dest.* variables.
	-- CyberTech
	*/
	class isxGetTempBuffer
	{
	public:

		isxGetTempBuffer(void){}
		~isxGetTempBuffer(void){}

		// int64 Conversion: Assign to Dest.Int64Ptr
		int64_t * Convert(int64_t Value);
		uint64_t * Convert(uint64_t Value);

		// Double Conversion: Assign to Dest.Float64Ptr
		double * Convert(double Value);

		// Char * Conversion: Assign to Dest.CharPtr
		const char * Convert(const char * Value);

		// STD::String Conversion: Assign to Dest.Charptr
		const char * Convert(std::string & Value);
		const char * Convert(const std::string & Value);
	};

	extern isxGetTempBuffer GetTempBuffer;
}

#pragma region VersionInfo
////////////////////////////////////////////////////////////////////
// Implementation of the VersionInfo class.
//
////////////////////////////////////////////////////////////////////
#pragma comment(lib, "version")
class VersionInfo
{
public:
   std::string GetProductName();
   std::string GetProductVersion();
   std::string GetProductYear();
   std::string GetCompanyName(); 
   std::string GetCopyright();
   std::string GetTrademark();
  
   VersionInfo(void);        // for calling module version info
   virtual ~VersionInfo(void);

private:
   BOOL m_bOk;
   BYTE* m_pVerData;
   std::string NoVerDataString();
   std::string m_sSearch;
};
#pragma endregion