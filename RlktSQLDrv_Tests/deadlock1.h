#pragma once

class CSQLConnection;
class deadlock1 : public Test, public CSQLConnection
{
public:
	deadlock1();
	virtual void Process();
};