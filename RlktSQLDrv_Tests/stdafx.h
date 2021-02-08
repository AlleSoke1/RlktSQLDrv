#pragma once

#include <Windows.h>

#include <vector>
#include <string>
#include <thread>

#include <sql.h>
#include <sqlext.h>

#include "SQLResult.h"
#include "SQLConnection.h"
#include "RlktTest.h"


//Macros
#define DELETE_PTR(p) if(p){ delete p; p = NULL; }