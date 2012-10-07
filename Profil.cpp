// Profil.cpp: implementation of the CProfil class.
//
//////////////////////////////////////////////////////////////////////

#include "Profil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProfil::CProfil(unsigned char ucNumProfil/*=0*/,CMainTask *pMain/*=NULL*/) : CThread (PTHREAD_CREATE_DETACHED) {
    pMainTask=pMain;
    cAutoDelete=1;
    ucNumeroProfil=ucNumProfil;
    iInit ();
}

CProfil::~CProfil() {

}

int CProfil::iInit (){
    if (pMainTask && ucNumeroProfil) {
        PARAMETRE_APPLI *pParamAppli=pMainTask->GetParam ();
        if (pParamAppli) {
            for (int iNumProfil=0;iNumProfil<MAX_PROFIL;iNumProfil++) {
                if (pParamAppli->stParamProfil[iNumProfil].ucNumeroProfil==iNumProfil) {
                    memcpy (&stParam,&pParamAppli->stParamProfil[iNumProfil],sizeof(PARAM_PROFIL));
                }
            }
        }
    }
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
	if (iIdPipe[0]!=-1) {
		int n=0;
		fd_set rfds;

        printf ("debut profil %d start %d\n",ucNumeroProfil,(int)time (NULL));
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
	return NULL;
}
