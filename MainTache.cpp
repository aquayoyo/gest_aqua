// MainTache.cpp: implementation of the CMainTask class.
//
//////////////////////////////////////////////////////////////////////

#include "MainTache.h"

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

	printf ("CMainTask::Thread 1\n");
	
	printf ("CMainTask::Thread 2\n");
	return NULL;
}
