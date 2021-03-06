#include "Timer.h"
#include <stdio.h>
#include <errno.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <winsock.h>
#endif

// h_objet_attache est le pointeur qui sera pass� en param�tre lors des appels de la callback
CTimer::CTimer() {
	Init();
}

CTimer::~CTimer() {
	if (IsStarted())
		Stop();
}

static void *CallbackCTimer(void *arg) {
	printf("%s\n", "CallbackCTimer 1");
	return NULL;
}

// Appelable uniquement � partir des constructeurs
void CTimer::Init() {
	ui64TpsUsec  = 0;
	m_hCallBack = CallbackCTimer;
	m_hObjetAttache = NULL;
	uiFlagEvent=TIME_ONESHOT;
}

void CTimer::Start(uint64_t ui64_usec, void *h_objet_attache,unsigned int uiFlag) {
	if (m_bStarted || ui64_usec==0) {
		return;
	}
	printf ("ui64_usec=%llu\n",ui64_usec);
	ui64TpsUsec = ui64_usec;
	m_hObjetAttache = h_objet_attache;
	uiFlagEvent=uiFlag;
	// D�marrge du thread
	Create ();
}
void *CTimer::Thread (void *pThis) {
	int ErrSelect;
	struct timeval Tv;

	if(!m_hCallBack	|| ui64TpsUsec==0 || !pThis)
		return NULL;
	m_bStarted=true;

	if (iIdPipe[0]!=-1) {
		int n=0;
		fd_set rfds;
		FD_ZERO(&rfds);
		n=iIdPipe[0];
		n++;
		while(!cGetArretThread()) {
			FD_SET(iIdPipe[0], &rfds);

			LockMutex ();
			Tv.tv_sec	= ui64TpsUsec /MICRO_SEC;
			Tv.tv_usec	= (ui64TpsUsec %MICRO_SEC);
			UnLockMutex ();

			ErrSelect = select(n,&rfds,NULL,NULL,&Tv);
			if (ErrSelect > 0) {
				if (FD_ISSET(iIdPipe[0], &rfds)) {
					break;
				}
			}else if (ErrSelect == 0) {	// time out
				if (!cGetArretThread()) {
					m_hCallBack(m_hObjetAttache);
					if ((uiFlagEvent&0x1)==TIME_ONESHOT) {
						break;
					}
				}
			}else if(errno != EINTR) {
				break;
			}
		}
	}
	printf ("CTimer::stop thread\n");
	m_bStarted = false;
	return NULL;
}

void CTimer::Stop() {
	if (!IsStarted()  || cGetArretThread()==2)
		return;

	printf ("CTimer::Stop 1\n");
	if (iIdPipe[1]>=0) {
		printf ("CTimer::Stop 2\n");
		write (iIdPipe[1],"stop",4);
		printf ("CTimer::Stop 3\n");
		if (!IsAutoDelete()) {
			if (!IsDetach())
				pthread_join(thread_id,NULL);
		}
		printf ("CTimer::Stop 4\n");
		m_hObjetAttache = NULL;
	}
}

void CTimer::SetCallback(pFctCallback callback) {
	m_hCallBack = callback;
}

