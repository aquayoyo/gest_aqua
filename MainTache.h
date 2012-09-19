// MainTache.h: interface for the CMainTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINTACHE_H__890EC1EE_CAB3_4102_931E_9FD26CBBA5AD__INCLUDED_)
#define AFX_MAINTACHE_H__890EC1EE_CAB3_4102_931E_9FD26CBBA5AD__INCLUDED_

#include <stdio.h>
#include <Thread.h>
#include <PwmGpio.h>

class CMainTask : public CThread 
{
public:
	CMainTask();
	virtual ~CMainTask();
	void *Thread(void *pParam);

	CPwmGpio m_PwmGpio;
};

#endif // !defined(AFX_MAINTACHE_H__890EC1EE_CAB3_4102_931E_9FD26CBBA5AD__INCLUDED_)
