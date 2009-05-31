#pragma once
#include "lua.h"
void RegJyLib(lua_State* L);
void JyInit(lua_State* L);
struct JYKey
{
	int key, bD;
};