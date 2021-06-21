#pragma once
#include <Windows.h>

class GameObjectManager {
	uintptr_t Address;

	GameObjectManager(DWORD64 Addr);
};