#include "stdafx.h"
#include "SQLConnection.h"
#include "deadlock2.h"

#include <string>

deadlock2::deadlock2()
	: Test("deadlock-update")
	, CSQLConnection(1)
{

}

void deadlock2::Process()
{
	if (Query(std::wstring(L"EXEC test_deadlock_sel"), NULL, true) == false)
	{
		printf("deadlock2 query failed...\n");
#ifdef DEBUG
		DebugBreak();
#endif
	}
}