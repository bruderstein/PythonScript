// PythonScript.Tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <gtest/gtest.h>
#include "ReplaceEntry.h"
#include "Replacer.h"

void deleteEntry(NppPythonScript::ReplaceEntry* entry)
{
    delete entry;
}

void runReplace()
{
    NppPythonScript::Replacer<NppPythonScript::Utf8CharTraits> replacer;
    std::list<NppPythonScript::ReplaceEntry* > entries;
    bool moreEntries = replacer.startReplace("aaabbbaaabb", 12, 0, 0, "(b+)", "x$1x", NppPythonScript::python_re_flag_normal, entries);
    ASSERT_EQ(2, entries.size());
    std::list<NppPythonScript::ReplaceEntry*>::const_iterator it = entries.begin();
    for_each(entries.begin(), entries.end(), deleteEntry);
}


int main(int argc, char* argv[])
{
#ifdef _DEBUG
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
    _CrtMemState state;
    _CrtMemCheckpoint(&state);
#endif


    ::testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();

    // This function call is used to check if we've created memory leaks using startReplace
    // Gtest creates various static objects, so it can be a bit tricky to identify leaks with Gtest running
    // It's left commented out as it normally serves no purpose.  If it looks like there's leaks, then remove the
    // test code above, and uncomment this runReplace() call to try to identify the source of the leak.
    // runReplace();

#ifdef _DEBUG
    _CrtMemDumpAllObjectsSince(&state);
#endif

}

