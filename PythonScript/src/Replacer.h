#include "ReplaceEntry.h"
#include "Match.h"

namespace NppPythonScript 
{
    typedef ReplaceEntry* (*matchConverter)(const char *, Match *, void *state);

	class Replacer {

	public:
		Replacer() { }

        bool startReplace(const char *text, const int textLength, const char *search, matchConverter converter, void *converterState, std::list<ReplaceEntry*>& replacements);
        bool startReplace(const char *text, const int textLength, const char *search, const char *replace, std::list<ReplaceEntry*>& replacements);

	private:
        static ReplaceEntry* matchToReplaceEntry(const char *text, Match *match, void *state);

        const char *m_replaceFormat;
	};
}