#include "TextureUtils.h"

TextureUtils::TextureUtils(D3D11_TEXTURE2D_DESC _defaultTextureDecs, ID3D11Device* _defaultDevice)
{
	defaultTextureDecs = _defaultTextureDecs;
	defaultDevice = _defaultDevice;
}

TextureUtils::~TextureUtils()
{

}

void TextureUtils::createRenderTextureView(ID3D11Texture2D** resultTexture, ID3D11RenderTargetView** resultTextureView)
{
	D3D11_TEXTURE2D_DESC textureDesc;

	textureDesc.Width = defaultTextureDecs.Width;
	textureDesc.Height = defaultTextureDecs.Height;
	textureDesc.Format = defaultTextureDecs.Format;

	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Quality = 0;

	defaultDevice->CreateTexture2D(&textureDesc, nullptr, resultTexture);
	defaultDevice->CreateRenderTargetView(*resultTexture, nullptr, resultTextureView);
}

void TextureUtils::createMainView(D3D11_TEXTURE2D_DESC textureDesc1)
{
	D3D11_TEXTURE2D_DESC textureDesc;

	textureDesc.Width = defaultTextureDecs.Width;
	textureDesc.Height = defaultTextureDecs.Height;
	textureDesc.Format = defaultTextureDecs.Format;

	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Quality = 0;

	defaultDevice->CreateTexture2D(&textureDesc1, nullptr, &mainRenderTexture);
	defaultDevice->CreateRenderTargetView(mainRenderTexture, nullptr, &mainRenderTextureView);
}