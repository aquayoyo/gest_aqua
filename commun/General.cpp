#include "General.h"

#ifdef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int pipe (int iIdPipe[2]) {
	iIdPipe[0]=0;
	iIdPipe[1]=0;
    iIdPipe[0]=open ("c:\\temp.txt",O_RDWR | O_CREAT);
    iIdPipe[1]=iIdPipe[0];
	return 0;
}
#else
int nop=0;
#endif
