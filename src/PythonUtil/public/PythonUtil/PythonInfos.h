#ifndef PYTHON_INFOS_H
#define PYTHON_INFOS_H

#include <TkUtil/util_config.h>
#include <TkUtil/Version.h>


/*!
 * \mainpage    Page principale de la bibliothèque PythonUtil
 *
 * \section presentation    Présentation
 * La bibliothèque <I>PythonUtil</I> propose une classe de gestion d'une session
 * <I>Python</I> dans un programme <I>C++</I>.
 *
 * \section depend      Dépendances
 * Cette bibliothèque utilise la bibliothèque TkUtil.
 * Les directives de compilation et d'édition des liens de cette bibliothèque
 * s'appliquent également à celle-ci.
 */


BEGIN_NAMESPACE_UTIL


/**
 * Classe permettant de connaître la version de cette bibliothèque.
 * @author	Charles PIGNEROL, CEA/DAM/DSSI
 */
class PythonInfos
{
	public :

	/**
	 * @return		La version de cette bibliothèque
	 */
	static const Version& getVersion ( )
	{ return _version; }


	protected :

	/**
	 * Constructeurs, opérateur = et destructeur : interdits.
	 */
	PythonInfos ( );
	PythonInfos (const PythonInfos& exc);
	PythonInfos& operator = (const PythonInfos& exc);
	~PythonInfos ( );


	private :

	/**
	 * La version de cette bibliothèque.
	 */
	static const Version		_version;
};	// class PythonInfos


END_NAMESPACE_UTIL


#endif	// PYTHON_INFOS_H
