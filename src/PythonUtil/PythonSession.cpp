#include "PythonUtil/PythonSession.h"
#include <TkUtil/InternalError.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/File.h>
#ifdef USE_DUPLICATION_STREAM
#include <TkUtil/DuplicatedStream.h>
#else	// USE_DUPLICATION_STREAM
#include <TkUtil/FileRedirection.h>
#endif	// USE_DUPLICATION_STREAM

#include <Python.h> 
#include <stdlib.h>
#include <assert.h>
#include <memory>


USING_STD

static const TkUtil::Charset	charset ("àéèùô");


#define CHECK_PYTHON_EXC(command)                                           \
if (NULL != PyErr_Occurred ( ))                                             \
{                                                                           \
	PyErr_Print ( );                                                        \
	PyErr_Clear ( );                                                        \
	UTF8String	message (charset);                                          \
	message << "Une erreur s'est produite durant l'exécution de la "        \
	        << "commande Python " << command << ". Vous trouverez "         \
	        << "davantage d'informations sur la console de lancement de ce "\
	        << "logiciel.";                                                 \
	throw Exception (message);                                              \
}	// if (NULL != PyErr_Occurred ( ))


BEGIN_NAMESPACE_UTIL


size_t			PythonSession::_instanceCount	= 0;

vector<string>	PythonSession::_sysPaths;


PythonSession::PythonSession ( )
	: _logEnabled (true), _outputs ( ), _outText ( ), _errorText ()
{
	if (0 == _instanceCount)
	{
		if (false == Py_IsInitialized ( ))
			Py_Initialize ( ); 
		else
		{
#ifndef SWIG_FORBIDS_FINALIZATION
			INTERNAL_ERROR (exc, "Python est déjà initialisé.", "PythonSession::PythonSession")
			throw exc;
#endif	// SWIG_FORBIDS_FINALIZATION
		}
	}	// if (0 == _instanceCount)

	_instanceCount++;

#ifdef SWIG_FORBIDS_FINALIZATION
if (1 == _instanceCount)
{
#endif	// SWIG_FORBIDS_FINALIZATION
	if (false == _sysPaths.empty ( ))
		execCommand ("import sys");
	for (vector<string>::const_iterator it = _sysPaths.begin ( );
	     _sysPaths.end ( ) != it; it++)
	{
		UTF8String	command (charset);
		command << "sys.path.append(\"" << *it << "\")";
		execCommand (command.utf8 ( ));
	}	// for (vector<string>::const_iterator it = _sysPaths.begin ( ); ...
	CHECK_PYTHON_EXC ("execCommand")
#ifdef SWIG_FORBIDS_FINALIZATION
}	// if (1 == _instanceCount)
#endif	// SWIG_FORBIDS_FINALIZATION
}	// PythonSession::PythonSession


PythonSession::PythonSession (const PythonSession&)
	: _outText ( ), _errorText ()
{
	assert (0 && "PythonSession copy constructor is not allowed.");
}	// PythonSession::PythonSession


PythonSession& PythonSession::operator =(const PythonSession&)
{
	assert (0 && "PythonSession operator = is not allowed.");
	return *this;
}	// PythonSession::PythonSession


PythonSession::~PythonSession ( )
{
	assert ((0 != _instanceCount) && "PythonSession::~PythonSession");
#ifndef SWIG_FORBIDS_FINALIZATION
	_instanceCount--;
#endif	// SWIG_FORBIDS_FINALIZATION

	if (false == Py_IsInitialized ( ))
	{
//		INTERNAL_ERROR (exc, "La session python n'est pas initialisée.", "PythonSession::~PythonSession")
//		throw exc;
cerr << "ERREUR  dans PythonSession::~PythonSession : La session python n'est pas initialisée." << endl;	// v 5.2.0
	}	// if (false == Py_IsInitialized ( ))

#ifndef SWIG_FORBIDS_FINALIZATION
	if (0 == _instanceCount)
		Py_Finalize ( );
#endif	// SWIG_FORBIDS_FINALIZATION
}	// PythonSession::~PythonSession


void PythonSession::addSysPath (const string& path)
{
	for (vector<string>::const_iterator it = _sysPaths.begin ( );
	     _sysPaths.end ( ) != it; it++)
		if (path == *it)
			return;

	_sysPaths.push_back (path);
}	// PythonSession::addSysPath


void PythonSession::logOutputs (bool log)
{
	_logEnabled	= log;
}	// PythonSession::logOutputs


const string& PythonSession::getOutputs ( ) const
{
	return _outputs;
}	// PythonSession::getOutputs


void PythonSession::execFile (const string& fileName, bool catchOutputs)
{
	assert (true == Py_IsInitialized ( ));
	_outText.clear ( );
	_errorText.clear ( );
#ifdef USE_DUPLICATION_STREAM
	unique_ptr<DuplicatedStream>	errRedirection, outRedirection;
#else	// USE_DUPLICATION_STREAM
	unique_ptr<FileRedirection>		errRedirection, outRedirection;
#endif	// USE_DUPLICATION_STREAM
	if (true == catchOutputs)
	{
		const string	errorFile	= File::createTemporaryName ("pythonStdErr", true);
		const string	outFile		= File::createTemporaryName ("pythonStdOut", true);
		errRedirection.reset (
#ifdef USE_DUPLICATION_STREAM
						new DuplicatedStream (stderr, errorFile));
#else	// USE_DUPLICATION_STREAM
						new FileRedirection (stderr, errorFile, false, true));
#endif	// USE_DUPLICATION_STREAM
		outRedirection.reset (
#ifdef USE_DUPLICATION_STREAM
						new DuplicatedStream (stdout, outFile));
#else	// USE_DUPLICATION_STREAM
						new FileRedirection (stdout, outFile, false, true));
#endif	// USE_DUPLICATION_STREAM
	}	// if (true == catchOutputs)
	UTF8String	errorMsg (charset);
	errorMsg << "Erreur lors de l'exécution du script python " << fileName << " :\n";
	FILE*	file	= fopen (fileName.c_str ( ), "r");

	if (NULL != file)
	{
		PyCompilerFlags	flags;	// v 2.22.1
//		flags.cf_flags	= CO_FUTURE_DIVISION;	// v 2.42.1
		flags.cf_flags	= 0;					// v 2.42.1
//		int	completionCode	= PyRun_SimpleFile (file, fileName.c_str ( ));
		int	completionCode	= PyRun_SimpleFileExFlags (file, fileName.c_str ( ), 0, &flags);
		fclose (file);
		file	= NULL;
		if (true == catchOutputs)
		{
			outRedirection->terminate ( );
			errRedirection->terminate ( );
			_errorText	= errRedirection->getText ( );
			_outText	= outRedirection->getText ( );
			addToLogs (_outText, _errorText);
		}	// if (true == catchOutputs)

		if (0 != completionCode)
		{
			errorMsg << "Echec de l'interprétation par python "
			         << Py_GetVersion ( ) << " : " 
			         << (true == catchOutputs ?
			errRedirection->getText ( ) : string ("Information non récupérée"));
//			throw Exception (errorMsg.trim ( ).iso ( ));
			throw Exception (errorMsg.trim ( ));	// v 5.5.1
		}	// if (0 != completionCode)
	}
	else
	{
		File	f (fileName);
		if (false == f.exists ( ))
			errorMsg << "fichier inexistant.";
		else if (false == f.isReadable ( ))
			errorMsg << "droits en lecture interdits.";
		else if (false == f.isExecutable ( ))
			errorMsg << "droits en exécution interdits.";
		else
			errorMsg << "erreur non documentée.";
//		throw Exception (errorMsg.iso ( ));
		throw Exception (errorMsg);	// v 5.5.1
	}
}	// PythonSession::execFile


static wchar_t* wstrdup (const char* str)	// v 5.5.0
{	// str : tableau multibytes de caractères dans l'encodage local
	// En retour même chaîne mais dans un table de wchar_t
	mbstate_t	state	= mbstate_t ( );
	const size_t	len	= 1 + mbsrtowcs (NULL, &str, 0, &state);
	wchar_t*	wstr	= (wchar_t*)malloc (len * sizeof (wchar_t));
	const size_t	wlen	= mbsrtowcs (wstr, &str, len, &state);

	return wstr;
}	// wstrdup


void PythonSession::execFile (
		const string& fileName, int argc, char* argv[], bool catchOutputs)
{
	assert (true == Py_IsInitialized ( ));

	// Recopie du tableau argv en ajoutant "-c" et fileName :
	const int	pyArgc	= argc + 2;
#if PY_MAJOR_VERSION < 3
	char**		pyArgv	= (char**)malloc (pyArgc * sizeof (char*));
	pyArgv [0]	= strdup (fileName.c_str ( ));
	pyArgv [1]	= strdup ("-c");
	for (int i = 2; i < pyArgc; i++)
		pyArgv [i]	= strdup (argv [i - 2]);
#else	// => v 2.*
	wchar_t**	pyArgv	= (wchar_t**)malloc (pyArgc * sizeof (wchar_t*));
	pyArgv [0]	= wstrdup (fileName.c_str ( ));
	pyArgv [1]	= wstrdup ("-c");
	for (int i = 2; i < pyArgc; i++)
		pyArgv [i]	= wstrdup (argv [i - 2]);
#endif	// #if PY_MAJOR_VERSION < 3

	// On affecte ces arguments à l'environnement d'exécution du script :
	PySys_SetArgv (pyArgc, pyArgv);
	CHECK_PYTHON_EXC ("PySys_SetArgv")

	// Exécution du script :
	execFile (fileName, catchOutputs);

	// Nettoyage :
	for (int j = 0; j < pyArgc; j++)
		free (pyArgv [j]);
	free (pyArgv);
}	// PythonSession::execFile


void PythonSession::execCommand (const string& command, bool catchOutputs)
{
	assert (true == Py_IsInitialized ( ));
	_outText.clear ( );
	_errorText.clear ( );
#ifdef USE_DUPLICATION_STREAM
	unique_ptr<DuplicatedStream>	errRedirection, outRedirection;
#else	// USE_DUPLICATION_STREAM
	unique_ptr<FileRedirection>	errRedirection, outRedirection;
#endif	// USE_DUPLICATION_STREAM
	if (true == catchOutputs)
	{
		const string	errorFile	= File::createTemporaryName ("pythonStdErr", true);
		const string	outFile		= File::createTemporaryName ("pythonStdOut", true);
		errRedirection.reset (
#ifdef USE_DUPLICATION_STREAM
						new DuplicatedStream (stderr, errorFile));
#else	// USE_DUPLICATION_STREAM
						new FileRedirection (stderr, errorFile, false, true));
#endif	// USE_DUPLICATION_STREAM
		outRedirection.reset (
#ifdef USE_DUPLICATION_STREAM
						new DuplicatedStream (stdout, outFile));
#else	// USE_DUPLICATION_STREAM
						new FileRedirection (stdout, outFile, false, true));
#endif	// USE_DUPLICATION_STREAM
	}	// if (true == catchOutputs)

//	int	retCode	= PyRun_SimpleString (command.c_str ( ));
	PyCompilerFlags	flags;	// v 2.22.1
//	flags.cf_flags	= CO_FUTURE_DIVISION;	// v 2.42.1
	flags.cf_flags	= 0;					// v 2.42.1
	int	retCode	= PyRun_SimpleStringFlags (command.c_str ( ), &flags);
	if (true == catchOutputs)
	{
		outRedirection->terminate ( );
		errRedirection->terminate ( );
		_errorText	= errRedirection->getText ( );
		_outText	= outRedirection->getText ( );
		addToLogs (_outText, _errorText);
	}	// if (true == catchOutputs)

	if (0 != retCode)
	{
		UTF8String	errorMsg (charset);
		errorMsg << "Erreur lors de l'interprétation de la commande python \"\n" 
	             << command << "\":" << _errorText;
		throw Exception (errorMsg.trim ( ));
	}
}	// PythonSession::execCommand


const string& PythonSession::getOutText ( ) const
{
	return _outText;
}	// PythonSession::getOutText


const string& PythonSession::getErrorText ( ) const
{
	return _errorText;
}	// PythonSession::getErrorText


void PythonSession::addToLogs (const string& out, const string& err)
{
	if (true == _logEnabled)
	{
		if (0 != out.length ( ))
		{
			if (0 != _outputs.length ( ))
				_outputs.append ("\n");
			_outputs.append (out);
		}	// if (0 != out.length ( ))
		if (0 != err.length ( ))
		{
			if (0 != _outputs.length ( ))
				_outputs.append ("\n");
			_outputs.append (err);
		}	// if (0 != err.length ( ))
	}	// if (true == _logEnabled)
}	// PythonSession::addToLogs


END_NAMESPACE_UTIL

