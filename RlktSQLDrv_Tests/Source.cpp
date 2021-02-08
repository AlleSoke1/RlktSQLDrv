#include "stdafx.h"

#include "deadlock1.h"
#include "deadlock2.h"
#include "test_query.h"

void CreateTestFunc()
{
	CSQLConnection *conn = new CSQLConnection(-1, "Create tables & data");

	if (conn)
	{
		//Create test table
		std::wstring table = L"DROP TABLE IF EXISTS test; CREATE TABLE test(id INTEGER IDENTITY(1,1) NOT NULL, random VARCHAR(32) NOT NULL);";
		conn->Query(table);
		
		//Populate test table with data
		for (int i = 0; i < 1000; i++)
		{
			srand(time(NULL));
			std::string random_str = "";
			int nRandomLen = (rand() % 16);
			for (int j = 0; j <nRandomLen; j++)
			{
				char crnd = (rand()%2 ? 'A' : 'a')  + (rand() % 26);
				random_str += crnd;
				//random_str.append((const char)crnd);
			}

			std::string table_data = "INSERT INTO test (random) VALUES ('" + random_str + "');";
			conn->Query(std::wstring(table_data.begin(), table_data.end()));
		}

		//Create test proc
		std::string proc_sel = "\
			CREATE OR ALTER PROCEDURE test_deadlock_sel \
			AS \
			BEGIN TRAN\
			SELECT * FROM test\
			COMMIT TRAN\
		";

		conn->Query(std::wstring(proc_sel.begin(), proc_sel.end()));
			//Create test proc
			std::string proc_upd = "CREATE OR ALTER PROCEDURE test_deadlock_upd(@id int, @random varchar(32)) \
				AS \
				BEGIN TRAN \
					UPDATE test set random=@random where ID=@id \
				COMMIT TRAN \
		";
		conn->Query(std::wstring(proc_upd.begin(), proc_upd.end()));


		//invalid query
		SQLResult* pResult = new SQLResult();
		conn->Query(L"select * from test", pResult);
		
		for (auto& row : pResult->vecData)
		{
			wprintf(L"id: %d random: %hs\n", row->pVecData[0].GetInt(), row->pVecData[1].GetString());
		}

		DELETE_PTR(pResult);

		//close sql connection and free the obj
		DELETE_PTR(conn);
	}

}

int main()
{
	CreateTestFunc();

	//test query
	testquery testQuery;
	testQuery.RunTestOnce();
	
	//Deeadlock1 test thread - SELECT
	std::thread t1([]() {
		deadlock1* pDeadlock1 = new deadlock1();
		pDeadlock1->RunTest();
	});

	//Deadlock2 test thread - UPDATE
	std::thread t2([]() {
		deadlock2* pDeadlock2 = new deadlock2();
		pDeadlock2->RunTest();
	});

	//
	while (1)
	{
		Sleep(100);
	}

	return 0;
}