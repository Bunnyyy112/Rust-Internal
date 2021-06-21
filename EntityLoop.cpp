#include <EntityLoop.h>
#include <game.h>
#include <Memory.h>
#include <iostream>

std::vector<Player*> players;
std::vector<Player*> scientists;

char* get_class_name(uintptr_t const networkable_class)
{
	return *reinterpret_cast<char**>(*reinterpret_cast<std::uintptr_t*>(networkable_class) + 0x10);
}

void EntityFilter()
{
	uintptr_t unk1 = impl::memory->read<uintptr_t>(baseNet + 0xb8);
	uintptr_t client_entities = impl::memory->read<uintptr_t>(unk1);
	uintptr_t list = impl::memory->read<uintptr_t>(client_entities + 0x10);
	uintptr_t values = impl::memory->read<uintptr_t>(list + 0x28);
	uintptr_t entities = impl::memory->read<uintptr_t>(values + 0x18);
	uint32_t size = impl::memory->read<uint32_t>(values + 0x10);
	for (int i = 0; i < size; i++)
	{
		uintptr_t element = impl::memory->read<uintptr_t>(entities + (0x20 + (i * 8)));
		if (!element) continue;

		uintptr_t baseObject = impl::memory->read<uintptr_t>(element + 0x10);
		if (!baseObject) continue;

		uintptr_t object = impl::memory->read<uintptr_t>(baseObject + 0x30);

		if (!object) continue;
		std::cout << get_class_name(element) << std::endl;
		//if (std::strstr(get_class_name(element), "BasePlayer") == nullptr) continue;

	}
}