#include <Globals.hpp>

char* get_class_name(void* const networkable_class)
{
	return *reinterpret_cast<char**>(*reinterpret_cast<std::uintptr_t*>(networkable_class) + 0x10);
}

void entity_iteration()
{
	while (!impl::shouldExit)
	{
		uintptr_t unk1 = impl::memory->read<uintptr_t>(impl::baseNet + 0xb8);
		uintptr_t client_entities = impl::memory->read<uintptr_t>(unk1);
		uintptr_t list = impl::memory->read<uintptr_t>(client_entities + 0x10);
		uintptr_t values = impl::memory->read<uintptr_t>(list + 0x28);
		uintptr_t entities = impl::memory->read<uintptr_t>(values + 0x18);
		uint32_t size = impl::memory->read<uint32_t>(values + 0x10);


		impl::player_list_mutex.lock();

		if (impl::players.size() >= 500)
			impl::players.clear();

		impl::player_list_mutex.unlock();

		for (int i = 0; i < size; i++)
		{
			uintptr_t element = impl::memory->read<uintptr_t>(entities + (0x20 + (i * 8)));

			if (!element) continue;

			uintptr_t baseObject = impl::memory->read<uintptr_t>(element + 0x10);
			if (!baseObject) continue;

			uintptr_t object = impl::memory->read<uintptr_t>(baseObject + 0x30);
			if (!object) continue;

			const auto UClass = impl::memory->read(object + 0x30); 

			if (!UClass) continue;

			const auto unk1 = impl::memory->read(UClass + 0x18);

			if (!unk1) continue;

			const auto entity = impl::memory->read(unk1 + 0x28);

			if (std::strstr(get_class_name((void*)element), "BasePlayer") != nullptr)
			{

				if (!entity || impl::memory->read<float>(entity + 0x1F4) <= 0.8f || std::find(impl::players.begin(), impl::players.end(), object) != impl::players.end()) continue;

				std::lock_guard guard(impl::player_list_mutex);

				impl::players.push_back(object);
			}
		}

		std::this_thread::sleep_for(std::chrono::seconds(10));
	}
}