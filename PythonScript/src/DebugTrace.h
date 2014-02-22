#ifndef DEBUGTRACE_20140215_H
#define DEBUGTRACE_20140215_H

#ifdef _DEBUG

    static long s_debugLineCount = 0;


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


#   define DEBUG_TRACE(msg)  debugTrace(msg)
#   define DEBUG_TRACE_S(args) debugTraceVars args
#else
#   define DEBUG_TRACE(msg)
#   define DEBUG_TRACE_S(args)
#endif


#endif // DEBUGTRACE_20140215_H