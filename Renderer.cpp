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
}

void Renderer::setRenderTarget(ID3D11RenderTargetView* currentTarget)
{
	// Bind render target
	m_deviceContext->OMSetRenderTargets(1, &currentTarget, nullptr);

	// Set viewpot
	auto viewPort = CD3D11_VIEWPORT(0.f, 0.f, (float)m_backBufferDesc.Width, (float)m_backBufferDesc.Height);
	m_deviceContext->RSSetViewports(1, &viewPort);

	// Set the background color
	float clearColor[] = { .25f, .5f, 1, 1 };
	m_deviceContext->ClearRenderTargetView(currentTarget, clearColor);
}

void Renderer::Present()
{
	m_swapChain->Present(1, 0);
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