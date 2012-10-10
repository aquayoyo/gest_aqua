#include <BaseSqlite.h>

CParametreApplication ::CParametreApplication () {
    memset (&stParamAppli,0,sizeof (PARAMETRE_APPLI));
    pstInit();
}

CParametreApplication ::~CParametreApplication () {
}

PARAMETRE_APPLI *CParametreApplication ::pstInit () {
    int iErr=0;
    PARAMETRE_APPLI stOldParam;
    memcpy (&stOldParam,&stParamAppli,sizeof (PARAMETRE_APPLI));

    iErr=sInitProfil ();
    stParamAppli.stParamChange.bParamProfil|=memcmp (stOldParam.stParamProfil,stParamAppli.stParamProfil,sizeof(PARAM_PROFIL)*MAX_PROFIL);
    iErr|=sInitPlanification ();
    stParamAppli.stParamChange.bParamPlanifProfil|=memcmp (stOldParam.stPlanifProfil,stParamAppli.stPlanifProfil,sizeof(DEF_PLANIFICATION_PROFIL)*MAX_PLANIF_PROFIL);
    return iErr?NULL:&stParamAppli;
}

short CParametreApplication ::sInitProfil () {
    short sErr=0;
    for (int iNumProfil=0;iNumProfil<MAX_PROFIL;iNumProfil++) {
        memset (&stParamAppli.stParamProfil [iNumProfil],0,sizeof (PARAM_PROFIL));
        sprintf (stParamAppli.stParamProfil [iNumProfil].strNom,"Profil%02d",iNumProfil+1);
        stParamAppli.stParamProfil [iNumProfil].ucNumeroProfil=iNumProfil+1;
        switch (iNumProfil) {
            case 0:
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.ucValide=1;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.ucAzimut=100;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stEphemeride.eType=TACHE_SOLEIL;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stEphemeride.tDebut=15*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stEphemeride.tPeriod=11*HEURE+59*MINUTE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stEphemeride.tRelance=0;

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.usNbSegment=2;

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [0].stParamTemps.tDebut=stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stEphemeride.tDebut;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [0].stParamTemps.tPeriod=1*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [0].stParamTemps.tRelance=0;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [0].usIntervalleX=1;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [0].stSegment.eType=E_COURBE_AFFINE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [0].stSegment.uCourbe.stCourbeAffine.dA=37.5;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [0].stSegment.uCourbe.stCourbeAffine.dB=12.5;

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [1].stParamTemps.tDebut=stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [0].stParamTemps.tDebut+stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [0].stParamTemps.tPeriod;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [1].stParamTemps.tPeriod=2*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [1].stParamTemps.tRelance=0;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [1].usIntervalleX=1;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [1].stSegment.eType=E_COURBE_AFFINE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [1].stSegment.uCourbe.stCourbeAffine.dA=15;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [1].stSegment.uCourbe.stCourbeAffine.dB=35;


            break;
            case 1:
            break;
        }
    }
    return sErr;
}

short CParametreApplication ::sInitPlanification () {
    short sErr=0;
    for (int iNumPlanif=0;iNumPlanif<MAX_PLANIF_PROFIL;iNumPlanif++) {
        stParamAppli.stPlanifProfil [iNumPlanif].ucNumeroPlanif=iNumPlanif+1;
        stParamAppli.stPlanifProfil [iNumPlanif].stPlanif.eType=TACHE_PROFIL;
        switch (iNumPlanif) {
            case 0:
            stParamAppli.stPlanifProfil [iNumPlanif].ucNumeroProfil=1;
            stParamAppli.stPlanifProfil [iNumPlanif].stPlanif.tDebut=8*HEURE;
            stParamAppli.stPlanifProfil [iNumPlanif].stPlanif.tPeriod=12*HEURE;
            break;
            case 1:
            stParamAppli.stPlanifProfil [iNumPlanif].ucNumeroProfil=2;
            stParamAppli.stPlanifProfil [iNumPlanif].stPlanif.tDebut=20*HEURE;
            stParamAppli.stPlanifProfil [iNumPlanif].stPlanif.tPeriod=12*HEURE;
            break;
        }
        stParamAppli.stPlanifProfil [iNumPlanif].stPlanif.tRelance=0;
    }
    return sErr;
}

