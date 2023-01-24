//
// Binding Python/Swig pour accéder aux services de PythonScriptingTest
//


%module PythonScriptingTest
%{
#include "PythonScriptingTest.h"
using namespace PythonScriptingTest;
%}  // module PythonScriptingTest

%include PythonScriptingTest.h
