#pragma once

#include "SQLQuery.h"
#include "SQLProcedure.h"

class CSQLConnection : public CSQLQuery, public CSQLProcedure
{
public:
	CSQLConnection(int iConnID, std::string strConnName = "");
	~CSQLConnection();

	const wchar_t* GetConnectionString();
	bool CheckAlive();
	bool SetupEnv();
	bool TryConnect();
	bool ReConnect();

	//
	void ErrorReport(int Type, SQLHANDLE hHandle);

	//
	bool Query(std::wstring strQuery, SQLResult* pResult = NULL, bool bDirect = false);

	//
	std::string GetConnectionName() { return m_strConnName; }
	int GetConnectionID() { return m_iConnID; }

protected:
	int m_iConnID;
	std::string m_strConnName;

	SQLHDBC m_hDbc;
	SQLHENV m_hEnv;
	SQLHSTMT m_hStmt;

	bool m_bConnected;
};