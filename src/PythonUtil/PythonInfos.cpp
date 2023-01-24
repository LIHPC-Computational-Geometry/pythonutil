#include "PythonUtil/PythonInfos.h"

#include <assert.h>


BEGIN_NAMESPACE_UTIL


const Version PythonInfos::_version (TK_PYTHON_UTIL_VERSION);


PythonInfos::PythonInfos ( )
{
	assert (0 && "PythonInfos::PythonInfos is not allowed.");
}


PythonInfos::PythonInfos (const PythonInfos&)
{
	assert (0 && "PythonInfos copy constructor is not allowed.");
}


PythonInfos& PythonInfos::operator = (const PythonInfos&)
{
	assert (0 && "PythonInfos operator = is not allowed.");
	return *this;
}


PythonInfos::~PythonInfos ( )
{
	assert (0 && "PythonInfos::~PythoPython is not allowed.");
}



END_NAMESPACE_UTIL
