/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com			
			(C) Alexander Blade 2015
*/

///Mod by YUNoCake

#include "script.h"
#include <string>
#include <fstream>
#include <ctime>

int key, playerID;

void readConfigFile()
{
	std::ifstream file("Kill Frenzy.ini");
	std::string temp;
	std::getline(file, temp);
	key = atoi(temp.c_str());
}

bool get_key_pressed(int nVirtKey)
{
	return (GetAsyncKeyState(nVirtKey) & 0x8000) != 0;
}

int $(char *hash)
{
	return GAMEPLAY::GET_HASH_KEY(hash);
}

std::string statusText;
DWORD statusTextDrawTicksMax;
bool statusTextGxtEntry;

void update_status_text()
{
	if (GetTickCount() < statusTextDrawTicksMax)
	{
		UI::SET_TEXT_FONT(0);
		UI::SET_TEXT_SCALE(3.5, 3.5);
		UI::SET_TEXT_COLOUR(230, 92, 0, 255);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(1);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(0, 87, 46, 176, 255);
		if (statusTextGxtEntry)
		{
			UI::_SET_TEXT_ENTRY((char *)statusText.c_str());
		}
		else
		{
			UI::_SET_TEXT_ENTRY("STRING");
			UI::_ADD_TEXT_COMPONENT_STRING((char *)statusText.c_str());
		}
		UI::_DRAW_TEXT(0.5, 0.4);
	}
}

void update_timer_text()
{
	if (GetTickCount() < statusTextDrawTicksMax)
	{
		UI::SET_TEXT_FONT(0);
		UI::SET_TEXT_SCALE(0.70, 0.70);
		UI::SET_TEXT_COLOUR(255, 255, 255, 255);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(1);
		UI::SET_TEXT_DROPSHADOW(10, 0, 0, 0, 200);
		UI::SET_TEXT_EDGE(0, 0, 0, 0, 205);
		if (statusTextGxtEntry)
		{
			UI::_SET_TEXT_ENTRY((char *)statusText.c_str());
		}
		else
		{
			UI::_SET_TEXT_ENTRY("STRING");
			UI::_ADD_TEXT_COMPONENT_STRING((char *)statusText.c_str());
		}
		UI::_DRAW_TEXT(0.5, 0.0);
	}
}

void update_ask_text()
{
	if (GetTickCount() < statusTextDrawTicksMax)
	{
		UI::SET_TEXT_FONT(1);
		UI::SET_TEXT_SCALE(0.90, 0.90);
		UI::SET_TEXT_COLOUR(255, 255, 255, 255);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(0);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(0, 0, 0, 0, 205);
		if (statusTextGxtEntry)
		{
			UI::_SET_TEXT_ENTRY((char *)statusText.c_str());
		}
		else
		{
			UI::_SET_TEXT_ENTRY("STRING");
			UI::_ADD_TEXT_COMPONENT_STRING((char *)statusText.c_str());
		}
		UI::_DRAW_TEXT(0.0, 0.2);
	}
}

void set_status_text(std::string str, DWORD time = 2500, bool isGxtEntry = false)
{
	statusText = str;
	statusTextDrawTicksMax = GetTickCount() + time;
	statusTextGxtEntry = isGxtEntry;
}

void addCash(long amount)
{
	char statusName[32];
	sprintf_s(statusName, "SP%d_TOTAL_CASH", playerID);
	Hash hash = $(statusName);
	int cash;
	STATS::STAT_GET_INT(hash, &cash, -1);
	cash += amount;
	STATS::STAT_SET_INT(hash, cash, 1);
}

void addAttributes()
{
	Player player = PLAYER::PLAYER_ID();

	PLAYER::SET_PLAYER_WANTED_LEVEL(player, 0, false);
	PLAYER::SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

void frenzyMessage(std::string message)
{
	DWORD time = GetTickCount() + 2500;

	set_status_text(message);

	while (GetTickCount() < time)
	{
		update_status_text();
		WAIT(0);
	}
}

void infiniteAmmo(char *weaponName)
{
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (WEAPON::IS_WEAPON_VALID($(weaponName)))
	{
		int maxClipAmmo;
		maxClipAmmo = WEAPON::GET_MAX_AMMO_IN_CLIP(playerPed, $(weaponName), 1);
		if (maxClipAmmo > 0)
			WEAPON::SET_AMMO_IN_CLIP(playerPed, $(weaponName), maxClipAmmo);
	}
}

void checkIfDestroyed(int killedPedsBeforeFrenzy, int totalPedsToKill, int &pedsToKill)
{
	int destroyedCars;

	char statusName[32];
	sprintf_s(statusName, "SP%d_CARS_EXPLODED", playerID);
	Hash hash = $(statusName);
    STATS::STAT_GET_INT(hash, &destroyedCars, -1);

	destroyedCars -= killedPedsBeforeFrenzy; 
	pedsToKill = totalPedsToKill - destroyedCars;
}

void checkIfDead(int killedPedsBeforeFrenzy, int totalPedsToKill, int &pedsToKill, bool copsOnly)
{
	int killedPeds;
	int killedInnocents = 0, killedCops = 0, killedSwat = 0;
	char statusName[32];
	Hash hash;

	if (copsOnly == false)
	{
		sprintf_s(statusName, "SP%d_KILLS_INNOCENTS", playerID);
		hash = $(statusName);
		STATS::STAT_GET_INT(hash, &killedInnocents, -1);
	}
	
	sprintf_s(statusName, "SP%d_KILLS_COP", playerID);
	hash = $(statusName);
	STATS::STAT_GET_INT(hash, &killedCops, -1);

	sprintf_s(statusName, "SP%d_KILLS_SWAT", playerID);
	hash = $(statusName);
	STATS::STAT_GET_INT(hash, &killedSwat, -1);
	
	killedPeds = killedInnocents + killedCops + killedSwat;
	killedPeds -= killedPedsBeforeFrenzy;
	pedsToKill = totalPedsToKill - killedPeds;
}

void convertToString(bool carFrenzy, int frenzyTime, int pedsToKill, std::string &stringTime, bool copsOnly)
{
	std::string stringMinutes, stringSeconds, stringPedsToKill;
	int seconds, minutes;
	seconds = frenzyTime;
	minutes = frenzyTime / 60;
	seconds = seconds - minutes * 60;
	stringMinutes = std::to_string(minutes);
	if (minutes < 10) stringMinutes = "0" + stringMinutes;
	stringSeconds = std::to_string(seconds);
	if (seconds < 10) stringSeconds = "0" + stringSeconds;
	stringPedsToKill = std::to_string(pedsToKill);
	if (copsOnly)
		stringTime = "Cops to kill: " + stringPedsToKill + '\n' + stringMinutes + ":" + stringSeconds;
	else if (carFrenzy == false)
		stringTime = "Pedestrians to kill: " + stringPedsToKill + '\n' + stringMinutes + ":" + stringSeconds;
	else
		stringTime = "Vehicles to destroy: " + stringPedsToKill + '\n' + stringMinutes + ":" + stringSeconds;
}

void startTimer(bool carFrenzy, int &frenzyTime, int killedPedsBeforeFrenzy, int totalPedsToKill, int pedsToKill, char *weaponName, bool copsOnly)
{
	Player player = PLAYER::PLAYER_ID();
	std::string stringTime;
	DWORD oneSecond;

	while (frenzyTime >= 0 && pedsToKill > 0)
	{
		convertToString(carFrenzy, frenzyTime, pedsToKill, stringTime, copsOnly);
		set_status_text(stringTime);
		oneSecond = GetTickCount() + 1000;
		while (GetTickCount() < oneSecond)
		{
			update_timer_text();
			if (carFrenzy == false)
				checkIfDead(killedPedsBeforeFrenzy, totalPedsToKill, pedsToKill, copsOnly);
			else
				checkIfDestroyed(killedPedsBeforeFrenzy, totalPedsToKill, pedsToKill);
			infiniteAmmo(weaponName);
			WAIT(0);
		}
		if (PLAYER::IS_PLAYER_DEAD(player)) frenzyTime = 0;
		frenzyTime--;
	}
}

void totalDestroyedCars(int &killedPedsBeforeFrenzy)
{
	char statusName[32];

	sprintf_s(statusName, "SP%d_CARS_EXPLODED", playerID);
	Hash hash = $(statusName);
	STATS::STAT_GET_INT(hash, &killedPedsBeforeFrenzy, -1);
}

void totalDeadPeds(int &killedPedsBeforeFrenzy, bool copsOnly)
{
	int killedInnocents, killedCops, killedSwat;
	char statusName[32];

	sprintf_s(statusName, "SP%d_KILLS_INNOCENTS", playerID);
	Hash hash = $(statusName);
	STATS::STAT_GET_INT(hash, &killedInnocents, -1);

	if (copsOnly) killedInnocents = 0;

	sprintf_s(statusName, "SP%d_KILLS_COP", playerID);
	hash = $(statusName);
	STATS::STAT_GET_INT(hash, &killedCops, -1);

	sprintf_s(statusName, "SP%d_KILLS_SWAT", playerID);
	hash = $(statusName);
	STATS::STAT_GET_INT(hash, &killedSwat, -1);

	killedPedsBeforeFrenzy = killedInnocents + killedCops + killedSwat;
}

void giveWeapon(char *weaponName)
{
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	int maxClipAmmo = WEAPON::GET_MAX_AMMO_IN_CLIP(playerPed, $(weaponName), 1);
	WEAPON::GIVE_WEAPON_TO_PED(playerPed, $(weaponName), maxClipAmmo, true, true);
}

void startKillFrenzy(bool carFrenzy, int frenzyTime, int totalPedsToKill, int reward, char *weaponName, bool copsOnly)
{
	Player player = PLAYER::PLAYER_ID();
	int pedsToKill,killedPedsBeforeFrenzy;

	giveWeapon(weaponName);
	frenzyMessage("KILL FRENZY!");
	pedsToKill = totalPedsToKill;
	if (carFrenzy == false)
		totalDeadPeds(killedPedsBeforeFrenzy, copsOnly);
	else
		totalDestroyedCars(killedPedsBeforeFrenzy);
	startTimer(carFrenzy, frenzyTime, killedPedsBeforeFrenzy, totalPedsToKill, pedsToKill, weaponName, copsOnly);
	if (PLAYER::IS_PLAYER_DEAD(player) == false) addAttributes();
	if (frenzyTime <= 0)
	{
		frenzyMessage("FRENZY FAILED");
	}
	else
	{
		frenzyMessage("FRENZY PASSED");
		addCash(10000);
		GAMEPLAY::DO_AUTO_SAVE();
	}
	if (PLAYER::IS_PLAYER_DEAD(player) == false) addAttributes();
}

void askToStart(bool carFrenzy, int frenzyTime, int pedsToKill, int reward, char *weaponName, bool copsOnly)
{
	set_status_text("Press the assigned key\nto start the Kill Frenzy!");
	DWORD oneSecond = GetTickCount() + 1000;
	while (GetTickCount() < oneSecond)
	{
		update_ask_text();
		if (get_key_pressed(key)) startKillFrenzy(carFrenzy, frenzyTime, pedsToKill, reward, weaponName, copsOnly);
		WAIT(0);
	}
}

void drawMarkers()
{
	Vector3 playerPosition;

	playerPosition = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 0.0, 0.0);

	//Main LS Customs
	GRAPHICS::DRAW_MARKER(0, -354.931, -125.301, 39.431, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 0, 143, 0, 100, false, true, 2, false, false, false, false);
	if (-356.931 <= playerPosition.x && -352.931 >= playerPosition.x && -127.301 <= playerPosition.y && -123.301 >= playerPosition.y && 37.431 <= playerPosition.z && 41.431 >= playerPosition.z)
	{
		while (-356.931 <= playerPosition.x && -352.931 >= playerPosition.x && -127.301 <= playerPosition.y && -123.301 >= playerPosition.y && 37.431 <= playerPosition.z && 41.431 >= playerPosition.z)
		{
			askToStart(false, 60, 5, 10000, "WEAPON_MICROSMG", true);
			playerPosition = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 0.0, 0.0);
		}
	}
	//LS Customs 2
	GRAPHICS::DRAW_MARKER(0, 721.836, -1070.22, 23.0624, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 0, 143, 0, 100, false, true, 2, false, false, false, false);
	if (719.836 <= playerPosition.x && 723.836 >= playerPosition.x && -1072.22 <= playerPosition.y && -1068.22 >= playerPosition.y && 21.0624 <= playerPosition.z && 25.0624 >= playerPosition.z)
	{
		while (719.836 <= playerPosition.x && 723.836 >= playerPosition.x && -1072.22 <= playerPosition.y && -1068.22 >= playerPosition.y && 21.0624 <= playerPosition.z && 25.0624 >= playerPosition.z)
		{
			askToStart(true, 60, 10, 20000, "WEAPON_RPG", false);
			playerPosition = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 0.0, 0.0);
		}
	}
	//LS Customs 3
	GRAPHICS::DRAW_MARKER(0, -1141.92, -1992.51, 13.1642, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 0, 143, 0, 100, false, true, 2, false, false, false, false);
	if (-1143.92 <= playerPosition.x && -1139.92 >= playerPosition.x && -1994.51 <= playerPosition.y && -1990.51 >= playerPosition.y && 11.1642 <= playerPosition.z && 15.1642 >= playerPosition.z)
	{
		while (-1143.92 <= playerPosition.x && -1139.92 >= playerPosition.x && -1994.51 <= playerPosition.y && -1990.51 >= playerPosition.y && 11.1642 <= playerPosition.z && 15.1642 >= playerPosition.z)
		{
			askToStart(false, 180, 30, 50000, "WEAPON_RPG", false);
			playerPosition = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 0.0, 0.0);
		}
	}
	//LS Customs 4
	GRAPHICS::DRAW_MARKER(0, 1205.62, 2657.84, 37.827, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 0, 143, 0, 100, false, true, 2, false, false, false, false);
	if (1203.62 <= playerPosition.x && 1207.62 >= playerPosition.x && 2655.84 <= playerPosition.y && 2659.84 >= playerPosition.y && 35.827 <= playerPosition.z && 39.827 >= playerPosition.z)
	{
		while (1203.62 <= playerPosition.x && 1207.62 >= playerPosition.x && 2655.84 <= playerPosition.y && 2659.84 >= playerPosition.y && 35.827 <= playerPosition.z && 39.827 >= playerPosition.z)
		{
			askToStart(false, 60, 5, 50000, "WEAPON_PISTOL", true);
			playerPosition = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 0.0, 0.0);
		}
	}
	//LS Customs 5
	GRAPHICS::DRAW_MARKER(0, 133.336, 6637.04, 31.7842, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 0, 143, 0, 100, false, true, 2, false, false, false, false);
	if (131.336 <= playerPosition.x && 135.336 >= playerPosition.x && 6635.04 <= playerPosition.y && 6639.04 >= playerPosition.y && 29.7842 <= playerPosition.z && 33.7842 >= playerPosition.z)
	{
		while (131.336 <= playerPosition.x && 135.336 >= playerPosition.x && 6635.04 <= playerPosition.y && 6639.04 >= playerPosition.y && 29.7842 <= playerPosition.z && 33.7842 >= playerPosition.z)
		{
			askToStart(false, 60, 10, 25000, "WEAPON_RPG", true);
			playerPosition = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 0.0, 0.0);
		}
	}
	//AmmuNation 1
	GRAPHICS::DRAW_MARKER(0, -3157.51, 1090.03, 20.8561, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 0, 143, 0, 100, false, true, 2, false, false, false, false);
	if (-3159.51 <= playerPosition.x && -3155.51 >= playerPosition.x && 1088.03 <= playerPosition.y && 1092.03 >= playerPosition.y && 18.8561 <= playerPosition.z && 22.8561 >= playerPosition.z)
	{
		while (-3159.51 <= playerPosition.x && -3155.51 >= playerPosition.x && 1088.03 <= playerPosition.y && 1092.03 >= playerPosition.y && 18.8561 <= playerPosition.z && 22.8561 >= playerPosition.z)
		{
			askToStart(false, 120, 15, 50000, "WEAPON_PUMPSHOTGUN", false);
			playerPosition = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 0.0, 0.0);
		}
	}
	//AmmuNation 2
	GRAPHICS::DRAW_MARKER(0, 2555.89, 288.768, 108.461, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f, 0, 143, 0, 100, false, true, 2, false, false, false, false);
	if (2553.89 <= playerPosition.x && 2557.89 >= playerPosition.x && 286.768 <= playerPosition.y && 290.768 >= playerPosition.y && 106.461 <= playerPosition.z && 110.461 >= playerPosition.z)
	{
		while (2553.89 <= playerPosition.x && 2557.89 >= playerPosition.x && 286.768 <= playerPosition.y && 290.768 >= playerPosition.y && 106.461 <= playerPosition.z && 110.461 >= playerPosition.z)
		{
			askToStart(false, 120, 20, 20000, "WEAPON_MINIGUN", false);
			playerPosition = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(PLAYER::PLAYER_PED_ID(), 0.0, 0.0, 0.0);
		}
	}
}

void main()
{
	Player player = PLAYER::PLAYER_ID();

	readConfigFile();

	while (true)
	{
		if (PLAYER::IS_PLAYER_DEAD(player) == false)
		{
			Ped playerPed = PLAYER::PLAYER_PED_ID();

			if (PED::IS_PED_MODEL(playerPed, $("Player_Zero"))) playerID = 0;
			else if (PED::IS_PED_MODEL(playerPed, $("Player_One"))) playerID = 1;
			else if (PED::IS_PED_MODEL(playerPed, $("Player_Two"))) playerID = 2;
			
			drawMarkers();
		}
		WAIT(0);
	}

	
}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}