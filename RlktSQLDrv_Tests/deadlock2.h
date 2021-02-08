#pragma once

class CSQLConnection;
class deadlock2 : public Test, public CSQLConnection
{
public:
	deadlock2();
	virtual void Process();
};