#pragma once
#include "Renderer.h"

struct RenderData {
	int frame;
	int NumberOfSpheres;
	float NumberOfRaysPerPixel;
	float _f;
};
struct Vertex {
	float x, y;
	float r, g, b;
};

struct float3 {
	float x;
	float y;
	float z;
	float3() {}
	float3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

struct float4 {
	float x;
	float y;
	float z;
	float w;
	float4() {}
	float4(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
};

struct RayTracingMaterial
{
	float4 color;
	float4 emissionColor;
	float emissionStrength;
	float smoothness;
	float specularProbability;
	float _;
	float4 specularColor;
};

struct Sphere
{
	float3 position;
	float radius;
	RayTracingMaterial material;
	Sphere()
	{

	}
};

struct RenderData1 {
	Sphere spheres[4];
};

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