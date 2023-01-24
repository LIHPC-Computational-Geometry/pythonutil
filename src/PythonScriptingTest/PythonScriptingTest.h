/**
 * Classes tests pour l'API de scripting reposant sur python.
 */

#ifndef PYTHON_SCRIPTING_TESTS_H
#define PYTHON_SCRIPTING_TESTS_H

#include <TkUtil/ReferencedObject.h>
#include "PythonUtil/PythonLogOutputStream.h"


#ifndef SWIG
namespace PythonScriptingTest
{
#endif	// #ifndef SWIG

class DefaultComponent;
class MainContainer;

/**
 * A appeler pour initialiser l'API lors de l'exécution d'un script (initialisation SWIG ...).
 */
void initAppScripting ( );

void test1 (const char* name, long l, double d, bool b);
long test2 (const char* name, long l, double d, bool b);

/**
 * Le manager d'objets spécialisé pour cette API.
 */
class PythonScriptingTestManager : public IN_UTIL ReferencedObjectManager
{
	public :

	PythonScriptingTestManager ( );
	virtual ~PythonScriptingTestManager ( );

#ifndef SWIG
	MainContainer* getContainerInstance (const IN_STD string& name);
#else	// #ifndef SWIG
	MainContainer* getContainerInstance (const char* name);
#endif	// #ifndef SWIG

#ifndef SWIG
	DefaultComponent* getDCInstance (const IN_STD string& name);
#else	// #ifndef SWIG
	DefaultComponent* getDCInstance (const char* name);
#endif	// #ifndef SWIG


	private :

	PythonScriptingTestManager (const PythonScriptingTestManager&);
	PythonScriptingTestManager& operator = (const PythonScriptingTestManager&);
};	// class PythonScriptingTestManager


extern PythonScriptingTestManager& getPythonScriptingTestManager ( );


/**
 * L'outil d'écriture des scripts reposant sur Python et sur l'API
 * PythonScriptingTestManager.
 */
class PythonScriptingTestWriter : public IN_UTIL PythonLogOutputStream
{
	public :

	/**
	 * @param		Nom du fichier script utilisé.
	 * @param		Jeu de caractères utilisé pour l'encodage
	 * @param		Masque appliqué.
	 * @param		true si les dates doivent être affichées
	 * @param		true si les heures doivent être affichées
	 */
	PythonScriptingTestWriter (
			const IN_UTIL UTF8String& fileName, const IN_UTIL Charset& charset,
			IN_UTIL Log::TYPE mask = IN_UTIL Log::SCRIPTING, bool enableDate = true, bool enableTime = true);

	/**
	 * Destructeur. RAS.
	 */
	virtual ~PythonScriptingTestWriter ( );


	protected :

	/**
	 * Utilise getPythonScriptingTestManager, getContainerInstance et
	 * getDCInstance.
	 */
	virtual IN_STD string formatInstanceAccess(const IN_UTIL ScriptingLog& log);

	/**
	 * Utilise getPythonScriptingTestManager, getContainerInstance et
	 * getDCInstance.
	 */
	virtual IN_STD string formatInstanceVariableAccess (const IN_STD string& uniqueName);


	private :

	PythonScriptingTestWriter (const PythonScriptingTestWriter&);
	PythonScriptingTestWriter& operator = (const PythonScriptingTestWriter&);
};	// class PythonScriptingTestWriter


/**
 * Conteneur racine contenant des classes dont les instances ont des noms
 * uniques. Les appels aux méthodes sont enregistrables dans un script, et
 * l'exécution du script permet de rejouer la séquence.
 */
class MainContainer : public IN_UTIL ReferencedNamedObject
{
	public :

	/**
	 * Constructeur.
	 * @param		Nom unique de l'instance.
	 */
#ifndef SWIG
	MainContainer (const IN_STD string& uniqueName);
#else	// #ifndef SWIG
	MainContainer (const char* uniqueName);
#endif	// #ifndef SWIG

	/**
	 * destructeur. RAS.
	 */
	virtual ~MainContainer ( );

	/**
	 * Ajoute à ses composants une instance dont le nom de la classe est
	 * tranmis en argument.
	 * @return		Un pointeur sur l'instance créée.
	 */
#ifndef SWIG
	virtual DefaultComponent* instanciate (const IN_STD string& className);
#else	// #ifndef SWIG
	virtual DefaultComponent* instanciate (const char* className);
#endif	// #ifndef SWIG

	/**
	 * @return		le composant dont le nom est transmis en argument.
	 */
#ifndef SWIG
	virtual DefaultComponent* getChild (const IN_STD string& name);
#else	// #ifndef SWIG
	virtual DefaultComponent* getChild (const char* name);
#endif	// #ifndef SWIG

	/**
	 * Affiche le nom des instances transmises en argument.
	 */
	virtual void f (DefaultComponent* c1, DefaultComponent* c2);


	private :

#ifndef SWIG
	MainContainer (const MainContainer&);
	MainContainer& operator = (const MainContainer&);
#endif	// #ifndef SWIG

	/** La liste des enfants de l'instance. */
	IN_STD vector<DefaultComponent*>	_children;
};	// class MainContainer


/**
 * Instancie le conteneur dont le nom unique est tranmis en argument.
 */
#ifndef SWIG
MainContainer* instanciateContainer (const IN_STD string& name);
#else	// #ifndef SWIG
MainContainer* instanciateContainer (const char* name);
#endif	// #ifndef SWIG


/**
 * Classe de "composant de base".
 */
class DefaultComponent : public IN_UTIL ReferencedNamedObject
{
	public :

	/**
	 * Constructeur.
	 * @param		Nom unique de l'instance.
	 */
#ifndef SWIG
	DefaultComponent (const IN_STD string& uniqueName);
#else	// #ifndef SWIG
	DefaultComponent (const char* uniqueName);
#endif	// #ifndef SWIG

	/**
	 * destructeur. RAS.
	 */
	virtual ~DefaultComponent ( );

	/**
	 * Méthodes sans grand intérêt.
	 */
	virtual void f1 ( );
	virtual void f2 (bool b);
#ifndef SWIG
	virtual void f3 (const IN_STD string& s, long l, double d);
#else	// #ifndef SWIG
	virtual void f3 (const char*, long l, double d);
#endif	// #ifndef SWIG


	private :

#ifndef SWIG
	DefaultComponent (const DefaultComponent&);
	DefaultComponent& operator = (const DefaultComponent&);
#endif	// #ifndef SWIG
};	// class DefaultComponent


/**
 * Classe de "composant spécialisé".
*/
class ComponentA : public DefaultComponent
{
	public :

	/**
	 * Constructeur.
	 * @param		Nom unique de l'instance.
	 */
#ifndef SWIG
	ComponentA (const IN_STD string& uniqueName);
#else	// #ifndef SWIG
	ComponentA (const const char* uniqueName);
#endif	// #ifndef SWIG

	/**
	 * destructeur. RAS.
	 */
	virtual ~ComponentA ( );

	/**
	 * Méthodes sans grand intérêt.
	 */
	virtual void f1 ( );
	virtual void f2 (bool v);
#ifndef SWIG
	virtual void f3 (const IN_STD string& s, long l, double d);
#else	// #ifndef SWIG
	virtual void f3 (const char* s, long l, double d);
#endif	// #ifndef SWIG


	private :

#ifndef SWIG
	ComponentA (const ComponentA&);
	ComponentA& operator = (const ComponentA&);
#endif	// #ifndef SWIG
};	// class ComponentA


/**
 * Classe de "composant spécialisé".
*/
class ComponentB : public DefaultComponent
{
	public :

	/**
	 * Constructeur.
	 * @param		Nom unique de l'instance.
	 */
#ifndef SWIG
	ComponentB (const IN_STD string& uniqueName);
#else	// #ifndef SWIG
	ComponentB (const char* uniqueName);
#endif	// #ifndef SWIG

	/**
	 * destructeur. RAS.
	 */
	virtual ~ComponentB ( );

	/**
	 * Méthodes sans grand intérêt.
	 */
	virtual void f1 ( );
	virtual void f2 (bool v);
#ifndef SWIG
	virtual void f3 (const IN_STD string& s, long l, double d);
#else	// #ifndef SWIG
	virtual void f3 (const char*, long l, double d);
#endif	// #ifndef SWIG


	private :

#ifndef SWIG
	ComponentB (const ComponentB&);
	ComponentB& operator = (const ComponentB&);
#endif	// #ifndef SWIG
};	// class ComponentB

#ifndef SWIG
}	// namespace PythonScriptingTest
#endif	// #ifndef SWIG


#endif	//PYTHON_SCRIPTING_TESTS_H
