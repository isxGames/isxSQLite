/*
	isxSQLite is an extension for InnerSpace (http://www.lavishsoft.com).   
	Copyright 2011-2016 isxGames.com (http://www.isxgames.com)

	Permission to use the source code in this file is granted under the 
	Creative Commons Attribution 3.0 Unported (CC BY 3.0) license.  Visit
	http://creativecommons.org/licenses/by/3.0/ for a summary of what 
	rights are granted under this license.
*/
#include "../isxSQLite.h"
//------------------------------------------------------------------
// This file contains code which was extracted from libisxgames.  
// Libisxgames is a closed source product written by Amadeus and used
// in all other isxGames extensions.
//------------------------------------------------------------------


////////////////////////////////////////////////////////////////////
//// Definitions for libisxgames "globals" 
////////////////////////////////////////////////////////////////////
ISInterface *pISInterface=0;
bool gDoPreUnloadExtension;
bool gDoUnloadExtension;
bool gUnloadingExtension;
bool gDidPreUnload;
bool gExtensionLoadDone = false;
bool gExtensionLoading = true;
bool gRunPatcherNow = false;
bool gUseTestVersion = false;
unsigned int isxGames_onHTTPResponse = 0;
namespace ISXGames
{
	isxGetTempBuffer GetTempBuffer;
}
LSType *pStringType=0;
LSType *pMutableStringType=0;
LSType *pWStringType=0;
LSType *pUniStringType=0;
LSType *pIntType=0;
LSType *pUIntType=0;
LSType *pInt64Type=0;
LSType *pInt64PtrType=0;
LSType *pBoolType=0;
LSType *pFloatType=0;
LSType *pTimeType=0;
LSType *pByteType=0;
LSType *pIntPtrType=0;
LSType *pBoolPtrType=0;
LSType *pFloatPtrType=0;
LSType *pFloat64Type=0;
LSType *pFloat64PtrType=0;
LSType *pBytePtrType=0;
LSType *pPoint3fType=0;
LSType *pIndexType=0;
LSType *pVectorType=0;
LSType *pMapType=0;
LSType *pSetType=0;

////////////////////////////////////////////////////////////////////
//// Utility functions that are normally exported from libisxgames
////////////////////////////////////////////////////////////////////
bool FileExists(const char *strFilename)
{
	struct stat stFileInfo;
	bool blnReturn;
	int intStat;

	// Attempt to get the file attributes
	intStat = stat(strFilename,&stFileInfo);
	if(intStat == 0)
	{
		// We were able to get the file attributes
		// so the file obviously exists.
		blnReturn = true;
	}
	else
	{
		// We were not able to get the file attributes.
		// This may mean that we don't have permission to
		// access the folder which contains this file. If you
		// need to do that level of checking, lookup the
		// return values of stat which will give you
		// more details on why stat failed.
		blnReturn = false;
  }

  return(blnReturn);
}

bool IsConsoleOpen()
{
	LGUIElement *pConsole = g_UIManager.FindElement("Console");
	if (!pConsole)
		return false;
	return pConsole->IsVisible();
}

////////////
// Routines to spew information to debugger.  
////////////
void DebugSpew(const PCHAR szFormat, ...)
{
    va_list vaList;
	va_start( vaList, szFormat );
	std::string output = format_arg_list(szFormat,vaList);

	OutputDebugString("[isxSQLite]");
    OutputDebugString(output.c_str());
    OutputDebugString("\n");

	return;
}

void DebugSpew(int PipeToLoc, const PCHAR szFormat,...)
{
    va_list vaList;
	va_start( vaList, szFormat );
	std::string output = format_arg_list(szFormat,vaList);
 
	if (PipeToLoc == TOFILEOVERWRITE)
	{
		output+="\n";
		WriteToFile("isxSQLite.txt",PIPE_TOFILE_OVERWRITE,output.c_str());
		return;
	}
	else if (PipeToLoc == TOFILE)
	{
		output+="\n";
		WriteToFile("isxSQLite.txt",PIPE_TOFILE_APPEND,output.c_str());
		return;
	}
	else
	{
		OutputDebugString("[isxSQLite]");
		OutputDebugString(output.c_str());
		OutputDebugString("\n");
		return;
	}
	
	return;
}

void WriteToFile(const char *FileName, int Type, const char* Output)
{
	int mode, fd;
	std::string Buffer;

	if (Type == PIPE_TOFILE_OVERWRITE || Type == PIPE_TOFILE_OVERWRITE_RONLY)
		mode = _O_RDWR|_O_TRUNC|_O_CREAT;
	if (Type == PIPE_TOFILE_APPEND)
		mode = _O_RDWR|_O_APPEND|_O_CREAT;
 
	
	Buffer = format("%s\\%s",ModulePath,FileName);

	if (Type == PIPE_TOFILE_OVERWRITE_RONLY)
		fd = _open(Buffer.c_str(),mode);
	else
		fd = _open(Buffer.c_str(), mode, _S_IWRITE);	

	if (fd == -1)
	{
		if ( Type == PIPE_TOFILE_OVERWRITE || Type == PIPE_TOFILE_OVERWRITE_RONLY)
			printf("isxSQLite was unable to create the file, %s, for your output.", FileName);
		else if ( Type == PIPE_TOFILE_APPEND)
			printf("isxSQLite was unable to append the file, %s.", FileName);
		printf("It is possible that this file may be set to READ ONLY.");
		_close(fd);
		return;
	}

	_write(fd, Output, strlen(Output));		
	_close(fd);
	return;

	//could also use fprintf() in the future
}

std::string GetArgvStringForTestingCalls(int argc, char *argv[])
{
	std::string Result = "";
	std::string Buffer;

	if (argc == 0 || !argv)
		return Result;

	for (int i = 0; i < argc; i++)
	{
		if (i)
			Buffer = format(",%s",argv[i]);
		else
			Buffer = format("%s",argv[i]);
		Result += Buffer;
	}

	return Result;
}

////////////////////////////////////////////////////////////////////
//// Custom TempBuffer class by Cybertech
////////////////////////////////////////////////////////////////////
namespace ISXGames
{
	// int64 Conversion: Assign to  Dest.Int64Ptr
	int64_t * isxGetTempBuffer::Convert(int64_t Value)
	{
		return (int64_t *)pISInterface->GetTempBuffer(sizeof(int64_t), (void*)&Value);
	}

	// uint64 Conversion: Assign to  Dest.Int64Ptr
	uint64_t * isxGetTempBuffer::Convert(uint64_t Value)
	{
		return (uint64_t *)pISInterface->GetTempBuffer(sizeof(uint64_t), (void*)&Value);
	}

	// Double Conversion: Assign to Dest.Float64Ptr
	double * isxGetTempBuffer::Convert(double Value)
	{
		return (double *)pISInterface->GetTempBuffer(sizeof(double), (void*)&Value);
	}

	// Char * Conversion: Assign to Dest.CharPtr
	const char * isxGetTempBuffer::Convert(const char * Value)
	{
		return (const char *)pISInterface->GetTempBuffer(strlen(Value)+1, (void*)Value);
	}

	// STD::String Conversion: Assign to Dest.Charptr
	const char * isxGetTempBuffer::Convert(std::string & Value)
	{
		return (const char *)pISInterface->GetTempBuffer(Value.length()+1, (void*)Value.c_str());
	}

	const char * isxGetTempBuffer::Convert(const std::string & Value)
	{
		return (const char *)pISInterface->GetTempBuffer(Value.length()+1, (void*)Value.c_str());
	}
}

#pragma region VersionInfo
//////////////////////////////////////////////////////////////////////
// Implementation of the VersionInfo class.
//
//////////////////////////////////////////////////////////////////////
// ---------------------------------------------------------------------
// use this constructer to get the version info for the calling program
VersionInfo::VersionInfo()
{   
	m_bOk = FALSE;
	m_pVerData = NULL;
	DWORD handle;

	DWORD size = GetFileVersionInfoSize("isxSQLite.dll", &handle);
	if (size == 0) 
	{
		m_pVerData = NULL;
		//DWORD err = GetLastError();
		return;
	}

	m_pVerData = new BYTE[size+1];
	m_bOk = GetFileVersionInfo("isxSQLite.dll", handle, size, m_pVerData);
	//AfxCheckMemory();

	// VerQueryValue returns pairs of Language/characterset information
	// (need to use debug to get the exact format)
	UINT cbTrans = 0;
	DWORD *lpdwTrans = NULL;
	int rc = VerQueryValue(m_pVerData, ("\\VarFileInfo\\Translation"), (VOID**)&lpdwTrans, &cbTrans);
	if (rc) 
	{
		//  WinNT = 040904B0, Win95 = 040904E4, etc
		m_sSearch = format("\\StringFileInfo\\%04x%04x\\", LOWORD(lpdwTrans[(cbTrans/sizeof(DWORD))-1]), HIWORD(lpdwTrans[(cbTrans/sizeof(DWORD))-1]));
	}
}

VersionInfo::~VersionInfo()
{
	delete [] m_pVerData;
}

// -----------------------------------------------------------------------
std::string VersionInfo::NoVerDataString()
{
	return "<no version info found>";   
}

// -----------------------------------------------------------------------
std::string VersionInfo::GetProductName()
{   
	if (!m_bOk)
		return NoVerDataString();
   
	LPBYTE data = NULL;
	unsigned len = 0;
	std::string ProductName = m_sSearch + "ProductName";
	VerQueryValue((LPVOID)m_pVerData, (LPSTR)(ProductName.c_str()), (VOID FAR* FAR*)&data, (UINT FAR*)&len);
    
	return (char*)data;
}

// ------------------------------------------------------------------------

std::string __fastcall ConvertProductVersionToYear(char *in)
{
	std::string Buffer = "";

	if (!in)
		return Buffer;

	while(char c=*in)
	{
		if ( c == ',')
			return Buffer;
		else
		{
			Buffer += c;
			in++;
			continue;
		}
	}

	return Buffer;

}

std::string __fastcall ConvertProductVersionToExtensionVersion(char *in)
{
	bool FirstCommaReached = false;
	bool SecondCommaReached = false;
	bool ThirdCommaReached = false;

	std::string Buffer = "";

	if (!in)
		return Buffer;

	while(char c=*in)
	{
		if (c == ',')
		{
			if (SecondCommaReached)
			{
				ThirdCommaReached = true;
				in++;
				continue;
			}
			else if (FirstCommaReached)
				SecondCommaReached = true;
			else
				FirstCommaReached = true;
		}
		
		if (ThirdCommaReached)
		{
			Buffer += c;
			in++;
			continue;
		}
		in++;
		continue;
	}

	return Buffer;
}

std::string VersionInfo::GetProductVersion()
{   
	if (!m_bOk)
		return NoVerDataString();
   
	LPBYTE data = NULL;
	unsigned len = 0;
	std::string ProductVersion = m_sSearch + "ProductVersion";
	VerQueryValue((LPVOID)m_pVerData, (LPSTR)(ProductVersion.c_str()), (VOID FAR* FAR*)&data, (UINT FAR*)&len);
    
	return format("%s.%s",__isxSQLiteVersion, ConvertProductVersionToExtensionVersion((char*)data).c_str()).c_str();
}

std::string VersionInfo::GetProductYear()
{
   if (!m_bOk)
      return NoVerDataString();
   
   LPBYTE data = NULL;
   unsigned len = 0;
   std::string ProductVersion = m_sSearch + "ProductVersion";
   VerQueryValue((LPVOID)m_pVerData, (LPSTR)(ProductVersion.c_str()), (VOID FAR* FAR*)&data, (UINT FAR*)&len);
    

   return ConvertProductVersionToYear((char*)data).c_str();
}

// ------------------------------------------------------------------------
std::string VersionInfo::GetCompanyName()
{   
   if (!m_bOk)
      return NoVerDataString();
   
   LPBYTE data = NULL;
   unsigned len = 0;
   std::string CompanyName = m_sSearch + "CompanyName";
   VerQueryValue((LPVOID)m_pVerData, (LPSTR)(CompanyName.c_str()), (VOID FAR* FAR*)&data, (UINT FAR*)&len);
    
   return (char*)data;
}

// ------------------------------------------------------------------------
std::string VersionInfo::GetCopyright()
{   
   if (!m_bOk)
      return NoVerDataString();
   
   LPBYTE data = NULL;
   unsigned len = 0;
   std::string LegalCopyright = m_sSearch + "LegalCopyright";
   VerQueryValue((LPVOID)m_pVerData, (LPSTR)(LegalCopyright.c_str()), (VOID FAR* FAR*)&data, (UINT FAR*)&len);
    
   return (char*)data;
}

// ------------------------------------------------------------------------
std::string VersionInfo::GetTrademark()
{   
   if (!m_bOk)
      return NoVerDataString();
   
   LPBYTE data = NULL;
   unsigned len = 0;
   std::string LegalTrademarks = m_sSearch + "LegalTrademarks";
   VerQueryValue((LPVOID)m_pVerData, (LPSTR)(LegalTrademarks.c_str()), (VOID FAR* FAR*)&data, (UINT FAR*)&len);
    
   return (char*)data;
}
#pragma endregion