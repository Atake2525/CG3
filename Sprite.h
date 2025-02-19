#include <d3d12.h>
#include <wrl.h>
#include "WinApp.h"
#include "kMath.h"
#include <string>

#pragma once

class SpriteBase;

class Sprite {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(SpriteBase* spriteBase, std::string textureFilePath);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// テクスチャ変更
	/// </summary>
	void ChangeTexture(std::string textureFilePath);
	
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:

	struct Transform {
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

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

private:

	// 位置
	Vector2 position = {0.0f, 0.0f};
	// 回転
	float rotation = 0.0f;
	// 拡縮
	Vector2 scale = {0.0f, 0.0f};
	// 色(RGBA)
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	// 回転の原点
	Vector2 anchorPoint = {0.0f, 0.0f};

	// テクスチャ番号
	uint32_t textureIndex = 0;

public:
	// Getter(Position)
	const Vector2& GetPosition() const { return position; }
	// Getter(Rotation)
	const float& GetRotation() const { return rotation; }
	// Getter(Scale)
	const Vector2& GetScale() const { return scale; }
	// Getter(Color)
	const Vector4& GetColor() const { return materialData->color; }
	// Getter(AnchorPoint)
	const Vector2& GetAnchorPoint() const { return anchorPoint; }

	// Setter(Position)
	void SetPosition(const Vector2& pos) { position = pos; }
	// Setter(Rotation)
	void SetRotatioin(const float& rotate) { rotation = rotate; }
	// Setter(Scale)
	void SetScale(const Vector2& size) { scale = size; }
	// Setter(Color)
	void SetColor(const Vector4& color) { materialData->color = color; }
	// Setter(AnchorPoint)
	void SetAnchorPoint(const Vector2& anchPoint) { anchorPoint = anchPoint; }

	//void SetTransform(Transform transform);
	//void SetMaterial(Material* material);
	// 
	// 初期化時などの一度に変更したい場合に
	void SetStatus(const Vector2& position, const float& rotation, const Vector2& scale, const Vector4& color);

};
