#ifndef REPLACEMENTCONTAINER_H_2340149124091
#define REPLACEMENTCONTAINER_H_2340149124091 

namespace NppPythonScript
{
class ReplaceEntry;
class ScintillaWrapper;

class ReplacementContainer
{
public:
    ReplacementContainer(std::list<ReplaceEntry*>* replacements, ScintillaWrapper* scintillaWrapper)
		: m_replacements(replacements),
          m_scintillaWrapper(scintillaWrapper)
	{}

    void runReplacements();

private:
    std::list<ReplaceEntry*> *m_replacements;
    ScintillaWrapper *m_scintillaWrapper;
};


}

#endif // REPLACEMENTCONTAINER_H_2340149124091