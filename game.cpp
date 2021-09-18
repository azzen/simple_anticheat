#include "pch.h"
#include "game.h"
#include <iostream>


void game::print(std::string txt) {
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->GetField(-1, "print"); // _G["print"]

	LUA->PushString(txt.c_str());
	if (LUA->PCall(1, 0, 0) != 0)
		printf("error while printing: %s", LUA->GetString());

	LUA->Pop(1);
}

void game::appendToQueue(int _eventType, std::string _details) {
	struct detection detec = {_eventType, _details };
	detections.push(detec);
}
