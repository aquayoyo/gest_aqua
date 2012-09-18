// MainTache.cpp: implementation of the CMainTask class.
//
//////////////////////////////////////////////////////////////////////

#include "MainTache.h"

#ifndef WIN32
#include <unistd.h>
#else
#include <stdio.h>
#include <winsock.h>
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainTask::CMainTask() : CThread (PTHREAD_CREATE_JOINABLE) {

	printf ("CMainTask\n");
}

CMainTask::~CMainTask()
{

}

void *CMainTask::Thread(void *pParam) {

	int ErrSelect;
	struct timeval Tv;

	printf ("CMainTask::Thread 1\n");
	
	while(!cGetArretThread()) {			
		Tv.tv_sec	= 1;
		Tv.tv_usec	= 0;
		
		ErrSelect = select(0,NULL,NULL,NULL,&Tv);
		if (ErrSelect > 0) {
			printf ("CMainTask::Thread 1-1\n");
			break;
		}else if (ErrSelect == 0) {	// time out			
			printf ("CMainTask::Thread 1-2\n");
		}else if(errno != EINTR) {		
			printf ("CMainTask::Thread 1-3\n");
			break;
		}
	}

	printf ("CMainTask::Thread 2\n");
	return NULL;
}
