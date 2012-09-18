// Thread.h: interface for the Thread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREAD_H__703E7BCA_2BAA_48B1_B195_432D838EB0B5__INCLUDED_)
#define AFX_THREAD_H__703E7BCA_2BAA_48B1_B195_432D838EB0B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <pthread.h>
#include <semaphore.h>

static void *FctThread (void *);

class CThread  
{
		// fonction //
/////////////////////////////////////
public:
	CThread(int detachstate = PTHREAD_CREATE_JOINABLE);
	virtual ~CThread();

	unsigned char GetInit () {return Init;};	
	pthread_t GetThread_id () {return thread_id;};

	int Create (char cWaitEnd=0);

	void SetArretThread (char DemArret) {
		pthread_mutex_lock(&mutex);
		ArretThread=DemArret;
		pthread_mutex_unlock(&mutex);
	};

	char cGetArretThread () {
		char cRet=0;
		pthread_mutex_lock(&mutex);
		cRet=ArretThread;
		pthread_mutex_unlock(&mutex);
		return cRet;
	};

	virtual void *Thread(void *)=0;

	char IsAutoDelete() {return cAutoDelete;};
	char IsDetach() {return cIsdetachstate;};
	void SetAutoDelete(char cDelete) {cAutoDelete=cDelete;};
protected:
	char cAutoDelete;
private:
	char cIsdetachstate;
	pthread_mutex_t mutex;
	char ArretThread;
	// variable //
////////////////////////////////////
public:
	
protected:
	
	pthread_attr_t attr;   /* initial thread attributes              */
    pthread_t thread_id;   /* child thread identifier                */
private:
	unsigned char Init;
};

#endif // !defined(AFX_THREAD_H__703E7BCA_2BAA_48B1_B195_432D838EB0B5__INCLUDED_)
