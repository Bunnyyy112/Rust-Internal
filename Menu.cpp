#include "menu.hpp"
#include <D3D.hpp>

MenuItem::MenuItem(const std::wstring& name, int* value, int parent, const std::vector<std::wstring>& options)
	: m_name(name), m_value(value), m_options(options), m_parent(parent)
{
}

Menu::Menu(const std::wstring& title, Vector2 pos, Vector2 bounds, DWORD flags) :
	m_title(title), m_show(true), m_selected(0), m_mouseSelected(-1), m_bounds(bounds), m_pos(pos)
{
}

int Menu::addItem(const std::wstring& name, int* value, int parent, const std::vector<std::wstring>& options)
{
	m_items.emplace_back(name, value, parent, options);
	return m_items.size() - 1;
}

int Menu::addGroup(const std::wstring& name, int* value, const std::vector<std::wstring>& options)
{
	return addItem(name, value, -1, options);
}

int Menu::addLabel(const std::wstring& name, int parent)
{
	return addItem(name, nullptr, parent, {});
}

bool Menu::handleInput(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KEYDOWN && wParam == VK_INSERT)
		m_show = !m_show;

	if (!m_show)
		return false;

	bool dontForward = false;

	if (message == WM_KEYDOWN)
	{
		if (wParam == VK_UP)
		{
			int nextItem = -1;

			// find next rendered item from m_selected - 1 to top
			for (int i = m_selected - 1; i >= 0; --i)
			{
				// ignore if parents are 0
				int parent = m_items[i].m_parent;

				while (parent >= 0)
				{
					if (*m_items[parent].m_value <= 0)
						break;

					parent = m_items[parent].m_parent;
				}

				if (parent >= 0)
					continue;

				// ignore labels
				if (!m_items[i].m_value)
					continue;

				nextItem = i;
				break;
			}

			if (nextItem <= -1)
			{
				// find next rendered item from bottom to top
				for (int i = m_items.size() - 1; i >= 0; --i)
				{
					int parent = m_items[i].m_parent;

					while (parent >= 0)
					{
						if (*m_items[parent].m_value <= 0)
							break;

						parent = m_items[parent].m_parent;
					}

					if (parent >= 0)
						continue;

					if (!m_items[i].m_value)
						continue;

					nextItem = i;
					break;
				}
			}

			m_selected = nextItem;
			dontForward = true;
		}

		if (wParam == VK_DOWN)
		{
			int nextItem = 0;

			for (int i = m_selected + 1; i < m_items.size(); ++i)
			{
				int parent = m_items[i].m_parent;

				while (parent >= 0)
				{
					if (*m_items[parent].m_value <= 0)
						break;

					parent = m_items[parent].m_parent;
				}

				if (parent >= 0)
					continue;

				if (!m_items[i].m_value)
					continue;

				nextItem = i;
				break;
			}

			m_selected = nextItem;
			dontForward = true;
		}

		if (wParam == VK_LEFT)
		{
			if (m_items[m_selected].m_value &&
				*m_items[m_selected].m_value > 0)
			{
				--* m_items[m_selected].m_value;
			}

			dontForward = true;
		}

		if (wParam == VK_RIGHT)
		{
			if (m_items[m_selected].m_value &&
				*m_items[m_selected].m_value < m_items[m_selected].m_options.size() - 1)
			{
				++* m_items[m_selected].m_value;
			}

			dontForward = true;
		}
	}

	return dontForward;
}

void Menu::render()
{
	menuOpen = m_show;
	if (m_show)
	{
		Renderer::OutlineRect(m_pos, m_bounds, Colors::bg, Colors::border, 0.5f);
		Vector2 renderPos = { 35.f,m_pos.y };

		Renderer::OutlineRect({ renderPos.x - 5.f,renderPos.y }, { m_bounds.x,m_pos.y }, Colors::bg, Colors::border, 0.5f);
		Renderer::DrawString({ renderPos.x + 85.f,renderPos.y + 12.5f }, m_title.c_str(), true, Colors::title);

		renderPos = renderPos + Vector2(0, 30.f);

		int itemsToRender = 0;

		for (int i = 0; i < m_items.size(); ++i)
		{
			int parent = m_items[i].m_parent;

			while (parent >= 0) {
				if (*m_items[parent].m_value <= 0)
					break;

				parent = m_items[parent].m_parent;
			}

			if (parent >= 0)
				continue;

			++itemsToRender;
		}


		for (const auto& item : m_items)
		{
			if (item.m_parent >= 0 && *m_items[item.m_parent].m_value <= 0)
				continue;

			float childIndent = 0.f;
			int parent = item.m_parent;

			while (parent >= 0)
			{
				if (*m_items[parent].m_value <= 0)
					break;

				childIndent += 10.f;
				parent = m_items[parent].m_parent;
			}

			if (parent >= 0)
				continue;

			D2D1::ColorF color = Colors::item;

			if (item.m_parent < 0 && item.m_value) {
				color = Colors::group;
			}
			else if (!item.m_value) {
				color = Colors::label;
			}
			else if (*item.m_value > 0 || (item.m_parent >= 0 && m_items[item.m_parent].m_parent >= 0)) // nested child
			{
				color = Colors::active;
			}

			if (&item == &m_items[m_selected]) {
				color = Colors::selected;
			}

			Renderer::DrawString({ renderPos.x + childIndent , renderPos.y }, item.m_name.c_str(), false, color);

			if (item.m_value) {
				Renderer::DrawString({ 30.f + m_bounds.x - 20.f, renderPos.y }, item.m_options[*item.m_value].c_str(), false, color);
			}

			renderPos = renderPos + Vector2(0.f, 15.f);
		}

	}
}