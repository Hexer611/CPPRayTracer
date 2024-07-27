#include <windows.h>
#include "Window.h"
#include "Renderer.h"
#include "Triangle.h"
#include "Accumulator.h"
#include "TextureUtils.h"
#include <iostream>
#include <sstream>
#include <string>
#include "ObjReader.h"
#include "BVHCalculator.h"
#include <dwrite.h>

#define MAX_NAME_STRING 256
#define HInstance() GetModuleHandle(NULL)

WCHAR WindowClass[MAX_NAME_STRING];
WCHAR WindowTitle[MAX_NAME_STRING];

INT WindowWidth;
INT WindowHeight;

int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	ObjReader reader;
	reader.ReadFile("dragonlow.obj", false, float3(0,0,0), float3(0,180,0), float3(1,1,1));

	ObjReader reader1;
	//reader1.ReadFile("suzanne.obj", false, float3(0,0,-0.4), float3(0,-20,0), float3(1,1,1)/6);

	ObjReader reader2;
	//reader2.ReadFile("fireplace_room_low.obj", false, float3(0, 0, 0), float3(0, -90, 0), float3(1, 1, 1) / 6);

	Window window(1920/2, 1080/2);
	Renderer renderer(window);
	Triangle triangle(renderer);
	Accumulator accumulator(renderer);
	TextureUtils textureUtil(renderer.m_backBufferDesc, renderer.getDevice());

	ID3D11Texture2D* resultTexture;

	textureUtil.createRenderTextureView(&resultTexture);
	textureUtil.currentRenderTexture = resultTexture;

	textureUtil.createRenderTextureView(&resultTexture);
	textureUtil.prevRenderTexture = resultTexture;

	triangle.addData(reader);
	//triangle.addData(reader1);
	//triangle.addData(reader2);

	triangle.createBuffers(renderer);

	triangle.draw(renderer);
	renderer.copyRenderTexture(&textureUtil.prevRenderTexture);

	triangle.isTestVisualizer = 0;

	MSG msg = { 0 };
	int curFrame = 0;
	int displayTrheshold = 100;
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
			triangle.xRot = msg.pt.x;
			triangle.yRot = msg.pt.y;
		}

		// Main loop
		// Update
		// Draw
		triangle.draw(renderer);
		renderer.copyRenderTexture(&textureUtil.currentRenderTexture);

		if (triangle.isTestVisualizer == 0)
		{
			accumulator.draw(renderer, textureUtil.prevRenderTexture, textureUtil.currentRenderTexture);
			renderer.copyRenderTexture(&textureUtil.prevRenderTexture);
		}

		curFrame++;
		wchar_t frameText[256];
		swprintf_s(frameText, L"%d", curFrame);

		if (curFrame % 1 == 0 && triangle.isTestVisualizer == 0)
		{
			renderer.CreateText(frameText);
			renderer.Present();
		}
		if (triangle.isTestVisualizer == 1)
		{
			renderer.CreateDebugText(reader);
			renderer.Present();
		}
		continue;
		renderer.saveRenderTexture(textureUtil.prevRenderTexture, "C:/Users/Administrator/Desktop/test/rtx.dds");

		std::string outFolder = "C:/Users/Administrator/Desktop/test/";
		outFolder += "accumulator";
		outFolder += std::to_string(accumulator.frame);
		outFolder += ".dds";
		
		renderer.saveRenderTexture(textureUtil.prevRenderTexture, outFolder.c_str());

		if (curFrame == 10)
		{
			break;
		}
	}

	return 0;
}
