#pragma once
#include "Window.h"
#include <d3d11.h>

class Renderer {
public:
	Renderer(Window& window);
	void setRenderTarget();
	void Present();
	void beginFrame2();
	void endFrame2();
	void copyRenderTexture(ID3D11Texture2D** target);
	void saveRenderTexture(const char a[]);
	void saveRenderTexture(ID3D11Resource* target, const char a[]);
	void saveRenderTexture(ID3D11RenderTargetView* target, const char a[]);
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();

	ID3D11Texture2D* renderTextureMain;
	D3D11_TEXTURE2D_DESC m_backBufferDesc;

	// Render targets
	ID3D11RenderTargetView* m_accumulateTargetView = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
private:

	void createDevice(Window& window);
	void createRenderTarget();

	// Device stuff
	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
};
