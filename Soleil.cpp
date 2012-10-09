// Soleil.cpp: implementation of the CSoleil class.
//
//////////////////////////////////////////////////////////////////////

#include "Soleil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoleil::CSoleil(PARAM_SOLEIL *pParam/*=NULL*/) {
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
            Tv.tv_sec	= 1;
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

    return iErr;
}
