// PwmGpio.cpp: implementation of the CPwmGpio class.
//
//////////////////////////////////////////////////////////////////////

#include "PwmGpio.h"

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
	
	if (uiFreq>0 && uiFreq<=10 * KHz)
		uiFrequence=uiFreq;
	else
		uiFrequence=1*KHz;

	if (ucRapport>0 && ucRapport<100)
		ucRapportCyclique=ucRapport;
	else
		ucRapportCyclique=50;


	ui64_usec_Period=(1/uiFrequence)*NANO_SEC;
	
	ui64_usecEtatHaut=ui64_usec_Period*ucRapportCyclique/100;
	ui64_usecEtatBas=ui64_usec_Period-ui64_usecEtatHaut;

	ucEtat=0;

	return iErr;
}

int CPwmGpio::GestionPwm() {
	int iRetour=0;
	return iRetour;
}