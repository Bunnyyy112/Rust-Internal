#include <RenderHandler.h>
#include <Globals.hpp>

void DrawScene()
{
	EntityLoop();
	if (impl::crosshair)
	{
		Renderer::Line({ ScreenCenterX,ScreenCenterY - 2 }, { ScreenCenterX,ScreenCenterY + 2 }, D2D1::ColorF::White, 0.1f);
		Renderer::Line({ ScreenCenterX - 2,ScreenCenterY }, { ScreenCenterX + 2,ScreenCenterY }, D2D1::ColorF::White, 0.1f);
	}
	if (impl::fovCircle)
	{
		Renderer::Circle({ ScreenCenterX,ScreenCenterY }, D2D1::ColorF::Red, impl::fovSlider, 1.0f);
	}
}