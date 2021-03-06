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
#include <Profil.h>
class CProfil;
class CMainTask : public CThread
{
public:
	CMainTask();
	virtual ~CMainTask();
	void *Thread(void *pParam);
    int iStartProfil (unsigned char ucNumeroPlanif=0);
    int iStopProfil (unsigned char ucNumeroProfil=0,CProfil *pProfil=NULL);

	PARAMETRE_APPLI *GetParam() {return m_ParametreAppli.GetParamAppli();};
private:
    short sLastDay;
    CParametreApplication m_ParametreAppli;

    map <unsigned char,CTimer *> mLanceurProfil;
	map <unsigned char,CTimer*> mStopProfil;

    int iInitPlanification (PARAMETRE_APPLI *pParamAppli=NULL);
};

#endif // !defined(AFX_MAINTACHE_H__890EC1EE_CAB3_4102_931E_9FD26CBBA5AD__INCLUDED_)
