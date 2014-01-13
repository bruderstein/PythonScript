#include "ReplaceEntry.h"
#include "Match.h"

namespace NppPythonScript 
{
    typedef ReplaceEntry (*matchConverter)(const char *, Match *);

	class Replacer {

	public:
		Replacer() { }

        bool startReplace(const char *text, const int textLength, const char *search, matchConverter converter, std::list<std::shared_ptr<ReplaceEntry> > &replacements);
	};
}