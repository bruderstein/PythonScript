#ifndef DEBUGTRACE_20140215_H
#define DEBUGTRACE_20140215_H

#define DEBUG_TRACE(msg)    { std::wstringstream debug; debug << L"PY>" << ::GetCurrentThreadId() << L" " << msg; OutputDebugString(debug.str().c_str()); }


#endif // DEBUGTRACE_20140215_H