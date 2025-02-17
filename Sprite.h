#include <d3d12.h>
#include <wrl.h>
#include "WinApp.h"
#include "kMath.h"

#pragma once

class SpriteBase;

class Sprite {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteBase* spriteBase);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);

private:
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float pad[3];
		Matrix4x4 uvTransform;
		float shininess;
		Vector3 specularColor;
	};

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	// VertexResourceを作成する
	void CreateVertexResource();
	// IndexResourceを作成する
	void CreateIndexResource();
	// MaterialResouceを作成する
	void CreateMaterialResource();
	// TransformationMatrixを作成する
	void CreateTransformationMatrixResource();

	// VertexBufferViewを作成する (値を設定するだけ)
	void CreateVertexBufferView();
	// IndexBufferViewを作成する (値を設定するだけ)
	void CreateIndexBufferView();
	// Materialの値を設定
	void SetMaterial();
	// TransformationMatrixの値を設定
	void SetTransformatinMatrix();


private:
	SpriteBase* spriteBase_ = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	// バッファリソース内のデータを指すポインタ
	uint32_t* indexData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	// バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexbufferView;

};
