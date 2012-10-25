#ifndef _FileLog_INCLUDED
#define _FileLog_INCLUDED
/*
	Niveau
	 0 off
	 1 demarrage, arret, redemarrage, erreur
	 2 1 + connexion (TCP/IP,PPP,RTC,RNIS,DIRECTE) + info sserie
	 3 2 + alarme
	 4 3 + commandes
*/

enum FL_NIVEAU {
	FL_N0 = 0,
		FL_OFF = FL_N0,

	FL_N1,
		FL_ERR = FL_N1,
		FL_RUN = FL_N1,

	FL_N2,
		FL_CON = FL_N2,

	FL_N3,
		FL_EVEN = FL_N3,

	FL_N4,
		FL_CMD = FL_N4,
};

class FileLog
{
public:
    FileLog();
	virtual ~FileLog();

	void Init(LPCSTR pFileRep, LPCSTR pFileName, int Niveau, short FlushCount);
	void SetRep(LPCSTR pFileRep);

	void SetNbjour(short nb) {m_nbjour = nb;};
	void SetNiveau(short val) {m_Niveau = val;};



	void Log(short Niveau, LPCSTR lpszFormat, ...);
	void DelOldLog(void );

private:
    pthread_mutex_t mutexLockfile;
    HANDLE	m_hLogfile;

	short	m_LogCount;
	char	m_FileRep[_MAX_PATH];
	char	m_FileName[_MAX_PATH];
	short	m_Niveau;
	short	m_sFileFlush;
	short	m_nbjour;		// nombre de jours pour les fichiers
	time_t  tLastWrite;	// date derniere ecriture ddans le fichier
};
#endif //_FileLog_INCLUDED
