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
class CSequenceurTache : public CThread {
public:
	CSequenceurTache ();
	virtual ~CSequenceurTache ();
	
	void *Thread (void *pThis);
private: // Data
	
	list <CTimer *> ListeLanceurTache;

	CProfilJour mProfilJour;
	CProfilNuit mProfilNuit;
};

#endif