#ifndef GENERAL_H
#define GENERAL_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32

    #include <windows.h>
    #include <io.h>

    int pipe (int iIdPipe[2]);

    #ifndef uint64_t
        #define uint64_t ULONGLONG
    #endif // uint64_t

#else
    #include <unistd.h>
    #include <stdint.h>
#endif

#define MICRO_SEC  1000000
#define MILLI_SEC  1000

/********************************/
#define SECONDE    1
#define MINUTE     60*SECONDE
#define HEURE      60*MINUTE
#define JOUR       24*HEURE


enum TYPE_TACHE{
    TACHE_INCONNU=0,
    TACHE_PROFIL,
    TACHE_LECTURE_PH,
    TACHE_LECTURE_TEMP,
    TACHE_SOLEIL,
    TACHE_LUNE,
    TACHE_VENT,
    TACHE_PLUIE,
    TACHE_CO2,
};

time_t tGetOffsetFromMidnight (time_t tCourant=0);

#endif //GENERAL_H
