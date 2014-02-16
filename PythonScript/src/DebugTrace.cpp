#include "stdafx.h"
#include "DebugTrace.h"

// stati volatile long s_debugLineCount;



volatile long *allocateLineCount()
{
    static volatile long *lineCount;

    lineCount = (long*)_aligned_malloc(sizeof(long), 32);
    *lineCount = 0;
    return lineCount;
}


void debugTraceStr(const std::wstringstream& item)
{
    static volatile long *lineCount = allocateLineCount(); 

    long currentLine = InterlockedIncrement(lineCount);
    std::wstringstream debug;
	debug << L"PY>" << std::setfill(L'0') << std::setw(5) << ::GetCurrentThreadId() << L" " << std::setw(5) << std::setfill(L' ') << currentLine << L"  " << item.str();
	OutputDebugString(debug.str().c_str());
}
