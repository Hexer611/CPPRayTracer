#include "Accumulator.h"
#include <fstream>
#include <vector>
#include <iostream>

Accumulator::Accumulator(Renderer& renderer)
{
	createShaders(renderer);
}

Accumulator::~Accumulator()
{
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_inputLayout->Release();
}

void Accumulator::createShaders(Renderer& renderer)
{
	// Create shaders
	std::ifstream vsFile("AccumulatorVertexShader.cso", std::ios::binary);
	std::ifstream psFile("AccumulatorPixelShader.cso", std::ios::binary);

	std::vector<char> vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	std::vector<char> psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };

	renderer.getDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &m_vertexShader);
	renderer.getDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &m_pixelShader);

	// Create input layouts
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	renderer.getDevice()->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &m_inputLayout);
}

void Accumulator::draw(Renderer& renderer)
{
	auto deviceContext = renderer.getDeviceContext();

	deviceContext->IASetInputLayout(m_inputLayout);

	// Bind shaders
	deviceContext->IASetInputLayout(m_inputLayout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	ID3D11ShaderResourceView *shaderResource;
	ID3D11Texture2D* shTexture;

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = renderer.m_backBufferDesc.Width;
	textureDesc.Height = renderer.m_backBufferDesc.Height;
	textureDesc.Format = renderer.m_backBufferDesc.Format;

	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Quality = 0;

	std::vector<UINT32> redData(textureDesc.Width * textureDesc.Height, 0xFF00F0F0);
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = redData.data();
	initData.SysMemPitch = textureDesc.Width * sizeof(UINT32); // Pitch of the texture

	renderer.getDevice()->CreateTexture2D(&textureDesc, &initData, &shTexture);

	deviceContext->CopyResource(shTexture, renderer.renderTextureMain);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	auto res = renderer.getDevice()->CreateShaderResourceView(shTexture, nullptr, &shaderResource);
	deviceContext->PSSetShaderResources(0, 1, &shaderResource);

	ID3D11Buffer* m_vertexBuffer = nullptr;
	Vertex vertices[] = {
		{-1,-1,1,0,0},
		{-1,1,0,1,0},
		{1,-1,0,0,1},

		{1,-1,1,0,1},
		{-1,1,1,0,0},
		{1,1,1,1,0},
	};

	// Create our vertex buffer
	auto vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;

	renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// Bind our vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Update our constant buffer
	//Triangle::updateRenderData(deviceContext);

	// Draw
	deviceContext->Draw(6, 0);
	m_vertexBuffer->Release();
}
