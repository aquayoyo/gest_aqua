// SequenceurTache.h: interface for the CSequenceurTache class.
//
//////////////////////////////////////////////////////////////////////
#ifndef SEQUENCEURTACHE_H
#define SEQUENCEURTACHE_H
#include <General.h>

#include <list>
using namespace std;

#include <Timer.h>
#include <Thread.h>

#include <ProfilJour.h>
#include <ProfilNuit.h>
enum TYPE_TACHE{
    INCONNU=0,
    PROFIL_JOUR,
    PROFIL_NUIT,
    LECTURE_PH,
    LECTURE_TEMP,
    VENT,
    PLUIE,
    CO2,
};

typedef struct {
    enum  TYPE_TACHE eType;
    time_t tPeriod;
    time_t tDebut;
    CTimer *pTimerStart;
    CThread *pTache;
}STRUCT_TACHE,*PSTRUCT_TACHE;

class CSequenceurTache : public CThread {
public:
	CSequenceurTache ();
	virtual ~CSequenceurTache ();

	void *Thread (void *pThis);
private: // Data
	short sLastDay;

	int iInitTimerLanceTache (time_t tCourant);
	list <CTimer *> ListeLanceurTache;
	//list <CThread *> ListeTache;

	CProfilJour mProfilJour;
	CProfilNuit mProfilNuit;
};

#endif
