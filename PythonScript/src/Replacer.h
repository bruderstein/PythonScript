#include "ReplaceEntry.h"
#include "Match.h"
#include "UTF8Iterator.h"
#include "ANSIIterator.h"
#include "ConstString.h"
#include "UtfConversion.h"

namespace NppPythonScript 
{
    typedef ReplaceEntry* (*matchConverter)(const char *, Match *, void *state);
    
    using UtfConversion::toStringType;

    typedef boost::basic_regex<U32, u32_regex_traits> u32_regex;
    typedef boost::regex_iterator<UTF8Iterator, U32, u32_regex_traits> u32_regex_iterator;


    
template <class BidiIterator>
class BoostRegexGroupDetail : public GroupDetail
{
public:
    BoostRegexGroupDetail(const boost::sub_match<BidiIterator>& subMatch)
		: m_subMatch(subMatch)
	{}
	int start() const { return  m_subMatch.first.pos(); }
	int end() const { return m_subMatch.second.pos(); }

private:
    boost::sub_match<BidiIterator> m_subMatch;
};

template<class BidiIterator>
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
    
	void setMatchResults(boost::match_results<BidiIterator>* match) { m_match = match; }

	virtual int groupCount() { return m_match->size(); }

    virtual GroupDetail* group(int groupNo);
    virtual GroupDetail* groupName(const char *groupName);
    virtual void expand(const char* format, char **result, int *resultLength);

private: 
    const char *m_text;
    boost::match_results<BidiIterator>* m_match;
    std::list<BoostRegexGroupDetail<BidiIterator>* > m_allocatedGroupDetails;

    static void deleteEntry(BoostRegexGroupDetail<BidiIterator>*);
};



template <class BidiIterator>
void BoostRegexMatch<BidiIterator>::deleteEntry(BoostRegexGroupDetail<BidiIterator>* entry)
{
    delete entry;
}

template <class BidiIterator>
BoostRegexMatch<BidiIterator>::~BoostRegexMatch() 
{
    for_each(m_allocatedGroupDetails.begin(), m_allocatedGroupDetails.end(), deleteEntry);
}

template <class BidiIterator>
GroupDetail* BoostRegexMatch<BidiIterator>::group(int groupNo) 
{
    BoostRegexGroupDetail<BidiIterator>* groupDetail = new BoostRegexGroupDetail<BidiIterator>((*m_match)[groupNo]);
    m_allocatedGroupDetails.push_back(groupDetail);
    return groupDetail;
}

template <class BidiIterator>
GroupDetail* BoostRegexMatch<BidiIterator>::groupName(const char *groupName) 
{
    u32string groupNameU32 = toStringType<u32string>(ConstString<char>(groupName));
    BoostRegexGroupDetail<BidiIterator>* groupDetail =  new BoostRegexGroupDetail<BidiIterator>((*m_match)[groupNameU32.c_str()]);
    m_allocatedGroupDetails.push_back(groupDetail);
    return groupDetail;
}

template <class BidiIterator>
void BoostRegexMatch<BidiIterator>::expand(const char *format, char **result, int *resultLength)
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


    class Utf8CharTraits {
	public:
        typedef u32string string_type;
        typedef u32_regex regex_type;
        typedef u32_regex_iterator regex_iterator_type;
        typedef UTF8Iterator text_iterator_type;
	};

    class AnsiCharTraits {
	public:
        typedef std::string string_type;
        typedef boost::regex regex_type;
        typedef boost::regex_iterator<ANSIIterator, char> regex_iterator_type;
        typedef ANSIIterator text_iterator_type;
	};

    template <class CharTraitsT>
	class Replacer {

	public:
		Replacer() { }

        bool startReplace(const char *text, const int textLength, const char *search, matchConverter converter, void *converterState, std::list<ReplaceEntry*>& replacements);
        bool startReplace(const char *text, const int textLength, const char *search, const char *replace, std::list<ReplaceEntry*>& replacements);

	private:
        static ReplaceEntry* matchToReplaceEntry(const char *text, Match *match, void *state);

        const char *m_replaceFormat;
	};

    
template<class CharTraitsT>
ReplaceEntry* NppPythonScript::Replacer<CharTraitsT>::matchToReplaceEntry(const char * /* text */, Match *match, void *state)
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

template<class CharTraitsT>
bool NppPythonScript::Replacer<CharTraitsT>::startReplace(const char *text, const int textLength, const char *search,
    const char *replace, 
    std::list<ReplaceEntry*>& replacements)
{
    m_replaceFormat = replace;
    return startReplace(text, textLength, search, matchToReplaceEntry, this, replacements);
}

template<class CharTraitsT>
bool NppPythonScript::Replacer<CharTraitsT>::startReplace(const char *text, const int textLength, const char *search, 
	matchConverter converter,
    void *converterState,
	std::list<ReplaceEntry*> &replacements) {
    CharTraitsT::regex_type r = CharTraitsT::regex_type(toStringType<CharTraitsT::string_type>(ConstString<char>(search)));

    CharTraitsT::text_iterator_type start(text, 0, textLength);
    CharTraitsT::text_iterator_type end(text, textLength, textLength);
    CharTraitsT::regex_iterator_type iteratorEnd;
    BoostRegexMatch<CharTraitsT::text_iterator_type> match(text);
    for(CharTraitsT::regex_iterator_type it(start, end, r); it != iteratorEnd; ++it) {
        boost::match_results<CharTraitsT::text_iterator_type> boost_match_results(*it);

        match.setMatchResults(&boost_match_results); 
        ReplaceEntry* entry = converter(text, &match, converterState);
        replacements.push_back(entry);
	}

    return false;
}

}