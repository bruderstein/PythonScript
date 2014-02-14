
#include "stdafx.h"


#include <gtest/gtest.h>
#include "Replacer.h"
#include "ReplaceEntry.h"

namespace NppPythonScript 
{
	
class ReplacerTest : public ::testing::Test {
    virtual void SetUp() {

	}

};


ReplaceEntry convert(const char *text, Match *match)
{
    GroupDetail *fullMatch = match->group(0);
    return ReplaceEntry(fullMatch->start(), fullMatch->end(), "yyy", 3);
}


void deleteEntry(ReplaceEntry* entry)
{
    delete entry;
}

TEST_F(ReplacerTest, SimpleReplace) {
    NppPythonScript::Replacer<NppPythonScript::Utf8CharTraits> replacer;
    std::list<NppPythonScript::ReplaceEntry* > entries;
    bool moreEntries = replacer.startReplace("aaabbbaaabb", 12, 0, 0, "(b+)", "x$1x", NppPythonScript::python_re_flag_normal, entries);
    ASSERT_EQ(2, entries.size());
    std::list<NppPythonScript::ReplaceEntry*>::const_iterator it = entries.begin();

    ASSERT_EQ(3, (*it)->getStart());
    ASSERT_STREQ("xbbbx", (*it)->getReplacement());
    ASSERT_EQ(5, (*it)->getReplacementLength());

    ++it;

    ASSERT_EQ(9, (*it)->getStart());
    ASSERT_STREQ("xbbx", (*it)->getReplacement());
    ASSERT_EQ(4, (*it)->getReplacementLength());
    ASSERT_EQ(false, moreEntries);

    for_each(entries.begin(), entries.end(), deleteEntry);
}


/** Test ensures that the replacer is treating the text as UTF8, and not simply a byte sequence 
 */
TEST_F(ReplacerTest, ReplaceUtf8) {
    NppPythonScript::Replacer<NppPythonScript::Utf8CharTraits> replacer;
    std::list<NppPythonScript::ReplaceEntry* > entries;
    bool moreEntries = replacer.startReplace("aaa\xc3\xb4" "bbbaaa\xc3\xbc" "bb", 15, 0, 0,  "aaa([\xc3\xbc])", "x$1x", NppPythonScript::python_re_flag_normal,entries);
    ASSERT_EQ(1, entries.size());
    std::list<NppPythonScript::ReplaceEntry*>::const_iterator it = entries.begin();

    ASSERT_EQ(8, (*it)->getStart());
    ASSERT_STREQ("x\xC3\xBCx", (*it)->getReplacement());
    ASSERT_EQ(4, (*it)->getReplacementLength());


    for_each(entries.begin(), entries.end(), deleteEntry);
}

/** Test ensures characters outside of the BMP are matched correctly
 */
TEST_F(ReplacerTest, ReplaceExtendedUtf8) {
    NppPythonScript::Replacer<NppPythonScript::Utf8CharTraits> replacer;
    std::list<NppPythonScript::ReplaceEntry* > entries;
    bool moreEntries = replacer.startReplace("aaa\xF0\x9F\x82\xB7" "ZZZ" "bbbaaa\xF0\x9F\x82\xB8" "ZZZ", 23, 0, 0, "aaa([\xF0\x9F\x82\xB8])", "x$1x", NppPythonScript::python_re_flag_normal,entries);
    ASSERT_EQ(1, entries.size());
    std::list<NppPythonScript::ReplaceEntry*>::const_iterator it = entries.begin();

    ASSERT_EQ(13, (*it)->getStart());
    ASSERT_STREQ("x\xF0\x9F\x82\xB8x", (*it)->getReplacement());
    ASSERT_EQ(6, (*it)->getReplacementLength());

    for_each(entries.begin(), entries.end(), deleteEntry);
}

/** Tests simple ANSI (8 bit ASCII) replacement
 */
TEST_F(ReplacerTest, ReplaceSimpleAnsi) {

    NppPythonScript::Replacer<NppPythonScript::AnsiCharTraits> replacer;
    std::list<NppPythonScript::ReplaceEntry* > entries;
    bool moreEntries = replacer.startReplace("aaa\xF0\x9F" "ZZZ" "aaa\x9F\xB8" "ZZZ", 16, 0, 0, "aaa([\xF0\x9F])", "x$1x", NppPythonScript::python_re_flag_normal,entries);
    ASSERT_EQ(2, entries.size());
    std::list<NppPythonScript::ReplaceEntry*>::const_iterator it = entries.begin();
    ASSERT_EQ(0, (*it)->getStart());
    ASSERT_STREQ("x\xF0x", (*it)->getReplacement());
    ASSERT_EQ(3, (*it)->getReplacementLength());

    ++it;

    ASSERT_EQ(8, (*it)->getStart());
    ASSERT_STREQ("x\x9Fx", (*it)->getReplacement());
    ASSERT_EQ(3, (*it)->getReplacementLength());

    for_each(entries.begin(), entries.end(), deleteEntry);
}


}