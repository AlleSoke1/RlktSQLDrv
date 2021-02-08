#include "stdafx.h"
#include <windows.h>

#include <string>
#include <map>

#include "SQLConnection.h"
#include "SQLManager.h"

CSQLManager g_SQLManager;

CSQLManager::CSQLManager()
{
	// Open connection threads
	for (int i = 0; i < settings::OPEN_CONNECTIONS; i++)
	{
		//
		CSQLConnection* pConn = new CSQLConnection(i);

		//
		Connection conn;
		conn.pConn = pConn;
		m_mapConnPool.insert({ i, conn });
	}
}

CSQLManager::~CSQLManager()
{
	for (auto& it : m_mapConnPool)
		DELETE_PTR(it.second.pConn);
}


CSQLConnection* CSQLManager::GetConnection()
{
	for (const auto& it : m_mapConnPool)
	{
		if (!it.second.bBusy)
		{
			return it.second.pConn;
		}
	}

	throw("All SQL Connections are Busy!");
	return NULL;
}