#include <windows.h>
#include "Window.h"
#include "Renderer.h"
#include "Triangle.h"
#include "Accumulator.h"
#include "TextureUtils.h"

#define MAX_NAME_STRING 256
#define HInstance() GetModuleHandle(NULL)

WCHAR WindowClass[MAX_NAME_STRING];
WCHAR WindowTitle[MAX_NAME_STRING];

INT WindowWidth;
INT WindowHeight;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	Window window(600, 400);
	Renderer renderer(window);
	Triangle triangle(renderer);
	Accumulator accumulator(renderer);
	TextureUtils textureUtil(renderer.m_backBufferDesc, renderer.getDevice());

	ID3D11Texture2D* resultTexture;
	ID3D11RenderTargetView* resultTextureView;

	textureUtil.createRenderTextureView(&resultTexture, &resultTextureView);
	textureUtil.rayTracerRenderTexture = resultTexture;
	textureUtil.rayTracerRenderTextureView = resultTextureView;
	//textureUtil.createRenderTextureView(&textureUtil.accumulateRenderTexture, &textureUtil.accumulateRenderTextureView);

	MSG msg = { 0 };
	while (true)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				break;
			}
		} 

		// Main loop
		// Update
		// Draw
		renderer.setRenderTarget(textureUtil.accumulateRenderTextureView);
		triangle.draw(renderer);
		//renderer.saveRenderTexture();

		//renderer.beginFrame1();
		//accumulator.draw(renderer);
		renderer.Present();
	}

	return 0;
}
