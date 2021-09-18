#include "pch.h"
#include "config.h"
#include "game.h"

game* iGame;

#include "monitoring.h"
#include "memory_tools.h"
#include "crc32.h"

typedef int* (*luaL_loadbufferFn)(lua_State* L, const char* buff, size_t sz, const char* name);
typedef int* (*luaopen_debugFn)(lua_State* L);

using namespace GarrysMod::Lua;

LUA_FUNCTION(GetLoadedModules) {
	memory_tools* memory = new memory_tools();

	auto modules = memory->GetModules();

	delete memory;

	LUA->CreateTable();

	for (const auto& mod : modules) {
		char address[19];
		sprintf_s(address, "0x%p", mod.address);

		LUA->PushString(address);
		LUA->SetField(-2, mod.name.c_str());
	}

	return 1;
}

LUA_FUNCTION(FreeConsole) {
	FreeConsole();
	return 0;
}

LUA_FUNCTION(AllocConsole) {
	AllocConsole();
	FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;

	freopen_s(&fpstdin, "CONIN$", "r", stdin);
	freopen_s(&fpstdout, "CONOUT$", "w", stdout);
	freopen_s(&fpstderr, "CONOUT$", "w", stderr);
	return 0;
}

LUA_FUNCTION(CRC_str) {
	const char* dat = LUA->CheckString(-1);

	LUA->Pop(1);

	LUA->PushNumber(crc32buf((char*)dat, strlen(dat)));
	return 1;
}

LUA_FUNCTION(CRC_file) {
	if (LUA->Top() < 2) {
		LUA->PushBool(false);
		return 1;
	}

	const char* path = LUA->CheckString(-2);
	const char* salt = LUA->CheckString(-1);

	LUA->Pop(2);

	DWORD crc = crc32file((char*)path, (char*)salt);
	if (crc == false)
		LUA->PushBool(false);
	else
		LUA->PushNumber(crc);

	return 1;
}

LUA_FUNCTION(hooks) {

	monitoring* monitor = new monitoring(LUA);
	(*monitor).hookLibrarys();
	delete monitor;

	return 0;
}

LUA_FUNCTION(GetVersion) {
	LUA->PushString(VERSION);
	LUA->PushString(KEY);
	return 1;
}

LUA_FUNCTION(LoadBC) {
	const char* buffer = LUA->CheckString(-2);
	int len = (int)LUA->CheckNumber(-1);

	auto lua_shared = GetModuleHandleA("lua_shared.dll");
	if (lua_shared != 0) {
		auto luaL_loadbuffer = (luaL_loadbufferFn)GetProcAddress(lua_shared, "luaL_loadbuffer");
		luaL_loadbuffer(L, buffer, len, ":");
	}
	else {
		LUA->PushString("lua_shared.dll could not be accessed");
	}
	return 1;
}

LUA_FUNCTION(RestoreDebug) {
	auto lua_shared = GetModuleHandleA("lua_shared.dll");
	if (lua_shared != 0) {
		auto luaopen_debug = (luaopen_debugFn)GetProcAddress(lua_shared, "luaopen_debug");
		luaopen_debug(L);
		LUA->PushBool(true);
	}
	else
		LUA->PushBool(false);
	return 1;
}

LUA_FUNCTION(testhook) {
	iGame->appendToQueue(0, "test");
	return 0;
}

LUA_FUNCTION(GetDetections) {
	LUA->CreateTable();
	int i = 1;
	while (!iGame->detections.empty()) {
		LUA->PushNumber(i);

		LUA->CreateTable();

		auto &detect = iGame->detections.front();

		LUA->PushNumber(detect.eventType);
		LUA->SetField(-2, "type");
		LUA->PushString(detect.details.c_str());
		LUA->SetField(-2, "details");
		LUA->RawSet(-3);

		iGame->detections.pop();
		++i;
	}

	return 1;
}


void setFunctions(GarrysMod::Lua::ILuaBase* LUA) {
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // _G

	LUA->CreateTable();

	LUA->PushCFunction(GetLoadedModules);
	LUA->SetField(-2, "GetLoadedModules");

	LUA->PushCFunction(FreeConsole);
	LUA->SetField(-2, "FreeConsole");

	LUA->PushCFunction(AllocConsole);
	LUA->SetField(-2, "AllocConsole");

	LUA->PushCFunction(CRC_str);
	LUA->SetField(-2, "CRC_str");

	LUA->PushCFunction(CRC_file);
	LUA->SetField(-2, "CRC_file");

	LUA->PushCFunction(hooks);
	LUA->SetField(-2, "hooks");

	LUA->PushCFunction(GetVersion);
	LUA->SetField(-2, "GetVersion");

	LUA->PushCFunction(LoadBC);
	LUA->SetField(-2, "LoadBC");

	LUA->PushCFunction(RestoreDebug);
	LUA->SetField(-2, "RestoreDebug");

	LUA->PushCFunction(testhook);
	LUA->SetField(-2, "testhook");

	LUA->PushCFunction(GetDetections);
	LUA->SetField(-2, "GetDetections");

	LUA->SetField(-2, "simple_anticheat");

}

GMOD_MODULE_OPEN() {

	iGame = new game(LUA);

	iGame->print("[Simple-AC] Loaded !");
	setFunctions(LUA);

	return 0;
}

GMOD_MODULE_CLOSE() {
	delete iGame;
	FreeConsole();
	return 0;
}