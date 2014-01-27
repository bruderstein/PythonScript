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
    BoostRegexGroupDetail(const boost::sub_match<UTF8Iterator>& subMatch)
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
          m_match(match)
	{}

    BoostRegexMatch(const char *text)
		: m_text(text),
          m_match(NULL)
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
    
	void setMatchResults(boost::match_results<UTF8Iterator>* match) { m_match = match; }

	virtual int groupCount() { return m_match->size(); }

    virtual GroupDetail* group(int groupNo);
    virtual GroupDetail* groupName(const char *groupName);
    virtual void expand(const char* format, char **result, int *resultLength);

private: 
    const char *m_text;
    boost::match_results<UTF8Iterator>* m_match;
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
    BoostRegexGroupDetail* groupDetail = new BoostRegexGroupDetail((*m_match)[groupNo]);
    m_allocatedGroupDetails.push_back(groupDetail);
    return groupDetail;
}

GroupDetail* BoostRegexMatch::groupName(const char *groupName) 
{
    u32string groupNameU32 = toStringType<u32string>(ConstString<char>(groupName));
    BoostRegexGroupDetail* groupDetail =  new BoostRegexGroupDetail((*m_match)[groupNameU32.c_str()]);
    m_allocatedGroupDetails.push_back(groupDetail);
    return groupDetail;
}

void BoostRegexMatch::expand(const char *format, char **result, int *resultLength)
{
    u32string resultString = m_match->format(format);

    // TODO:  There's probably more copying, allocing and deleting going on here than there actually needs to be
    // We just want a u32string to utf8 char*
    u8string utf8result(UtfConversion::toUtf8(ConstString<U32>(resultString)));

    *resultLength = utf8result.size();
    *result = new char[(*resultLength) + 1];
    memcpy(*result, utf8result.c_str(), *resultLength);
    (*result)[*resultLength] = '\0';
}


ReplaceEntry* NppPythonScript::Replacer::matchToReplaceEntry(const char * /* text */, Match *match, void *state)
{
    // TODO: state is replacer instance, and contains the replacement string
	// need to add format call in here, 
    Replacer *replacer = reinterpret_cast<Replacer*>(state);
    char *replacement;
    int replacementLength;
	match->expand(replacer->m_replaceFormat, &replacement, &replacementLength);

    GroupDetail *fullMatch = match->group(0);
    ReplaceEntry* replaceEntry = new ReplaceEntry(fullMatch->start(), fullMatch->end(), replacement, replacementLength);
    delete [] replacement;
    return replaceEntry;
}

bool NppPythonScript::Replacer::startReplace(const char *text, const int textLength, const char *search,
    const char *replace, 
    std::list<ReplaceEntry*>& replacements)
{
    m_replaceFormat = replace;
    return startReplace(text, textLength, search, matchToReplaceEntry, this, replacements);
}

bool NppPythonScript::Replacer::startReplace(const char *text, const int textLength, const char *search, 
	matchConverter converter,
    void *converterState,
	std::list<ReplaceEntry*> &replacements) {
    
    u32_regex r = u32_regex(toStringType<u32string>(ConstString<char>(search)));
    UTF8Iterator start(text, 0, textLength);
    UTF8Iterator end(text, textLength, textLength);
    u32_regex_iterator iteratorEnd;
    BoostRegexMatch match(text);
    for(u32_regex_iterator it(start, end, r); it != iteratorEnd; ++it) {
        boost::match_results<UTF8Iterator> boost_match_results(*it);

        match.setMatchResults(&boost_match_results); 
        ReplaceEntry* entry = converter(text, &match, converterState);
        replacements.push_back(entry);
	}

    return false;
}

}
#endif 
