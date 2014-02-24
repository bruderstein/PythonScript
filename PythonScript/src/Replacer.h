#ifndef REPLACER_20140209_H
#define REPLACER_20140209_H
#include "ReplaceEntry.h"
#include "Match.h"
#include "UTF8Iterator.h"
#include "ANSIIterator.h"
#include "ConstString.h"
#include "UtfConversion.h"

namespace NppPythonScript 
{
    typedef ReplaceEntry* (*matchConverter)(const char *, Match *, void *state);
    typedef bool (*searchResultHandler)(const char *, Match *, void *state);
    
    using UtfConversion::toStringType;



    typedef enum _python_re_flags {
        python_re_flag_normal = 0,
		python_re_flag_ignorecase = 2,
        python_re_flag_locale = 4,
        python_re_flag_multiline = 8,
        python_re_flag_dotall = 16,
        python_re_flag_wholedoc = 0x10000000,    // This flag is the opposite of python_re_flag_multiline, which is the default
                                                 // Multiline is always true, except when this flag is specified
        // Internal flags
        python_re_flag_literal = 0x80000000
	} python_re_flags;


    
template <class CharTraitsT>
class BoostRegexGroupDetail : public GroupDetail
{
public:
    BoostRegexGroupDetail(const boost::sub_match<typename CharTraitsT::text_iterator_type>& subMatch)
		: m_subMatch(subMatch)
	{}
	int start() const { return  m_subMatch.first.pos(); }
	int end() const { return m_subMatch.second.pos(); }
	bool matched() const { return m_subMatch.matched; }

private:
    boost::sub_match<typename CharTraitsT::text_iterator_type> m_subMatch;
};

template<class CharTraitsT>
class BoostRegexMatch : public Match
{
public:
    BoostRegexMatch(const char *text, boost::match_results<typename CharTraitsT::text_iterator_type>* match)
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
    
	void setMatchResults(boost::match_results<typename CharTraitsT::text_iterator_type>* match) { m_match = match; }

	virtual int groupCount() { return m_match->size(); }

    virtual GroupDetail* group(int groupNo);
    virtual GroupDetail* groupName(const char *groupName);

    virtual std::string getTextForGroup(GroupDetail* group);

    virtual void expand(const char* format, char **result, int *resultLength);

    virtual int groupIndexFromName(const char *groupName);

private: 
    const char *m_text;
    boost::match_results<typename CharTraitsT::text_iterator_type>* m_match;
    std::list<BoostRegexGroupDetail<CharTraitsT>* > m_allocatedGroupDetails;

    static void deleteEntry(BoostRegexGroupDetail<CharTraitsT>*);
};



template <class CharTraitsT>
void BoostRegexMatch<CharTraitsT>::deleteEntry(BoostRegexGroupDetail<CharTraitsT>* entry)
{
    delete entry;
}

template <class CharTraitsT>
BoostRegexMatch<CharTraitsT>::~BoostRegexMatch() 
{
    for_each(m_allocatedGroupDetails.begin(), m_allocatedGroupDetails.end(), deleteEntry);
}

template <class CharTraitsT>
GroupDetail* BoostRegexMatch<CharTraitsT>::group(int groupNo) 
{
    if (groupNo < 0 || groupNo >= static_cast<int>(m_match->size()))
	{
        return NULL;
	}

    BoostRegexGroupDetail<CharTraitsT>* groupDetail = new BoostRegexGroupDetail<CharTraitsT>((*m_match)[groupNo]);
    m_allocatedGroupDetails.push_back(groupDetail);
    return groupDetail;
}

template <class CharTraitsT>
GroupDetail* BoostRegexMatch<CharTraitsT>::groupName(const char *groupName) 
{
    CharTraitsT::string_type groupNameU32 = toStringType<CharTraitsT::string_type>(ConstString<char>(groupName));
    int groupIndex = m_match->named_subexpression_index(groupNameU32.c_str(), groupNameU32.c_str() + groupNameU32.size());
    return group(groupIndex);
}

template <class CharTraitsT>
int BoostRegexMatch<CharTraitsT>::groupIndexFromName(const char *groupName)
{   
    CharTraitsT::string_type groupNameU32 = toStringType<CharTraitsT::string_type>(ConstString<char>(groupName));
    return m_match->named_subexpression_index(groupNameU32.c_str(), groupNameU32.c_str() + groupNameU32.size());
}

template <class CharTraitsT>
void BoostRegexMatch<CharTraitsT>::expand(const char *format, char **result, int *resultLength)
{
    CharTraitsT::string_type resultString = m_match->format(format);

    std::string charResult(CharTraitsT::toCharString(resultString));

    *resultLength = charResult.size();
    *result = new char[(*resultLength) + 1];
    memcpy(*result, charResult.c_str(), *resultLength);
    (*result)[*resultLength] = '\0';
}

template <class CharTraitsT>
typename std::string BoostRegexMatch<CharTraitsT>::getTextForGroup(GroupDetail* groupDetail)
{
    return std::string(m_text + groupDetail->start(), m_text + groupDetail->end());
}

    class Utf8CharTraits {
	public:
        typedef std::basic_string<U32> string_type;
        typedef boost::basic_regex<U32, u32_regex_traits> regex_type;
        typedef boost::regex_iterator<UTF8Iterator, U32, u32_regex_traits> regex_iterator_type;
        typedef UTF8Iterator text_iterator_type;

		static std::basic_string<char> toCharString(const string_type& source) { 
            // TODO:  There's probably more copying, allocing and deleting going on here than there actually needs to be
            // We just want a u32string to utf8 char*
		    return std::basic_string<char>(UtfConversion::toUtf8(ConstString<U32>(source)));
		}
	};

    class AnsiCharTraits {
	public:
        typedef std::string string_type;
        typedef boost::regex regex_type;
        typedef boost::regex_iterator<ANSIIterator, char> regex_iterator_type;
        typedef ANSIIterator text_iterator_type;

        static std::basic_string<char> toCharString(const string_type& source) { 
		    return source;
		}
	};

    template <class CharTraitsT>
	class Replacer {

	public:
		Replacer()
		{ }


        bool startReplace(const char *text, const int textLength, int maxCount, const int startPosition, const char *search, matchConverter converter, void *converterState, python_re_flags flags, std::list<ReplaceEntry*>& replacements);
        bool startReplace(const char *text, const int textLength, int maxCount, const int startPosition, const char *search, const char *replace, python_re_flags flags, std::list<ReplaceEntry*>& replacements);

        void search(const char *text, const int textLength, const int startPosition, int maxCount, const char *search, searchResultHandler resultHandler, void *resultHandlerState, python_re_flags flags);

	private:
        static ReplaceEntry* matchToReplaceEntry(const char *text, Match *match, void *state);

        boost::regex_constants::match_flag_type getMatchFlags(python_re_flags flags);
        boost::regex_constants::syntax_option_type getSyntaxFlags(python_re_flags flags); 

        const char *m_replaceFormat;
	};

    

template<class CharTraitsT>
ReplaceEntry* Replacer<CharTraitsT>::matchToReplaceEntry(const char * /* text */, Match *match, void *state)
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
bool Replacer<CharTraitsT>::startReplace(const char *text, const int textLength, const int startPosition, 
    int maxCount,
	const char *search,
    const char *replace, 
    python_re_flags flags,
    std::list<ReplaceEntry*>& replacements)
{
    m_replaceFormat = replace;
    return startReplace(text, textLength, startPosition, maxCount, search, matchToReplaceEntry, this, flags, replacements);
}

template<class CharTraitsT>
boost::regex_constants::match_flag_type Replacer<CharTraitsT>::getMatchFlags(python_re_flags flags)
{
    boost::regex_constants::match_flag_type resultBoostFlags = boost::regex_constants::match_default;

    // If we've not got the dotall flag, we want to add the match_not_dot_newline.  I love negative based flags. grrr.
    if (0 == (flags & python_re_flag_dotall)) 
	{
        resultBoostFlags |= boost::regex_constants::match_not_dot_newline;
	}

    if (flags & python_re_flag_wholedoc)
	{
        resultBoostFlags |= boost::regex_constants::match_single_line;
	}

    return resultBoostFlags;
}


template<class CharTraitsT>
boost::regex_constants::syntax_option_type Replacer<CharTraitsT>::getSyntaxFlags(python_re_flags flags)
{
    boost::regex_constants::syntax_option_type resultBoostFlags;

    if (flags & python_re_flag_literal) 
	{
	    resultBoostFlags = boost::regex_constants::literal;
	}
	else
	{
		resultBoostFlags = boost::regex_constants::normal;
	}

    if (flags & python_re_flag_ignorecase)
	{
        resultBoostFlags |= boost::regex_constants::icase;
	}

    return resultBoostFlags;
}

template<class CharTraitsT>
bool Replacer<CharTraitsT>::startReplace(const char *text, const int textLength, 
	const int startPosition, 
    int maxCount,
	const char *search, 
	matchConverter converter,
    void *converterState,
    python_re_flags flags,
	std::list<ReplaceEntry*> &replacements) 
{

    boost::regex_constants::syntax_option_type syntax_flags = getSyntaxFlags(flags);

    CharTraitsT::regex_type r = CharTraitsT::regex_type(toStringType<CharTraitsT::string_type>(ConstString<char>(search)), syntax_flags);

    CharTraitsT::text_iterator_type start(text, startPosition, textLength);
    CharTraitsT::text_iterator_type end(text, textLength, textLength);
    CharTraitsT::regex_iterator_type iteratorEnd;
    BoostRegexMatch<CharTraitsT> match(text);

    bool checkCountOfReplaces = false;
    if (maxCount > 0) 
	{
        checkCountOfReplaces = true;
	}



    for(CharTraitsT::regex_iterator_type it(start, end, r, getMatchFlags(flags)); it != iteratorEnd; ++it) 
	{
        boost::match_results<CharTraitsT::text_iterator_type> boost_match_results(*it);

        match.setMatchResults(&boost_match_results); 
        ReplaceEntry* entry = converter(text, &match, converterState);
        replacements.push_back(entry);
        if (checkCountOfReplaces && 0 == --maxCount) 
		{
            break;
		}
	}

    return false;
}


template<class CharTraitsT>
void Replacer<CharTraitsT>::search(const char *text, const int textLength, 
	const int startPosition, 
    int maxCount,
	const char *search, 
	searchResultHandler resultHandler,
    void *resultHandlerState,
    python_re_flags flags) 
{

    boost::regex_constants::syntax_option_type syntax_flags = getSyntaxFlags(flags);

    CharTraitsT::regex_type r = CharTraitsT::regex_type(toStringType<CharTraitsT::string_type>(ConstString<char>(search)), syntax_flags);

    CharTraitsT::text_iterator_type start(text, startPosition, textLength);
    CharTraitsT::text_iterator_type end(text, textLength, textLength);
    CharTraitsT::regex_iterator_type iteratorEnd;
    BoostRegexMatch<CharTraitsT> match(text);

    bool checkCountOfSearches = false;
    if (maxCount > 0) 
	{
        checkCountOfSearches = true;
	}

    for(CharTraitsT::regex_iterator_type it(start, end, r, getMatchFlags(flags)); it != iteratorEnd; ++it) 
	{
        boost::match_results<CharTraitsT::text_iterator_type> boost_match_results(*it);

        match.setMatchResults(&boost_match_results); 
        bool shouldContinue = resultHandler(text, &match, resultHandlerState);
        if (!shouldContinue || (checkCountOfSearches && 0 == --maxCount)) 
		{
            break;
		}
	}

}

}

#endif // REPLACER_20140209_H   