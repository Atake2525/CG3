#include <d3d12.h>
#include "externels/DirectXTex/DirectXTex.h"
#include <string>
#include <wrl.h>
#include <vector>

class DirectXBase;

#pragma once
class TextureManager {
private:
	// シングルトンパターンを適用
	static TextureManager* instance;

	// コンストラクタ、デストラクタの隠蔽
	TextureManager() = default;
	~TextureManager() = default;
	// コピーコンストラクタ、コピー代入演算子の封印
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>TextureManager* instance</returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXBase* directxBase);

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath">テクスチャファイルのパス</param>
	void LoadTexture(const std::string& filePath);

	// SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	// SRVインデックスからファイルパスを取得
	//std::string GetfilePathByTextureIndex(uint32_t textureIndex);

	// テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

	// テクスチャのメタデータを取得
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

private:
	// テクスチャ1枚分のデータ
	struct TextureData {
		std::string filePath; // 画像のファイルパス
		DirectX::TexMetadata metadata; // 画像の幅や高さなどの情報
		Microsoft::WRL::ComPtr<ID3D12Resource> resource; // テクスチャリソース
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU; // SRV作成時に必要なCPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU; // 描画コマンドに必要なGPUハンドル
	};
	// テクスチャデータ
	std::vector<TextureData> textureDatas;

	// 最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount;

	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

	DirectXBase* directxBase_ = nullptr;

};
