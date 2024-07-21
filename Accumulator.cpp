#include "Accumulator.h"
#include <fstream>
#include <vector>
#include <iostream>

Accumulator::Accumulator(Renderer& renderer)
{
	viewPortWidth = renderer.viewPortWidth;
	viewPortHeight = renderer.viewPortHeight;

	createShaders(renderer);
	createMidTextures(renderer);
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

void Accumulator::createMidTextures(Renderer& renderer)
{
	auto deviceContext = renderer.getDeviceContext();

	// TODO: Use texture utils here
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

	std::vector<UINT32> redData(textureDesc.Width * textureDesc.Height, 0xFF0000F0);
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = redData.data();
	initData.SysMemPitch = textureDesc.Width * sizeof(UINT32); // Pitch of the texture

	renderer.getDevice()->CreateTexture2D(&textureDesc, &initData, &previousCopyTexture);
	renderer.getDevice()->CreateTexture2D(&textureDesc, &initData, &currentCopyTexture);

	renderer.getDevice()->CreateShaderResourceView(previousCopyTexture, nullptr, &previousCopyTextureResourceView);
	renderer.getDevice()->CreateShaderResourceView(currentCopyTexture, nullptr, &currentCopyTextureResourceView);
	
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

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(AccumulatorData);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	renderer.getDevice()->CreateBuffer(&cbDesc, nullptr, &m_constantBuffer);
}

void Accumulator::draw(Renderer& renderer, ID3D11Texture2D* previousFrame, ID3D11Texture2D* currentFrame)
{
	auto deviceContext = renderer.getDeviceContext();

	deviceContext->IASetInputLayout(m_inputLayout);

	// Bind shaders
	deviceContext->IASetInputLayout(m_inputLayout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	deviceContext->CopyResource(previousCopyTexture, previousFrame);
	deviceContext->CopyResource(currentCopyTexture, currentFrame);

	deviceContext->PSSetShaderResources(0, 1, &currentCopyTextureResourceView);
	deviceContext->PSSetShaderResources(1, 1, &previousCopyTextureResourceView);

	// Bind our vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Update our constant buffer

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	AccumulatorData constBuffData;
	constBuffData.frame = frame;
	constBuffData.screenWidth = viewPortWidth;
	constBuffData.screenHeight = viewPortHeight;
	frame += 1;

	memcpy(resource.pData, &constBuffData, sizeof(AccumulatorData));
	deviceContext->Unmap(m_constantBuffer, 0);

	deviceContext->PSSetConstantBuffers(2, 1, &m_constantBuffer);

	// Draw
	deviceContext->Draw(6, 0);
}
