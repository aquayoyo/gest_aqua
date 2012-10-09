// BaseSqlite.h class.
//
//////////////////////////////////////////////////////////////////////
#ifndef BASESQLITE_H
#define BASESQLITE_H
#include <General.h>
#include <Thread.h>

#include <list>
using namespace std;

#define MAX_PROFIL 2
#define MAX_PLANIF_PROFIL 2
typedef struct _PlanificationTACHE {
    enum  TYPE_TACHE eType;
    time_t tDebut;
    time_t tPeriod;
    time_t tRelance;
}PLANIFICATION_TACHE;

enum TYPE_COURBE {
    E_COURBE_AFFINE=1,
    E_COURBE_LOG,
};

typedef struct _Courbe_Affine {
    int iA;
    int iB;
}COURBE_AFFINE;

typedef struct _Courbe_Log {
    int iA;
}COURBE_LOG;

typedef struct _Courbe {
    enum TYPE_COURBE eType;
    union {
        COURBE_AFFINE stCourbeAffine;
        COURBE_LOG stCourbeLog;
    }uCourbe;
}COURBE;

typedef struct _SegmentSolaire {
    PLANIFICATION_TACHE stParamTemps;
    unsigned short usIntervalleX; //en seconde
    COURBE stSegment;
}SEGMENT_SOLAIRE;

typedef struct _CourbeSolaire {
    unsigned short usNbSegment;
    SEGMENT_SOLAIRE lSegment [32];
}COURBE_SOLAIRE;

typedef struct _Soleil  {
    unsigned char ucValide;
    unsigned char ucAzimut;
    PLANIFICATION_TACHE stEphemeride;
    COURBE_SOLAIRE stCourbeSolaire;
}PARAM_SOLEIL;

typedef struct _Profil  {
    char strNom [32+1];
    unsigned char ucNumeroProfil;
    PARAM_SOLEIL stParamSoleil;
}PARAM_PROFIL;

typedef struct _DEF_PLANIFICATION_PROFIL{
    unsigned char ucNumeroPlanif;
    unsigned char ucNumeroProfil;
    PLANIFICATION_TACHE stPlanif;
}DEF_PLANIFICATION_PROFIL;

typedef union
{
    struct
	{
		uint64_t bParamProfil       : 1;
		uint64_t bParamPlanifProfil : 1;
		uint64_t bVide              : 62;

	};
	uint64_t	ul;
}PARAM_CHANGE;

typedef struct _Param  {
    PARAM_PROFIL stParamProfil [MAX_PROFIL];
    DEF_PLANIFICATION_PROFIL stPlanifProfil [MAX_PLANIF_PROFIL];
    PARAM_CHANGE stParamChange;
}PARAMETRE_APPLI;

class CBaseSqlite : public CThread {

public:
    CBaseSqlite ();
    virtual ~CBaseSqlite ();

     void *Thread(void *){;};
};

class CParametreApplication {

public:
    CParametreApplication ();
    virtual ~CParametreApplication ();

    PARAMETRE_APPLI *GetParamAppli () {return &stParamAppli;};
    PARAMETRE_APPLI *pstInit ();


private:



    short sInitProfil ();
    short sInitPlanification ();
    PARAMETRE_APPLI stParamAppli;
};

#endif //BASESQLITE_H
