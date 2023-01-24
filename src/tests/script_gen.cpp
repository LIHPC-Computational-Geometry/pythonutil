// Petit programme test de génération d'un script python dont l'exécution dépend d'une bibliothèque c++.
// ATTENTION : PATH DES BINDINGS A ADAPTER
#include "PythonUtil/PythonLogOutputStream.h"
#include <TkUtil/Exception.h>
#include <TkUtil/NumericConversions.h>
#include <TkUtil/Process.h>
#include <TkUtil/ScriptingTools.h>
#include "PythonScriptingTest.h"

#include <iostream>
#include <math.h>


USING_UTIL
USING_STD
using namespace PythonScriptingTest;

static const Charset	charset ("àéèùô");


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
		Process::initialize (argc, argv, envp);
		Process::setCurrentSoftware ("Python ScriptGEN", Version ("1.0.0"));
		PythonScriptingTestWriter	script (argv [1], Charset (Charset::UTF_8));
		script.recordInputFiles (true);
		script.addPackage ("TkUtilScripting");
		script.addPackage ("PythonScriptingTest");
		script.addPackagePath ("/tmp/TkUtil-5.7.0");		// Pipeau
		script.addPackagePath ("/tmp/PythonUtil-5.5.6");	// Pipeau
		script.addPackagePath (SHARED_UTIL_PATH_SO);		// Apporté à la compile, fixé par cmake
		script.addPackagePath (SHARED_PYTHON_TEST_PATH_SO);	// Apporté à la compile, fixé par cmake
		script.writeHeader ( );
		script.addBlankLine ( );
		script.addBlankLine ( );

		script.declareTryBlock ( );

		ScriptingLog	initLog ("", "initAppScripting", "", UTF8String ("Initialisation de l'API.", Charset (Charset::UTF_8)));
		script.log (initLog);
		
		// Petit test avec 2 fonctions très simples définies dans src/PythonScriptingTest/PythonScriptingTest.h :
		static const char*	str = "Coucou";
		static const long	l	= -3;
		static const double	d	= 0.25e-2;
		static const bool	b	= true;
		ScriptingLog		test1Log ("", "test1", "", UTF8String ("J'appelle void test1 (const char* name, long l, double d, bool b)", Charset (Charset::UTF_8)));
		test1Log << str << l << d << b;
		script.log (test1Log);
		ScriptingLog		test2Log ("", "test2", "retval", UTF8String ("J'appelle int test2 (const char* name, long l, double d, bool b) et affecte le résultat à retval", Charset (Charset::UTF_8)));
		test2Log << str << (-l) << (2*d) << (!b);
		script.log (test2Log);
		ScriptingLog		test2bLog ("", "print", "", UTF8String ("Affichage de la valeur de retour de test2", Charset (Charset::UTF_8)));
		test2bLog.addVariable ("retval");
		script.log (test2bLog);
		
		size_t	i	= 0;
		cout << "Instanciating main containers ..." << endl;
		vector<MainContainer*>	containers;
		for (i = 0; i < 5; i++)
		{
			UTF8String	name (charset);
			name << "Container_" << (unsigned long)i;
			MainContainer*	container	= instanciateContainer (name);
			containers.push_back (container);
			ScriptingLog	sl ("", "instanciateContainer", "", "Instanciation d'un\nconteneur principal");
			TaggedValue	tv ("CONTENEUR", name);
			tv.addAttribute ("MainContainer");
			tv.addAttribute (NumericConversions::toStr ((unsigned long)i));
			sl.addTag (tv);
			sl << name;
			script.log (sl);
		}	// for (i = 0; i < 5; i++)
		cout << "Main containers instancied." << endl;

		// Instanciation des composants :
		cout << "Container's components instanciation ..." << endl;
		for (vector<MainContainer*>::iterator it1 = containers.begin ( );
		     containers.end ( ) != it1; it1++)
		{
			ScriptingLog	slA (**it1,
				"instanciate", "", "Instanciation d'un composant de classe A");
			slA << "ComponentA";
			DefaultComponent*	c	= (*it1)->instanciate ("ComponentA");
			script.log (slA);
			ScriptingLog	slB (**it1,
				"instanciate", "", "Instanciation d'un composant de classe B");
			slB << "ComponentB";
			c	= (*it1)->instanciate ("ComponentB");
			getObjectManagerInstance ( ).registerObject (c);
			script.log (slB);
			// Le 'é' ne sera pas pris en compte dans le log ci-dessous, car
			// passe par le constructeur UTF8String sans charset.
			ScriptingLog	slDC (**it1,
				"instanciate",  "", "Instanciation d'un composant par défaut");
			slDC << "DefaultComponent";
			c	= (*it1)->instanciate ("DefaultComponent");
			getObjectManagerInstance ( ).registerObject (c);
			script.log (slDC);
		}	// for (vector<MainContainer*>::iterator it1 = containers.begin ( );

		cout << "Container's components instanciated." << endl;

		cout << endl << "Method calls ..." << endl;
		i	= 0;
		for (vector<MainContainer*>::iterator	it2 = containers.begin ( );
		     it2 != containers.end ( ); it2++, i++)
		{
			UTF8String	uniqueName (charset), methodName (charset);
			static	bool	b		= true;
			static	double	d		= 2.;
			static	long	l		= 2;
			static	string	s;
			uniqueName << (*it2)->getName ( ) << ".instance_"
			           << (unsigned long)(i % 3);
			switch (i % 3)
			{
				case	0	: methodName << "f1";
					break;
				case	1	: methodName << "f2";	b	= !b;
					break;
				default		: methodName << "f3";	s	+= (char)('a' + i);
					d	= sqrt (d);		l	*= l;
			}	// switch (i % 3)
			ReferencedNamedObject*	o	=
						getObjectManagerInstance ( ).getInstance (uniqueName);
			assert (0 != o);
			UTF8String	comment (charset);
			comment << "Invocation de la méthode " << methodName
			        << " de l'instance " << uniqueName << ".";
			ScriptingLog	slMC (*o, methodName,  "", comment);
			if (1 == (i % 3))
				slMC << b;
			else if (2 == (i % 3))
				slMC << s << l << d;
			script.log (slMC);
		}	// for (vector<MainContainer*>::iterator it2 = containers.begin ( );
		cout << endl << "End of method calls ..." << endl;

		// Invocations de MainContainer.f () :
		MainContainer*	container0	= containers [0];
		UTF8String	comment (charset);
		comment << "Invocation de la méthode f de l'instance "
		        << container0->getUniqueName ( ) << " avec deux instances.";
		ScriptingLog	s1 (*container0, "f",  "", comment);
		s1 << container0->getChild ("Container_0.instance_0")
		   << container0->getChild ("Container_0.instance_1");
		container0->f (
					container0->getChild ("Container_0.instance_0"),
					container0->getChild ("Container_0.instance_1"));
		// Pour test on insère un fichier en entrée :	
/*
		s1.addTag (ScriptingTools::inputFileTag, "Makefile");
		s1.addTag (ScriptingTools::inputFileTag, "/tmp/python_util/src/tests/python_runner.cpp");
		script.log (s1);
		container0->f (0, container0->getChild ("Container_0.instance_1"));
		comment.clear ( );
		comment << "Invocation de la méthode f de l'instance "
		        << container0->getUniqueName ( ) << " avec une instance nulle.";
		ScriptingLog	s2 (*container0, "f",  "", comment);
		s2 << (DefaultComponent*)0 
		   << container0->getChild ("Container_0.instance_1");
		s2.addTag (ScriptingTools::inputFileTag, "/tmp/python_util/src/tests/hello.cpp");
		script.log (s2);
*/

		script.addBlankLine ( );
		script.closeUtilExcTryBlock ( );

		// Destructions :
		cout << "Destroying main containers ..." << endl;
		for (vector<MainContainer*>::iterator it = containers.begin ( );
		     containers.end ( ) != it; it++)
			delete *it;
		containers.clear ( );
		cout << "Main containers destroyed." << endl;
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
