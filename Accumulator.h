#pragma once
#include "Renderer.h"
#include "DataTypes.h"

class Accumulator {
public:
	Accumulator(Renderer& renderer);
	~Accumulator();

	void createShaders(Renderer& renderer);
	void createMidTextures(Renderer& renderer);
	void draw(Renderer& renderer, ID3D11Texture2D* previousFrame, ID3D11Texture2D* currentFrame);

	float frame = 0;
	int viewPortWidth;
	int viewPortHeight;

private:
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;

	ID3D11InputLayout* m_inputLayout = nullptr;

	ID3D11Texture2D* previousCopyTexture;
	ID3D11Texture2D* currentCopyTexture;
	ID3D11Texture2D* currentTexture;
};
