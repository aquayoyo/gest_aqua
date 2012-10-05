// MainTache.cpp: implementation of the CMainTask class.
//
//////////////////////////////////////////////////////////////////////

#include "MainTache.h"
#include <stdio.h>

#ifndef WIN32
#include <unistd.h>
#include <errno.h>
#else
#include <winsock.h>
#endif

//*******************************************************************
// fonction static lanceur de tache
typedef struct _param_Callback_Start_profil  {
    unsigned char ucNumeroPlanif;
    CMainTask *pMainTask;
}PARAM_CALLBACK_START_PROFIL;

static void *fctStartProfil (void *arg) {
    PARAM_CALLBACK_START_PROFIL *pParam=(PARAM_CALLBACK_START_PROFIL *)arg;
    if (pParam) {
        printf ("fctStartProfil %d \n",pParam->ucNumeroPlanif);
    }
    return NULL;
}
//*******************************************************************
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainTask::CMainTask() : CThread (PTHREAD_CREATE_JOINABLE) {
	printf ("CMainTask\n");
	sLastDay=-1;
}

CMainTask::~CMainTask()
{

}

void *CMainTask::Thread(void *pParam) {

	int ErrSelect;
	struct timeval Tv;

	int cpt=0;
	printf ("CMainTask::Thread 1 toffsetMidnight=%d\n",tGetOffsetFromMidnight());

	/*m_PwmGpio.iInit(0,10*HZ,99);
	m_PwmGpio.iStart ();
	*/

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
                printf ("CMainTask::Thread 1-1\n");
                break;
            }else if (ErrSelect == 0) {	// time out
                if (!cGetArretThread()) {
					time_t tCourant=time (NULL);
					struct tm *stTmCourant=gmtime (&tCourant);
					if (stTmCourant) {
					    if (stTmCourant->tm_yday!=sLastDay) {
					        sLastDay=stTmCourant->tm_yday;
					        PARAMETRE_APPLI *pParamAppli=m_ParametreAppli.pstInit();
					        if (pParamAppli) {
					            iInitPlanification (pParamAppli);
					        }
					    }
                    }
				}
            }else if(errno != EINTR) {
                printf ("CMainTask::Thread 1-3 errno=%d\n",errno);
                break;
            }
        }
    }
	//m_PwmGpio.iStop();
	printf ("CMainTask::Thread 2\n");
/*	m_SeqTask.SetArretThread (1);
	pthread_join (m_SeqTask.GetThread_id(),NULL);*/

	return NULL;
}

int CMainTask::iInitPlanification (PARAMETRE_APPLI *pParamAppli/*=NULL*/) {
    int iErr=0;
    if (pParamAppli) {
        time_t tOffsetMidnight=tGetOffsetFromMidnight ();
        for (int iNumPlanif=0;iNumPlanif<MAX_PLANIF_PROFIL;iNumPlanif++) {
            if (pParamAppli->stPlanifProfil[iNumPlanif].ucNumeroPlanif) {

                if (pParamAppli->stPlanifProfil [iNumPlanif].stPlanif.tDebut>tOffsetMidnight) {
                    // la tache ne doit pas être encore lancée car car la date de debut est > à maintenant

                    //recherche le timer dans la map des timers
                    map<unsigned char,CTimer *>::iterator it;
                    it=mLanceurProfil.find (pParamAppli->stPlanifProfil [iNumPlanif].ucNumeroPlanif);
                    if (it!=mLanceurProfil.end()) {
                        // si trouve on le supprime et on le recrée avec le nouveau tps
                        CTimer *pOldTimer=it->second;
                        if (pOldTimer) {
                            pOldTimer->Stop ();
                            mLanceurProfil.erase (it);
                            delete pOldTimer;
                            pOldTimer=NULL;
                        }
                    }
                    PARAM_CALLBACK_START_PROFIL *pParam=new PARAM_CALLBACK_START_PROFIL;
                    if (pParam) {
                        pParam->pMainTask=this;
                        pParam->ucNumeroPlanif=pParamAppli->stPlanifProfil [iNumPlanif].ucNumeroPlanif;

                        CTimer *LanceTacheprofil=new CTimer ();
                        if (LanceTacheprofil) {
                            LanceTacheprofil->SetCallback (fctStartProfil);
                            LanceTacheprofil->Start ((pParamAppli->stPlanifProfil [iNumPlanif].stPlanif.tDebut-tOffsetMidnight)*MICRO_SEC,pParam);
                            mLanceurProfil [pParamAppli->stPlanifProfil [iNumPlanif].ucNumeroPlanif]=LanceTacheprofil;
                        }else {
                            delete pParam;
                            pParam=NULL;
                             iErr=-1;
                        }
                    }else
                        iErr=-1;
                }else {
                    // verifier si le profil doit etre lancé
                    if (pParamAppli->stPlanifProfil [iNumPlanif].stPlanif.tDebut+pParamAppli->stPlanifProfil [iNumPlanif].stPlanif.tPeriod>tOffsetMidnight) {
                        // le temps debut + period est plus grand le profil doit être lancé si pas déja fait
                        PARAM_CALLBACK_START_PROFIL *pParam=new PARAM_CALLBACK_START_PROFIL;
                        if (pParam) {
                            pParam->pMainTask=this;
                            pParam->ucNumeroPlanif=pParamAppli->stPlanifProfil [iNumPlanif].ucNumeroPlanif;
                            CTimer *LanceTacheprofil=new CTimer ();
                            if (LanceTacheprofil) {
                                LanceTacheprofil->SetCallback (fctStartProfil);
                                LanceTacheprofil->Start (100,pParam);
                                mLanceurProfil [pParamAppli->stPlanifProfil [iNumPlanif].ucNumeroPlanif]=LanceTacheprofil;
                            }else {
                                delete pParam;
                                pParam=NULL;
                                 iErr=-1;
                            }
                        }else
                            iErr=-1;
                    }
                }
            }
        }
        pParamAppli->stParamChange.bParamPlanifProfil=0;
    }else
        iErr=-1;

    return iErr;
}
