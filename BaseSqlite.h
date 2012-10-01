// BaseSqlite.h class.
//
//////////////////////////////////////////////////////////////////////
#ifndef BASESQLITE_H
#define BASESQLITE_H
#include <General.h>
#include <Thread.h>



typedef struct _PlanificationTACHE {
    enum  TYPE_TACHE eType;
    time_t tDebut;
    time_t tPeriod;
    time_t tRelance;
}PLANIFICATION_TACHE;

typedef struct _Profil  {
    char strNom [32+1];
    PLANIFICATION_TACHE stPlanif;
}PARAM_PROFIL;

typedef struct _Param  {
    PARAM_PROFIL stParamProfil [2];
}PARAMETRE_APPLI;

class CBaseSqlite : public CThread {

public:
    CBaseSqlite ();
    virtual ~CBaseSqlite ();
    PARAMETRE_APPLI *GetParamAppli () {return &stParamAppli;};
     void *Thread(void *){;};
private:

    PARAMETRE_APPLI stParamAppli;
};
#endif //BASESQLITE_H
