#include "Timer.h"
#include <stdio.h>
#include <errno.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <stdio.h>
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
	printf("%s\n", "CallbackCTimer");
}

// Appelable uniquement à partir des constructeurs
void CTimer::Init() {
	m_bStarted = false;
	m_ulDuree_ms = 0;
	m_hCallBack = CallbackCTimer;
	m_hObjetAttache = NULL;
}

void CTimer::Start(unsigned long usec, void *h_objet_attache) {
	if (m_bStarted || usec==0) {		
		return;
	}
	m_ulDuree_ms = usec;
	m_hObjetAttache = h_objet_attache;
	// Démarrge du thread
	Create ();
}
void *CTimer::Thread (void *pThis) {
	
	if(!m_hCallBack	|| m_ulDuree_ms==0 || pThis)
		return NULL;
	int ErrSelect;
	struct timeval Tv;
	
	while(!cGetArretThread()) {			
		Tv.tv_sec	= m_ulDuree_ms/1000;
		Tv.tv_usec	= (m_ulDuree_ms%1000)*1000;
		
		ErrSelect = select(0,NULL,NULL,NULL,&Tv);
		if (ErrSelect > 0) {
			break;
		}else if (ErrSelect == 0) {	// time out
			if (!cGetArretThread()) {
				m_hCallBack(m_hObjetAttache);
			}
		}else if(errno != EINTR) {				
			break;
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

