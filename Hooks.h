#pragma once
#include <Windows.h>
#include <D3D11.h>
#include <vector.h>

namespace Hooks {

	#define CREATE_PROJECTILE_OFFSET 0x3D35D0
	#define CAN_ATTACK_OFFSET 0x2D4FB0
	#define CREATE_PROJECTILE_MELEE_OFFSET 0x2CE490
	#define BLOCK_SPRINT_OFFSET 0x264EB0
	#define GETINDEXEDSPREADSCALAR_OFFSET 0x341520
	#define GETSPREADSCALAR_OFFSET 0x3415F0
	#define SENDPROJECTILEATTACK_OFFSET 0x2EB280
     #define oAntiAim 0x2A4E50 //internal void SendClientTick() { }
    #define oFakeCard 0x354820 //public int get_accessLevel() { }
   #define oHandleRunning 0x5FA2B0
  #define oDoHitNotify 0x2F3710
  #define oAimdirection 0x781AD0
  #define oFastHeal 0x2DC630
  #define oMagic 0x17F8640

	//#define GETFLYING_OFFSET 0x17F9730
	//#define SETFLYING_OFFSET 0x17F9800
	//#define ISFLYING_OFFSET 0x363C10

	typedef void(__fastcall* SendProjectileAttack_fn)(void*, void*);
	inline SendProjectileAttack_fn original_sendprojectileattack{ };

	//typedef bool(__fastcall* GetFlying_fn)(void*);
	//inline GetFlying_fn original_getflying{ };

	//typedef bool(__fastcall* IsFlying_fn)(void*);
	//inline IsFlying_fn original_isflying{};

	//typedef void(__fastcall* SetFlying_fn)(void*, void*);
	//inline SetFlying_fn original_setflying{};

	typedef uintptr_t(__fastcall* create_projectile_fn)(void*, void*, Vector3, Vector3, Vector3);
	inline create_projectile_fn original_create_projectile{ };

	typedef bool(__fastcall* can_attack_fn)(void*, void*);
	inline can_attack_fn original_canattack;

	typedef void(__fastcall* block_sprint_fn)(void*);
	inline block_sprint_fn original_blocksprint;

	typedef float(__fastcall* getindexedspreadscalar_fn)(void*, void*);
	inline getindexedspreadscalar_fn original_getindexedspreadscalar;

	typedef float(__fastcall* getspreadscalar_fn)(void*, void*);
	inline getspreadscalar_fn original_getspreadscalar;

	typedef void(__fastcall* AntiAim)(void*);
	inline AntiAim original_sendclienttick;

	typedef bool(__fastcall* HandleRunning)(void*, void*);
	inline HandleRunning original_handleRunning;



	void Init();
}