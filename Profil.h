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

class CProfil : public CThread  
{
public:
	CProfil();
	virtual ~CProfil();

	virtual void *Thread(void *){return NULL;};

	void SetDebut (time_t tDebutProfil) {tDebut=tDebutProfil;};
	time_t tGetDebut () {return tDebut;};

	void SetDuree (time_t tDureeProfil) {tDuree=tDureeProfil;};
	time_t tGetDuree () {return tDuree;};

	void Start();
private :
	time_t tDebut;
	time_t tDuree;


};

#endif // !defined(AFX_PROFIL_H__C9BDFB2D_275E_4701_928E_74135E9E5401__INCLUDED_)
