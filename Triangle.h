#pragma once
#include "Renderer.h"
#include "DataTypes.h"

class Triangle {
public:
	Triangle(Renderer& renderer);
	~Triangle();
	void draw(Renderer& renderer);
	void draw2(Renderer& renderer);
	void updateRenderData(ID3D11DeviceContext* deviceContext);

private:
	float frame = 0;
	void createMesh(Renderer& renderer);
	void createShaders(Renderer& renderer);

	RenderData* m_renderData = nullptr;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_constantBuffer = NULL;
	ID3D11Buffer* m_constantSphereBuffer = NULL;
	ID3D11ShaderResourceView* m_constantResourceView = nullptr;
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11PixelShader* m_pixelShader1 = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
};