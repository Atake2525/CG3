#include <cassert>
#include "TextureManager.h"
#include "DirectXBase.h"
#include "Logger.h"
#include "StringUtility.h"
#include "DirectXBase.h"

using namespace Logger;
using namespace StringUtility;

TextureManager* TextureManager::instance = nullptr;

// ImGuiで0番を使用するため、1番から使用
uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance() {
	if (instance == nullptr) {
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void TextureManager::Initialize(DirectXBase* directxBase) {
	directxBase_ = directxBase;
	// SRVの数と同数
	textureDatas.reserve(DirectXBase::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath) {
	// 読み込み済テクスチャを検索
	auto it = std::find_if(
		textureDatas.begin(), 
		textureDatas.end(), 
		[&](TextureData& textureData) { return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end()) {
		// 読み込み済なら早期return
		return;
	}

	// テクスチャ枚数上限チェック
	assert(textureDatas.size() + kSRVIndexTop < DirectXBase::kMaxSRVCount); 

	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// テクスチャデータを追加
	textureDatas.resize(textureDatas.size() + 1);

	// 追加したテクスチャデータの差印象を取得する
	TextureData& textureData = textureDatas.back();

	// テクスチャデータをtextureDatasの末尾に追加する
	textureData.filePath = filePath;
	textureData.metadata = image.GetMetadata();
	textureData.resource = directxBase_->CreateTextureResource(textureData.metadata);

	directxBase_->UploadTextureData(textureData.resource, image);

		// テクスチャデータの要素番号をSRVのインデックスとする
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;

	textureData.srvHandleCPU = directxBase_->GetSRVCPUDescriptorHandle(srvIndex);
	textureData.srvHandleGPU = directxBase_->GetSRVGPUDescriptorHandle(srvIndex);

	// SRVの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	// SRVの設定を行う
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);

	// 設定をもとにSRVの生成
	directxBase_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);

	// MipMap(ミニマップ) : 元画像より小さなテクスチャ群

}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath) {
	// 読み込まれているテクスチャデータを検索
	auto it = std::find_if(
		textureDatas.begin(), 
		textureDatas.end(), 
		[&](TextureData& textureData) { return textureData.filePath == filePath; });
	if (it != textureDatas.end()) {
		// 読み込み済なら要素番号を返す
		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
		return textureIndex;
	}

	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex) {
	// 範囲外指定チェック
	assert(textureIndex + kSRVIndexTop < DirectXBase::kMaxSRVCount);

	TextureData& textureData = textureDatas.at(textureIndex);
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(uint32_t textureIndex) {
	// 範囲外指定違反チェック
	assert(textureIndex + kSRVIndexTop < DirectXBase::kMaxSRVCount);

	TextureData& textureData = textureDatas.at(textureIndex);
	return textureData.metadata;
}