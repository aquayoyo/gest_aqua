// PwmGpio.h: interface for the CPwmGpio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PWMGPIO_H__FAC8CE87_20AE_4FD1_B5A0_FF5F6EB41629__INCLUDED_)
#define AFX_PWMGPIO_H__FAC8CE87_20AE_4FD1_B5A0_FF5F6EB41629__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Timer.h"

class CPwmGpio : public CTimer  
{
public:
	CPwmGpio();
	virtual ~CPwmGpio();

private : //data
	unsigned char ucGpio;
	unsigned int uiFrequence
	unsigned char ucRapportCyclique; // rapport en pourcentage du niveau haut

};

#endif // !defined(AFX_PWMGPIO_H__FAC8CE87_20AE_4FD1_B5A0_FF5F6EB41629__INCLUDED_)
