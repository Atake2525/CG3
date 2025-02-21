#include "Object3d.h"
#include "Object3dBase.h"
#include "DirectXBase.h"
#include "TextureManager.h"
#include "kMath.h"
#include "WinApp.h"
#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>

using namespace Microsoft::WRL;

void Object3d::Initialize(Object3dBase* object3dBase) { 
	object3dBase_ = object3dBase;
	//// モデル読み込み
	//modelData = LoadObjFile("Resources", filename);

	//// Resourceの作成
	//CreateVertexResource();
	//CreateMaterialResouce();
	CreateTransformationMatrixResrouce();
	CreateLightResource();
	CreateCameraResource();

	//// BufferResourceの作成
	//CreateVertexBufferView();	

	//// VertexResourceにデータを書き込むためのアドレスを取得してvertexDataに割り当てる
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); 
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size()); // 頂点データをリソースにコピー
	////  書き込むためのアドレスを取得
	//materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// 書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrix));
	// 平行光源リソースに書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	// 点光源リソースに書き込むためのアドレスを取得
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));
	// スポットライトリソースに書き込むためのアドレスを取得
	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));
	// 書き込むためのアドレスを取得
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	// 単位行列を書き込んでおく
	transformationMatrix->WVP = MakeIdentity4x4();
	transformationMatrix->World = MakeIdentity4x4();

	// 平行光源にデータを書き込む
	directionalLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	directionalLightData->direction = {0.0f, -1.0f, 0.0f};
	directionalLightData->intensity = 1.0f;

	// 点光源にデータを書き込む
	pointLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	pointLightData->position = {0.0f, 2.0f, 0.0f};
	pointLightData->intensity = 0.0f;
	pointLightData->radius = 5.0f;
	pointLightData->dacay = 5.0f;

	// スポットライトにデータを書き込む
	spotLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	spotLightData->position = {0.0f, 0.0f, 0.0f};
	spotLightData->distance = 10.0f;
	spotLightData->direction = Normalize({-1.0f, 0.0f, 0.0f});
	spotLightData->intensity = 0.0f;
	spotLightData->dacay = 2.0f;
	spotLightData->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 2.6f);

	// cosFalloffStartがcosAngleより下にならないように調整
	spotLightData->cosFalloffStart = max(spotLightData->cosFalloffStart, spotLightData->cosAngle);

	transform = {
	    {1.0f, -1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };
	cameraTransform = {
	    {1.0f, 1.0f, 1.0f  },
        {0.3f, 0.0f, 0.0f  },
        {0.0f, 6.0f, -19.0f}
    };

	cameraData->worldPosition = cameraTransform.translate;
}

void Object3d::Update(Transform& camera) {

	cameraTransform = camera;

	//directionalLightData->intensity -= 0.01f;

	// 3DのTransform処理
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePrespectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrix->WVP = worldViewProjectionMatrix;
	transformationMatrix->World = worldMatrix;
}

void Object3d::Draw() {
	//// ModelTerrain
	//object3dBase_->GetDxBase()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定

	//// wvp用のCBufferの場所を設定
	//object3dBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	//object3dBase_->GetDxBase()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureIndex));

	if (model_) {
		model_->SetIA();
	}

	object3dBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(4, directionalLightResource->GetGPUVirtualAddress());

	object3dBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());

	object3dBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());

	// wvp用のCBufferの場所を設定
	object3dBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	object3dBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource->GetGPUVirtualAddress());

	//object3dBase_->GetDxBase()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
	// 3Dモデルが割り当てられていれば描画する
	if (model_) {
		model_->Draw();
	}
}

//MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
//	// 1, 中で必要となる変数の宣言
//	MaterialData materialData; // 構築するMaterialData
//	std::string line;          // ファイルから読んだ１行を格納するもの
//	// 2, ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
//	assert(file.is_open());                             // とりあえず開けなかったら止める
//	// 3, 実際にファイルを読み、MaterialDataを構築していく
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		// identifierに応じた処理
//		if (identifier == "map_Kd") {
//			std::string textureFilename;
//			s >> textureFilename;
//			// 連結してファイルパスにする
//			materialData.textureFilePath = directoryPath + "/" + textureFilename;
//		}
//	}
//	// 4, MaterialDataを返す
//	return materialData;
//}
//
//ModelData Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
//	// 1. 中で必要となる変数の宣言
//	ModelData modelData;            // 構築するModelData
//	std::vector<Vector4> positions; // 位置
//	std::vector<Vector3> normals;   // 法線
//	std::vector<Vector2> texcoords; // テクスチャ座標
//	std::string line;               // ファイルから読んだ1行を格納するもの
//
//	// 2. ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
//	assert(file.is_open());                             // とりあえず開けなかったら止める
//	// 3. 実際にファイルを読み、ModelDataを構築していく
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier; // 先頭の識別子を読む
//
//		// identifierに応じた処理
//		if (identifier == "v") {
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			positions.push_back(position);
//		} else if (identifier == "vt") {
//			Vector2 texcoord;
//			s >> texcoord.x >> texcoord.y;
//			texcoords.push_back(texcoord);
//		} else if (identifier == "vn") {
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			normals.push_back(normal);
//		} else if (identifier == "f") {
//			VertexData triangle[3];
//
//			// 面は三角形限定。その他は未対応
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3];
//				for (int32_t element = 0; element < 3; ++element) {
//					std::string index;
//					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
//					elementIndices[element] = std::stoi(index);
//				}
//				// 要素へのIndexから、実際の要素を値を取得して、頂点を構築する
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texcoord = texcoords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//				// VertexData vertex = { position, texcoord, normal };
//				// modelData.vertices.push_back(vertex);
//				position.x *= -1.0f;
//				position.y *= -1.0f;
//				normal.x *= -1.0f;
//				texcoord.y = 1.0f - texcoord.y;
//
//				triangle[faceVertex] = {position, texcoord, normal};
//			}
//			// 頂点を逆順で登録することで、周り順を逆にする
//			modelData.vertices.push_back(triangle[2]);
//			modelData.vertices.push_back(triangle[1]);
//			modelData.vertices.push_back(triangle[0]);
//		} else if (identifier == "mtllib") {
//			// materialTemplateLibraryファイルの名前を取得する
//			std::string materialFilename;
//			s >> materialFilename;
//			// 基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
//			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
//		}
//	}
//	// 4. ModelDataを返す
//	return modelData;
//}
//
//void Object3d::CreateVertexResource() {
//	// 頂点リソースの作成
//	vertexResource = object3dBase_->GetDxBase()->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
//}
//
//void Object3d::CreateVertexBufferView() {
//	// 頂点バッファビューを作成する
//	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
//	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size()); // 使用するリソースのサイズは頂点サイズ
//	vertexBufferView.StrideInBytes = sizeof(VertexData);                                        // １頂点あたりのサイズ
//}
//
//void Object3d::CreateMaterialResouce() { 
//	materialResource = object3dBase_->GetDxBase()->CreateBufferResource(sizeof(Material)); 
//}

void Object3d::CreateTransformationMatrixResrouce() {
	transformationMatrixResource = object3dBase_->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix));
}

void Object3d::CreateLightResource() {
	CreateDirectionalLightResource();
	CreatePointLightResource();
	CreateSpotLightResource();
}

void Object3d::CreateDirectionalLightResource() { 
	directionalLightResource = object3dBase_->GetDxBase()->CreateBufferResource(sizeof(DirectionalLight)); }

void Object3d::CreatePointLightResource() { 
	pointLightResource = object3dBase_->GetDxBase()->CreateBufferResource(sizeof(PointLight)); 
}

void Object3d::CreateSpotLightResource() { 
	spotLightResource = object3dBase_->GetDxBase()->CreateBufferResource(sizeof(SpotLight));
}

void Object3d::CreateCameraResource() { 
	cameraResource = object3dBase_->GetDxBase()->CreateBufferResource(sizeof(CameraForGPU));
}

void Object3d::SetDirectionalLight(DirectionalLight* lightData) {
	directionalLightData = lightData;
}