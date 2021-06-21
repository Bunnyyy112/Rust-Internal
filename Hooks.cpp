#include <Hooks.h>

#include <MinHook.h>
#include <iostream>
#include <Globals.hpp>
#include <Utils.h>
#include <StringObf.h>
#include <chrono>
#include <thread>


namespace Hooks {
	void __fastcall blocksprint(void* a1)
	{
		if (impl::sprintAim)
			return;

		return original_blocksprint(a1);
	}

	bool __fastcall can_attack(void* a1, void* a2)
	{
		if (impl::jumpAim)
			return true;

		return original_canattack(a1, a2);
	}

	float __fastcall getindexedspreadscalar(void* a1, void* a2)
	{
		if (impl::noSpread)
			return 0.0f;

		return original_getindexedspreadscalar(a1, a2);
	}

	float __fastcall getspreadscalar(void* a1, void* a2)
	{
		if (impl::noSpread)
			return 0.0f;

		return original_getindexedspreadscalar(a1, a2);
	}

	bool __fastcall Omnisprint(void* a1, void* a2, bool wantsRun)
	{
		wantsRun = (GetAsyncKeyState(0x56)) && (GetAsyncKeyState(0x53)) && (GetAsyncKeyState(0x58)) && (GetAsyncKeyState(0x44));
		if (impl::Omnisprint)
			wantsRun = (GetAsyncKeyState(0x56)) && (GetAsyncKeyState(0x53)) && (GetAsyncKeyState(0x58)) && (GetAsyncKeyState(0x44));

		return original_handleRunning(a1, a2);
	}

	inline void __fastcall hook_sendclienttick(void* self)
	{
		if (impl::Antiaim)
		{
			auto input = impl::memory->read<uintptr_t>((uintptr_t)self + 0x4C8);  //Input
			if (!input)
			{
				//std::cout << "Failed to get input!" << std::endl;
				return original_sendclienttick(self);
			}

			auto state = impl::memory->read<uintptr_t>((uintptr_t)input + 0x20);
			if (!state)
			{
				//std::cout << "Failed to get state!" << std::endl;
				return original_sendclienttick(self);
			}

			auto current = impl::memory->read<uintptr_t>((uintptr_t)state + 0x10);
			if (!current)
			{
				//std::cout << "Failed to get current!" << std::endl;
				return original_sendclienttick(self);
			}
			impl::memory->write(current + 0x18,(100, rand() % 999 + -999, rand() % 999 + -999));
		}
		return original_sendclienttick(self);
	}

	void __fastcall sendprojectileattack(void* a1, void* a2)
	{
		if (impl::silentAim || impl::heliSilent)
		{
			if (impl::closestPlayer != NULL && impl::silentAim)
			{
				uintptr_t PlayerAttack = impl::memory->read<uintptr_t>((uintptr_t)a2 + 0x10);
				uintptr_t Attack = impl::memory->read<uintptr_t>(PlayerAttack + 0x10);
				impl::memory->write<uint32_t>(Attack + 0x2c, 698017942);
				impl::memory->write<uint32_t>(Attack + 0x60, 16144115);
				impl::memory->write<uint32_t>(Attack + 0x28, impl::memory->read<uint32_t>(impl::memory->read<uintptr_t>(impl::closestPlayer + 0x30) + 0x10));
			}
			else
			{
				if (impl::closestHeli != NULL && impl::heliSilent)
				{
					uintptr_t PlayerAttack = impl::memory->read<uintptr_t>((uintptr_t)a2 + 0x10);
					uintptr_t Attack = impl::memory->read<uintptr_t>(PlayerAttack + 0x10);
					int health = (int)ceil(impl::memory->read<float>(impl::closestHeli + 0x1F4));
					if (health <= 5000)
					{
						impl::memory->write<uint32_t>(Attack + 0x2c, 224139191);
					}
					else
					{
						impl::memory->write<uint32_t>(Attack + 0x2c, 2699525250);
					}
				}
			}
		}
		return original_sendprojectileattack(a1, a2);
	}

	uintptr_t __fastcall create_projectile(void* BaseProjectile, void* prefab_path, Vector3 pos, Vector3 forward, Vector3 velocity) {
		uintptr_t projectile = original_create_projectile(BaseProjectile, prefab_path, pos, forward, velocity);
		if (impl::bulletThickness)
		{
			impl::memory->write(projectile + 0x2C, 1.f);
		}

		return projectile;
	}

	//bool __fastcall GetFlying(void* a1)
	//{
	//	return false;
	//}


	//bool __fastcall IsFlying(void* a1)
	//{
	//	return false;
	//}

	//void __fastcall SetFlying(void* a1, void* a2) {}


	void HookFunction(void* Function, void** Original, void* Detour, bool autoEnable = true)
	{
		if (MH_Initialize() != MH_OK && MH_Initialize() != MH_ERROR_ALREADY_INITIALIZED) { std::cout << pr("Failed to initialize MinHook") << std::endl; return; }
		MH_CreateHook(Function, Detour, Original);
		if (autoEnable)
			MH_EnableHook(Function);
	}

	void Init()
	{
		if (MH_Initialize() != MH_OK && MH_Initialize() != MH_ERROR_ALREADY_INITIALIZED) { std::cout << pr("Failed to initialize MinHook") << std::endl; return; }
		HookFunction((void*)(uintptr_t)(impl::gBase + CREATE_PROJECTILE_OFFSET), (void**)&original_create_projectile, create_projectile);
		HookFunction((void*)(uintptr_t)(impl::gBase + oAntiAim), (void**)&original_sendclienttick, hook_sendclienttick);
		HookFunction((void*)(uintptr_t)(impl::gBase + oHandleRunning), (void**)&original_handleRunning, Omnisprint);
		HookFunction((void*)(uintptr_t)(impl::gBase + CAN_ATTACK_OFFSET), (void**)&original_canattack, can_attack);
		HookFunction((void*)(uintptr_t)(impl::gBase + BLOCK_SPRINT_OFFSET), (void**)&original_blocksprint, blocksprint);
		HookFunction((void*)(uintptr_t)(impl::gBase + GETINDEXEDSPREADSCALAR_OFFSET), (void**)&original_getindexedspreadscalar, getindexedspreadscalar);
		HookFunction((void*)(uintptr_t)(impl::gBase + GETSPREADSCALAR_OFFSET), (void**)&original_getspreadscalar, getspreadscalar);
		//HookFunction((void*)(uintptr_t)(impl::gBase + GETFLYING_OFFSET), (void**)&original_getflying, GetFlying);
		//HookFunction((void*)(uintptr_t)(impl::gBase + ISFLYING_OFFSET), (void**)&original_isflying, IsFlying);
		//HookFunction((void*)(uintptr_t)(impl::gBase + SETFLYING_OFFSET), (void**)&original_setflying, SetFlying);
		HookFunction((void*)(uintptr_t)(impl::gBase + SENDPROJECTILEATTACK_OFFSET), (void**)&original_sendprojectileattack, sendprojectileattack);

	}
}