// BaseMysql.cpp: implementation of the CBaseMysql class.
//
//////////////////////////////////////////////////////////////////////
#include "BaseMysql.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBaseMysql::CBaseMysql():Connection (use_exceptions) {

}

CBaseMysql::~CBaseMysql()
{

}

int CBaseMysql::Select (Result& iRetour,string strFrom,string strSelect,string strWhere,unsigned int uiOffset,string strAppend) {
	int Retour=-1;

	try {
		if (ping()==0) {
			
			// requete sur la table scrutation 
			string strRequete;

			Query Requete = query();			
			strRequete.Format ("select %s from %s where %s %s",strSelect,strFrom,strWhere,strAppend);
			Requete<<strRequete.GetBuffer (0);
			iRetour = Requete.store ();
			if (iRetour)
				Retour=iRetour.size ();		
			
		}
	} catch (BadQuery er){ // handle any connection 
		// or query errors that may come up
		LastErr=er.error.c_str ();
		::AfxMessageBox (LastErr,IDOK,0);
	}catch (BadConversion er) {    
		LastErr=er.data.c_str ();
		::AfxMessageBox (LastErr,IDOK,0);
	}
	return Retour;
}

int CBaseMysql::Select (string strFrom,string strSelect,string strWhere,string strAppend,unsigned int uiOffset) {
	int Retour=-1;

	try {
		if (ping()==0) {
			string strRequete;						
			Query Requete = query();			
			strRequete.Format ("select %s from %s where %s %s",strSelect,strFrom,strWhere,strAppend);
			Requete<<strRequete.GetBuffer (0);
			ResultatRequete = Requete.store ();
			if (ResultatRequete)
				Retour=ResultatRequete.size ();
			
		}
	} catch (BadQuery er){ // handle any connection 
		// or query errors that may come up
		LastErr=er.error.c_str ();
		::AfxMessageBox (LastErr,IDOK,0);
	}catch (BadConversion er) {    
		LastErr=er.data.c_str ();
		::AfxMessageBox (LastErr,IDOK,0);
	}
	return Retour;
}

int CBaseMysql::Replace (string strFrom,string strCols,string strValues,string strAppend) {
	int Retour=-1;

	try {
		if (ping()==0) {
			
			// requete sur la table scrutation 
			string strRequete;
			
			
			Query Requete = query();			
			strRequete.Format ("replace into %s (%s) values (%s) %s",strFrom,strCols,strValues,strAppend);
			Requete<<strRequete.GetBuffer (0);
			Requete.store ();

			if (Requete.success ()) {
				Retour=insert_id();
			}
			
		}
	} catch (BadQuery er){ // handle any connection 
		// or query errors that may come up
		LastErr=er.error.c_str ();
		//::AfxMessageBox (LastErr,IDOK,0);
	}catch (BadConversion er) {    
		LastErr=er.data.c_str ();
		//::AfxMessageBox (LastErr,IDOK,0);
	}
	return Retour;
}

int CBaseMysql::Update (string strFrom,string strCol,string strValue,string strWhere/*="1"*/,string strAppend/*=""*/) {
	int Retour=-1;

	try {
		if (ping()==0) {
			 
			string strRequete;
			
			
			Query Requete = query();			
			strRequete.Format ("update %s set %s=%s where %s %s",strFrom,strCol,strValue,strWhere,strAppend);
			Requete<<strRequete.GetBuffer (0);
			Requete.store ();

			
			Retour=Requete.success ();
			
			
		}
	} catch (BadQuery er){ // handle any connection 
		// or query errors that may come up
		LastErr=er.error.c_str ();
		//::AfxMessageBox (LastErr,IDOK,0);
	}catch (BadConversion er) {    
		LastErr=er.data.c_str ();
		//::AfxMessageBox (LastErr,IDOK,0);
	}
	return Retour;
}


int CBaseMysql::Delete (string strFrom,string strCol/*="*"*/,string strWhere/*="1"*/,string strAppend/*=""*/) {
	int Retour=-1;

	try {
		if (ping()==0) {
			 
			string strRequete;
			
			
			Query Requete = query();			
			strRequete.Format ("delete %s from %s where %s %s",strCol,strFrom,strWhere,strAppend);
			Requete<<strRequete.GetBuffer (0);
			Requete.store ();

			
			Retour=Requete.success ();
			
			
		}
	} catch (BadQuery er){ // handle any connection 
		// or query errors that may come up
		LastErr=er.error.c_str ();
		//::AfxMessageBox (LastErr,IDOK,0);
	}catch (BadConversion er) {    
		LastErr=er.data.c_str ();
		//::AfxMessageBox (LastErr,IDOK,0);
	}
	return Retour;
}


const Row CBaseMysql::GetResult (Result &ResultRequete) {
	return ResultRequete.fetch_row ();	
}

const Row CBaseMysql::GetResult () {
	return ResultatRequete.fetch_row ();	
}


//2005-05-02 10:09:09
string CBaseMysql::ConvertirDateToStr (time_t iDate) {	
	string strDate;
	char tmpstrDate [255];
	struct tm tmDate;
	tmDate=*gmtime (&iDate);	
	strftime(tmpstrDate,sizeof (tmpstrDate),"%Y-%m-%d %H:%M:%S",&tmDate);
	strDate=tmpstrDate;
	return strDate;
}


DateTime CBaseMysql::ConvertirDate (time_t iDate) {
	DateTime DateRetour;
	char strDate [255];
	struct tm tmDate;

	tmDate=*gmtime (&iDate);
	
	strftime(strDate,sizeof (strDate),"%Y-%m-%d %H:%M:%S",&tmDate);


	DateRetour.convert (strDate);


	return DateRetour;
}