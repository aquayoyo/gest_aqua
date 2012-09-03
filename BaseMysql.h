// BaseMysql.h: interface for the CBaseMysql class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEMYSQL_H__EBBDCB85_433E_4367_B15A_A37D9D572F66__INCLUDED_)
#define AFX_BASEMYSQL_H__EBBDCB85_433E_4367_B15A_A37D9D572F66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;


class CBaseMysql : public Connection  
{
	string LastErr;
	Result ResultatRequete;
public:
	CBaseMysql();
	virtual ~CBaseMysql();
	int Select (Result&,string strFrom,string strSelect,string strWhere="1",unsigned int uiOffset=0,string strAppend="");
	int Select (string strFrom,string strSelect,string strWhere="1",string strAppend="",unsigned int uiOffset=0);

	int Replace (string strFrom,string strCols,string strValues,string strAppend="");
	int Update (string strFrom,string strCol,string strValue,string strWhere="1",string strAppend="");
	int Delete (string strFrom,string strCol="*",string strWhere="1",string strAppend="");
	const Row GetResult (Result &ResultRequete);

	const Row GetResult ();

	string GetLastErr (){return LastErr;};

	DateTime ConvertirDate (time_t iDate);
	string ConvertirDateToStr (time_t iDate);
};

#endif // !defined(AFX_BASEMYSQL_H__EBBDCB85_433E_4367_B15A_A37D9D572F66__INCLUDED_)
