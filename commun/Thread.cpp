// Thread.cpp: implementation of the Thread class.
//
//////////////////////////////////////////////////////////////////////
#include "Thread.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifndef WIN32
#include <strings.h>
#include <unistd.h>
#endif
// get pid
#ifndef TRACE 
#define TRACE printf
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////////////// 


static void *FctThread (void *thread) {
	void *RetVal=NULL;
	if (thread) {
		//TRACE("FctThread : %p : %d\n", ((CThread*)thread), getpid());
		((CThread*)thread)->SetArretThread (0);
		((CThread*)thread)->Thread (thread); // fonction thread
		((CThread*)thread)->SetArretThread (2);

		if (((CThread*)thread)->IsAutoDelete())
			delete (CThread*)thread;
	}

	pthread_exit (RetVal);
	return RetVal;
}

CThread::CThread(int detachstate){
	//TRACE("CThread::CThread\n");
	Init=0;
	cAutoDelete=0;
	/* Initialize the POSIX threads attribute structure. */
    if (pthread_attr_init (&attr) == 0) {
		/* The main thread never joins with the children. */
		if (pthread_attr_setdetachstate (&attr, detachstate) == 0) {
			if ( pthread_mutex_init(&mutex, NULL)==0)
				Init=1;		
		}else 
			fputs ("failed to set detached state attribute\n", stderr);			
			
	}else
		fputs ("failed to initialize pthread attributes\n", stderr);
	//TRACE("Fin de CThread::CThread\n");
}

CThread::~CThread() {
	pthread_mutex_destroy(&mutex);
}

int CThread::Create() {
	int err=0;
	/* Create a thread to handle the client. */
	//size_t s =  1*1024*1024;
	//pthread_attr_setstacksize(&attr,s);
	if (pthread_create (&thread_id, &attr,(void* (*) (void*))&FctThread, this) == 0) {// Enclenche FctThread sur un autre thread		
		err=1;
		pthread_attr_destroy(&attr);
	}else {
		fputs ("failed to create thread\n", stderr);		
		err=-1;
	}
	return err;
}
