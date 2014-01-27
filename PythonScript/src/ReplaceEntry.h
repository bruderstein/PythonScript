#ifndef REPLACEENTRY_H_DGB_20131120
#define REPLACEENTRY_H_DGB_20131120

namespace NppPythonScript 
{

class ReplaceEntry {
    
public:
    ReplaceEntry(int start, int length, const char* replacement, int replacementLength)
		: m_start(start),
          m_length(length),
          m_replacementLength(replacementLength)
	{ 
        m_replacement = new char[replacementLength + 1];
        memcpy(m_replacement, replacement, replacementLength);
		m_replacement[replacementLength] = '\0';
	}

    ReplaceEntry(const ReplaceEntry& copy) 
		: m_start(copy.m_start),
          m_length(copy.m_length),
          m_replacementLength(copy.m_replacementLength)
	{
        m_replacement = new char[m_replacementLength];
        memcpy(m_replacement, copy.m_replacement, m_replacementLength);
	}

    ~ReplaceEntry() {
        delete[] m_replacement;
	}

	int getStart() { return m_start; }
	int getLength() { return m_length; }
	char *getReplacement() { return m_replacement; }
	int getReplacementLength() { return m_replacementLength; }

private:
    int m_start;
    int m_length;
    int m_replacementLength;
    char *m_replacement;



};

}
#endif