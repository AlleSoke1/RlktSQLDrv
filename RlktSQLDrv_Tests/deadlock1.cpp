#include "stdafx.h"
#include "deadlock1.h"

#include <string>

deadlock1::deadlock1()
	: Test("deadlock-select") 
	, CSQLConnection(0)
{

}

void deadlock1::Process()
{
	if ( Query(std::wstring(L"EXEC test_deadlock_sel"), NULL, true) == false)
	{
		printf("deadlock1 query failed...\n");
#ifdef DEBUG
		DebugBreak();
#endif
	}
}