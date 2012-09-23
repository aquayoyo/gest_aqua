#ifndef GENERAL_H
#define GENERAL_H

#include <stdio.h>
#include <errno.h>

#ifdef WIN32

int pipe (int iIdPipe[2]);
#else
#include <unistd.h>
#endif


#endif //GENERAL_H
