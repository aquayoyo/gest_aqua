// MainTache.h: interface for the CMainTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINTACHE_H__890EC1EE_CAB3_4102_931E_9FD26CBBA5AD__INCLUDED_)
#define AFX_MAINTACHE_H__890EC1EE_CAB3_4102_931E_9FD26CBBA5AD__INCLUDED_
#include <General.h>

#include <stdio.h>

#include <map>
using namespace std;

#include <Thread.h>
#include <Timer.h>
#include <PwmGpio.h>
#include <BaseSqlite.h>

class CMainTask : public CThread
{
public:
	CMainTask();
	virtual ~CMainTask();
	void *Thread(void *pParam);

	CPwmGpio m_PwmGpio;

	//CSequenceurTache m_SeqTask;
private:
    int iInitPlanification (PARAMETRE_APPLI *pParamAppli=NULL);

	short sLastDay;
	map <unsigned char,CTimer *> mLanceurProfil;
	CParametreApplication m_ParametreAppli;
};

#endif // !defined(AFX_MAINTACHE_H__890EC1EE_CAB3_4102_931E_9FD26CBBA5AD__INCLUDED_)
