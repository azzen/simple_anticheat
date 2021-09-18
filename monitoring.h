#pragma once

extern game* iGame;

class monitoring
{
	GarrysMod::Lua::ILuaBase* LUA;
public:
	monitoring(GarrysMod::Lua::ILuaBase* _LUA) {
		LUA = _LUA;
	}
	void hookLibrarys();
};

