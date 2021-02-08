#pragma once

enum SQLDataType
{
	TINYINT,
	SMALLINT,
	INTEGER,
	BIGINT,
	STRING, //varchar/nvarchar/text
};

struct SQLColumnData
{
	std::wstring name;
	SQLDataType type;
	SQLColumnData() {}
	SQLColumnData(int nDataType, std::wstring strColumnName) : type((SQLDataType)nDataType),  name(strColumnName) {}
};

struct SQLRow {
	//Data
	void* pData;

public:
	//Constr
	SQLRow() : pData(NULL) {}
	SQLRow(void *pPtr) : pData(pPtr) {}

	//Help func
	const wchar_t* GetWString() { return (const wchar_t*)pData; }
	const char* GetString() { return (const char*)pData; }

	char GetChar() { return *(char*)pData; }
	unsigned char GetByte() { return *(unsigned char*)pData; }
	short GetShort() { return *(short*)pData; }
	unsigned short GetUShort() { return *(unsigned short*)pData; }
	int GetInt() { return *(int*)pData; }
	unsigned int GetUInt() { return *(unsigned int*)pData; }
	uint64_t GetInt64() { return *(uint64_t*)pData; }

	float GetFloat() { return *(float*)pData; }
	double GetDouble() { return *(double*)pData; }

	__time32_t GetTime32() { return *(__time32_t*)pData; }
	__time64_t GetTime64() { return *(__time64_t*)pData; }
};

struct SQLRowData
{
	std::vector<SQLRow> pVecData; //pData will hold pointer to data of the result set

	~SQLRowData()
	{
		for (auto& it : pVecData)
		{
			free(it.pData);
			it.pData = NULL;
		}
	}
};

//Result set data
struct SQLResult
{
	int nSqlRet;						//SQL Stmt return SQL_OK/SQL_ERROR
	int nReturn;						//Return data from procedure
	std::vector<SQLColumnData*> vecColumns; //Column data
	std::vector<SQLRowData*>	vecData;	//Row data

	~SQLResult()
	{
		for (auto& it : vecColumns)
		{
			delete it;
			it = NULL;
		}

		for (auto& it : vecData)
		{
			delete it;
			it = NULL;
		}

		vecData.clear();
		vecColumns.clear();
	}

	int GetNumColumns() { return (int)vecColumns.size(); }
	int GetNumRows() { return (int)vecData.size(); }
};

//Helpers for data conversion
static int GetDataCType(SQLINTEGER Type)
{
	switch (Type)
	{
	case SQL_CHAR:
	case SQL_VARCHAR: return SQL_C_CHAR; break;

	case SQL_WLONGVARCHAR:
	case SQL_WVARCHAR: return SQL_C_WCHAR; break;

	case SQL_TINYINT: return SQL_C_TINYINT; break;
	case SQL_SMALLINT: return SQL_C_SHORT; break;
	case SQL_BIGINT: return SQL_C_SBIGINT; break; //bigint is always signed?

	default: return Type; break;
	}
}

static int GetDataTypeLen(SQLINTEGER Type, SQLINTEGER nSize)
{
	switch (Type)
	{
	case SQL_TINYINT:
		return sizeof(char); break;

	case SQL_SMALLINT:
		return sizeof(short); break;

	case SQL_INTEGER:
		return sizeof(int); break;

	case SQL_C_UBIGINT:
	case SQL_C_SBIGINT:
	case SQL_BIGINT:
		return sizeof(uint64_t); break;

	case SQL_REAL:
	case SQL_FLOAT:
		return sizeof(float); break;

	case SQL_DECIMAL:
	case SQL_NUMERIC:
	case SQL_DOUBLE:
		return sizeof(double); break;

	case SQL_VARCHAR:
	case SQL_CHAR:
		return sizeof(char) * nSize; break;

	case SQL_WLONGVARCHAR:
	case SQL_WVARCHAR:
	case SQL_WCHAR:
		return sizeof(wchar_t) * nSize; break;

#ifdef DEBUG
	default:
		fwprintf(stderr, L"[%s] Type[%d] not encased.\n", __FUNCTIONW__, Type);
		break;
#endif
	}

	// Unk?	
	return sizeof(void*);
}