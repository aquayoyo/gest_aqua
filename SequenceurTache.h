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

class CSequenceurTache : public CThread {
public:
	CSequenceurTache ();
	virtual ~CSequenceurTache ();
	
	void *Thread (void *pThis);
private: // Data
	int iIdPipe [2];
	
	list <CTimer *> ListeLanceurTache;
};

#endif