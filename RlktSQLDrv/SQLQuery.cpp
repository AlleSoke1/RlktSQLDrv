#include "stdafx.h"
#include "SQLResult.h"
#include "SQLQuery.h"

CSQLQuery::CSQLQuery()
	: m_hCurStmt(NULL)
	, m_bAutoCloseStmt(false)
{

}

void CSQLQuery::ErrorReport(int Type, SQLHANDLE hHandle)
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

/*
* Params:
* @bDirect: Close the statement, default is set to false.
* @pResult: SQLResult pointer with result set data and column info.
*/
bool CSQLQuery::Query(SQLHSTMT hStmt, std::wstring strQuery, SQLResult *pResult /*= NULL*/,  bool bDirect /*= false*/)
{	
	//In production allow only procedures to be executed.
	if (settings::DISABLE_DIRECT_QUERIES)
	{
		fprintf(stderr, "[%s][CRITICAL] DIRECT QUERIES NOT ALLOWED, ONLY PROCEDURE EXECUTION ALLOWED.\n", __FUNCTION__);
		return false;
	}

	//If there's an previous statement, close it.
	if (m_bAutoCloseStmt && hStmt != m_hCurStmt && m_hCurStmt != NULL)
	{
		if (IS_SQLERROR(SQLFreeStmt(m_hCurStmt, SQL_CLOSE)))
		{
			fprintf(stderr, "[%s] failed to close previous query statement.\n", __FUNCTION__);
			return false;
		}
	}

	//Set current stmt.
	m_hCurStmt = hStmt;

	SQLRETURN ret = SQLExecDirect(hStmt, (SQLWCHAR*)strQuery.c_str(), SQL_NTS);
	if (ret == SQL_ERROR)
	{
		SQLSMALLINT nRecord = 0;
		SQLINTEGER nError = 0;
		wchar_t wszSqlMsg[2048] = { 0 };
		wchar_t wszSqlState[SQL_SQLSTATE_SIZE+1] = { 0 };

		while( SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, nRecord, wszSqlState, &nError, wszSqlMsg, (SQLSMALLINT)(sizeof(wszSqlMsg) / sizeof(WCHAR)), (SQLSMALLINT*)NULL) == SQL_SUCCESS)
		{
			fwprintf(stderr, L"[FAILED QUERY][%s][%5.5s] %s (%d) Query[%s]\n", __FUNCTIONW__, wszSqlState, wszSqlMsg, nError, strQuery.c_str());
		}
		return false;
	}
	else if(IS_SQLOK(ret))
	{
		if (pResult)
		{
			pResult->nSqlRet = ret;
			FetchResult(pResult);
		}

		if (m_bAutoCloseStmt)
			Close();

		return true;
	}
	else
	{
		if (m_bAutoCloseStmt)
			Close();

		return false;
	}
}

bool CSQLQuery::Close()
{
	if (!m_hCurStmt)
		return false;

	if (IS_SQLERROR(SQLFreeStmt(m_hCurStmt, SQL_CLOSE)))
	{
		fprintf(stderr, "[%s] failed to close query statement.\n", __FUNCTION__);
		return false;
	}


	m_hCurStmt = NULL;

	return true;
}

/*
* SQLRowCount returns the number of rows affected by an UPDATE, INSERT, or DELETE statement
*/
int CSQLQuery::GetNumRows()
{
	if (!m_hCurStmt)
		return SQL_ERROR;

	SQLLEN nNumRows = 0;
	if (IS_SQLOK(SQLRowCount(m_hCurStmt, &nNumRows)))
		return nNumRows;

	return SQL_ERROR;
}

/*
* SQLNumResultCols returns the number of columns in a result set.
*/
int CSQLQuery::GetNumColumns()
{
	if (!m_hCurStmt)
		return SQL_ERROR;

	SQLSMALLINT nColumnCount = 0;
	if (IS_SQLOK(SQLNumResultCols(m_hCurStmt, &nColumnCount)))
		return nColumnCount;

	return SQL_ERROR;
}

/*
* Fetch data from result set into pResult.
*/
bool CSQLQuery::FetchResult(SQLResult* pResult)
{
	if (!m_hCurStmt)
		return false;

	SQLLEN cchDisplay = 0;    

	//Column binding for temporary data storage.
	struct ColBindingData
	{
		int cType;
		void* pData;
		SQLINTEGER nSize;
		SQLINTEGER nMaxDataLen;
	};
	std::vector<ColBindingData> vecBindData;

	//Fetch columns and data types
	int nNumCols = GetNumColumns();
	for (int iCol = 1; iCol <= nNumCols; iCol++)
	{
		ColBindingData bindData;
		
		//Fetch column datatype max size
		if (IS_SQLERROR(
			SQLColAttribute(m_hCurStmt,
				iCol,
				SQL_DESC_LENGTH,
				NULL,
				0,
				NULL,
				&bindData.nSize)))
		{
			fprintf(stderr, "[%s] failed to fetch column max size [%d].\n", __FUNCTION__, iCol);
			return false;
		}

		//Fetch column datatype
		if (IS_SQLERROR(
			SQLColAttribute(m_hCurStmt,
				iCol,
				SQL_DESC_CONCISE_TYPE,
				NULL,
				0,
				NULL,
				&bindData.cType)))
		{
			fprintf(stderr, "[%s] failed to fetch column type [%d].\n", __FUNCTION__, iCol);
			return false;
		}

		//Alloc memory for data buffer, to be free'd later.
		bindData.nMaxDataLen = GetDataTypeLen(bindData.cType, bindData.nSize + 1);
		bindData.pData = calloc(1, bindData.nMaxDataLen); //temp buffer, +1 in case of varchar/nvarchar

		//Bind columns with specific data type.
		SQLINTEGER len = 0;
		if (IS_SQLERRORINFO(
			SQLBindCol(m_hCurStmt, 
				iCol, 
				GetDataCType(bindData.cType),
				(SQLPOINTER)bindData.pData, 
				GetDataTypeLen(bindData.cType, bindData.nSize+1), 
				&len)
		))
		{
			ErrorReport(SQL_HANDLE_STMT, m_hCurStmt);
			fprintf(stderr, "[%s] failed to bind col [%d] Type[%d].\n", __FUNCTION__, iCol, bindData.cType);
			return false;
		}

		SQLINTEGER nColNameSize = 64;
		wchar_t wszColumnName[64]{ 0 };
		if (IS_SQLERROR(
			SQLColAttribute(m_hCurStmt,
			iCol,
			SQL_DESC_NAME,
			wszColumnName,
			sizeof(wszColumnName),
			NULL,
			NULL)))
		{
			fprintf(stderr, "[%s] failed to get column length [%d].\n", __FUNCTION__, iCol);
			return false;
		}

		wprintf(L"ColID[%d] DataType[%d] ColName[%s]\n", iCol, bindData.cType, wszColumnName);
		pResult->vecColumns.push_back(new SQLColumnData(bindData.cType, wszColumnName));
		vecBindData.push_back(bindData);
	}

	//Fetch Data   
	bool fNoData = false;
	while (!fNoData)
	{
		SQLRETURN ret = SQLFetch(m_hCurStmt);
		if (ret == SQL_NO_DATA_FOUND)
		{
			fNoData = true;
			break;
		}

		SQLRowData *row = new SQLRowData;
		
		for (const auto& it : vecBindData)
		{
			void* pData = malloc(it.nMaxDataLen);
			memcpy_s(pData, it.nMaxDataLen, it.pData, it.nMaxDataLen);

			row->pVecData.push_back( SQLRow(pData) );
			//printf("Size[%d] Buffer[%s]\n", it.nSize, (const char*)it.pData);
		}

		pResult->vecData.push_back(row);
	}

	//free temp buffers
	for (auto& it : vecBindData)
	{
		free(it.pData);
		it.pData = NULL;
	}
	vecBindData.clear();
}
