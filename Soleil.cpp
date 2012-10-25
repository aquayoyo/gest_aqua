// Soleil.cpp: implementation of the CSoleil class.
//
//////////////////////////////////////////////////////////////////////

#include "Soleil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoleil::CSoleil(PARAM_SOLEIL *pParam/*=NULL*/):GestionCourbeSolaire(GPIO_16,1000,0) {
    sIndexlSegment=-1;
    usIntervalleX=1;
    if (pParam) {
        memcpy (&stParam,pParam,sizeof (PARAM_SOLEIL));
    }
}

CSoleil::~CSoleil()
{

}

void *CSoleil::Thread(void *pThis) {
    int ErrSelect;
	struct timeval Tv;

    printf ("debut CSoleil start %d\n",(int)time (NULL));
	if (iIdPipe[0]!=-1) {
        int n=0;
        fd_set rfds;
        FD_ZERO(&rfds);

        n=iIdPipe[0];
        n++;
        while(!cGetArretThread()) {
            FD_SET(iIdPipe[0], &rfds);
            Tv.tv_sec = usIntervalleX;
            Tv.tv_usec	= 0;

            ErrSelect = select(n,&rfds,NULL,NULL,&Tv);
            if (ErrSelect > 0) {
                if (FD_ISSET(iIdPipe[0], &rfds)) {
                    break;
                }
            }else if (ErrSelect == 0) {	// time out
                if (!cGetArretThread()) {
                    iGererCourbe ();
                }
            }else if(errno != EINTR) {
                break;
            }
        }
	}
	return NULL;
}


int CSoleil::iGererCourbe (){
    int iErr=0;

    time_t tOffsetMidnight=tGetOffsetFromMidnight ();
    if (sIndexlSegment==-1) {
        for (int iCpt=0;iCpt<stParam.stCourbeSolaire.usNbSegment;iCpt++) {
            if (tOffsetMidnight>=stParam.stCourbeSolaire.lSegment [iCpt].stParamTemps.tDebut &&
                tOffsetMidnight<(stParam.stCourbeSolaire.lSegment [iCpt].stParamTemps.tDebut+stParam.stCourbeSolaire.lSegment [iCpt].stParamTemps.tPeriod)) {
                    sIndexlSegment=iCpt;
                    usIntervalleX= stParam.stCourbeSolaire.lSegment [sIndexlSegment].usIntervalleX;
                    dX=0;
                    break;
            }
        }
    }else
        dX++;
    dX=((double)(tOffsetMidnight-stParam.stEphemeride.tDebut))/(double)3600;
    printf ("iGererCourbe %f\n",dX);
    if (sIndexlSegment>=0) {
        if (stParam.stCourbeSolaire.lSegment [sIndexlSegment].stSegment.eType==E_COURBE_AFFINE){
            double y=stParam.stCourbeSolaire.lSegment [sIndexlSegment].stSegment.uCourbe.stCourbeAffine.dA*dX+stParam.stCourbeSolaire.lSegment [sIndexlSegment].stSegment.uCourbe.stCourbeAffine.dB;
            GestionCourbeSolaire.SetRapportCyclique (y);
            printf ("iGererCourbe y=%f\n",y);

        }


        if (tOffsetMidnight+stParam.stCourbeSolaire.lSegment [sIndexlSegment].usIntervalleX>=
            stParam.stCourbeSolaire.lSegment [sIndexlSegment].stParamTemps.tDebut+stParam.stCourbeSolaire.lSegment [sIndexlSegment].stParamTemps.tPeriod)
            sIndexlSegment=-1;

    }
    return iErr;
}
