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

CPwmGpio::CPwmGpio(unsigned char ucNumGpio/*=0*/,unsigned int uiFreq/*=1000*/, unsigned char ucRapport/*=50*/) {
	iInit(ucNumGpio,uiFreq,ucRapport);
}

CPwmGpio::~CPwmGpio()
{

}

int CPwmGpio::iInit (unsigned char ucNumGpio/*=0*/,unsigned int uiFreq/*=1000*/, unsigned char ucRapport/*=50*/) {
	int iErr=0;
	
	ucGpio = ucNumGpio;
	gpio_output( 2,1);
	
	if (uiFreq>0 && uiFreq<=10 * KHz)
		uiFrequence=uiFreq;
	else
		uiFrequence=1*KHz;

	if (ucRapport>0 && ucRapport<100)
		ucRapportCyclique=ucRapport;
	else
		ucRapportCyclique=50;


	ui64_usec_Period=(NANO_SEC/uiFrequence);
	
	ui64_usecEtatHaut=ui64_usec_Period*ucRapportCyclique/100;
	ui64_usecEtatBas=ui64_usec_Period-ui64_usecEtatHaut;

	ucEtat=0;
	GPIO_WRITE_PIN( 65,ucEtat);
	
	return iErr;
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
	GPIO_WRITE_PIN( 65,ucEtat);
	printf ("ucEtat=%d\n",ucEtat);
	return iRetour;
}