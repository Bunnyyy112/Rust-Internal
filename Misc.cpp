#include <Misc.h>

#include <game.h>
#include <Globals.hpp>

void Weapons()
{
	while (!impl::shouldExit)
	{
		if (impl::noSpread || impl::instaEoka || impl::automatic || impl::Runmeele || impl::noRecoil || impl::spiderMan || impl::fakeAdmin || impl::playerfov || impl::Unlimitedjump || impl::fakelag || impl::girrafe)
		{
			if (impl::LocalPlayer == NULL)
				continue;
			//uintptr_t BaseObject = impl::memory->read<uintptr_t>(LocalPlayer + 0x30);
			//uintptr_t BaseEntity = impl::memory->read<uintptr_t>(BaseObject + 0x18);
			//uintptr_t BasePlayer = impl::memory->read<uintptr_t>(BaseEntity + 0x28);
			//Player* BasePlayer_ = (Player*)BasePlayer;
			Player* LPlayer = (Player*)impl::LocalPlayer;
			uintptr_t BasePlayer = (DWORD64)LPlayer;
			if (impl::spiderMan)
			{
				uintptr_t BaseMovement = impl::memory->read<uintptr_t>(BasePlayer + 0x4D0);
				impl::memory->write<float>(BaseMovement + 0xB4, .0f);
				impl::memory->write<float>(BaseMovement + 0xB8, .0f);
			}
			if (impl::playerfov)
			{
				uintptr_t klass = impl::memory->read<uintptr_t>(impl::gBase + 0x2984A00);
				uintptr_t static_fields = impl::memory->read<uintptr_t>(klass + 0xB8);
				impl::memory->write<float>(static_fields + 0x18, 120.f);
			}
			if (impl::girrafe)
			{
				uintptr_t Playereyes = impl::memory->read<uintptr_t>(BasePlayer + 0x600);
				impl::memory->write<float>(Playereyes + 0x18, (0, 6.f, 0));
				impl::memory->read<uintptr_t>(Playereyes + 0x38);
			}
			if (impl::Unlimitedjump)
			{
				uintptr_t BaseMovement = impl::memory->read<uintptr_t>(BasePlayer + 0x4D0);
				impl::memory->write<float>(BaseMovement + 0xC0, .0f);
				impl::memory->write<float>(BaseMovement + 0xBC, (9999999, 9999999, 9999999));
				impl::memory->write<float>(BaseMovement + 0xC4, .0f);
			}
			if (impl::fakelag)
			{
				impl::memory->write<float>(BasePlayer + 0x5C8, 0.4f);
				impl::memory->write<float>(BasePlayer + 0x5C8, 0.5f);
			}
			if (impl::fakeAdmin)
			{
				int flags = impl::memory->read<int>(BasePlayer + 0x5F8);
				impl::memory->write<int>(BasePlayer + 0x5F8, flags | 4);
			}
			if (impl::neverSink)
			{
				uintptr_t BaseMovement = impl::memory->read<uintptr_t>(BasePlayer + 0x4D0);
				impl::memory->write(BaseMovement + 0x74, 0.f);
			}

			uintptr_t Inventory = impl::memory->read<uintptr_t>(BasePlayer + 0x608);
			uintptr_t Belt = impl::memory->read<uintptr_t>(Inventory + 0x28);
			uintptr_t ItemList = impl::memory->read<uintptr_t>(Belt + 0x38);
			uintptr_t Items = impl::memory->read<uintptr_t>(ItemList + 0x10);
			int activeWeaponUID = impl::memory->read<int>(BasePlayer + 0x570);
			if (impl::noSpread || impl::instaEoka || impl::automatic || impl::Runmeele || impl::noRecoil)
				if (activeWeaponUID != 0)
				{
					for (int x = 0; x < 6; x++)
					{
						uintptr_t Item = impl::memory->read<uintptr_t>(Items + 0x20 + (x * 8));
						int ItemID = impl::memory->read<int>(Item + 0x28);
						if (ItemID == activeWeaponUID)
						{
							uintptr_t BaseProjectile = impl::memory->read<uintptr_t>(Item + 0x98);
							impl::memory->write<float>(BaseProjectile + 0x398, 0.f);
							impl::memory->write<float>(BaseProjectile + 0x3A0, 1.f);
							float reloadTime = impl::memory->read<float>(BaseProjectile + 0x298);
							if (reloadTime >= 1 && reloadTime <= 5.5 || reloadTime == 6)
							{
								uintptr_t RecoilProperties = impl::memory->read<uintptr_t>(BaseProjectile + 0x2C0);
								if (impl::rapidFire)
								{
									impl::memory->write<float>(BaseProjectile + 0x1DC, 0.09f);
								}
								if (impl::noSpread)
								{
									impl::memory->write<float>(BaseProjectile + 0x304, -1.0f);
									impl::memory->write<float>(BaseProjectile + 0x308, -1.0f);
									impl::memory->write<float>(BaseProjectile + 0x2b8, 0.0f);
									impl::memory->write<float>(BaseProjectile + 0x2bc, 0.0f);
									impl::memory->write<float>(BaseProjectile + 0x2d0, 0.0f);
									impl::memory->write<float>(BaseProjectile + 0x2d4, 0.0f);
									impl::memory->write<float>(BaseProjectile + 0x2d8, 0.0f);
									impl::memory->write<float>(BaseProjectile + 0x2dc, 0.0f);
									impl::memory->write<float>(BaseProjectile + 0x2E0, 0.0f);
									impl::memory->write<float>(BaseProjectile + 0x2E4, 0.0f);
									impl::memory->write<float>(BaseProjectile + 0x2E8, 0.0f);
								}
								if (impl::Runmeele)
								{
									impl::memory->write<float>(BaseProjectile + 0x344, true);
								}
								if (impl::automatic)
								{
									impl::memory->write<bool>(BaseProjectile + 0x270, true);
								}
								if (impl::noRecoil)
								{
									impl::memory->write<float>(RecoilProperties + 0x18, 0.1f);
									impl::memory->write<float>(RecoilProperties + 0x1c, 0.1f);
									impl::memory->write<float>(RecoilProperties + 0x20, 0.1f);
									impl::memory->write<float>(RecoilProperties + 0x24, 0.1f);
								}

								if (impl::instaEoka)
								{
									impl::memory->write<float>(BaseProjectile + 0x340, 1.0f);
								}


							}
							break;
						}
					}
				}
		}
	}
}