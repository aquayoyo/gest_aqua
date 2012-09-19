// PwmGpio.h: interface for the CPwmGpio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PWMGPIO_H__FAC8CE87_20AE_4FD1_B5A0_FF5F6EB41629__INCLUDED_)
#define AFX_PWMGPIO_H__FAC8CE87_20AE_4FD1_B5A0_FF5F6EB41629__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Timer.h"
#include "gpio-mmap.h"

#define KHz 1000
#define NANO_SEC 1000000
static void *CallbackTimerPwmGpio (void *arg);

class CPwmGpio : public CTimer  
{
public:
	CPwmGpio(unsigned char ucNumGpio=0,unsigned int uiFreq=1000, unsigned char ucRapport=50);
	virtual ~CPwmGpio();

	int GestionPwm();
private : //data
	unsigned char ucEtat;

	int iInit (unsigned char ucNumGpio=0,unsigned int uiFreq=1000, unsigned char ucRapport=50);

	unsigned char ucGpio;
	unsigned int uiFrequence;
	unsigned char ucRapportCyclique; // rapport en pourcentage du niveau haut

	uint64_t ui64_usecEtatHaut;
	uint64_t ui64_usecEtatBas;

	uint64_t ui64_usec_Period;
};

#endif // !defined(AFX_PWMGPIO_H__FAC8CE87_20AE_4FD1_B5A0_FF5F6EB41629__INCLUDED_)
