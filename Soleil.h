// Soleil.h: interface for the CSoleil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOLEIL_H__992B9529_59D7_40DC_A4EB_11BD8891D947__INCLUDED_)
#define AFX_SOLEIL_H__992B9529_59D7_40DC_A4EB_11BD8891D947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PwmGpio.h"
#include <BaseSqlite.h>

class CSoleil
{
public:
	CSoleil(PARAM_SOLEIL *pParam=NULL);
	virtual ~CSoleil();

	CPwmGpio GestionCourbeSolaire;
private :
    PARAM_SOLEIL stParam;
};

#endif // !defined(AFX_SOLEIL_H__992B9529_59D7_40DC_A4EB_11BD8891D947__INCLUDED_)
