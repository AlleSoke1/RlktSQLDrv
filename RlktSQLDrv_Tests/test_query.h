#pragma once

class CSQLConnection;
class testquery : public Test, public CSQLConnection
{
public:
	testquery();
	virtual void Process();
};