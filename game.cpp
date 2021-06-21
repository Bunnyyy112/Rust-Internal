#include <game.h>

#include <Renderer.h>
#include <Globals.hpp>

std::wstring Phrase::GetEnglish()
{
	if (!this) return L"";

	wchar_t buffer[256];
	impl::memory->read_unicode(impl::memory->read<uintptr_t>((uintptr_t)this + 0x18), buffer);

	return std::wstring(buffer);
}

std::wstring ItemDefinition::GetShortName()
{
	if (!this) return L"";

	wchar_t buffer[256];
	impl::memory->read_unicode(impl::memory->read<uintptr_t>((uintptr_t)this + 0x20), buffer);

	return std::wstring(buffer);
}

std::wstring ItemDefinition::GetDisplayName()
{
	if (!this) return L"";

	return impl::memory->read<Phrase*>((uintptr_t)this + 0x28)->GetEnglish();
}

ItemDefinition* Item::GetItemDefinition()
{
	if (!this) return NULL;

	return impl::memory->read<ItemDefinition*>((uintptr_t)this + 0x20);
}

int Item::GetItemID()
{
	if (!this) return 0;

	return impl::memory->read<int>((uintptr_t)this + 0x20);
}

template<typename T>
T* List::GetByIdx(int idx)
{
	if (!this) return NULL;
	return impl::memory->read<T*>(impl::memory->read<uintptr_t>((uintptr_t)this + 0x10) + 0x20 + (idx * 0x8));
}

List* ItemContainer::GetItems()
{
	if (!this) return NULL;

	return impl::memory->read<List*>((uintptr_t)this + 0x38);
}

ItemContainer* PlayerInventory::GetBelt()
{
	if (!this) return NULL;

	return impl::memory->read<ItemContainer*>((uintptr_t)this + 0x28);
}

void Player::DrawBox(D2D1::ColorF color)
{
	Vector2 tempFeetR, tempFeetL, tempHead;

	if (WorldToScreen(this->GetBonePos(r_foot), &tempFeetR) && WorldToScreen(this->GetBonePos(l_foot), &tempFeetL) && WorldToScreen(this->GetBonePos(jaw) + Vector3(0.f, 0.16f, 0.f), &tempHead))
	{
		Vector2 tempFeet = (tempFeetR + tempFeetL) / 2.f;
		float Entity_h = tempHead.y - tempFeet.y;
		float w = Entity_h / 4;
		float Entity_x = tempFeet.x - w;
		float Entity_y = tempFeet.y;
		float Entity_w = Entity_h / 2;
		Renderer::Rectangle({ Entity_x, Entity_y }, Vector2{ Entity_w, Entity_h }, { 0.f, 0.f, 0.f, 255.f }, 3.f);
		Renderer::Rectangle(Vector2{ Entity_x, Entity_y }, Vector2{ Entity_w, Entity_h }, color, 1.5f);
	}
}

void Player::DrawSkeleton(D2D1::ColorF color)
{
	//D2D1::ColorF color = D2D1::ColorF::Red;
	BoneList Bones[15] = {
		/*LF*/l_foot, l_knee, l_hip,
		/*RF*/r_foot, r_knee, r_hip,
		/*BD*/spine1, neck, head,
		/*LH*/l_upperarm, l_forearm, l_hand,
		/*RH*/r_upperarm, r_forearm, r_hand
	}; Vector2 BonesPos[15];

	for (int j = 0; j < 15; j++) {
		if (!WorldToScreen(GetBoneByID((uintptr_t)this, Bones[j]), &BonesPos[j]))
			return;
	}

	for (int j = 0; j < 15; j += 3) {
		Renderer::Line({ BonesPos[j].x, BonesPos[j].y }, { BonesPos[j + 1].x, BonesPos[j + 1].y }, color, 1.0f);
		Renderer::Line({ BonesPos[j + 1].x, BonesPos[j + 1].y }, { BonesPos[j + 2].x, BonesPos[j + 2].y }, color, 1.0f);
	}

	Renderer::Line({ BonesPos[2].x, BonesPos[2].y }, { BonesPos[6].x, BonesPos[6].y }, color, 1.0f);
	Renderer::Line({ BonesPos[5].x, BonesPos[5].y }, { BonesPos[6].x, BonesPos[6].y }, color, 1.0f);
	Renderer::Line({ BonesPos[9].x, BonesPos[9].y }, { BonesPos[7].x, BonesPos[7].y }, color, 1.0f);
	Renderer::Line({ BonesPos[12].x, BonesPos[12].y }, { BonesPos[7].x, BonesPos[7].y }, color, 1.0f);
}

bool PlayerModel::IsVisible()
{
	if (!this) return false;

	return impl::memory->read<bool>((uintptr_t)this + 0x240);
}

bool PlayerModel::IsNpc()
{
	if (!this) return false;

	return impl::memory->read<bool>((uintptr_t)this + 0x2B8);
}

PlayerModel* Player::GetModel()
{
	if (!this) return NULL;

	return impl::memory->read<PlayerModel*>((uintptr_t)this + 0x4A8);
}

std::wstring Player::GetName()
{
	uintptr_t namePtr = impl::memory->read<uintptr_t>((uintptr_t)this + 0x650);
	wchar_t name[0x100]{};
	impl::memory->read_unicode(namePtr, name);
	return std::wstring(name);
}

int Player::GetActiveItemUID()
{
	return impl::memory->read<int>((uintptr_t)this + 0x570);
}

Item* Player::GetHeldItem()
{
	if (!this) return NULL;

	if (GetActiveItemUID() != 0)
	{

		if (GetInventory() && GetInventory()->GetBelt() && GetInventory()->GetBelt()->GetItems())
		{
			for (int x = 0; x < 6; x++)
			{
				Item* item = GetInventory()->GetBelt()->GetItems()->GetByIdx<Item>(x);
				if (item->GetItemID() != 0 && GetActiveItemUID() != 0)
					if (item->GetItemID() == GetActiveItemUID())
						return item;
			}
		}
	}

	return NULL;
}

Vector3 Player::GetBonePos(BoneList bone)
{
	return GetBoneByID((uintptr_t)this, bone);
}

Vector3 Player::GetPosition()
{
	return GetBonePos(r_foot).midPoint(GetBonePos(l_foot));
}

PlayerInventory* Player::GetInventory()
{
	if (!this) return NULL;

	return impl::memory->read<PlayerInventory*>((uintptr_t)this + 0x608);
}

float Player::GetHealth()
{
	if (!this) return NULL;

	return impl::memory->read<float>((uintptr_t)this + 0x1F4);
}

bool Player::IsSleeping()
{
	if (!this) return false;

	return HasFlag(16);
}

bool Player::IsDead()
{
	if (!this) return true;

	return (int)ceil(GetHealth()) <= 0;
}

bool Player::IsNPC()
{
	if (!this) return NULL;

	return GetModel()->IsNpc();
}

bool Player::IsVisible(Player* target)
{
	if (!this) return false;
        DWORD64 PlayerModel = impl::memory->read<bool>((uintptr_t)this + 0x4A8);
		return impl::memory->read(PlayerModel + 0x248);
		return true;
	
}

bool Player::HasFlag(int Flag)
{
	if (!this) return NULL;

	return (impl::memory->read<DWORD64>((DWORD64)this + 0x5F8) & Flag);
}

uintptr_t Array::getValue(int idx)
{
	if (!this) return NULL;

	return impl::memory->read<uintptr_t>(0x20 + (idx * 8));
}

uint32_t BufferList::getSize()
{
	return impl::memory->read<uint32_t>((uintptr_t)this + 0x10);
}

Array* BufferList::getValues()
{
	if (!this) return NULL;

	return impl::memory->read<Array*>((uintptr_t)this + 0x18);
}