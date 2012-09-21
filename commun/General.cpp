#include "General.h"

#ifdef WIN32
int pipe (int iIdPipe[2]) {
	iIdPipe[0]=0;
	iIdPipe[1]=0;
	return 0;
}
#endif