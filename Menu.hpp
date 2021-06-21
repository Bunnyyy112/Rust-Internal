#pragma once

#include <string>
#include <vector>
#include <codecvt>
#include "Renderer.h"
#include <Utils.h>

#pragma warning(disable:4018)

#define LOWORD(l)           ((WORD)(((uintptr_t)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((uintptr_t)(l)) >> 16) & 0xffff))

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

#define D3DCOLOR_ARGB(a,r,g,b) D2D1::ColorF((float)r / 255.f,(float)g / 255.f,(float)b / 255.f,(float)a / 255.f);

namespace Colors
{
	//== CONFIGURE YOUR COLORS HERE ==//
	const D2D1::ColorF title = D3DCOLOR_ARGB(255, 255, 255, 000);
	const D2D1::ColorF border = D3DCOLOR_ARGB(255, 070, 130, 180);
	const D2D1::ColorF bg = D3DCOLOR_ARGB(150, 050, 050, 050);

	const D2D1::ColorF group = D3DCOLOR_ARGB(255, 255, 255, 000);
	const D2D1::ColorF item = D3DCOLOR_ARGB(255, 255, 255, 255);
	const D2D1::ColorF label = D3DCOLOR_ARGB(255, 255, 255, 255);
	const D2D1::ColorF selected = D3DCOLOR_ARGB(255, 255, 000, 000);
	const D2D1::ColorF mouseOver = D3DCOLOR_ARGB(255, 255, 000, 000);
	const D2D1::ColorF active = D3DCOLOR_ARGB(255, 070, 130, 180);
}

struct MenuItem
{
	MenuItem(const std::wstring& name, int* value, int parent, const std::vector<std::wstring>& options);

	std::wstring m_name;
	std::vector<std::wstring> m_options;
	int* m_value;
	int m_parent;
};

class Menu
{
public:
	Menu(const std::wstring& title,Vector2 pos,Vector2 bounds, DWORD flags = 0);

	int addGroup(const std::wstring& name, int* value, const std::vector<std::wstring>& options = { L"-", L"+" });
	int addItem(const std::wstring& name, int* value, int parent = -1, const std::vector<std::wstring>& options = { L"Off", L"On" });
	int addLabel(const std::wstring& name, int parent = -1);

	bool handleInput(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

	void render();

private:
	std::wstring m_title;

	Vector2 m_bounds;
	Vector2 m_pos;
	int m_selected;
	int m_mouseSelected;

	bool m_show;

	std::vector<MenuItem> m_items;
};