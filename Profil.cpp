// Profil.cpp: implementation of the CProfil class.
//
//////////////////////////////////////////////////////////////////////

#include "Profil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProfil::CProfil(CMainTask *pMain/*=NULL*/) : CThread (PTHREAD_CREATE_DETACHED) {
    pMainTask=pMain;
    cAutoDelete=1;
    Start();
}

CProfil::~CProfil() {

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
					printf ("CProfil::Thread 2\n");
					break;
				}
			}else if (ErrSelect == 0) {	// time out
				if (!cGetArretThread()) {
                    printf ("CProfil::Thread timeout\n");
				}
			}else if(errno != EINTR) {
				break;
			}
		}
	}
	return NULL;
}
