// Petit programme test d'exécution d'un script python.
//
// Exemple d'usage avec script_gen fournit conjointement :
// script_gen toto.py
// export PYTHONPATH=/dir/pythonutil/src/PythonScriptingTest:/install/lib/python2.7/site-packages
// src/tests/python_runner toto.py
#include <TkUtil/Exception.h>
#include "PythonScriptingTest.h"
#include "PythonUtil/PythonSession.h"

#include <iostream>


using namespace PythonScriptingTest;
USING_UTIL
USING_STD


int main (int argc, char* argv[], char* envp [])
{
	if (2 != argc)
	{
		cout << "Syntaxe : " << argv [0] << " filename.py" << endl;
		return -1;
	}	// if (2 != argc)

	try
	{
		PythonScriptingTestManager	manager;

		PythonSession	pySession;
		pySession.logOutputs (true);
		pySession.execFile (argv [1]);

		cout << "Sorties du script " << argv [1] << " :" << endl << endl;
		UTF8String	outputs (pySession.getOutputs ( ));
		const string	str	= outputs.trim ( ).iso ( );
		cout << str << endl;
		cout << endl << "Fin des sorties du script " << argv [1] << "." << endl;
	}
	catch (const Exception& exc)
	{
		cout << "Exception : " << exc.getFullMessage ( ) << endl;
	}
	catch (...)
	{
		cout << "Exception non documentée." << endl;
	}

	return 0;
}
