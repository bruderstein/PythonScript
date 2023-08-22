#include "stdafx.h"

#ifdef _DEBUG

#include "DebugTrace.h"
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <Windows.h>
#include <DbgHelp.h>

// stati volatile long s_debugLineCount;



volatile long *allocateLineCount()
{
    static volatile long *lineCount;

    lineCount = (long*)_aligned_malloc(sizeof(long), 32);
    *lineCount = 0;
    return lineCount;
}

void printStack()
{
     unsigned int   i;
     void         * stack[ 100 ];
     unsigned short frames;
     SYMBOL_INFO  * symbol;
     HANDLE         process;
     char buffer[500]{};
     int bufferRemaining = 500;
     int usedBuffer;
     char *output = buffer;

     process = GetCurrentProcess();

     SymInitialize( process, NULL, TRUE );

     frames               = CaptureStackBackTrace( 1, 15, stack, NULL );
     symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
     symbol->MaxNameLen   = 255;
     symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

     
     for( i = 0; i < frames; i++ )
     {
         SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );

         usedBuffer = _snprintf_s(output, bufferRemaining, bufferRemaining, "%i: %s - 0x%I64X\n", frames - i - 1, symbol->Name, symbol->Address );
         bufferRemaining -= usedBuffer;
         output += usedBuffer;
     }

     debugTraceVars("STACK %s", buffer);

     free( symbol );
}

void debugTraceStr(const std::wstringstream& item)
{
    static volatile long *lineCount = allocateLineCount(); 
    FILETIME timestamp;
    GetSystemTimeAsFileTime(&timestamp);

    
    long currentLine = InterlockedIncrement(lineCount);
    std::wstringstream debug;
	debug << L"PY>" << std::setfill(L'0') << std::setw(5) << ::GetCurrentThreadId() 
		        << L" " << std::setw(5) << std::setfill(L' ') << currentLine 
				<< L" " << std::setw(8) << std::setfill(L' ') << timestamp.dwLowDateTime 
				<< L" " << item.str();

	OutputDebugString(debug.str().c_str());
}

void debugTraceVars(const char *format, ...)
{
    va_list args;
    va_start( args, format );
    char buffer[500];
    vsprintf_s<500>(buffer, format, args); 
    std::wstringstream result;
	result << buffer;
    debugTraceStr(result);
}


#endif 