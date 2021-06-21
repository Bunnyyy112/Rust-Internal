#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

class Memory {
private:
	int fail(unsigned int code, struct _EXCEPTION_POINTERS* ep)
	{
		if (code == EXCEPTION_ACCESS_VIOLATION)
		{
			return EXCEPTION_EXECUTE_HANDLER;
		}
		else
		{
			return EXCEPTION_CONTINUE_SEARCH;
		};
	}
public:
	Memory(){}

	uintptr_t getModule(LPCSTR modName)
	{
		return (uintptr_t)GetModuleHandle(modName);
	}

	template<typename T = uintptr_t>
	T read(uintptr_t Address)
	{
		T buffer{};

		__try {
			((((ULONG64)Address) > 0x400000) && (((ULONG64)Address + sizeof(T)) < 0x00007FFFFFFF0000)) ? buffer = *(T*)((ULONG64)Address) : buffer = T{};
		}
		__except (fail(GetExceptionCode(), GetExceptionInformation()))
		{

		}

		return buffer;
	}

	template<typename T = uintptr_t>
	void write(uintptr_t Address, T value)
	{
		__try
		{
			*reinterpret_cast<T*>((ULONG64)Address) = (value);
		}
		__except (fail(GetExceptionCode(), GetExceptionInformation()))
		{

		}
	}

	void read_unicode(uintptr_t address, wchar_t* buffer)
	{
		if (read<uint16_t>(address + 0x10) > 0)
		{
			for (int i = 0; i < read<uint16_t>(address + 0x10) * 2; i++)
			{
				buffer[i] = read<wchar_t>(address + 0x14 + (i * 2));
			}
		}
	}

	void read_ascii(uintptr_t address, char* buffer)
	{
		buffer = read<char*>(address);
	}

	void read_ustring(uintptr_t Address, uint16_t size, char* string)
	{
		__try {
			for (int i = 0; i < size; i++)
			{
				string[i] = read<char>(Address + i);
			}
		}
		__except (fail(GetExceptionCode(), GetExceptionInformation()))
		{

		}
	}


};