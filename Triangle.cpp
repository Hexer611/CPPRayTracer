#include "Triangle.h"
#include <fstream>
#include <vector>

struct Vertex {
	float x, y;
	float r, g, b;
};

Triangle::Triangle(Renderer& renderer)
{
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

		{1,-1,0,0,1},
		{-1,1,1,0,0},
		{1,1,0,1,0},
	};

	// Create our vertex buffer
	auto vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;

	renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
}

void Triangle::createShaders(Renderer& renderer)
{
	// Create shaders
	std::ifstream vsFile("VertexShader.cso", std::ios::binary);
	std::ifstream psFile("PixelShader.cso", std::ios::binary);

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
