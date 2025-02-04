#pragma once
#include "Renderer.h"
#include "DataTypes.h"
#include "BVHDataTypes.h"
#include "ObjReader.h"
#include <vector>

class Triangle {
public:
	Triangle(Renderer& renderer);
	~Triangle();
	void draw(Renderer& renderer);
	void addSpheres(ID3D11DeviceContext* deviceContext);
	void addTriangles(ID3D11DeviceContext* device);
	void updateRenderData(ID3D11DeviceContext* deviceContext);
	void addData(ObjReader& reader);
	void createBuffers(Renderer& renderer);
	
	int isTestVisualizer;

	// Viewport
	int viewPortWidth;
	int viewPortHeight;
	float xRot = 0;
	float yRot = 0;
private:
	float frame = 0;
	void createData();
	void createShaders(Renderer& renderer);

	RenderData* m_renderData = nullptr;

	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_constantBuffer = NULL;
	ID3D11Buffer* m_constantSphereBuffer = NULL;

	ID3D11Buffer* m_nodeBuffer = NULL;
	ID3D11Buffer* m_meshBuffer = NULL;
	ID3D11Buffer* m_triangleBuffer = NULL;

	std::vector<BVHNode> Nodes;
	std::vector<BVHTriangleData> Triangles;
	std::vector<MeshInfo> MeshInfos;

	ID3D11ShaderResourceView* m_nodeResourceView = nullptr;
	ID3D11ShaderResourceView* m_triangleResourceView = nullptr;
	ID3D11ShaderResourceView* m_meshResourceView = nullptr;

	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
};