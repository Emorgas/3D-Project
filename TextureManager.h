#pragma once

#include <d3d11_1.h>
#include <vector>
#include "DDSTextureLoader.h"

using namespace DirectX;

class TextureManager
{
private:
	std::vector<ID3D11ShaderResourceView*> _textures;
	ID3D11Device* _device;
public:
	TextureManager(ID3D11Device* device);
	~TextureManager();
	void AddTexture(LPCWSTR name);
	ID3D11ShaderResourceView* GetTextureByIndex(int index) { return _textures.at(index); }
};

