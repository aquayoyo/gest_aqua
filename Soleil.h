// Soleil.h: interface for the CSoleil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOLEIL_H__992B9529_59D7_40DC_A4EB_11BD8891D947__INCLUDED_)
#define AFX_SOLEIL_H__992B9529_59D7_40DC_A4EB_11BD8891D947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PwmGpio.h"
class CSoleil  
{
public:
	CSoleil();
	virtual ~CSoleil();

	CPwmGpio GestionCourbeSolaire;
};

#endif // !defined(AFX_SOLEIL_H__992B9529_59D7_40DC_A4EB_11BD8891D947__INCLUDED_)
