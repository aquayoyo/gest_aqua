// Profil.h: interface for the CProfil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFIL_H__C9BDFB2D_275E_4701_928E_74135E9E5401__INCLUDED_)
#define AFX_PROFIL_H__C9BDFB2D_275E_4701_928E_74135E9E5401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Thread.h>

class CProfil : public CThread  
{
public:
	CProfil();
	virtual ~CProfil();

};

#endif // !defined(AFX_PROFIL_H__C9BDFB2D_275E_4701_928E_74135E9E5401__INCLUDED_)
