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
#include <Thread.h>

class CSoleil : public CThread
{
public:
	CSoleil(PARAM_SOLEIL *pParam=NULL);
	virtual ~CSoleil();
	virtual void *Thread(void *);
private :
    PARAM_SOLEIL stParam;
    CPwmGpio GestionCourbeSolaire;

    short sIndexlSegment;
    unsigned short usIntervalleX;
    int iX;

    int iGererCourbe ();
};

#endif // !defined(AFX_SOLEIL_H__992B9529_59D7_40DC_A4EB_11BD8891D947__INCLUDED_)
