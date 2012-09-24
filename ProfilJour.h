// ProfilJour.h: interface for the CProfilJour class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFILJOUR_H__CF9D88D4_98DC_4920_AC9C_CC21AEE56EF5__INCLUDED_)
#define AFX_PROFILJOUR_H__CF9D88D4_98DC_4920_AC9C_CC21AEE56EF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <General.h>
#include "Profil.h"
#include "Soleil.h"


class CProfilJour : public CProfil
{
public:
	CProfilJour();
	virtual ~CProfilJour();

	void *Thread(void *pThis);

	CSoleil mSoleil;

};

#endif // !defined(AFX_PROFILJOUR_H__CF9D88D4_98DC_4920_AC9C_CC21AEE56EF5__INCLUDED_)
