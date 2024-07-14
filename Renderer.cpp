#include "Renderer.h"
#include "D3DX11tex.h"

Renderer::Renderer(Window& window)
{
	createDevice(window);
	createRenderTarget();
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
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
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

	backBuffer->GetDesc(&m_backBufferDesc);
	backBuffer->Release();

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = m_backBufferDesc.Width;
	textureDesc.Height = m_backBufferDesc.Height;
	textureDesc.Format = m_backBufferDesc.Format;

	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = 0;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Quality = 0;

	m_device->CreateTexture2D(&textureDesc, nullptr, &renderTextureMain);
}

void Renderer::setRenderTarget(ID3D11RenderTargetView* currentTarget)
{
	// Bind render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	// Set viewpot
	auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, (float)m_backBufferDesc.Width, (float)m_backBufferDesc.Height);
	m_deviceContext->RSSetViewports(1, &viewPort);

	// Set the background color
	float clearColor[] = { .25f, .5f, 1, 1 };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
}

void Renderer::Present()
{
	m_swapChain->Present(1, 0);
}

void Renderer::saveRenderTexture()
{
	ID3D11Resource* backResource;
	m_renderTargetView->GetResource(&backResource);
	m_deviceContext->CopyResource(renderTextureMain, backResource);

	auto res = D3DX11SaveTextureToFile(m_deviceContext, renderTextureMain, D3DX11_IFF_DDS, "C:/Users/Administrator/Desktop/test/erdem.dds");

	backResource->Release();
}

void Renderer::beginFrame2()
{
	// Bind render target
	m_deviceContext->OMSetRenderTargets(1, &m_accumulateTargetView, nullptr);

	// Set viewpot
	auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, (float)m_backBufferDesc.Width, (float)m_backBufferDesc.Height);
	m_deviceContext->RSSetViewports(1, &viewPort);

	// Set the background color
	float clearColor[] = { .25f, .5f, 1, 1 };
	m_deviceContext->ClearRenderTargetView(m_accumulateTargetView, clearColor);
}

void Renderer::endFrame2()
{
	m_swapChain->Present(1, 0);
	m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

ID3D11Device* Renderer::getDevice()
{
	return m_device;
}
ID3D11DeviceContext* Renderer::getDeviceContext()
{
	return m_deviceContext;
}