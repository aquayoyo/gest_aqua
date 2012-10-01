#ifndef GENERAL_H
#define GENERAL_H

#include <stdio.h>
#include <errno.h>

#ifdef WIN32
#include <io.h>
int pipe (int iIdPipe[2]);
#else
#include <unistd.h>
#endif


enum TYPE_TACHE{
    INCONNU=0,
    PROFIL_JOUR,
    PROFIL_NUIT,
    LECTURE_PH,
    LECTURE_TEMP,
    VENT,
    PLUIE,
    CO2,
};

#endif //GENERAL_H
