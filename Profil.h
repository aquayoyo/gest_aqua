// Profil.h: interface for the CProfil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFIL_H__C9BDFB2D_275E_4701_928E_74135E9E5401__INCLUDED_)
#define AFX_PROFIL_H__C9BDFB2D_275E_4701_928E_74135E9E5401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Thread.h>
#include <General.h>
#include <MainTache.h>
#include <Soleil.h>
#include <Timer.h>

class CMainTask;
class CProfil : public CThread
{
public:
	CProfil(unsigned char ucNumProfil/*=0*/,CMainTask *pMain=NULL);
	virtual ~CProfil();

	virtual void *Thread(void *);

	void SetDebut (time_t tDebutProfil) {tDebut=tDebutProfil;};
	time_t tGetDebut () {return tDebut;};

	void SetDuree (time_t tDureeProfil) {tDuree=tDureeProfil;};
	time_t tGetDuree () {return tDuree;};

	void Start();
private :
    CMainTask *pMainTask;
    CSoleil *pSoleil;
    PARAM_PROFIL stParam;

    CTimer TimerStopProfil;

    unsigned char ucNumeroProfil;
	time_t tDebut;
	time_t tDuree;

    int iInit ();

    int iStartProfil ();

};

#endif // !defined(AFX_PROFIL_H__C9BDFB2D_275E_4701_928E_74135E9E5401__INCLUDED_)
