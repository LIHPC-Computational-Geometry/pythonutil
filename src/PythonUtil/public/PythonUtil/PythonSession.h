#ifndef PYTHON_SESSION_H
#define PYTHON_SESSION_H


#include <TkUtil/util_config.h>

#include <string>
#include <vector>


BEGIN_NAMESPACE_UTIL


/**
 * Classe gérant l'ouverture et la fermeture de sessions python, ainsi que
 * quelques services python. Elle permet l'exécution simultanée de plusieurs
 * sessions. La session python du programme n'est fermée que lorsque toutes
 * les instances de cette classe sont détruites.
 * @author	Charles PIGNEROL, CEA/DAM/DSSI
 */
class PythonSession
{
	public :

	/**
	 * Initialise python si nécéssaire.
	 */
	PythonSession ( );

	/**
	 * Ferme la session python si c'est la dernière instance chargée de cette
	 * classe.
	 */
	virtual ~PythonSession ( );

	/**
	 * <P>Ajoute le chemin reçu en argument à l'environnement système de Python,
	 * à des fins de recherche de bibliothèques dynamiques. Toute session
	 * python commencera par un <I>sys.path.append (path_i)</I> de chaque
	 * chemin transmis à cette fonction.</P>
	 */
	static void addSysPath (const IN_STD string& path);

	/**
	 * @param		true si les sorties de l'interpréteur doivent être
	 *				sauvegardées, sinon false. Les sorties sont enregistrées
	 *				par défaut.
	 * @see			getOutputs ( )
	 */
	virtual void logOutputs (bool log);

	/**
	 * @return		les sorties standard et erreur de l'interpréteur Python.
	 * @see			logOutputs
	 */
	const IN_STD string& getOutputs ( ) const;

	/**
	 * Exécute le script dont le nom de fichier est transmis en argument.
	 * @param			<I>true</I> s'il faut récupérer les sorties standard et
	 *					erreur de l'exécution de la commande, <I>false</I>
	 *					dans le cas contraire.
	 * @exception		En cas d'échec de l'exécution du script
	 * @see				getOutputs
	 */
	virtual void execFile (const IN_STD string& fileName, bool catchOutputs = true);

	/**
	 * Exécute le script dont le nom de fichier est transmis en premier
	 * argument, et avec les arguments tranmis en second et troisème arguments.
	 * <B>Important :</B> argv [0] n'a pas besoin d'être identique
	 * à <I>fileName</I>, cette méthode se charge de le faire. De plus,
	 * l'option <B>-c</B> est ajouté aux arguments de lancement de
	 * l'interpréteur Python afin que le script soit en mesure de récupérer les
	 * arguments de ligne de commande.
	 * @param			Script à exécuter.
	 * @param			<I>true</I> s'il faut récupérer les sorties standard et
	 *					erreur de l'exécution de la commande, <I>false</I>
	 *					dans le cas contraire.
	 * @exception		En cas d'échec de l'exécution du script
	 * @see				getOutputs
	 */
	virtual void execFile (const IN_STD string& fileName, int argc, char* argv[], bool catchOutputs = true);

	/**
	 * Exécute la commande dont le texte est transmis en argument.
	 * @param			Commande à exécuter.
	 * @param			<I>true</I> s'il faut récupérer les sorties standard et
	 *					erreur de l'exécution de la commande, <I>false</I>
	 *					dans le cas contraire.
	 * @exception		En cas d'échec de l'exécution du script
	 * @see				getOutputs
	 */
	virtual void execCommand (const IN_STD string& command, bool catchOutputs = true);

	/**
	 * @return		La sortie standard de la dernière commande ou exécution de
	 *				script effectuée.
	 */
	virtual const IN_STD string& getOutText ( ) const;

	/**
	 * @return		La sortie erreur de la dernière commande ou exécution de
	 *				script effectuée.
	 */
	virtual const IN_STD string& getErrorText ( ) const;



	protected :

	/**
	 * Ajoute aux sorties standard les informations transmises en argument.
	 * @param		sortie normale
	 * @param		sortie erreur
	 */
	virtual void addToLogs (const IN_STD string& out, const IN_STD string& err);


	private :

	/**
	 * Constructeur de copie et opérateur = : interdits.
	 */
	PythonSession (const PythonSession&);
	PythonSession& operator = (const PythonSession&);

	/**
	 * Faut-il enregistrer stdout et stderr ? 
	 */
	bool										_logEnabled;

	/**
	 * Les sorties stdout et stderr enregistrées de l'interpréteur.
	 */
	IN_STD string								_outputs;

	/**
	 * La sortie standard de la dernière commande ou exécution de script
	 * effectuée.
	 */
	IN_STD string								_outText;

	/**
	 * La sortie erreur de la dernière commande ou exécution de script
	 * effectuée.
	 */
	IN_STD string								_errorText;

	/**
	 * Compteur d'instances en cours.
	 */
	static size_t								_instanceCount;

	/**
	 * Les chemins d'accès aux librairies dynamiques. 
	 */
	static	IN_STD vector <IN_STD string>		_sysPaths;
};	// class PythonSession


END_NAMESPACE_UTIL


#endif	// PYTHON_SESSION_H
