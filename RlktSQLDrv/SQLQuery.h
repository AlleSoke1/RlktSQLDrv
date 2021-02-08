#pragma once

class CSQLQuery
{
public:
	CSQLQuery();

	void ErrorReport(int Type, SQLHANDLE hHandle);

	bool Query(SQLHSTMT hStmt, std::wstring strQuery, SQLResult* pResult = NULL, bool bDirect = false);
	virtual bool Close();

	virtual int GetNumRows();    //Row Count
	virtual int GetNumColumns(); //Result Columns

	bool FetchResult(SQLResult* pResult); 

	//
	void SetAutoCloseStmt(bool bFlag) { m_bAutoCloseStmt = bFlag; }

protected:
	SQLHSTMT m_hCurStmt;
	bool m_bAutoCloseStmt;
};