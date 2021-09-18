#pragma once

class game
{
public:
	struct detection {
		int eventType;
		std::string details;
	};

private:
	GarrysMod::Lua::ILuaBase* LUA;

public:
	std::queue<detection> detections;

	game(GarrysMod::Lua::ILuaBase* _LUA) {
		LUA = _LUA;
	}

	void print(std::string txt);

	void appendToQueue(int eventType, std::string details);
	
};

