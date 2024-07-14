#include "Triangle.h"
#include <fstream>
#include <vector>
#include <iostream>

Triangle::Triangle(Renderer& renderer)
{
	createMesh(renderer);
	createShaders(renderer);
	auto _renderData = RenderData();
	m_renderData = &_renderData;
}

Triangle::~Triangle()
{
	m_vertexBuffer->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_pixelShader1->Release();
	m_inputLayout->Release();
}

void Triangle::updateRenderData(ID3D11DeviceContext* deviceContext)
{
	// Constant render sphere buffer
	RenderData1 SphereData;
	SphereData.spheres[0] = {};
	SphereData.spheres[1] = {};
	SphereData.spheres[2] = {};
	
	SphereData.spheres[0].position = float3(-2,-2,10);
	SphereData.spheres[0].radius = 1;
	SphereData.spheres[0].material.color = float4(1, 1, 1, 1);
	SphereData.spheres[0].material.emissionColor = float4(0,0,1,1);
	SphereData.spheres[0].material.emissionStrength = 5;

	SphereData.spheres[1].position = float3(2,-2,10);
	SphereData.spheres[1].radius = 1;
	SphereData.spheres[1].material.color = float4(1, 1, 1, 1);
	SphereData.spheres[1].material.smoothness = 1;

	SphereData.spheres[2].position = float3(0, 2, 10);
	SphereData.spheres[2].radius = 1;
	SphereData.spheres[2].material.color = float4(1, 1, 1, 1);
	SphereData.spheres[2].material.emissionColor = float4(0, 1, 0, 1);
	SphereData.spheres[2].material.emissionStrength = 5;
	
	D3D11_MAPPED_SUBRESOURCE sphereResource;
	ZeroMemory(&sphereResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deviceContext->Map(m_constantSphereBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sphereResource);

	memcpy(sphereResource.pData, &SphereData, sizeof(SphereData));
	deviceContext->Unmap(m_constantBuffer, 0);

	deviceContext->PSSetConstantBuffers(1, 1, &m_constantSphereBuffer);
	
	// Constant render buffer
	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	RenderData constBuffData;
	constBuffData.frame = frame;
	constBuffData.NumberOfRaysPerPixel = 10.0;
	constBuffData.NumberOfSpheres = 3;
	frame += 1;

	memcpy(resource.pData, &constBuffData, sizeof(RenderData));
	deviceContext->Unmap(m_constantBuffer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);
}

void Triangle::draw(Renderer& renderer)
{
	auto deviceContext = renderer.getDeviceContext();

	// Bind shaders
	deviceContext->IASetInputLayout(m_inputLayout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	// Bind our vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Update our constant buffer
	Triangle::updateRenderData(deviceContext);

	// Draw
	deviceContext->Draw(6, 0);
}

void Triangle::draw2(Renderer& renderer)
{
	auto deviceContext = renderer.getDeviceContext();

	// Bind shaders
	deviceContext->IASetInputLayout(m_inputLayout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader1, nullptr, 0);

	// Bind our vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Update our constant buffer
	//Triangle::updateRenderData(deviceContext);

	// Draw
	deviceContext->Draw(6, 0);
}
void Triangle::createMesh(Renderer& renderer)
{
	// Define our vertices
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

	RenderData constBuffData;
	constBuffData.frame = 0.0;
	constBuffData.NumberOfRaysPerPixel = 10.0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &constBuffData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(RenderData);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	renderer.getDevice()->CreateBuffer(&cbDesc, &InitData, &m_constantBuffer);

	RenderData1 sphereData;

	D3D11_SUBRESOURCE_DATA sphereSubData;
	sphereSubData.pSysMem = &sphereData;
	sphereSubData.SysMemPitch = 0;
	sphereSubData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC sphereDataDesc;
	sphereDataDesc.ByteWidth = sizeof(sphereData);
	sphereDataDesc.Usage = D3D11_USAGE_DYNAMIC;
	sphereDataDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sphereDataDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sphereDataDesc.MiscFlags = 0;
	sphereDataDesc.StructureByteStride = 0;

	renderer.getDevice()->CreateBuffer(&sphereDataDesc, &sphereSubData, &m_constantSphereBuffer);
}

void Triangle::createShaders(Renderer& renderer)
{
	// Create shaders
	std::ifstream vsFile("VertexShader.cso", std::ios::binary);
	std::ifstream psFile("PixelShader.cso", std::ios::binary);
	std::ifstream ps1File("PixelShader1.cso", std::ios::binary);

	std::vector<char> vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	std::vector<char> psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	std::vector<char> ps1Data = { std::istreambuf_iterator<char>(ps1File), std::istreambuf_iterator<char>() };

	renderer.getDevice()->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &m_vertexShader);
	renderer.getDevice()->CreatePixelShader(psData.data(), psData.size(), nullptr, &m_pixelShader);
	renderer.getDevice()->CreatePixelShader(ps1Data.data(), ps1Data.size(), nullptr, &m_pixelShader1);

	// Create input layouts
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	renderer.getDevice()->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &m_inputLayout);
}
