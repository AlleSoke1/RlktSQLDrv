#include "stdafx.h"
#include "SQLResult.h"
#include "SQLQuery.h"
#include "SQLProcedure.h"
#include "SQLConnection.h"



CSQLConnection::CSQLConnection(int iConnID, std::string strConnName)
	: CSQLQuery()
	, CSQLProcedure()
	, m_iConnID(iConnID)
	, m_strConnName(strConnName)
	, m_hEnv(NULL)
	, m_hDbc(NULL)
	, m_hStmt(NULL)
	, m_bConnected(false)
{
	//add a critical section lock ?
	SetupEnv();
	TryConnect();
}

CSQLConnection::~CSQLConnection()
{
	//Gracefully close the connection and free handles
	if (m_hStmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, m_hStmt);
	}

	if (m_hDbc)
	{
		SQLDisconnect(m_hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, m_hDbc);
	}

	if (m_hEnv)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, m_hEnv);
	}

#ifdef DEBUG
	fwprintf(stdout, L"Connection[%hs:%d] closed.\n", m_strConnName.c_str(), m_iConnID);
#endif
}

const wchar_t* CSQLConnection::GetConnectionString()
{
	wchar_t tempConnString[4096] = { 0 };
	if (settings::DB_USE_SSPI)
	{
		swprintf_s(tempConnString, L"DRIVER=SQL Server;Server=%s,%s;Database=%s;Trusted_Connection=True;", settings::DB_HOST, settings::DB_PORT, settings::DB_NAME);
	}
	else if (settings::DB_USE_DSNFILE)
	{
		swprintf_s(tempConnString, L"DRIVER=SQL Server;FILEDSN=%s;", settings::DB_DSN);
	}
	else
	{
		swprintf_s(tempConnString, L"DRIVER=SQL Server;Server=%s,%s;Database=%s;UID=%s;PWD=%s", settings::DB_HOST, settings::DB_PORT, settings::DB_NAME, settings::DB_USER, settings::DB_PASS);
	}

	return tempConnString;
}

bool CSQLConnection::CheckAlive()
{
	return true;
}

bool CSQLConnection::SetupEnv()
{
	// Allocate an environment
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv) == SQL_ERROR)
	{
		fwprintf(stderr, L"Unable to allocate an environment handle\n");
		return false;
	}

	// Register this as an application that expects 3.x behavior,
	// you must register something if you use AllocHandle
	if (SQLSetEnvAttr(m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0) == SQL_ERROR)
	{
		fwprintf(stderr, L"Unable to set ODBC environment variables.\n");

		ErrorReport(SQL_HANDLE_ENV, m_hEnv);

		return false;
	}

	//Set QUERY to auto close/free stmts after each query execution.
	SetAutoCloseStmt(settings::AUTO_CLOSE_STMT);
	
	return true;
}

bool CSQLConnection::TryConnect()
{
	// Allocate a connection
	if (SQLAllocHandle(SQL_HANDLE_DBC, m_hEnv, &m_hDbc) == SQL_ERROR)
	{
		fwprintf(stderr, L"Unable to allocate an database connection handle\n");
		return false;
	}

#ifdef DEBUG
	wprintf(L"Connection String: %s\n", GetConnectionString());
#endif

	//Connect
	const SQLINTEGER OutConnSize = 255;
	SQLWCHAR OutConnStr[OutConnSize] = { 0 };
	SQLSMALLINT OutConnStrLen;
	if (SQLDriverConnect(m_hDbc,
		GetDesktopWindow(),
		(SQLWCHAR*)GetConnectionString(),
		SQL_NTS,
		OutConnStr,
		OutConnSize,
		&OutConnStrLen,
		SQL_DRIVER_NOPROMPT) == SQL_ERROR)
	{
		fwprintf(stderr, L"Could not open a sql connection to %s@%s:%s\n", settings::DB_USER, settings::DB_HOST, settings::DB_PORT);
		
		ErrorReport(SQL_HANDLE_DBC, m_hDbc);

		return false;
	}

#ifdef DEBUG
	printf("connected to %S.\n", settings::DB_HOST);
#endif

	m_bConnected = true;
	return true;
}

bool CSQLConnection::ReConnect()
{
	return TryConnect();
}

void CSQLConnection::ErrorReport(int Type, SQLHANDLE hHandle)
{
	//Get a more detailed error report.
	SQLSMALLINT nRecord = 0;
	SQLINTEGER nError = 0;
	wchar_t wszSqlMsg[2048] = { 0 };
	wchar_t wszSqlState[SQL_SQLSTATE_SIZE + 1] = { 0 };

	while (SQLGetDiagRec(Type, hHandle, ++nRecord, wszSqlState, &nError, wszSqlMsg, (SQLSMALLINT)(sizeof(wszSqlMsg) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS)
	{
		fwprintf(stderr, L"[%s][%5.5s] %s (%d)\n", __FUNCTIONW__, wszSqlState, wszSqlMsg, nError);
	}
}

bool CSQLConnection::Query(std::wstring strQuery, SQLResult* pResult /*= NULL*/,  bool bDirect /*= false*/)
{
	if (!m_bConnected)
	{
		fwprintf(stderr, L"[%s] SQL connection is not opened!\n", __FUNCTIONW__);
		return false;
	}

	//Alloc Statement handle
	if (SQLAllocHandle(SQL_HANDLE_STMT, m_hDbc, &m_hStmt) == SQL_ERROR)
	{
		fwprintf(stderr, L"Unable to allocate an statement handle\n");
		return false;
	}

	return CSQLQuery::Query(m_hStmt, strQuery, pResult, bDirect);
}

