#include <D3D.hpp>
#include <Utils.h>
#include <iostream>
#include <codecvt>
#include <Globals.hpp>

void safe_memcpy_wrapper(ULONG64 Dst, ULONG64 Src, ULONG Sz)
{
	if ((((ULONG64)Dst) > 0x400000) && (((ULONG64)Dst + Sz) < 0x00007FFFFFFF0000))
	{
		while (true)
		{
			//copy 8 byte
			if (Sz >= 8)
			{
				*(ULONG64*)Dst = *(ULONG64*)Src;
				Dst += 8; Src += 8; Sz -= 8;
			}

			//copy 4 byte
			else if (Sz >= 4)
			{
				*(ULONG*)Dst = *(ULONG*)Src;
				Dst += 4; Src += 4; Sz -= 4;
			}

			//copy 2 byte
			else if (Sz >= 2)
			{
				*(WORD*)Dst = *(WORD*)Src;
				Dst += 2; Src += 2; Sz -= 2;
			}

			//copy last byte
			else if (Sz)
			{
				*(BYTE*)Dst = *(BYTE*)Src;
				break;
			}

			//if(Sz == 0)
			else
				break;
		}
	}
}

unsigned short CRC(unsigned char* pcBlock, unsigned short len)
{
	unsigned short crc = 0xFFFF;
	while (len--)
		crc = (crc << 8) ^ Crc16Table[(crc >> 8) ^ *pcBlock++];
	return crc;
}

unsigned int CRC32(const unsigned char* buf, int len)
{
	unsigned int crc = 0xFFFF;
	while (len--)
	{
		crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *buf) & 255];
		buf++;
	}
	return crc;
}

Vector3 GeetPosition(void* transform)
{
	if (!transform) return Vector3(0, 0, 0);

	Vector3 pos = {};
	static const auto getPosition = reinterpret_cast<uint64_t(__fastcall*)(void*, Vector3&)>(std::uintptr_t(GetModuleHandleA("UnityPlayer.dll")) + 0x1AB46C0);
	getPosition(transform, pos);

	return pos;
}

struct Matrix34
{
	Vector3 vec0;
	Vector3 vec1;
	Vector3 vec2;
};

Vector3 GotPosition(DWORD64 transform)
{
	if (!transform) return Vector3{ 0.f, 0.f, 0.f };

	struct Matrix34 { BYTE vec0[16]; BYTE vec1[16]; BYTE vec2[16]; };
	const __m128 mulVec0 = { -2.000, 2.000, -2.000, 0.000 };
	const __m128 mulVec1 = { 2.000, -2.000, -2.000, 0.000 };
	const __m128 mulVec2 = { -2.000, -2.000, 2.000, 0.000 };

	int Index = *(PINT)(transform + 0x40);
	DWORD64 pTransformData = (transform + 0x38);
	DWORD64 transformData[2];
	safe_memcpy(&transformData, (PVOID)(pTransformData + 0x18), 16);

	size_t sizeMatriciesBuf = 48 * Index + 48;
	size_t sizeIndicesBuf = 4 * Index + 4;

	int pIndicesBuf[100];
	Matrix34 pMatriciesBuf[1000];

	safe_memcpy(pMatriciesBuf, (PVOID)transformData[0], sizeMatriciesBuf);
	safe_memcpy(pIndicesBuf, (PVOID)transformData[1], sizeIndicesBuf);

	__m128 result = *(__m128*)((ULONGLONG)pMatriciesBuf + 0x30 * Index);
	int transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * Index);

	while (transformIndex >= 0)
	{
		Matrix34 matrix34 = *(Matrix34*)((ULONGLONG)pMatriciesBuf + 0x30 * transformIndex);
		__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x00));
		__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x55));
		__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x8E));
		__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xDB));
		__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0xAA));
		__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34.vec1), 0x71));
		__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34.vec2), result);

		result = _mm_add_ps(
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
							_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
						_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
							_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
						_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
				_mm_add_ps(
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
							_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
						_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
					tmp7)), *(__m128*)(&matrix34.vec0));

		transformIndex = *(int*)((ULONGLONG)pIndicesBuf + 0x4 * transformIndex);
	}

	return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
}

Vector3 GetBoneByID(uintptr_t BasePlayer, BoneList BoneID) {
	uintptr_t model = impl::memory->read<uintptr_t>(BasePlayer + 0x118);
	uintptr_t BoneTransforms = impl::memory->read<uintptr_t>(model + 0x48);
	uintptr_t entity_bone = impl::memory->read<uintptr_t>(BoneTransforms + (0x20 + (((int)BoneID) * 0x8)));
	return GotPosition(impl::memory->read<uintptr_t>(entity_bone + 0x10));
}


std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

std::wstring s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

bool WorldToScreen(Vector3 world, Vector2* screen)
{
	const auto matrix = impl::viewMatrix.transpose();

	const Vector3 translation = { matrix[3][0], matrix[3][1], matrix[3][2] };
	const Vector3 up = { matrix[1][0], matrix[1][1], matrix[1][2] };
	const Vector3 right = { matrix[0][0], matrix[0][1], matrix[0][2] };

	const auto w = translation.dot_product(world) + matrix[3][3];

	if (w < 0.1f)
		return false;

	const auto x = right.dot_product(world) + matrix[0][3];
	const auto y = up.dot_product(world) + matrix[1][3];

	*screen =
	{
		(viewport.Width * 0.5f) * (1.f + x / w),
		(viewport.Height * 0.5f) * (1.f - y / w)
	};

	return true;
}