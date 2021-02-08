#pragma once

//Includes
#include <windows.h>

#include <string>
#include <map>
#include <vector>

#include <sql.h>
#include <sqlext.h>

#include "Config.h"
#include "SQLResult.h"

//Macros
#define DELETE_PTR(p) if(p){ delete p; p = NULL; }


#define IS_SQLOK(ret) (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
#define IS_SQLERROR(ret) (ret == SQL_ERROR)
#define IS_SQLERRORINFO(ret) (ret == SQL_ERROR || ret == SQL_SUCCESS_WITH_INFO)

