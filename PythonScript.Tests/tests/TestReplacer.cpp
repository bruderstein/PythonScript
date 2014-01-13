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

TEST_F(ReplacerTest, SimpleReplace) {
    NppPythonScript::Replacer replacer;
    std::list<std::shared_ptr<NppPythonScript::ReplaceEntry> > entries;
    bool moreEntries = replacer.startReplace("aaabbbaaabbb", 12, "bbb", convert, entries);
    ASSERT_EQ(2, entries.size());
    std::list<std::shared_ptr<NppPythonScript::ReplaceEntry> >::const_iterator it = entries.begin();

    ASSERT_EQ(3, (*it)->getStart());
    ++it;
    ASSERT_EQ(9, (*it)->getStart());
    ASSERT_EQ(false, moreEntries);
}

}