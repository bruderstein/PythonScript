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
        virtual std::string getTextForGroup(GroupDetail* group) = 0;


        boost::python::str py_group_number(int groupNumber);
        boost::python::str py_group_name(boost::python::str groupName);
        boost::python::str py_expand(boost::python::object replaceFormat);
        boost::python::tuple py_groups();

        boost::python::tuple py_group_tuple2(boost::python::object group1, boost::python::object group2);
        boost::python::tuple py_group_tuple3(boost::python::object group1, boost::python::object group2, boost::python::object group3);
        boost::python::tuple py_group_tuple4(boost::python::object group1, boost::python::object group2, boost::python::object group3, boost::python::object group4);

	private:
        boost::python::str getGroup(boost::python::object groupIdentifier);

	};

    

}


#endif // MATCH_H_123091238901