#include "TextureManager.h"

TextureManager::TextureManager(ID3D11Device* device)
{
	_device = device;
}

TextureManager::~TextureManager()
{
}

void TextureManager::AddTexture(LPCWSTR name)
{
	ID3D11ShaderResourceView* tempTex = nullptr;
	CreateDDSTextureFromFile(_device, name, nullptr, &tempTex);
	_textures.emplace_back(tempTex);
	tempTex = nullptr;
}