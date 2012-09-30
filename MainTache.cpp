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

	int cpt=0;
	printf ("CMainTask::Thread 1\n");

	m_SeqTask.Create ();
	m_PwmGpio.iInit(0,10*HZ,99);
	m_PwmGpio.iStart ();

	if (iIdPipe[0]!=-1) {
		int n=0;
		fd_set rfds;
		FD_ZERO(&rfds);
		n=iIdPipe[0];
		n++;

        while(!cGetArretThread()) {
            Tv.tv_sec	= 1;
            Tv.tv_usec	= 0;

            ErrSelect = select(n,NULL,NULL,NULL,&Tv);
            if (ErrSelect > 0) {
                printf ("CMainTask::Thread 1-1\n");
                break;
            }else if (ErrSelect == 0) {	// time out
                printf ("CMainTask::Thread 1-2\n");
                cpt++;
                /*if (cpt>=30)
                    break;
                */
            }else if(errno != EINTR) {
                printf ("CMainTask::Thread 1-3\n");
                break;
            }
        }
    }
	m_PwmGpio.iStop();
	printf ("CMainTask::Thread 2\n");
	m_SeqTask.SetArretThread (1);
	pthread_join (m_SeqTask.GetThread_id(),NULL);

	return NULL;
}
