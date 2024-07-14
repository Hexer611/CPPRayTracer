#pragma once
#include "Renderer.h"
#include "DataTypes.h"

class Accumulator {
public:
	Accumulator(Renderer& renderer);
	~Accumulator();

	void createShaders(Renderer& renderer);
	void draw(Renderer& renderer);

private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;

	ID3D11InputLayout* m_inputLayout = nullptr;
};
