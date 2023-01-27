// Petit programme test de génération d'un script python.
#include "PythonUtil/PythonLogOutputStream.h"
#include "TkUtil/Exception.h"
#include "TkUtil/Process.h"
#include "TkUtil/UserData.h"

#include <iostream>


USING_UTIL
USING_STD


int main (int argc, char* argv[], char* envp [])
{
	try
	{
		Process::initialize (argc, argv, envp);
		Process::setCurrentSoftware ("Hello", Version ("1.0.0"));
		PythonLogOutputStream	script ("output.txt", Charset (Charset::UTF_8));
		script.writeHeader ( );
		ScriptingLog	i1 ("", "print", "", UTF8String ("Un petit bonjour", Charset (Charset::UTF_8)));
		i1 << "hello" << UserData ( ).getName ( );
		script.log (i1);
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
