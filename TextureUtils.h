#pragma once
#include <d3d11.h>

class TextureUtils {
public:
	TextureUtils(D3D11_TEXTURE2D_DESC _defaultTextureDecs, ID3D11Device* _defaultDevice);
	~TextureUtils();

	void createRenderTextureView(ID3D11Texture2D** resultTexture);

	ID3D11Texture2D* prevRenderTexture;
	ID3D11Texture2D* currentRenderTexture;
private:
	//
	ID3D11Device* defaultDevice;
	//

	D3D11_TEXTURE2D_DESC defaultTextureDecs;

};
