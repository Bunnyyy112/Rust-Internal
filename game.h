#include <Windows.h>
#include <wchar.h>
#include <string>
#include <vector.h>
#include <cstdint>
#include <Utils.h>
#include <D2D1.h>

class Phrase {
public:
	std::wstring GetEnglish();
};

class ItemDefinition {
public:

	std::wstring GetShortName();
	std::wstring GetDisplayName();
};

class Item {
public:
	ItemDefinition* GetItemDefinition();

	int GetItemID();
};

class List {
public:
	template<typename T>
	T* GetByIdx(int idx);
};

class ItemContainer {
public:
	List* GetItems();
};

class PlayerInventory {
public:
	ItemContainer* GetBelt();
};

class PlayerModel {
public:
	bool IsVisible();

	bool IsNpc();
};

class Player {
public:
	PlayerModel* GetModel();

	std::wstring GetName();

	int GetActiveItemUID();

	Item* GetHeldItem();

	bool HasFlag(int Flag);

	Vector3 GetBonePos(BoneList bone);

	void DrawSkeleton(D2D1::ColorF color);

	void DrawBox(D2D1::ColorF color);

	Vector3 GetPosition();

	PlayerInventory* GetInventory();

	float GetHealth();

	bool IsSleeping();

	bool IsDead();

	bool IsVisible(Player* target);

	bool IsNPC();
};

class Array {
public:
	uintptr_t getValue(int idx);
};

class BufferList {
public:
	uint32_t getSize();

	Array* getValues();
};