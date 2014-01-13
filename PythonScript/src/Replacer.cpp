#include "stdafx.h"

#ifndef REPLACER_H_DGB_20131120
#define REPLACER_H_DGB_20131120


#include "Replacer.h"
#include "ReplaceEntry.h"
#include "UTF8Iterator.h"
#include "ConstString.h"
#include "UtfConversion.h"
#include "Match.h"

namespace NppPythonScript
{

using UtfConversion::toStringType;

typedef boost::basic_regex<U32, u32_regex_traits> u32_regex;
typedef boost::regex_iterator<UTF8Iterator, U32, u32_regex_traits> u32_regex_iterator;

class BoostRegexGroupDetail : public GroupDetail
{
public:
    BoostRegexGroupDetail(boost::sub_match<UTF8Iterator> subMatch)
		: m_subMatch(subMatch)
	{}
	int start() const { return  m_subMatch.first.pos(); }
	int end() const { return m_subMatch.second.pos(); }

private:
    boost::sub_match<UTF8Iterator> m_subMatch;
};


class BoostRegexMatch : public Match
{
public:
    BoostRegexMatch(const char *text, boost::match_results<UTF8Iterator>* match)
		: m_text(text),
          m_match(*match)
	{}
    virtual ~BoostRegexMatch();
    BoostRegexMatch& operator= (BoostRegexMatch& rhs) {
        m_text = rhs.m_text;
        m_match = rhs.m_match;

        /* We explicitely don't copy the list, as the allocatedGroupDetails will simply be destructed when this object gets destroyed.
         * In theory, this would be bad, as we would delete the allocated GroupDetail objects when this object is deleted, 
         * even though the various groups may still be in use.  
         * In practice however, these GroupDetails don't actually live as long as this object, as we've created a ReplaceEntry 
         * by the time this object gets destroyed, and have no need for the allocated GroupDetails any more.
         */ 
	}

	virtual int groupCount() { return m_match.size(); }

    virtual GroupDetail* group(int groupNo);
    virtual GroupDetail* groupName(const char *groupName);

private: 
    const char *m_text;
    boost::match_results<UTF8Iterator> m_match;
    std::list<BoostRegexGroupDetail*> m_allocatedGroupDetails;

    static void deleteEntry(BoostRegexGroupDetail*);
};

void BoostRegexMatch::deleteEntry(BoostRegexGroupDetail* entry)
{
    delete entry;
}

BoostRegexMatch::~BoostRegexMatch() 
{
    for_each(m_allocatedGroupDetails.begin(), m_allocatedGroupDetails.end(), deleteEntry);
}

GroupDetail* BoostRegexMatch::group(int groupNo) 
{
    // TODO: Add to list of groupDetails to delete
    BoostRegexGroupDetail* groupDetail = new BoostRegexGroupDetail(m_match[groupNo]);
    m_allocatedGroupDetails.push_back(groupDetail);
    return groupDetail;
}

GroupDetail* BoostRegexMatch::groupName(const char *groupName) 
{
    u32string groupNameU32 = toStringType<u32string>(ConstString<char>(groupName));
    BoostRegexGroupDetail* groupDetail =  new BoostRegexGroupDetail(m_match[groupNameU32.c_str()]);
    m_allocatedGroupDetails.push_back(groupDetail);
    return groupDetail;
}



bool NppPythonScript::Replacer::startReplace(const char *text, const int textLength, const char *search, 
	matchConverter converter,
	std::list<std::shared_ptr<ReplaceEntry> > &replacements) {
    
    u32_regex r = u32_regex(toStringType<u32string>(ConstString<char>(search)));
    UTF8Iterator start(text, 0, textLength);
    UTF8Iterator end(text, textLength, textLength);
    u32_regex_iterator iteratorEnd;
    for(u32_regex_iterator it(start, end, r); it != iteratorEnd; ++it) {
        boost::match_results<UTF8Iterator> boost_match_results(*it);

        BoostRegexMatch *match = new BoostRegexMatch(text, &boost_match_results);

        // TODO: This assignment from ReplaceEntry(&replaceEntry) needs to copy, it copies the replacement text
        ReplaceEntry* entry = new ReplaceEntry(converter(text, match));

        replacements.push_back(std::shared_ptr<ReplaceEntry>(entry));
	}

    return false;
}

}
#endif 
