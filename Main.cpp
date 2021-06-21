#include <D3D.hpp>
#include <Globals.hpp>
#include <thread>
#include <Entities.h>
#include <vector.h>
#include <Hooks.h>
#include <StringObf.h>
#include <Misc.h>
#include <Utils.h>
#include <string.h>
#include <stdlib.h> /* NULL macro */

#define BASENETSHOW

char* memstr(char* haystack, const char* needle, int size)
{
	char* p;
	char needlesize = strlen(needle);

	for (p = haystack; p <= (haystack - needlesize + size); p++)
	{
		if (memcmp(p, needle, needlesize) == 0)
			return p; /* found */
	}
	return NULL;
}

uint64_t GScanForClass(const char* name) {
	auto base = (uintptr_t)GetModuleHandleA("GameAssembly.dll");
	auto dos_header = impl::memory->read<IMAGE_DOS_HEADER>(base);
	auto data_header = impl::memory->read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (3 * 40));
	auto next_section = impl::memory->read<IMAGE_SECTION_HEADER>(base + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS64) + (4 * 40));
	auto data_size = next_section.VirtualAddress - data_header.VirtualAddress;

	if (strcmp((char*)data_header.Name, ".data")) {
		printf("[!] Section order changed\n");
		return 0;
	}

	for (uint64_t offset = data_size; offset > 0; offset -= 8) {
		char klass_name[0x100] = { 0 };
		auto klass = impl::memory->read<uint64_t>(base + data_header.VirtualAddress + offset);
		if (klass == 0) { continue; }
		auto name_pointer = impl::memory->read<uint64_t>(klass + 0x10);
		if (name_pointer == 0) { continue; }
		impl::memory->read_ustring(name_pointer, sizeof(klass_name), klass_name);
		if (!strcmp(klass_name, name)) {
			#ifdef BASENETSHOW
				printf("[*] 0x%x -> %s\n", data_header.VirtualAddress + offset, name);
			#endif
			return klass;
		}
	}

	return NULL;
}

uint64_t scan_for_camera_list() {
	const auto base = reinterpret_cast<uint64_t>(GetModuleHandleA("UnityPlayer.dll"));

	if (!base)
		return 0;

	const auto dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
	const auto nt_header = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos_header->e_lfanew);

	uint64_t data_base;
	uint64_t data_size;

	for (int i = 0;;)
	{
		const auto section = reinterpret_cast<IMAGE_SECTION_HEADER*>(
			base + dos_header->e_lfanew + // nt_header base 
			sizeof(IMAGE_NT_HEADERS64) +  // start of section headers
			(i * sizeof(IMAGE_SECTION_HEADER))); // section header at our index

		if (strcmp((char*)section->Name,".data") == 0)
		{
			data_base = section->VirtualAddress + base;
			data_size = section->SizeOfRawData;
			break;
		}

		i++;

		if (i >= nt_header->FileHeader.NumberOfSections)
		{
			printf("[!] Section not found\n");
			return 0;
		}
	}

	uint64_t camera_table = 0;

	const auto camera_string = memstr((char*)data_base, "AllCameras",data_size);

	for (auto walker = (uint64_t*)camera_string; walker > 0; walker -= 1)
	{
		if (*walker > 0x100000 && *walker < 0xF00000000000000)
		{
			// [[[[unityplayer.dll + ctable offset]]] + 0x30] = Camera
			camera_table = *walker;
			break;
		}
	}

	if (camera_table)
		return camera_table;

	printf("[!] Unable to locate camera\n");
	return 0;
}

void __stdcall main(HMODULE module)
{
	AllocConsole();
	SetConsoleTitleA(pr("Rust - Internal"));
	freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	impl::memory = std::make_unique<Memory>();
	impl::base = impl::memory->getModule("UnityPlayer");
	impl::gBase = impl::memory->getModule("GameAssembly.dll");
	impl::baseNet = GScanForClass("BaseNetworkable");
	impl::shouldExit = false;
	Hooks::Init();
	Initialize();
	std::thread weapons(&Weapons);
	
	const auto camera_table = scan_for_camera_list();

	while (true)
	{
		const auto camera_parent = impl::memory->read(camera_table);
		const auto camera = impl::memory->read(camera_parent);
		impl::viewMatrix = impl::memory->read<Matrix>(camera + 0x2e4);
		impl::memory->write<uint8_t>(impl::gBase + 0x5A1FFF, 0);
	}
	impl::shouldExit = true;
	weapons.join();

	fclose(reinterpret_cast<FILE*>(stdin));
	fclose(reinterpret_cast<FILE*>(stdout));
	FreeConsole();
	PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
	FreeLibraryAndExitThread(module, EXIT_SUCCESS);
}

bool __stdcall DllMain(HMODULE module, std::uint32_t call_reason, void*)
{
	if (call_reason != DLL_PROCESS_ATTACH && call_reason != DLL_PROCESS_DETACH)
		return false;

	if (call_reason == DLL_PROCESS_ATTACH)
	{
		const auto handle = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(main), module, 0, nullptr);
		if (handle != NULL)
			CloseHandle(handle);
	}
	return true;
}
