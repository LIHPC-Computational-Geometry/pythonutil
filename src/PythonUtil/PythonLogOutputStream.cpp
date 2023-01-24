#include "PythonUtil/PythonLogOutputStream.h"
#include "PythonUtil/PythonInfos.h"
#include <TkUtil/InformationLog.h>
#include <TkUtil/UTF8String.h>
#include <TkUtil/Exception.h>
#include <TkUtil/InternalError.h>

#include <patchlevel.h>	// PY_VERSION_HEX
#include <string.h>
#include <assert.h>


USING_STD


BEGIN_NAMESPACE_UTIL

static const Charset	charset ("àéèùô");


string	PythonLogOutputStream::defaultPythonShell ("/usr/bin/python");


PythonLogOutputStream::PythonLogOutputStream (
		const UTF8String& fileName, const Charset& charset, Log::TYPE mask, bool enableDate, bool enableTime, bool executable)
	: ScriptLogOutputStream (fileName, mask, enableDate, enableTime, executable),
	  _indentLevel (0)
{
	setCharset (charset);
	setShell (defaultPythonShell);
}	// PythonLogOutputStream::PythonLogOutputStream ( )


PythonLogOutputStream::PythonLogOutputStream (const PythonLogOutputStream& stream)
	: ScriptLogOutputStream (""),
	  _indentLevel (0)
{
	assert (0 && "PythonLogOutputStream copy constructor is not allowed.");
}	// PythonLogOutputStream::PythonLogOutputStream (const PythonLogOutputStream&)


PythonLogOutputStream& PythonLogOutputStream::operator = (const PythonLogOutputStream&)
{
	assert (0 && "PythonLogOutputStream assignment operator is not allowed.");

	return *this;
}	// PythonLogOutputStream::operator =


PythonLogOutputStream::~PythonLogOutputStream ( )
{
}	// PythonLogOutputStream::~PythonLogOutputStream


void PythonLogOutputStream::writeShellDeclaration ( )
{
	ScriptLogOutputStream::writeShellDeclaration ( );

	UTF8String	encoding (charset);
	switch (getCharset ( ).charset ( ))
	{
		case	Charset::ASCII		:
			encoding	= "-*- coding: ascii -*-";
			break;
		case	Charset::ISO_8859	:
			encoding	= "-*- coding: iso-8859-1 -*-";
			break;
		case	Charset::UTF_8		:
			encoding	= "-*- coding: utf-8 -*-";
			break;
		case	Charset::UTF_16		:
			encoding	= "-*- coding: utf-16 -*-";
			break;
	}	// switch (getCharset ( ).charset ( ))
	if (0 != encoding.length ( ))
		logComment (InformationLog (encoding));
}	// PythonLogOutputStream::writeShellDeclaration


void PythonLogOutputStream::writeScriptComments ( )
{
	ScriptLogOutputStream::writeScriptComments ( );

	UTF8String	pythonVersion (charset);
	pythonVersion << "Version de Python      : " << PYTHON_VERSION;
	logComment (InformationLog (pythonVersion));

	UTF8String	swigVersion (charset);
	swigVersion << "Version de Swig        : " << SWIG_VERSION;
	logComment (InformationLog (swigVersion));

	UTF8String	packageVersion (charset);
	packageVersion << "Version API PythonUtil : "
	               << PythonInfos::getVersion ( ).getVersion ( );
	logComment (InformationLog (packageVersion));
}	// PythonLogOutputStream::writeScriptComments


void PythonLogOutputStream::writePackagesDeclaration ( )
{
	LOCK_LOG_STREAM

	write ("import sys");

	// Déclaration des chemins d'accès :
	const vector<string>	paths	= getPackagePaths ( );
	if (0 != paths.size ( ))
	{
		logComment (InformationLog (UTF8String (
				"Déclaration des chemins d'accès aux paquetages utilisés.", charset)));
		declareTryBlock ( );
		for (vector<string>::const_iterator it = paths.begin ( );
		     paths.end ( ) != it; it++)
		{
			if (0 == (*it).length ( ))
			{
				UTF8String	message (charset);
				message << "Déclaration d'un chemin d'accès aux paquetages utilisés du script python "
				        << getFileName ( ) << " impossible : chemin nul.";
				throw Exception (message);
			}	// if (0 == (*it).length ( ))

			ScriptingLog	sl1 ("sys.path", "append", "");
			sl1 << *it;
			logInstruction (sl1);
		}	// for (vector<string>::const_iterator it = paths.begin ( ); ...	
		closeUtilExcTryBlock ( );
	}	// if (0 != paths.size ( ))

	addBlankLine ( );

	// Déclaration des paquetages :
	const vector<string>	packages	= getPackages ( );
	if (0 != packages.size ( ))
	{
		logComment (InformationLog (
					UTF8String ("Déclaration des paquetages utilisés.", charset)));
		declareTryBlock ( );
		for (vector<string>::const_iterator it = packages.begin ( );
		     packages.end ( ) != it; it++)
		{
			if (0 == (*it).length ( ))
			{
				UTF8String	message (charset);
				message << "Déclaration d'un paquetage au script python "
				        << getFileName ( ) << " impossible : nom de paquetage nul.";
				throw Exception (message);
			}	// if (0 == (*it).length ( ))

			UTF8String	instruction (charset);
			instruction << "from " << *it << " import *";
			write (instruction);
		}	// for (vector<string>::const_iterator it = packages.begin ( );

		closeUtilExcTryBlock ( );
	}	// if (0 != packages.size ( ))
}	// PythonLogOutputStream::writePackagesDeclaration


string PythonLogOutputStream::formatInstruction (const ScriptingLog& log)
{
	UTF8String	instruction (charset);
	instruction << formatResult (log);

	if (0 != log.getName ( ).length ( ))
	{
		if (false == log.useObjectManagerApi ( ))
			instruction << log.getName ( );
		else
			instruction << formatInstanceAccess (log);
		if (false == log.getMethodName ( ).empty ( ))	// v 5.6.0
			instruction << "." << log.getMethodName ( ) << " (";
	}	// if (0 != log.getName ( ).length ( ))
	else
		if (false == log.getMethodName ( ).empty ( ))	// v 5.6.0
			instruction << log.getMethodName ( ) << " (";

	const size_t	argCount	= log.getMethodArgumentsCount ( );
	for (size_t i = 0; i < argCount; i++)
	{
		pair <ScriptingLog::ARG_TYPE, string>	arg	= log.getMethodArgument (i);
//		if ((0 == arg.second.length ( )) && (ScriptingLog::INSTANCE != arg.first))
		if ((0 == arg.second.length ( )) && (ScriptingLog::INSTANCE != arg.first) && (ScriptingLog::STRING != arg.first))	// v 5.5.9
		{
			UTF8String	message (charset);
			message << (unsigned long)i << "-ème argument de type inconnu ("
			        << (unsigned long)arg.first << ") pour l'appel python "
			        << log.getName ( ) << "." << log.getMethodName ( )
			        << ").";
			INTERNAL_ERROR (exc, message, "PythonLogOutputStream::formatInstruction")
			throw exc;
		}	// if (0 == arg.second.length ( ))

		if (0 != i)
			instruction << ", ";
		switch (arg.first)
		{
			case ScriptingLog::STRING	:
				instruction << "\"" << arg.second << "\"";
				break;
			case ScriptingLog::BOOL		:
				instruction << (0 == strcasecmp (arg.second.c_str ( ), "true") ? "True" : "False");	// v 5.0.0 
//						0 == strcasecmp (arg.second.c_str ( ), "true") ?
//						(short)1 : (short)0);
				break;
			case ScriptingLog::LONG		:
			case ScriptingLog::DOUBLE	:
				instruction << arg.second;	// v 5.6.3
				break;
			case ScriptingLog::INSTANCE	:
				instruction << formatInstanceVariableAccess (arg.second);
				break;
			default						:
			{
				UTF8String	message (charset);
				message << (unsigned long)i << "-ème argument de type inconnu ("
				        << (unsigned long)arg.first << ") pour l'appel python "
				        << log.getName ( ) << "." << log.getMethodName ( )
				        << ").";
				INTERNAL_ERROR (exc, message,
				                "PythonLogOutputStream::formatInstruction")
				throw exc;
			}	// default
		}	// switch (arg.first)
	}	// for (size_t i = 0; i < argCount; i++)
	if (false == log.getMethodName ( ).empty ( ))	// v 5.6.0
		instruction << ")";

//	return instruction.iso ( );
	return instruction.ascii ( );
}	// PythonLogOutputStream::formatInstruction


string PythonLogOutputStream::formatResult (const ScriptingLog& log)
{
	if (false == log.hasResult ( ))
		return "";

	return log.getResult ( ) + " = ";
}	// PythonLogOutputStream::formatResult


string PythonLogOutputStream::formatInstanceAccess (const ScriptingLog& log)
{
	if (false == log.useObjectManagerApi ( ))
	{
		UTF8String	message (charset);
		message << "Le log " << log.getName ( ) << "." << log.getMethodName ( )
		        << " n'est pas associé à un objet.";
		INTERNAL_ERROR (exc, message,
		                "PythonLogOutputStream::formatInstanceAccess")
		throw exc;
	}	// if (false == log.useObjectManagerApi ( ))

	UTF8String	access (charset);
	access << "getObjectManagerInstance ( ).getInstance (\""
	       << log.getName ( ) << "\")";

//	return access.iso ( );
	return access.ascii ( );
}	// PythonLogOutputStream::formatInstanceAccess


string PythonLogOutputStream::formatInstanceVariableAccess (
													const string& uniqueName)
{
	if (0 == uniqueName.length ( ))
		return "None";

	try
	{
		// On s'assure que l'instance est bien recensée :
		getObjectManagerInstance ( ).getInstance (uniqueName);

		UTF8String	access (charset);
		access << "getObjectManagerInstance ( ).getInstance (\""
		       << uniqueName << "\")";

//		return access.iso ( );
		return access.ascii ( );
	}
	catch (...)
	{	// Objet non référencé : variable du script ?
		return uniqueName;
	}
}	// PythonLogOutputStream::formatInstanceVariableAccess


UTF8String PythonLogOutputStream::formatComment (const Log& log, bool strict)
{
	return toComment (log.getText ( ));
}	// PythonLogOutputStream::formatComment


void PythonLogOutputStream::write (const UTF8String& str)
{
	if (true == isCommentLine (str))
	{
		ScriptLogOutputStream::write (str);
		return;
	}	// if (0 == strncmp (str.c_str ( ), "#", 1))

	if (0 == _indentLevel)
		ScriptLogOutputStream::write (str);
	else
	{
		UTF8String	line (charset);
		for (size_t i = 0; i < _indentLevel; i++)
			line << "\t";
		line << str;
		ScriptLogOutputStream::write (line);
	}	// else if (0 == _indentLevel)
}	// PythonLogOutputStream::write


void PythonLogOutputStream::declareBlock ( )
{
	indent ( );
}	// PythonLogOutputStream::declareBlock


void PythonLogOutputStream::closeBlock ( )
{
	try
	{
		unindent ( );
	}
	catch (const Exception& exc)
	{
		UTF8String	message (charset);
		message << "PythonLogOutputStream::closeBlock : Erreur : "
		        << exc.getFullMessage ( );
		throw Exception (message);
	}
	catch (...)
	{
		throw Exception ("PythonLogOutputStream::closeBlock : erreur non documentée.");
	}
}	// PythonLogOutputStream::closeBlock


void PythonLogOutputStream::declareTryBlock ( )
{
	write (UTF8String ("try :", charset));
	declareBlock ( );
}	// PythonLogOutputStream::declareTryBlock


void PythonLogOutputStream::closeTryBlock ( )
{
	closeBlock ( );
#if PY_VERSION_HEX >= 0x03000000	// v 5.6.2
	write (UTF8String ("except RuntimeError as re :", charset));
#else 	// #if PY_VERSION_HEX >= 0x03000000
	write (UTF8String ("except RuntimeError , re :", charset));
#endif	// #if PY_VERSION_HEX >= 0x03000000
	declareBlock ( );
	ScriptingLog	i1 ("sys.stderr", "write", "");
	i1 << "Erreur rencontrée : ";
	logInstruction (i1);
	ScriptingLog	i2 ("sys.stderr", "write", "");
	i2.addVariable ("str (re)");
	logInstruction (i2);
	closeBlock ( );
}	// PythonLogOutputStream::closeTryBlock


void PythonLogOutputStream::closeUtilExcTryBlock ( )
{
	closeBlock ( );
#if PY_VERSION_HEX >= 0x03000000	// v 5.6.2
	write (UTF8String ("except Exception as re :", charset));
#else 	// #if PY_VERSION_HEX >= 0x03000000
	write (UTF8String ("except Exception , exc :", charset));
#endif	// #if PY_VERSION_HEX >= 0x03000000
	declareBlock ( );
	ScriptingLog	i1 ("sys.stderr", "write", "");
	i1 << "Erreur rencontrée : ";
	logInstruction (i1);
	ScriptingLog	i2 ("sys.stderr", "write", "");
	i2.addVariable ("str (exc)");
	logInstruction (i2);

	// Exception standard :
	closeTryBlock ( );
}	// PythonLogOutputStream::closeUtilExcTryBlock


const string& PythonLogOutputStream::getCommentDeclaration ( )
{
	static const string	commentDeclaration ("#");
	return commentDeclaration;
}	// PythonLogOutputStream::getCommentDeclaration


bool PythonLogOutputStream::isCommentLine (const string& line)
{	
	return 0 == line.find (getCommentDeclaration ( )) ? true : false;
}	// PythonLogOutputStream::isCommentLine


UTF8String PythonLogOutputStream::toComment (const UTF8String& t)
{
	const char		newline ('\n');
	UTF8String		text (t);
	UTF8String		comment (charset);

	string::size_type	pos	= text.find (newline);
	const string		cd	= getCommentDeclaration ( );	// v 3.6.0
	while (string::npos != pos)
	{	// Rem : chaque ligne commence peut être déjà par un
		// getCommentDeclaration ( ) : on ne le rajoute que si nécessaire.
		if (0 != text.find (cd))	// v 3.6.0
			comment << cd << ' ';
		comment << text.substring (0, pos);// << "\n";
		const string::size_type	len	= text.length ( );
		if (len - 1 == pos)	// v 4.5.1 : la chaine finit par '\n' !
		{
			text.clear ( );
			pos		= string::npos;
		}
		else
		{
			text	= text.substring (pos + 1);
			pos		= text.find (newline);
		}
	}	// while (string::npos != pos)
	if (false == text.empty ( ))	// v 4.5.1
	{
		if (0 != text.find (cd))	// v 3.6.0
			comment << cd << ' ';
		comment << text;
	}	// if (false == text.empty ( ))

	return comment;
}	// PythonLogOutputStream::toComment


bool PythonLogOutputStream::isMultiLines (const string& block, vector<string>& lines)
{
	lines.clear ( );
	bool			completed	= false;

	string	s	= block;
	while (false == completed)
	{
		string::size_type	nl	= s.find ('\n');
		if (string::npos == nl)
		{
			lines.push_back (s);
			completed	= true;
		}	// if (string::npos == nl)
		else
		{
			string	s2	= s.substr (0, nl);
			lines.push_back (s2);
			s	= s.substr (nl + 1);
		}	// else if (string::npos == nl)
	}	// while (false == completed)

	return 1 <= lines.size ( ) ? false : true;
}	// PythonLogOutputStream::isMultiLines


void PythonLogOutputStream::indent ( )
{
	_indentLevel++;
}	// PythonLogOutputStream::indent


void PythonLogOutputStream::unindent ( )
{
	if (0 == _indentLevel)
		throw Exception ("PythonLogOutputStream::unindent : décrémentation de l'indentation impossible, niveau nul.");

	_indentLevel--;
}	// PythonLogOutputStream::unindent


END_NAMESPACE_UTIL

