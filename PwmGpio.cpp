// PwmGpio.cpp: implementation of the CPwmGpio class.
//
//////////////////////////////////////////////////////////////////////

#include "PwmGpio.h"
#include <stdio.h>
extern int *gpio_mmap;

static void *CallbackTimerPwmGpio (void *arg) {
	int iRetour=0;
	if (arg) {
		CPwmGpio* pm_PwmGpio=(CPwmGpio*)arg;
		iRetour=pm_PwmGpio->GestionPwm();
	}
	return (void*)iRetour;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGpio::CGpio (unsigned char ucNumGpio/*=0*/) {

}


CGpio::~CGpio () {
}


CPwmGpio::CPwmGpio(unsigned char ucNumGpio/*=0*/,unsigned int uiFreq/*=1000*/, double dRapport/*=50.0*/) {
	iInit(ucNumGpio,uiFreq,dRapport);
}

CPwmGpio::~CPwmGpio()
{

}

int CPwmGpio::iInit (unsigned char ucNumGpio/*=0*/,unsigned int uiFreq/*=1000*/, double dRapport/*=50.0*/) {
	int iErr=0;

	ucGpio = ucNumGpio;
	gpio_output( G_BANK(ucGpio),G_PIN(ucGpio));

	if (uiFreq>0 && uiFreq<=10 * KHz)
		uiFrequence=uiFreq;
	else
		uiFrequence=1*KHz;
	ui64_usec_Period=(MICRO_SEC/uiFrequence);

    SetRapportCyclique (dRapport);

	ucEtat=0;
	GPIO_WRITE( G_BANK(ucGpio),G_PIN(ucGpio),ucEtat);

	return iErr;
}
void CPwmGpio::SetRapportCyclique (double dRapport){

    if (dRapport>0 && dRapport<100)
		dRapportCyclique=dRapport;
	else
		dRapportCyclique=50.0;

    ui64_usecEtatHaut=ui64_usec_Period*dRapportCyclique/100;
	ui64_usecEtatBas=ui64_usec_Period-ui64_usecEtatHaut;
    m_TimerPwm.SetDuree (ui64_usecEtatHaut);
}

int CPwmGpio::iStart () {
	int iRetour=0;

	m_TimerPwm.SetCallback (CallbackTimerPwmGpio);
	printf ("ui64_usecEtatHaut %lld\n",ui64_usecEtatHaut);
	m_TimerPwm.Start (ui64_usecEtatHaut,this,TIME_PERIODIC);

	return iRetour;
}
int CPwmGpio::iStop () {
	int iRetour=0;

	m_TimerPwm.Stop ();

	return iRetour;
}
int CPwmGpio::GestionPwm() {
	int iRetour=0;
	ucEtat=ucEtat?0:1;
	GPIO_WRITE( G_BANK (ucGpio),G_PIN (ucGpio),ucEtat);
	return iRetour;
}
