#include "Timer.h"
#include <stdio.h>
#include <errno.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <winsock.h>
#endif

// h_objet_attache est le pointeur qui sera passé en paramètre lors des appels de la callback
CTimer::CTimer() {
	Init();
}

CTimer::~CTimer() {
	if (IsStarted())
		Stop();
}

static void CallbackCTimer(void *arg) {
	printf("%s\n", "CallbackCTimer 1");
}

// Appelable uniquement à partir des constructeurs
void CTimer::Init() {
	m_bStarted = false;
	ui64TpsUsec  = 0;
	m_hCallBack = CallbackCTimer;
	m_hObjetAttache = NULL;
	iIdPipe[0]=-1;
	iIdPipe[1]=-1;
}

void CTimer::Start(uint64_t ui64_usec, void *h_objet_attache) {
	if (m_bStarted || ui64_usec==0) {		
		return;
	}
	ui64TpsUsec = ui64_usec;
	m_hObjetAttache = h_objet_attache;
	// Démarrge du thread
	Create ();
}
void *CTimer::Thread (void *pThis) {
	int ErrSelect;
	struct timeval Tv;

	if(!m_hCallBack	|| ui64TpsUsec==0 || !pThis)
		return NULL;

	m_bStarted=true;
	if (pipe (iIdPipe)!=-1) {
		int n=0;
		fd_set rfds;

		FD_ZERO(&rfds);

		while(!cGetArretThread()) {			

			FD_SET(iIdPipe[0], &rfds);		

			Tv.tv_sec	= ui64TpsUsec /1000000;
			Tv.tv_usec	= (ui64TpsUsec %1000000);

			n=iIdPipe[0];
			n++;

			ErrSelect = select(n,&rfds,NULL,NULL,&Tv);
			if (ErrSelect > 0) {
				if (FD_ISSET(iIdPipe[0], &rfds)) {
					break;
				}
			}else if (ErrSelect == 0) {	// time out
				if (!cGetArretThread()) {
					m_hCallBack(m_hObjetAttache);
				}
			}else if(errno != EINTR) {				
				break;
			}
		}
	}
	return NULL;
}	

void CTimer::Stop() {
	if (!IsStarted()  || cGetArretThread()==2)
		return;
		
	int iRes = pthread_join(thread_id,NULL);
	if (iRes>0) {
		return;
	}
	m_bStarted = false;	
	m_hObjetAttache = NULL;
}

void CTimer::SetCallback(pFctCallback callback) {	
	m_hCallBack = callback;
}

