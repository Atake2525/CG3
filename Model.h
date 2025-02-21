#include <wrl.h>
#include <vector>
#include <d3d12.h>
#include <string>
#include <sstream>
#include <fstream>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#pragma once

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

struct MaterialData {
	std::string textureFilePath;
	uint32_t textureIndex = 0;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

class ModelBase;
class Model {
public:

	// 初期化
	void Initialize(ModelBase* modelBase, std::string directoryPath , std::string filename);
	
	// 更新
	void Draw();

	void SetIA();

private:

	ModelBase* modelBase_;

	// 頂点データのバッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// 頂点データのバッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	// バッファリソースの使い道を指定するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	// Objファイルのデータ
	ModelData modelData;

	// マテリアルのバッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	// マテリアルバッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;

private:
	// .mtlファイルの読み取り
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& fileName);
	// .objファイルの読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& fileName);

	// VertexResourceを作成する
	void CreateVertexResource();
	// MaterialResourceを作成する
	void CreateMaterialResouce();

	// VertexBufferViewを作成する(値を設定するだけ)
	void CreateVertexBufferView();
};
