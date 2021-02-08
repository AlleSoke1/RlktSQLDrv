#include "stdafx.h"
#include "test_query.h"

#include <string>

testquery::testquery()
	: Test("test_query")
	, CSQLConnection(0, "TEST QUERY")
{

}

void testquery::Process()
{
	SQLResult* pResult = new SQLResult();

	if (!Query(std::wstring(L"SELECT @@VERSION"), pResult))
	{
		printf("Exec failed...\n");
	}

	for (const auto& row : pResult->vecData)
	{
		for (auto& data : row->pVecData)
		{
			wprintf(L"GetVersion Result: %s\n", data.GetWString());
		}
	}

	DELETE_PTR(pResult);
}