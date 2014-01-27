#ifndef MATCH_H_123091238901
#define MATCH_H_123091238901

namespace NppPythonScript
{
	
	class GroupDetail 
	{
	public:
        virtual int start() const = 0;
        virtual int end() const = 0;
	
	};

    class Match
	{
	public:
        virtual int groupCount() = 0;
        virtual GroupDetail* group(int groupNumber) = 0;
        virtual GroupDetail* groupName(const char *groupName) = 0;
        virtual void expand(const char* format, char **result, int *resultLength) = 0;
	};

    

}


#endif // MATCH_H_123091238901