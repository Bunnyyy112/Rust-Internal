#include <Entities.h>
#include <iostream>
#include <Utils.h>
#include <vector.h>
#include <thread>
#include <game.h>
#include <Globals.hpp>

uintptr_t GetHeldItem(uintptr_t player)
{
	Player* plr = (Player*)player;
	uintptr_t Inventory = impl::memory->read<uintptr_t>(player + 0x608);
	uintptr_t Belt = impl::memory->read<uintptr_t>(Inventory + 0x28);
	uintptr_t ItemList = impl::memory->read<uintptr_t>(Belt + 0x38);
	uintptr_t Items = impl::memory->read<uintptr_t>(ItemList + 0x10);
	int activeWeaponUID = plr->GetActiveItemUID();
	uintptr_t HeldItem = NULL;
	if (activeWeaponUID != 0)
	{
		for (int x = 0; x < 6; x++)
		{
			uintptr_t Item = impl::memory->read<uintptr_t>(Items + 0x20 + (x * 8));
			int ItemID = impl::memory->read<int>(Item + 0x28);
			if (ItemID == activeWeaponUID)
				HeldItem = Item;
		}
	}
	return HeldItem;
}

unsigned short HeldItemCRC()
{
	uintptr_t LPlayer = impl::LocalPlayer;

	uintptr_t HeldItem = GetHeldItem(LPlayer);
	uintptr_t Info = impl::memory->read<uintptr_t>(HeldItem + 0x18);
	uintptr_t ShortName = impl::memory->read<uintptr_t>(Info + 0x20);
	int length = impl::memory->read<int>(ShortName + 0x10);
	wchar_t wBuffer[0x100]{};
	impl::memory->read_unicode(ShortName, wBuffer);
	UCHAR* ShortN = (UCHAR*)wBuffer;
	return CRC(ShortN, length * 2);
}

float GetBulletSpeed()
{
	switch (HeldItemCRC())
	{
	case 0:
		return 0.f;

	case 0x77E9:
		return 420.25f;
		break;

	case 0xCFA1:
		return 656.25f;
		break;

	case 0x573C:
		return 420.f;
		break;

	case 0x6A09:
		return 960.f;
		break;

	case 0x7983:
	case 0xE5EA:
	case 0x79C4:
		return 340.f;
		break;

	case 0xE97D:
		return 192.f;
		break;

	case 0x3511:
		return 190.f;
		break;

	case 0x67DA:
		return 260.f;
		break;

	case 0x62E5:
		return 260.f;
		break;

	case 0x3146:
		return 250.f;
		break;

	case 0x6B1:
		return 260.f;
		break;

	case 0x5A9F:
		return 60.f;
		break;

	case 0xC196:
		return 60.f;
		break;

	case 0xB0A0:
		return 95.f;
		break;

	case 0x746A:
		return 125.f;
		break;

	default:
		return 250.f;
	}
}


Vector3 Prediction(const Vector3& LP_Pos, uintptr_t Player, BoneList Bone)
{
	Vector3 BonePos = GetBoneByID(Player, Bone);
	float Dist = Calc3D_Dist(LP_Pos, BonePos);

	if (Dist > 0.001f) {
		float BulletTime = Dist / GetBulletSpeed();
		uintptr_t playerModel = impl::memory->read<uintptr_t>(Player + 0x4A8);

		Vector3 vel = impl::memory->read<Vector3>(playerModel + 0x1FC);
		Vector3 PredictVel = vel * BulletTime * 0.75f;
		BonePos += PredictVel; BonePos.y += (4.905f * BulletTime * BulletTime);
	} return BonePos;
}

float asinn(float x) {
	float negate = float(x < 0);
	x = abs(x);
	float ret = -0.0187293;
	ret *= x;
	ret += 0.0742610;
	ret *= x;
	ret -= 0.2121144;
	ret *= x;
	ret += 1.5707288;
	ret = 3.14159265358979 * 0.5 - sqrt(1.0 - x) * ret;
	return ret - 2 * negate * ret;
}

__forceinline Vector2 CalcAngle(const Vector3& Src, const Vector3& Dst) {
	Vector3 dir = Src - Dst;
	return Vector2{ RAD2DEG(asinn(dir.y / dir.length())), RAD2DEG(-atan2(dir.x, -dir.z)) };
}

float GetFov(uintptr_t Entity, BoneList Bone) {
	Vector2 ScreenPos;
	if (!WorldToScreen(GetBoneByID(Entity, Bone), &ScreenPos)) return 1000.f;
	return Calc2D_Dist(Vector2(viewport.Width / 2, viewport.Height / 2), ScreenPos);
}

float GetFovHeli(Vector3 pos)
{
	Vector2 ScreenPos;
	if (!WorldToScreen(pos, &ScreenPos)) return 1000.f;
	return Calc2D_Dist(Vector2(viewport.Width / 2, viewport.Height / 2), ScreenPos);
}

void Normalize(float& Yaw, float& Pitch) {
	if (Pitch < -89) Pitch = -89;
	else if (Pitch > 89) Pitch = 89;
	if (Yaw < -360) Yaw += 360;
	else if (Yaw > 360) Yaw -= 360;
}

void GoToTarget(uintptr_t player)
{
	uintptr_t LPlayer = impl::LocalPlayer;

	Vector3 Local = GetBoneByID(LPlayer, neck);
	Vector3 PlayerPos = { 0,0,0 };
	if (impl::aimPrediction)
		PlayerPos = Prediction(Local, player, head);
	else
		PlayerPos = GetBoneByID(player, head);

	uintptr_t LInput = impl::memory->read<uintptr_t>(LPlayer + 0x4C8);
	Vector2 Offset = CalcAngle(Local, PlayerPos) - impl::memory->read<Vector2>(LInput + 0x3C);
	Vector2 OriginalAngle = impl::memory->read<Vector2>(LInput + 0x3C);
	Normalize(Offset.y, Offset.x);

	Vector2 AngleToAim = { 0,0 };
	Vector2 viewAngle = impl::memory->read<Vector2>(LInput + 0x3C);

	AngleToAim = viewAngle + Offset;

	Normalize(AngleToAim.y, AngleToAim.x);

	impl::memory->write<Vector2>(LInput + 0x3C, AngleToAim);

}

void EntityLoop()
{
	if (impl::playerESP || impl::sleeperESP)
	{
		uintptr_t unk1 = impl::memory->read<uintptr_t>(impl::baseNet + 0xb8);
		uintptr_t client_entities = impl::memory->read<uintptr_t>(unk1);
		uintptr_t list = impl::memory->read<uintptr_t>(client_entities + 0x10);
		uintptr_t values = impl::memory->read<uintptr_t>(list + 0x28);
		uintptr_t entities = impl::memory->read<uintptr_t>(values + 0x18);
		uint32_t size = impl::memory->read<uint32_t>(values + 0x10);
		float FOV = impl::fovSlider, curFOV;
		for (int i = 0; i < size; i++)
		{
			uintptr_t element = impl::memory->read<uintptr_t>(entities + (0x20 + (i * 8)));
			uintptr_t baseObject = impl::memory->read<uintptr_t>(element + 0x10);
			uintptr_t object = impl::memory->read<uintptr_t>(baseObject + 0x30);
			WORD tag = impl::memory->read<WORD>(object + 0x54);
			uintptr_t localElement = impl::memory->read<uintptr_t>(entities + 0x20);
			uintptr_t localBO = impl::memory->read<uintptr_t>(localElement + 0x10);
			uintptr_t localPlayer = impl::memory->read<uintptr_t>(localBO + 0x30);
			uintptr_t localOC = impl::memory->read<uintptr_t>(localPlayer + 0x30);
			uintptr_t localEntity = impl::memory->read<uintptr_t>(localOC + 0x18);
			Player* LPlayer = (Player*)impl::memory->read<uintptr_t>(localEntity + 0x28);

			if (tag == 0 && impl::helicopterESP)
			{
				char* prefabName = impl::memory->read<char*>(object + 0x60);
				if (prefabName != nullptr && prefabName != "")
				{
					if (strcmp(prefabName, "assets/prefabs/npc/patrol helicopter/patrolhelicopter.prefab") == 0)
					{
						uintptr_t BaseObject = impl::memory->read<uintptr_t>(object + 0x30);
						uintptr_t BaseEntity = impl::memory->read<uintptr_t>(BaseObject + 0x18);
						uintptr_t BaseHelicopter = impl::memory->read<uintptr_t>(BaseEntity + 0x28);
						uintptr_t Transform = impl::memory->read<uintptr_t>(BaseObject + 0x8);
						uintptr_t VisualState = impl::memory->read<uintptr_t>(Transform + 0x38);
						Vector3 pos = impl::memory->read<Vector3>(VisualState + 0x90);
						int health = (int)ceil(impl::memory->read<float>(BaseHelicopter + 0x1F4));
						Vector2 screenPos;
						if (WorldToScreen(pos, &screenPos))
						{
							wchar_t buffer[256];
							swprintf(buffer, L"Patrol Heli - (%dm)", health, (int)ceil(LPlayer->GetPosition().distance(pos)));
							Renderer::DrawString(screenPos, buffer, true, D2D1::ColorF::Red,true);
							if (impl::closestPlayer == NULL)
							{

								if (impl::fovCircle)
									if (health > 0)
										if ((FOV > (curFOV = GetFovHeli(pos)))) {
											FOV = curFOV; impl::closestHeli = BaseHelicopter; impl::closestHeliObj = VisualState;
										}

							}

						}
					}
				}
			}

			if (tag == 6)
			{
				char* className = impl::memory->read<char*>(object + 0x60);
				uintptr_t objectClass = impl::memory->read<uintptr_t>(object + 0x30);
				uintptr_t entity = impl::memory->read<uintptr_t>(objectClass + 0x18);
				Player* player = (Player*)impl::memory->read<uintptr_t>(entity + 0x28);
				if (strcmp(className, "LocalPlayer") != 0)
				{
					Vector2 screenPos;
					if (WorldToScreen(player->GetPosition(), &screenPos))
					{
						wchar_t buffer[0x100]{};
						swprintf(buffer, L"%s - (%dm)", player->GetName().c_str(), (int)ceil(player->GetHealth()), (int)ceil(player->GetPosition().distance(LPlayer->GetPosition())));
						
						if (!player->IsDead())
						{
							if (!player->IsSleeping())
							{
								if (impl::playerESP)
								{
									if (player->IsNPC() && !impl::scientistESP)
										continue;

									Renderer::DrawString({ screenPos.x,screenPos.y }, buffer, true, D2D1::ColorF::White, true);

									if (impl::fovCircle)
									{
										if ((int)ceil(player->GetHealth()) > 0) {
											if ((FOV > (curFOV = GetFov((uintptr_t)player, BoneList(head))))) {
												FOV = curFOV; impl::closestPlayer = (uintptr_t)player; impl::closestObject = object;
											}
										}
									}

									if (impl::tracers && (uintptr_t)player != impl::closestPlayer)
									{
										Vector2 tracerPos;
										if (WorldToScreen(player->GetBonePos(head), &tracerPos))
											Renderer::Line({ ScreenCenterX,ScreenCenterY }, { tracerPos.x,tracerPos.y }, D2D1::ColorF::Black, 1.0f);
									}
									
									if (impl::boxESP)
										player->DrawBox(D2D1::ColorF::Red);

									//if (LPlayer->IsVisible(player))
									//	player->DrawBox(D2D1::ColorF::Red);
									//else
									//	player->DrawBox(D2D1::ColorF::Purple);

									if (impl::skeletonESP)
										player->DrawSkeleton(D2D1::ColorF::Red);

									//if (LPlayer->IsVisible(player))
									//	player->DrawSkeleton(D2D1::ColorF::Red);
									//else
									//	player->DrawSkeleton(D2D1::ColorF::Purple);

									if (impl::heldWeapon)
									{
										Item* heldItem = player->GetHeldItem();
										if (heldItem != NULL)
										{
											Renderer::DrawString({ screenPos.x,screenPos.y + 15 }, heldItem->GetItemDefinition()->GetDisplayName().c_str(), true, D2D1::ColorF::Red, true);

										}
										//int activeWeaponUID = player->GetActiveItemUID();

										//if (activeWeaponUID != 0)
										//{
										//	if (player->GetInventory() && player->GetInventory()->GetBelt() && player->GetInventory()->GetBelt()->GetItems())
										//	{
										//		for (int x = 0; x < 6; x++)
										//		{
										//			Item* item = player->GetInventory()->GetBelt()->GetItems()->GetByIdx<Item>(x);
										//			if (item->GetItemID() == activeWeaponUID)
										//			{
										//				Renderer::DrawString({ screenPos.x,screenPos.y + 15 }, item->GetItemDefinition()->GetDisplayName().c_str(), true, D2D1::ColorF::Red, true);

										//				break;
										//			}
										//		}
										//	}
										//}
									}
								}
							}
							else
								if (impl::sleeperESP)
								{
									Renderer::DrawString({ screenPos.x,screenPos.y }, buffer, true, D2D1::ColorF(52.f / 255.f, 152.f / 255.f, 235.f / 255.f), true);
									if (impl::skeletonESP)
										player->DrawSkeleton(D2D1::ColorF(52.f / 255.f, 152.f / 255.f, 235.f / 255.f));
								}
						}
					}
				}
				else
				{
					impl::LocalPlayer = (uintptr_t)player;
				}
			}
		}

		if (impl::closestHeli != NULL & impl::fovCircle)
		{
			Vector2 screenPos;
			if (WorldToScreen(impl::memory->read<Vector3>(impl::closestHeliObj + 0xB0), &screenPos))
			{
				Renderer::Line({ ScreenCenterX,ScreenCenterY }, { screenPos.x,screenPos.y }, D2D1::ColorF::DarkCyan, 1.0f);
			}
		}

		if (impl::closestPlayer != NULL && impl::fovCircle)
		{
			Vector2 screenPos;
			if (WorldToScreen(GetBoneByID(impl::closestPlayer, head), &screenPos))
			{
				Renderer::Line({ ScreenCenterX,ScreenCenterY }, { screenPos.x,screenPos.y }, D2D1::ColorF::Red, 1.0f);
			}
			else
			{
				impl::closestPlayer = NULL;
			}
		}
		if (impl::closestHeli != NULL && impl::closestPlayer == NULL && (FOV < (curFOV = GetFovHeli(impl::memory->read<Vector3>(impl::closestHeliObj + 0xB0)))))
		{
			impl::closestHeli = NULL;
			impl::closestHeliObj = NULL;
		}

		if ((FOV < (curFOV = GetFov(impl::closestPlayer, BoneList(head))))) {
			impl::closestPlayer = NULL;
		}
		if (impl::closestHeli != NULL && impl::memory->read<float>(impl::closestHeli + 0x1F4) <= 1)
			impl::closestHeli = NULL;

		if (impl::memory->read<uintptr_t>(impl::closestPlayer + 0x4A8) == NULL)
		{
			impl::closestPlayer = NULL;
		}
		if (GetAsyncKeyState(VK_RBUTTON) && impl::aimbot && impl::closestPlayer != NULL)
			GoToTarget(impl::closestPlayer);

		if (impl::inventoryESP && impl::closestPlayer != NULL && !menuOpen)
		{
			Player* plr = (Player*)impl::closestPlayer;

			if (plr->GetInventory() && plr->GetInventory()->GetBelt() && plr->GetInventory()->GetBelt()->GetItems())
			{
				Renderer::OutlineRect({ 30.f,25.f }, { 180.f,300.f }, Colors::bg, Colors::border, 0.5f);
				Vector2 renderPos = { 35.f,25.f };

				Renderer::OutlineRect({ renderPos.x - 5.f,renderPos.y }, { 180.f,25.f }, Colors::bg, Colors::border, 0.5f);
				Renderer::DrawString({ renderPos.x + 85.f,renderPos.y + 12.5f }, plr->GetName().c_str(), true, Colors::title);
				renderPos = renderPos + Vector2(0, 30.f);
				for (int x = 0; x < 6; x++)
				{
					Item* item = plr->GetInventory()->GetBelt()->GetItems()->GetByIdx<Item>(x);
					Renderer::DrawString({ renderPos.x,renderPos.y }, item->GetItemDefinition()->GetDisplayName().c_str(), false, D2D1::ColorF::White);
					renderPos = renderPos + Vector2(0.f, 15.f);
				}
			}
		}

	}
}