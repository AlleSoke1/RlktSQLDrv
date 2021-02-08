#pragma once

class CSQLManager
{
public:
	CSQLManager();
	~CSQLManager();

	CSQLConnection* GetConnection();

	struct Connection
	{
		CSQLConnection* pConn;
		bool			bBusy;

		Connection() : pConn(NULL), bBusy(false) {}
	};

protected:
	std::map<int, Connection> m_mapConnPool;
};

extern CSQLManager g_SQLManager;