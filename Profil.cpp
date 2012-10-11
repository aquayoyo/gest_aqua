// Profil.cpp: implementation of the CProfil class.
//
//////////////////////////////////////////////////////////////////////

#include "Profil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CProfil::CProfil(unsigned char ucNumProfil/*=0*/,CMainTask *pMain/*=NULL*/) : CThread (PTHREAD_CREATE_DETACHED) {
    pMainTask=pMain;
    pSoleil=NULL;
    cAutoDelete=1;
    ucNumeroProfil=ucNumProfil;
    iInit ();
}

CProfil::~CProfil() {

}

int CProfil::iInit (){
    int iErr=0;
    if (pMainTask && ucNumeroProfil) {
        PARAMETRE_APPLI *pParamAppli=pMainTask->GetParam ();
        if (pParamAppli) {
            for (int iNumProfil=0;iNumProfil<MAX_PROFIL;iNumProfil++) {
                if (pParamAppli->stParamProfil[iNumProfil].ucNumeroProfil==ucNumeroProfil) {
                    memcpy (&stParam,&pParamAppli->stParamProfil[iNumProfil],sizeof(PARAM_PROFIL));
                }
            }
        }else
            iErr=-1;
    }else
        iErr=-1;
    return iErr;
}
void CProfil::Start() {
	if (m_bStarted || tDuree==0) {
		return;
	}
	Create ();
}

void *CProfil::Thread(void *pThis) {
	int ErrSelect;
	struct timeval Tv;
    printf ("debut profil %d start %d\n",ucNumeroProfil,(int)time (NULL));
	if (iIdPipe[0]!=-1) {
	    if (iStartProfil ()==0) {
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
                        break;
                    }
                }else if (ErrSelect == 0) {	// time out
                    if (!cGetArretThread()) {
                    }
                }else if(errno != EINTR) {
                    break;
                }
            }
	    }
        if (pSoleil) {
            pSoleil->SetArretThread (1);
            if (!pSoleil->IsDetach())
                pthread_join(thread_id,NULL);
            if (!pSoleil->IsAutoDelete()) {
                delete pSoleil;
                pSoleil=NULL;
            }
        }
	}
	return NULL;
}

int CProfil::iStartProfil () {
    int iErr=0;
    printf ("CProfil::iStartProfil\n");
    if (stParam.stParamSoleil.ucValide) {
        if (pSoleil) {
            pSoleil->SetArretThread (1);
            if (!pSoleil->IsDetach())
				pthread_join(thread_id,NULL);
            if (!pSoleil->IsAutoDelete()) {
                delete pSoleil;
                pSoleil=NULL;
            }
        }else
            pSoleil=new CSoleil (&stParam.stParamSoleil);

        if (pSoleil) {
            pSoleil->Create();
        }
    }
    return iErr;
}
