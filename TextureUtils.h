#pragma once
#include <d3d11.h>

class TextureUtils {
public:
	TextureUtils(D3D11_TEXTURE2D_DESC _defaultTextureDecs, ID3D11Device* _defaultDevice);
	~TextureUtils();

	void createRenderTextureView(ID3D11Texture2D** resultTexture, ID3D11RenderTargetView** resultTextureView);
	void createMainView(D3D11_TEXTURE2D_DESC textureDesc1);

	ID3D11Texture2D* prevRenderTexture;
	ID3D11Texture2D* currentRenderTexture;

	ID3D11Texture2D* rayTracerRenderTexture;
	ID3D11RenderTargetView* rayTracerRenderTextureView;

	ID3D11Texture2D* accumulateRenderTexture;
	ID3D11RenderTargetView* accumulateRenderTextureView;

	ID3D11Texture2D* mainRenderTexture;
	ID3D11RenderTargetView* mainRenderTextureView;
private:
	//
	ID3D11Device* defaultDevice;
	//

	D3D11_TEXTURE2D_DESC defaultTextureDecs;

};
