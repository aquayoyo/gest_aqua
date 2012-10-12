// PwmGpio.h: interface for the CPwmGpio class.
//
//////////////////////////////////////////////////////////////////////
#ifndef PWMGPIO_H
#define PWMGPIO_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <General.h>

#include "Timer.h"
#include "gpio-mmap.h"
#define HZ	1
#define KHz 1000 * HZ

#define BANK (x)  x << 4
#define PIN (x)   x

#define G_BANK (x) x >> 4
#define G_PIN (x) x&0xf


#define GPIO_0 BANK (0) + PIN (0)
#define GPIO_1
#define GPIO_2
#define GPIO_3
#define GPIO_4
#define GPIO_5
#define GPIO_6
#define GPIO_7
#define GPIO_8

#define GPIO_17

#define GPIO_23
#define GPIO_25

#define GPIO_32
#define GPIO_33
#define GPIO_34
#define GPIO_35
#define GPIO_36
#define GPIO_37
#define GPIO_38
#define GPIO_39

#define GPIO_50
#define GPIO_51
#define GPIO_52
#define GPIO_53
#define GPIO_54
#define GPIO_55
#define GPIO_56
#define GPIO_57

#define GPIO_60

#define GPIO_91
#define GPIO_92


static void *CallbackTimerPwmGpio (void *arg);

class CGpio {
public:
    CGpio(unsigned char ucNumGpio=0);
    virtual ~CGpio();
private :
    unsigned char ucGpio;
};

class CPwmGpio
{
public:
	CPwmGpio(unsigned char ucNumGpio=0,unsigned int uiFreq=1000, unsigned char ucRapport=50);
	virtual ~CPwmGpio();

	int GestionPwm();

	int iInit (unsigned char ucNumGpio=0,unsigned int uiFreq=1000, unsigned char ucRapport=50);
	int iStart ();
	int iStop ();

private : //data
	CTimer  m_TimerPwm;
	unsigned char ucEtat;

	unsigned char ucGpio;
	unsigned int uiFrequence;
	unsigned char ucRapportCyclique; // rapport en pourcentage du niveau haut

	uint64_t ui64_usecEtatHaut;
	uint64_t ui64_usecEtatBas;

	uint64_t ui64_usec_Period;
};

#endif // !defined(AFX_PWMGPIO_H__FAC8CE87_20AE_4FD1_B5A0_FF5F6EB41629__INCLUDED_)
