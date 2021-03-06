// SequenceurTache.cpp: implementation of the CSequenceurTache class.
//
//////////////////////////////////////////////////////////////////////

#include "SequenceurTache.h"
//*******************************************************************
// fonction static lanceur de tache
static void *fctStartProfilJour (void *arg) {
        CSequenceurTache *pSeqTask=(CSequenceurTache *)arg;
        if (pSeqTask) {

        }
        return NULL;
}
//*******************************************************************
CSequenceurTache::CSequenceurTache() {
    sLastDay=-1;
}

CSequenceurTache::~CSequenceurTache() {
}

void *CSequenceurTache::Thread (void *pThis) {
	int ErrSelect;
	struct timeval Tv;
	if (iIdPipe[0]!=-1) {
		int n=0;
		fd_set rfds;

		FD_ZERO(&rfds);

		n=iIdPipe[0];
		n++;

		while(!cGetArretThread()) {
			FD_SET(iIdPipe[0], &rfds);
			Tv.tv_sec	= 1;
			Tv.tv_usec	= 0;

			ErrSelect = select(n,&rfds,NULL,NULL,&Tv);
			if (ErrSelect > 0) {
				if (FD_ISSET(iIdPipe[0], &rfds)) {
					printf ("CSequenceurTache::Thread 2\n");
					break;
				}
			}else if (ErrSelect == 0) {	// time out
				if (!cGetArretThread()) {
					time_t tCourant=time (NULL);
					struct tm *stTmCourant=gmtime (&tCourant);
					if (stTmCourant) {
					    if (stTmCourant->tm_yday!=sLastDay) {
					        sLastDay=stTmCourant->tm_yday;
					        iInitTimerLanceTache (tCourant);
					    }
                    }
				}
			}else if(errno != EINTR) {
				break;
			}
		}
	}
	return NULL;
}

int CSequenceurTache::iInitTimerLanceTache (time_t tCourant) {
    int iErr=0;
#if 0
    PSTRUCT_TACHE pTache=NULL;

    // tache profil jour
    pTache=new STRUCT_TACHE;
    if (pTache) {
        memset (pTache,0,sizeof (STRUCT_TACHE));
        pTache->pTimerStart=new CTimer ();
        if (pTache->pTimerStart) {
            pTache->eType=PROFIL_JOUR;
            pTache->tPeriod=0;
            pTache->tDebut=0;
            pTache->pTimerStart->SetCallback (fctStartProfilJour);

        }else
            iErr=1;

        if (iErr) {
            if (pTache->pTimerStart) {
                delete pTache->pTimerStart;
                pTache->pTimerStart=NULL;
            }

            if (pTache->pTache) {
                pTache->pTache->SetArretThread(1);
                if (!pTache->pTache->IsAutoDelete()) {
                    if (!pTache->pTache->IsDetach())
                        pthread_join(pTache->pTache->GetThread_id(),NULL);

                    delete pTache->pTache;
                    pTache->pTache=NULL;
                }
            }
        }

    }
#endif
    return iErr;
}
