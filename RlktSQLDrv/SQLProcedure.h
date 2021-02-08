#pragma once

class CSQLProcedure
{
public:
	enum SQLProcParamType
	{
		NONE,
		TINYINT,
		INTEGER,
		BIGINT,
		STRING
	};

	struct SQLProcParam
	{
		SQLProcParamType type;
		std::wstring str;     //NVARCHAR
		uint64_t i64Value;	  //BIGINT
		int iValue;			  //INTEGER
		unsigned char cValue; //BIT/TINYINT

		SQLProcParam() : type(SQLProcParamType::NONE), str(L""), i64Value(0), iValue(0), cValue(0) {}
		SQLProcParam(unsigned char cValue) : str(L""), i64Value(0), iValue(0), cValue(cValue) { type = SQLProcParamType::TINYINT; }
		SQLProcParam(int nValue) : str(L""), i64Value(0), iValue(nValue), cValue(0) { type = SQLProcParamType::INTEGER; }
		SQLProcParam(uint64_t nValue) : str(L""), i64Value(nValue), iValue(0), cValue(0) { type = SQLProcParamType::BIGINT; }
		SQLProcParam(std::wstring strValue) : str(strValue), i64Value(0), iValue(0), cValue(0) { type = SQLProcParamType::STRING; }
	};

	//Query	
	virtual bool Proc(std::wstring strProcName);
	virtual bool ProcParam(std::wstring strParamName, std::vector < SQLProcParam > param);

protected:
	SQLHSTMT* m_hStmt;
};
