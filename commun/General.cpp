#include "General.h"
#include <stdio.h>
#include <time.h>

#ifdef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int pipe (int iIdPipe[2]) {
    SECURITY_ATTRIBUTES saAttr;

    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
	iIdPipe[0]=0;
	iIdPipe[1]=0;
    iIdPipe[0]=CreatePipe ((void**)&iIdPipe[0],(void**)&iIdPipe[1],&saAttr, 0);
	return 0;
}
#else
int nop=0;
#endif


time_t tGetOffsetFromMidnight (time_t tCourant) {
    time_t tMidnight=0;
    time_t tOffsetMidnight=0;

    if (!tCourant)
        tCourant=time (NULL);


    struct tm *stTmCourant=localtime (&tCourant);
    if (stTmCourant) {
        struct tm stTmMidnight=*stTmCourant;

        stTmMidnight.tm_hour=0;
        stTmMidnight.tm_min=0;
        stTmMidnight.tm_sec=0;
        stTmMidnight.tm_isdst=-1;

        tMidnight=mktime (&stTmMidnight);
        tOffsetMidnight=tCourant-tMidnight;
    }

    return tOffsetMidnight;
}
