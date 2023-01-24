#ifndef PYTHON_LOG_OUTPUT_STREAM_H
#define PYTHON_LOG_OUTPUT_STREAM_H

#include <TkUtil/ScriptLogOutputStream.h>
#include <TkUtil/ScriptingLog.h>

#include <vector>


BEGIN_NAMESPACE_UTIL


/**
 * <P>Classe représentant un flux sortant de logs dirigé dans un fichier ASCII de type script Python.</P>
 * <P>Un tel fichier est voué à être exécuté via un interpréteur de scripts tel que python.</P>
 * @author	Charles PIGNEROL, CEA/DAM/DSSI
 */
class PythonLogOutputStream : public ScriptLogOutputStream
{
	public :

	/**
	 * Constructeur. Invoque setFile et est de ce fait susceptible de lever une exception. Affecte un shell d'exécution du script par défaut.
	 * @param		Nom du fichier script utilisé. Ecrase un éventuel fichier préexistant.
	 * @param		Encodage utilisé
	 * @param		Masque appliqué.
	 * @param		true si les dates doivent être affichées (sous forme de commentaires)
	 * @param		true si les heures doivent être affichées (sous forme de commentaires)
	 * @param		true si le script doit être exécutable (flag S_IXUSR)
	 * @see			setFile
	 * @see			setShell
	 * @see			defaultPythonShell
	 */
	PythonLogOutputStream (
		const UTF8String& fileName,
		const TkUtil::Charset& charset,	// v 4.0.0
        Log::TYPE mask = Log::SCRIPTING, bool enableDate = true,
		bool enableTime = true, bool executable = false);

	/**
	 * Destructeur. Ferme le fichier.
	 */
	virtual ~PythonLogOutputStream ( );

	/**
	 * Appelle indent.
	 * @see		closeBlock
	 * @see		indent
	 */
	virtual void declareBlock ( );

	/**
	 * Appelle unindent.
	 * @see		declareBlock
	 * @see		unindent
	 */
	virtual void closeBlock ( );

	/**
	 * Déclare le début d'un block try/except
	 * @see		closeTryBlock
	 */
	virtual void declareTryBlock ( );

	/**
	 * Déclare la fin d'un block try/except
	 * @see		declareTryBlock
	 * @see		closeUtilExcTryBlock
	 */
	virtual void closeTryBlock ( );

	/**
	 * Déclare la fin d'un block try/catch en récupérant une exception de type
	 * TkUtil::Exception puis une exception standard.
	 * @see		closeTryBlock
	 * @see		Exception
	 */
	virtual void closeUtilExcTryBlock ( );

	/**
	 * @return		La chaîne de caractères déclarant le début d'un commentaire.
	 */
	static const IN_STD string& getCommentDeclaration ( );

	/**
	 * @return		<I>true</I> si la ligne transmise en argument est une ligne
	 *				de commentaire, <I>false</I> dans le cas contraire.
	 */
	static bool isCommentLine (const IN_STD string& line);

	/**
	 * @return		La chaîne transmise en argument sous forme de commentaire
	 *				python, c.a.d. avec un # en début de chaque ligne.
	 */
	static UTF8String toComment (const UTF8String& text);

	/**
	 * @param		Bloc d'instructions évalué.
	 * @param		En retour, lignes composant le bloc d'instructions.
	 * @return		<I>true</I> si le bloc transmis en argument est composé de
	 *				plusieurs lignes, <I>false</I> dans le cas contraire.
	 */
	static bool isMultiLines (
				const std::string& block, std::vector< std::string >& lines);

	/**
	 * Shell affecté par défaut aux instances.
	 */
	static IN_STD string		defaultPythonShell;


	protected :

	/**
	 * Ecrit dans le script la déclaration du shell à invoquer et l'encodage
	 * utilisé si celui-ci n'est pas l'encodage par défaut.
	 * @see		setShell
	 * @see		getEncoding
	 */
	virtual void writeShellDeclaration ( );

	/**
	 * Ecrit dans le script des commentaires généraux (date de création,
	 * utilisateur, application qui a créé le script, ...
	 * @exception	En cas d'erreur d'écriture ou de flux invalide.
	 */
	virtual void writeScriptComments ( );

	/**
	 * Ecrit dans le script la déclaration des paquetages à utiliser.
	 * Ne fait rien par défaut, méthode à surcharger.
	 */
	virtual void writePackagesDeclaration ( );

	/**
	 * Formate l'instruction tranmise en argument conformément au langage
	 * python.
	 * @return		Instruction prête à être écrite dans le script.
	 * @exception	Méthode susceptible de lever une exception.	
	 * @warning		Fait appel à formatInstanceAccess dans le cas ou le log
	 *				fait référence à un objet nommé et à formatResult.
	 */
	virtual IN_STD string formatInstruction (const ScriptingLog& log);

	/**
	 * @return		La chaine "log.getResult ( ) = " si log.hasResult ( )"
	 *				vaut true, une chaine vide dans le cas contraire.
	 */
	virtual IN_STD string formatResult (const ScriptingLog& log);

	/**
	 * Méthode à surcharger pour retourner des types spécialisés.
	 * @return		La chaine de caractères permettant d'accéder à l'objet
	 *				nommé associé au log :
	 *				getObjectManagerInstance ( ).getInstance (log.getName ( )).
	 * @see			formatInstanceVariableAccess
	 */
	virtual IN_STD string formatInstanceAccess (const ScriptingLog& log);

	/**
	 * <P>Méthode à surcharger pour retourner des types spécialisés.</P>
	 * <P>Fonction utilisée pour accéder à une instance transmise en argument 
	 * dans un script. L'API actuelle ne fait pas de distinction entre les
	 * instances passées en IN, OUT et INOUT.</P>
	 * <P>Si l'instance transmise en argument n'est pas recensée par le
	 * gestionnaire d'objets nommés (classe <I>ReferencedObjectManager</I>),
	 * cette méthode retourne alors la chaine de caractères tranmise en
	 * argument. Ce dispositif permet de transmettre en argument une variable
	 * du script.</P>
	 * @param		Nom unique d'une instance de classe dérivée de
	 *				ReferencedNamedObject dont on souhaite obtenir une
	 *				chaine de caractères permettant dans un script d'y
	 *				accéder en tant qu'argument d'une méthode. <B>En cas de
	 *				chaine vide l'instance est supposée être nulle.</B>
	 * @return		La chaine de caractères permettant d'accéder à l'objet
	 *				dont le nom unique est donné en argument.
	 * @warning		Susceptible d'évoluer.
	 * @see			ReferencedObjectManager
	 */
    virtual IN_STD string formatInstanceVariableAccess (
                                        const IN_STD string& uniqueName);

	/**
	 * Formate le commentaire tranmis en argument conformément au langage
	 * python.
	 * @param		Commentaire à formater
	 * @param		<I>true</I> si le formatage doit être strict (minimum
	 *				nécessaire), <I>false s'il peut être agrémenté
	 *				(espaces, ...).
	 * @return		Commentaire prêt à être écrit dans le script.
	 * @exception	Méthode susceptible de lever une exception.
	 */
	virtual IN_UTIL UTF8String formatComment (
									const Log& log, bool scrict = false);

	/**
	 * Indente si nécessaire la ligne transmise en arguement.
	 */
	virtual void write (const IN_UTIL UTF8String& line);

	/**
	 * Augmente le niveau d'indentation.
	 */
	virtual void indent ( );

	/**
	 * Diminue le niveau d'indentation.
	 */
	virtual void unindent ( );


	private :

	/**
	 * Constructeurs de copie et opérateur = : interdits.
	 */
	PythonLogOutputStream (const PythonLogOutputStream&);
	PythonLogOutputStream& operator = (const PythonLogOutputStream&);

	/** Le niveau d'indentation courant. */
	size_t			_indentLevel;
};	// class PythonLogOutputStream


END_NAMESPACE_UTIL


#endif	// PYTHON_LOG_OUTPUT_STREAM_H
