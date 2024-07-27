#include "Renderer.h"
#include "D3DX11tex.h"
#include <memory>
#include <SpriteFont.h>
#include <SimpleMath.h>

Renderer::Renderer(Window& window)
{
	createDevice(window);
	createRenderTarget();
	setRenderTarget();
}

void Renderer::createDevice(Window& window)
{
	// Define our swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = window.getHandle();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	// Create swap chain device and device context

	auto result = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&m_swapChain, &m_device, nullptr, &m_deviceContext);

	// Check for errors

	if (result != S_OK)
	{
		MessageBox(nullptr, "There is a problem here man", "Error", MB_OK);
		exit(0);
	}
}

void Renderer::createRenderTarget()
{
	ID3D11Texture2D* backBuffer;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) & backBuffer);
	m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);

	m_font = std::make_unique<DirectX::SpriteFont>(m_device, L"myfile.spritefont");
	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_deviceContext);

	backBuffer->GetDesc(&m_backBufferDesc);
	backBuffer->Release();

	viewPortWidth = m_backBufferDesc.Width;
	viewPortHeight = m_backBufferDesc.Height;
}
 
void Renderer::setRenderTarget()
{
	// Bind render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	// Set viewpot
	auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, (float)m_backBufferDesc.Width, (float)m_backBufferDesc.Height);
	m_deviceContext->RSSetViewports(1, &viewPort);

	// Set the background color
	float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
}

void Renderer::CreateText(const wchar_t* text)
{
	DirectX::SimpleMath::Vector2 m_fontPos;
	
	m_fontPos.x = 100.0;
	m_fontPos.y = 100.0;

	//m_spriteBatch.reset();

	m_spriteBatch->Begin();

	DirectX::SimpleMath::Vector2 origin = m_font->MeasureString(text);
	origin.x /= 2;
	origin.y /= 2;

	m_font->DrawString(m_spriteBatch.get(), text,
		m_fontPos, DirectX::Colors::White, 0.f, origin);

	m_spriteBatch->End();
}

void Renderer::CreateDebugText(ObjReader reader)
{
	DirectX::SimpleMath::Vector2 m_fontPos;

	m_fontPos.x = 200.0;
	m_fontPos.y = 100.0;

	//m_spriteBatch.reset();

	string debugText = "";
	debugText.append("Max leaf trig : ");
	debugText.append(to_string(reader.debugData.maxLeafTrig));
	debugText.append("\n");
	debugText.append("Min leaf trig : ");
	debugText.append(to_string(reader.debugData.minLeafTrig));
	debugText.append("\n");
	debugText.append("Max depth trig : ");
	debugText.append(to_string(reader.debugData.maxLeafDepth));
	debugText.append("\n");
	debugText.append("Min depth trig : ");
	debugText.append(to_string(reader.debugData.minLeafDepth));
	debugText.append("\n");

	m_spriteBatch->Begin();

	DirectX::SimpleMath::Vector2 origin = m_font->MeasureString(debugText.c_str());
	origin.x /= 2;
	origin.y /= 2;

	m_font->DrawString(m_spriteBatch.get(), debugText.c_str(),
		m_fontPos, DirectX::Colors::White, 0.f, origin, 0.75f);

	m_spriteBatch->End();
}

void Renderer::Present()
{
	m_swapChain->Present(1, 0);

	float clearColor[] = { 0.f, 0.f, 0.f, 1.f };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
}

void Renderer::copyRenderTexture(ID3D11Texture2D** target)
{
	ID3D11Resource* backResource;
	m_renderTargetView->GetResource(&backResource);
	m_deviceContext->CopyResource(*target, backResource);
}

void Renderer::saveRenderTexture(const char str[])
{
	ID3D11Resource* backResource;
	m_renderTargetView->GetResource(&backResource);

	auto res = D3DX11SaveTextureToFile(m_deviceContext, backResource, D3DX11_IFF_DDS, str);

	backResource->Release();
}

void Renderer::saveRenderTexture(ID3D11Resource* target, const char str[])
{
	auto res = D3DX11SaveTextureToFile(m_deviceContext, target, D3DX11_IFF_DDS, str);
}

void Renderer::saveRenderTexture(ID3D11RenderTargetView* target, const char str[])
{
	ID3D11Resource* backResource;
	target->GetResource(&backResource);

	auto res = D3DX11SaveTextureToFile(m_deviceContext, backResource, D3DX11_IFF_DDS, str);

	backResource->Release();
}

ID3D11Device* Renderer::getDevice()
{
	return m_device;
}
ID3D11DeviceContext* Renderer::getDeviceContext()
{
	return m_deviceContext;
}