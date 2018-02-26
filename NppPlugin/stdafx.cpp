#include "stdafx.h"

//Added single function from N++ Common.cpp
#include "Common.h"
#include "tchar.h"

generic_string GetLastErrorAsString(DWORD errorCode)
{
	generic_string errorMsg(_T(""));
	// Get the error message, if any.
	// If both error codes (passed error n GetLastError) are 0, then return empty
	if (errorCode == 0)
		errorCode = GetLastError();
	if (errorCode == 0)
		return errorMsg; //No error message has been recorded

	LPWSTR messageBuffer = nullptr;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, nullptr);

	errorMsg += messageBuffer;

	//Free the buffer.
	LocalFree(messageBuffer);

	return errorMsg;
}