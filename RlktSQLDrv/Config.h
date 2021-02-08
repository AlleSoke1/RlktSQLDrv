#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG 

namespace settings
{
	// Number of connections to be open with SQL Server.
	const int OPEN_CONNECTIONS = 10;

	static const bool DB_USE_SSPI = false;		 //Use TRUSTED_CONNECTION or SSPI in connection string.
	static const bool DB_USE_DSNFILE = false;	 //Use FILEDNS in connection string

	static const WCHAR* DB_DSN = L".//DbSettings.dsn";

	static const WCHAR* DB_HOST = L"10.1.1.240";
	static const WCHAR* DB_USER = L"test";
	static const WCHAR* DB_PASS = L"test";
	static const WCHAR* DB_NAME = L"test";
	static const WCHAR* DB_PORT = L"1433";

	//
	static bool AUTO_CLOSE_STMT = true;

	static bool DISABLE_DIRECT_QUERIES = false; //Disable plain text queries, only procedure execution allowed.

}

#endif