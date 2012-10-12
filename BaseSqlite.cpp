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
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stEphemeride.tDebut=8*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stEphemeride.tPeriod=12*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stEphemeride.tRelance=0;

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.usNbSegment=8;

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

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [2].stParamTemps.tDebut=stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [1].stParamTemps.tDebut+stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [1].stParamTemps.tPeriod;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [2].stParamTemps.tPeriod=2*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [2].stParamTemps.tRelance=0;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [2].usIntervalleX=60;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [2].stSegment.eType=E_COURBE_AFFINE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [2].stSegment.uCourbe.stCourbeAffine.dA=7.5;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [2].stSegment.uCourbe.stCourbeAffine.dB=57.5;

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [3].stParamTemps.tDebut=stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [2].stParamTemps.tDebut+stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [2].stParamTemps.tPeriod;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [3].stParamTemps.tPeriod=1*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [3].stParamTemps.tRelance=0;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [3].usIntervalleX=30;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [3].stSegment.eType=E_COURBE_AFFINE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [3].stSegment.uCourbe.stCourbeAffine.dA=5;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [3].stSegment.uCourbe.stCourbeAffine.dB=70;

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [4].stParamTemps.tDebut=stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [3].stParamTemps.tDebut+stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [3].stParamTemps.tPeriod;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [4].stParamTemps.tPeriod=1*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [4].stParamTemps.tRelance=0;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [4].usIntervalleX=30;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [4].stSegment.eType=E_COURBE_AFFINE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [4].stSegment.uCourbe.stCourbeAffine.dA=-5;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [4].stSegment.uCourbe.stCourbeAffine.dB=130;

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [5].stParamTemps.tDebut=stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [4].stParamTemps.tDebut+stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [4].stParamTemps.tPeriod;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [5].stParamTemps.tPeriod=2*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [5].stParamTemps.tRelance=0;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [5].usIntervalleX=60;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [5].stSegment.eType=E_COURBE_AFFINE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [5].stSegment.uCourbe.stCourbeAffine.dA=-7.5;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [5].stSegment.uCourbe.stCourbeAffine.dB=147.5;

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [6].stParamTemps.tDebut=stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [5].stParamTemps.tDebut+stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [5].stParamTemps.tPeriod;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [6].stParamTemps.tPeriod=2*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [6].stParamTemps.tRelance=0;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [6].usIntervalleX=1;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [6].stSegment.eType=E_COURBE_AFFINE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [6].stSegment.uCourbe.stCourbeAffine.dA=-15;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [6].stSegment.uCourbe.stCourbeAffine.dB=215;

            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [7].stParamTemps.tDebut=stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [6].stParamTemps.tDebut+stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [6].stParamTemps.tPeriod;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [7].stParamTemps.tPeriod=1*HEURE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [7].stParamTemps.tRelance=0;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [7].usIntervalleX=1;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [7].stSegment.eType=E_COURBE_AFFINE;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [7].stSegment.uCourbe.stCourbeAffine.dA=-37.5;
            stParamAppli.stParamProfil [iNumProfil].stParamSoleil.stCourbeSolaire.lSegment [7].stSegment.uCourbe.stCourbeAffine.dB=462.5;
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
            stParamAppli.stPlanifProfil [iNumPlanif].stPlanif.tPeriod=12*HEURE+0*MINUTE;
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

