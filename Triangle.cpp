#include "Triangle.h"
#include <fstream>
#include <vector>
#include <iostream>
#include "BVHDataTypes.h"
#include "ObjReader.h"

Triangle::Triangle(Renderer& renderer, ObjReader& reader)
{
	viewPortWidth = renderer.viewPortWidth;
	viewPortHeight = renderer.viewPortHeight;

	createData(reader);
	createMesh(renderer);
	createShaders(renderer);
}

Triangle::~Triangle()
{
	m_vertexBuffer->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_inputLayout->Release();
}

void Triangle::addSpheres(ID3D11DeviceContext* deviceContext)
{
	// Constant render sphere buffer
	RenderData1 SphereData;
	SphereData.spheres[0] = {};
	SphereData.spheres[1] = {};
	SphereData.spheres[2] = {};

	SphereData.spheres[0].position = float3(0, -2, -2);
	SphereData.spheres[0].radius = 1;
	SphereData.spheres[0].material.color = float4(1, 1, 1, 1);
	SphereData.spheres[0].material.emissionColor = float4(0, 0, 1, 1);
	SphereData.spheres[0].material.emissionStrength = 50;

	SphereData.spheres[1].position = float3(0, -2, 2);
	SphereData.spheres[1].radius = 1;
	SphereData.spheres[1].material.color = float4(1, 1, 1, 1);
	SphereData.spheres[1].material.smoothness = 1;

	SphereData.spheres[2].position = float3(0, 1.75, 0);
	SphereData.spheres[2].radius = 1;
	SphereData.spheres[2].material.color = float4(1, 1, 1, 1);
	SphereData.spheres[2].material.emissionColor = float4(1, 1, 1, 1);
	SphereData.spheres[2].material.emissionStrength = 10;

	D3D11_MAPPED_SUBRESOURCE sphereResource;
	ZeroMemory(&sphereResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deviceContext->Map(m_constantSphereBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sphereResource);

	memcpy(sphereResource.pData, &SphereData, sizeof(SphereData));
	deviceContext->Unmap(m_constantSphereBuffer, 0);

	deviceContext->PSSetConstantBuffers(1, 1, &m_constantSphereBuffer);

	// Constant render buffer
	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	RenderData constBuffData;
	constBuffData.frame = frame;
	constBuffData.NumberOfSpheres = 3;
	constBuffData.NumMeshes = MeshInfos.size();
	constBuffData.NumberOfRaysPerPixel = 1.0;

	constBuffData.SunLightDirection = float4(-1, -1, -1);
	constBuffData.SkyColorHorizon = float4(1, 0, 0);
	constBuffData.SkyColorZenith = float4(1, 1, 0);
	constBuffData.GroundColor = float4(1, 1, 0);
	constBuffData.SunColor = float4(1, 1, 0);
	constBuffData.SunFocus = 1;
	constBuffData.SunIntensity = 0;
	constBuffData.EnvironmentIntensity = 0;
	constBuffData.isTestVisualizer = isTestVisualizer;
	constBuffData.screenWidth = viewPortWidth;
	constBuffData.screenHeight = viewPortHeight;

	if (isTestVisualizer == 0)
		frame += 1;

	memcpy(resource.pData, &constBuffData, sizeof(RenderData));
	deviceContext->Unmap(m_constantBuffer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);
}

void Triangle::addTriangles(ID3D11DeviceContext* deviceContext)
{
	// As soon as the resource view is created with buffer x, if we update buffer x we update the resource view
	deviceContext->PSSetShaderResources(2, 1, &m_nodeResourceView);
	deviceContext->PSSetShaderResources(3, 1, &m_triangleResourceView);
	deviceContext->PSSetShaderResources(4, 1, &m_meshResourceView);
}

void Triangle::createData(ObjReader& reader)
{
	Nodes = reader.bvhObject.Nodes;
	Triangles = reader.bvhObject.Triangles;
	MeshInfos.push_back(reader.bvhObject.MeshInfo);

	return;
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
	constBuffData.NumberOfRaysPerPixel = 1.0;

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

	D3D11_BUFFER_DESC nodeDesc = {};
	// Default takes initial value dynamic doesn't, dynamic needs to be set with map/unmap and stuff
	nodeDesc.Usage = D3D11_USAGE_DEFAULT;
	nodeDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	nodeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	nodeDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	nodeDesc.ByteWidth = sizeof(BVHNode) * Nodes.size(); // Total bytes
	nodeDesc.StructureByteStride = sizeof(BVHNode); // Size of one object

	D3D11_SUBRESOURCE_DATA nodesSubData = {};
	nodesSubData.pSysMem = Nodes.data();
	nodesSubData.SysMemPitch = 0;
	nodesSubData.SysMemSlicePitch = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC nodesViewDesc = {};
	nodesViewDesc.Format = DXGI_FORMAT_UNKNOWN; // Structured buffers use UNKNOWN format
	nodesViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	nodesViewDesc.Buffer.FirstElement = 0;
	nodesViewDesc.Buffer.ElementWidth = Nodes.size();

	renderer.getDevice()->CreateBuffer(&nodeDesc, &nodesSubData, &m_nodeBuffer);
	renderer.getDevice()->CreateShaderResourceView(m_nodeBuffer, &nodesViewDesc, &m_nodeResourceView);

	D3D11_BUFFER_DESC triangleDesc = {};
	triangleDesc.Usage = D3D11_USAGE_DEFAULT;
	triangleDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	triangleDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	triangleDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	triangleDesc.ByteWidth = sizeof(BVHTriangle) * Triangles.size(); // Total bytes
	triangleDesc.StructureByteStride = sizeof(BVHTriangle); // Size of one object

	D3D11_SUBRESOURCE_DATA trigsSubData = {};
	trigsSubData.pSysMem = Triangles.data();
	trigsSubData.SysMemPitch = 0;
	trigsSubData.SysMemSlicePitch = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC trigsViewDesc = {};
	trigsViewDesc.Format = DXGI_FORMAT_UNKNOWN; // Structured buffers use UNKNOWN format
	trigsViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	trigsViewDesc.Buffer.FirstElement = 0;
	trigsViewDesc.Buffer.ElementWidth = Triangles.size();

	renderer.getDevice()->CreateBuffer(&triangleDesc, &trigsSubData, &m_triangleBuffer);
	renderer.getDevice()->CreateShaderResourceView(m_triangleBuffer, &trigsViewDesc, &m_triangleResourceView);

	D3D11_BUFFER_DESC meshDesc = {};
	meshDesc.Usage = D3D11_USAGE_DEFAULT;
	meshDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	meshDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	meshDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	meshDesc.ByteWidth = sizeof(MeshInfo) * MeshInfos.size(); // Total bytes
	meshDesc.StructureByteStride = sizeof(MeshInfo); // Size of one object

	D3D11_SUBRESOURCE_DATA meshesSubData = {};
	meshesSubData.pSysMem = MeshInfos.data();
	meshesSubData.SysMemPitch = 0;
	meshesSubData.SysMemSlicePitch = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC meshesViewDesc = {};
	meshesViewDesc.Format = DXGI_FORMAT_UNKNOWN; // Structured buffers use UNKNOWN format
	meshesViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	meshesViewDesc.Buffer.FirstElement = 0;
	meshesViewDesc.Buffer.ElementWidth = MeshInfos.size();

	renderer.getDevice()->CreateBuffer(&meshDesc, &meshesSubData, &m_meshBuffer);
	renderer.getDevice()->CreateShaderResourceView(m_meshBuffer, &meshesViewDesc, &m_meshResourceView);
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

	// Create input layouts
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	renderer.getDevice()->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &m_inputLayout);
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
	Triangle::addSpheres(deviceContext);
	Triangle::addTriangles(deviceContext);

	// Draw to screen
	deviceContext->Draw(6, 0);
}
