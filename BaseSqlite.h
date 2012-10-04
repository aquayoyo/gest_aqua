// BaseSqlite.h class.
//
//////////////////////////////////////////////////////////////////////
#ifndef BASESQLITE_H
#define BASESQLITE_H
#include <General.h>
#include <Thread.h>

#define MAX_PROFIL 2
#define MAX_PLANIF_PROFIL 2

typedef struct _PlanificationTACHE {
    enum  TYPE_TACHE eType;
    time_t tDebut;
    time_t tPeriod;
    time_t tRelance;
}PLANIFICATION_TACHE;

typedef struct _Soleil  {
    unsigned char ucValide;
    unsigned char ucAzimut;
    PLANIFICATION_TACHE stEphemeride;
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
		ULONG bParamProfil  : 1;
		ULONG bParamPlanifProfil       : 1;
		ULONG bVide  : 62;

	};
	ULONGLONG	ul;
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
