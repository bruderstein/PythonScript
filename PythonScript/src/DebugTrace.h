#ifndef DEBUGTRACE_20140215_H
#define DEBUGTRACE_20140215_H

static long s_debugLineCount = 0;
/*
#define DEBUG_TRACE(msg)    { \
	long currentLineCount = InterlockedIncrement(&s_debugLineCount);\
	std::wstringstream debug; \
	debug << L"PY>" << std::setfill(L'0') << std::setw(5) << ::GetCurrentThreadId() << L" " << std::setw(5) << std::setfill(L' ') << currentLineCount << L"  " << msg; \
	OutputDebugString(debug.str().c_str()); \
    }
    */

template <typename T>
void debugTrace(T msg)
{
	std::wstringstream debug;
	debug << msg;
    debugTraceStr(debug);
}

void printStack();

void debugTraceStr(const std::wstringstream& item);
void debugTraceVars(const char *format, ...);


#define DEBUG_TRACE(msg)  debugTrace(msg)

#define DEBUG_TRACE_S(args) debugTraceVars args
#endif // DEBUGTRACE_20140215_H