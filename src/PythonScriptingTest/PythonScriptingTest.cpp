#include "PythonScriptingTest.h"
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>
#include <TkUtil/ThreadManager.h>

#include <patchlevel.h>	// PY_VERSION_HEX
#include <iostream>
#include <assert.h>


USING_UTIL
USING_STD


static const Charset	charset ("àéèùô");


extern "C"
{       // Générés par les bindings swig de TkUtil.
#if PY_VERSION_HEX >= 0x03000000
	void PyInit__TkUtilScripting ( );
#else
	void init_TkUtilScripting ( );
#endif	// #if PY_VERSION_HEX >= 0x03000000
}

namespace PythonScriptingTest
{

void initAppScripting ( )
{
	static bool	inited	= false;
	
cout << "initAppScripting called." << endl;
	if (false == inited)
	{
		try
		{
			ThreadManager::initialize (8);		// Inutile ici, juste pour l'exemple
			new PythonScriptingTestManager ( );	// Nécessaire si l'on utilise la gestion d'instances
		}
		catch (...)	// Already inited (in Lem gui) ?
		{
		}
cout << "CALLING init_TkUtilScripting ..." << endl;
#if PY_VERSION_HEX >= 0x03000000
	PyInit__TkUtilScripting ( );
#else
		init_TkUtilScripting ( );
#endif	// #if PY_VERSION_HEX >= 0x03000000
cout << "Binding initializations done." << endl;
		inited	= true;
	}	// if (false == inited)
cout << "initAppScripting done." << endl;
}	// initAppScripting


void test1 (const char* name, long l, double d, bool b)
{
	cout << "test1 (" << name << ", " << l << ", " << d << ", " << (true == b ? "true" : "false") << ")" << endl;
}	// test1


long test2 (const char* name, long l, double d, bool b)
{
	static const long	ret	= 5;
	cout << "test2 (" << name << ", " << l << ", " << d << ", " << (true == b ? "true" : "false") << ")" << " RETVAL=" << ret << endl;
	return ret;
}	// test2


// ============================================================================
//                   LA CLASSE PythonScriptingTestManager
// ============================================================================

PythonScriptingTestManager::PythonScriptingTestManager ( )
	: ReferencedObjectManager ( )
{
}	// PythonScriptingTestManager::PythonScriptingTestManager


PythonScriptingTestManager::PythonScriptingTestManager (const PythonScriptingTestManager&)
	: ReferencedObjectManager ( )
{
	assert (0 && "PythonScriptingTestManager copy constructor is not allowed.");
}	// PythonScriptingTestManager::PythonScriptingTestManager


PythonScriptingTestManager& PythonScriptingTestManager::operator = (const PythonScriptingTestManager&)
{
	assert (0 && "PythonScriptingTestManager operator = is not allowed.");
	return *this;
}	// PythonScriptingTestManager::operator =


PythonScriptingTestManager::~PythonScriptingTestManager ( )
{
	unregisterReferences ( );
}	// PythonScriptingTestManager::~PythonScriptingTestManager


MainContainer* PythonScriptingTestManager::getContainerInstance (const string& name)
{
	ReferencedNamedObject*	o	= ReferencedObjectManager::getInstance (name);
	MainContainer*			c	= dynamic_cast<MainContainer*>(o);

	if (0 == c)
	{
		UTF8String	message (charset);
		message << "Le manager n'a pas d'instance de la classe MainContainer du nom de " << name << ".";
		throw Exception (message);
	}	// if (0 == c)

	return c;
}	// PythonScriptingTestManager::getContainerInstance


DefaultComponent* PythonScriptingTestManager::getDCInstance (const string& name)
{
	ReferencedNamedObject*	o	= ReferencedObjectManager::getInstance (name);
	DefaultComponent*		c	= dynamic_cast<DefaultComponent*>(o);

	if (0 == c)
	{
		UTF8String	message (charset);
		message << "Le manager n'a pas d'instance de la classe DefaultComponent du nom de " << name << ".";
		throw Exception (message);
	}	// if (0 == c)

	return c;
}	// PythonScriptingTestManager::getDCInstance


PythonScriptingTestManager& getPythonScriptingTestManager ( )
{
	return dynamic_cast<PythonScriptingTestManager&>(getObjectManagerInstance ( ));
}	// getPythonScriptingTestManager


// ============================================================================
//                      LA CLASSE PythonScriptingTestManager
// ============================================================================


PythonScriptingTestWriter::PythonScriptingTestWriter (
					const UTF8String& fileName, const Charset& charset,
					Log::TYPE mask, bool enableDate, bool enableTime)
	: PythonLogOutputStream (fileName, charset, mask, enableDate, enableTime)
{
}	// PythonScriptingTestWriter::PythonScriptingTestWriter


PythonScriptingTestWriter::PythonScriptingTestWriter (const PythonScriptingTestWriter&)
	: PythonLogOutputStream ("", Charset (Charset::UTF_8))
{
	assert (0 && "PythonScriptingTestWriter copy constructor is not allowed.");
}	// PythonScriptingTestWriter::PythonScriptingTestWriter


PythonScriptingTestWriter& PythonScriptingTestWriter::operator = (const PythonScriptingTestWriter&)
{
	assert (0 && "PythonScriptingTestWriter operator = is not allowed.");
	return *this;
}	// PythonScriptingTestWriter::PythonScriptingTestWriter


PythonScriptingTestWriter::~PythonScriptingTestWriter ( )
{
}	// PythonScriptingTestWriter::~PythonScriptingTestWriter


string PythonScriptingTestWriter::formatInstanceAccess (const ScriptingLog& log)
{
	if (false == log.useObjectManagerApi ( ))
	{
		UTF8String	message (charset);
		message << "Le log " << log.getName ( ) << "." << log.getMethodName ( )
		        << " n'est pas associé à un objet.";
		INTERNAL_ERROR (exc, message,
		                "PythonScriptingTestWriter::formatInstanceAccess")
		throw exc;
	}	// if (false == log.useObjectManagerApi ( ))

	UTF8String	access (charset);
	access << "getPythonScriptingTestManager ( ).";

	const ReferencedNamedObject*	object	= log.getNamedObject ( );
	const MainContainer*			mc		= dynamic_cast<const MainContainer*>(object);
	const DefaultComponent*			dc		= dynamic_cast<const DefaultComponent*>(object);

	if (0 != mc)
		access << "getContainerInstance (\"" << log.getName ( ) << "\")";
	else if (0 != dc)
		access << "getDCInstance (\"" << log.getName ( ) << "\")";
	else
	{
		UTF8String	message (charset);
		message << "Le log " << log.getName ( ) << "." << log.getMethodName ( )
		        << " n'est pas associé à un objet de type connu.";
		INTERNAL_ERROR (exc, message, "PythonScriptingTestWriter::formatInstanceAccess")
	}

	return access.iso ( );
}	// PythonScriptingTestWriter::formatInstanceAccess


string PythonScriptingTestWriter::formatInstanceVariableAccess (const string& uniqueName)
{
	if (0 == uniqueName.length ( ))
		return PythonLogOutputStream::formatInstanceVariableAccess (uniqueName);

	UTF8String	access (charset);
	access << "getPythonScriptingTestManager ( ).";

	try
	{
		const ReferencedNamedObject*	object	= ReferencedObjectManager::getManagerInstance ( ).getInstance(uniqueName);
		const MainContainer*			mc		= dynamic_cast<const MainContainer*>(object);
		const DefaultComponent*			dc		= dynamic_cast<const DefaultComponent*>(object);

		if (0 != mc)
			access << "getContainerInstance (\"" << uniqueName << "\")";
		else if (0 != dc)
			access << "getDCInstance (\"" << uniqueName << "\")";
		else
			return PythonLogOutputStream::formatInstanceVariableAccess (uniqueName);
	}
	catch (...)
	{
		return PythonLogOutputStream::formatInstanceVariableAccess (uniqueName);
	}

	return access.iso ( );
}	// PythonScriptingTestWriter::formatInstanceVariableAccess


// ============================================================================
//                         LA CLASSE MainContainer
// ============================================================================

MainContainer* instanciateContainer (const IN_STD string& name)
{
	MainContainer*	container	= new MainContainer (name);

	return container;
}	// instanciateContainer


MainContainer::MainContainer (const string& name)
	: ReferencedNamedObject (name), _children ( )
{
	getObjectManagerInstance ( ).registerObject (this);
}	// MainContainer::MainContainer


MainContainer::MainContainer (const MainContainer&)
	: ReferencedNamedObject (""), _children ( )
{
	assert (0 && "MainContainer copy constructor is not allowed.");
}	// MainContainer::MainContainer


MainContainer& MainContainer::operator = (const MainContainer&)
{
	assert (0 && "MainContainer operator = is not allowed.");
	return *this;
}	// MainContainer::MainContainer


MainContainer::~MainContainer ( )
{
	getObjectManagerInstance ( ).unregisterObject (this);
	notifyObserversForDestruction ( );
	unregisterReferences ( );
}	// MainContainer::~MainContainer


DefaultComponent* MainContainer::instanciate (const string& className)
{
	UTF8String			name (charset);
	DefaultComponent*	instance	= 0;
	name << getUniqueName ( ) << ".instance_"
	     << (unsigned long)(_children.size ( ));

	if (className == "DefaultComponent")
		instance	= new DefaultComponent (name);
	else if (className == "ComponentA")
		instance	= new ComponentA (name);
	else if (className == "ComponentB")
		instance	= new ComponentB (name);

	if (0 == instance)
	{
		UTF8String	message	(charset);
		message << "Classe " << className
		        << " inconnue de la classe MainContainer.";
		throw Exception (message);
	}	// if (0 == instance)

	registerObservable (instance, true);

	_children.push_back (instance);

	return instance;
}	// MainContainer::instanciate


DefaultComponent* MainContainer::getChild (const string& name)
{
	for (vector<DefaultComponent*>::iterator it = _children.begin ( );
	     _children.end ( ) != it; it++)
		if ((*it)->getName ( ) == name)
			return *it;

	UTF8String	message (charset);
	message << "Le conteneur " << getName ( ) << " n'a pas d'enfant du nom de "
	        << name << ".";
	throw Exception (message);
}	// MainContainer::getChild


void MainContainer::f (DefaultComponent* c1, DefaultComponent* c2)
{
	cout << "Fonction f appelée pour le conteneur " << getName ( )
	     << ". Arguments : "
	     << (0 == c1 ? string ("objet null") : c1->getUniqueName ( )) << ", "
	     << (0 == c2 ? string ("objet null") : c2->getUniqueName ( )) << "."
	     << endl;
}	// MainContainer::f


// ============================================================================
//                         LA CLASSE DefaultComponent
// ============================================================================

DefaultComponent::DefaultComponent (const string& name)
	: ReferencedNamedObject (name)
{
	getObjectManagerInstance ( ).registerObject (this);
}	// DefaultComponent::DefaultComponent


DefaultComponent::DefaultComponent (const DefaultComponent&)
	: ReferencedNamedObject ("")
{
	assert (0 && "DefaultComponent copy constructor is not allowed.");
}	// DefaultComponent::DefaultComponent


DefaultComponent& DefaultComponent::operator = (const DefaultComponent&)
{
	assert (0 && "DefaultComponent operator = is not allowed.");
	return *this;
}	// DefaultComponent::DefaultComponent


DefaultComponent::~DefaultComponent ( )
{
	getObjectManagerInstance ( ).unregisterObject (this);
	notifyObserversForDestruction ( );
	unregisterReferences ( );
}	// DefaultComponent::~DefaultComponent


void DefaultComponent::f1 ( )
{
	cout << "DefaultComponent::f1 () called for " << getUniqueName ( ) << "."
	     << endl;
}	// DefaultComponent::f1


void DefaultComponent::f2 (bool b)
{
	cout << "DefaultComponent::f2 (" << (true == b ? "true" : "false")
	     << ") called for " << getUniqueName ( ) << "."
	     << endl;
}	// DefaultComponent::f2


void DefaultComponent::f3 (const string& s, long l, double d)
{
	cout << "DefaultComponent::f3 (" << s << ", " << l << ", " << d
	     << ") called for " << getUniqueName ( ) << "."
	     << endl;
//throw Exception ("FAILED");	// Pour test sortie en erreur
}	// DefaultComponent::f3


// ============================================================================
//                         LA CLASSE ComponentA
// ============================================================================

ComponentA::ComponentA (const string& name)
	: DefaultComponent (name)
{
}	// ComponentA::ComponentA


ComponentA::ComponentA (const ComponentA&)
	: DefaultComponent ("")
{
	assert (0 && "ComponentA copy constructor is not allowed.");
}	// ComponentA::ComponentA


ComponentA& ComponentA::operator = (const ComponentA&)
{
	assert (0 && "ComponentA operator = is not allowed.");
	return *this;
}	// ComponentA::ComponentA


ComponentA::~ComponentA ( )
{
	notifyObserversForDestruction ( );
	unregisterReferences ( );
}	// ComponentA::~ComponentA


void ComponentA::f1 ( )
{
	cout << "ComponentA::f1 () called for " << getUniqueName ( ) << "."
	     << endl;
}	// ComponentA::f1


void ComponentA::f2 (bool b)
{
	cout << "ComponentA::f2 (" << (true == b ? "true" : "false")
	     << ") called for " << getUniqueName ( ) << "."
	     << endl;
}	// ComponentA::f2


void ComponentA::f3 (const string& s, long l, double d)
{
	cout << "ComponentA::f3 (" << s << ", " << l << ", " << d
	     << ") called for " << getUniqueName ( ) << "."
	     << endl;
}	// ComponentA::f3


// ============================================================================
//                         LA CLASSE ComponentB
// ============================================================================

ComponentB::ComponentB (const string& name)
	: DefaultComponent (name)
{
}	// ComponentB::ComponentB


ComponentB::ComponentB (const ComponentB&)
	: DefaultComponent ("")
{
	assert (0 && "ComponentB copy constructor is not allowed.");
}	// ComponentB::ComponentB


ComponentB& ComponentB::operator = (const ComponentB&)
{
	assert (0 && "ComponentB operator = is not allowed.");
	return *this;
}	// ComponentB::ComponentB


ComponentB::~ComponentB ( )
{
	notifyObserversForDestruction ( );
	unregisterReferences ( );
}	// ComponentB::~ComponentB


void ComponentB::f1 ( )
{
	cout << "ComponentB::f1 () called for " << getUniqueName ( ) << "."
	     << endl;
}	// ComponentB::f1


void ComponentB::f2 (bool b)
{
	cout << "ComponentB::f2 (" << (true == b ? "true" : "false")
	     << ") called for " << getUniqueName ( ) << "."
	     << endl;
}	// ComponentB::f2


void ComponentB::f3 (const string& s, long l, double d)
{
	cout << "ComponentB::f3 (" << s << ", " << l << ", " << d
	     << ") called for " << getUniqueName ( ) << "."
	     << endl;
}	// ComponentB::f3



}	// namespace PythonScriptingTest
