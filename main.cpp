#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>

// DirectXBase
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>


#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <fstream>
#include <sstream>
#include "Vector4.h"
#include "Matrix4x4.h"
#include "kMath.h"

// クラス化した部分
#include "Input.h"
#include "WinApp.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"
#include "DirectXBase.h"
#include "SpriteBase.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Object3dBase.h"
#include "Object3d.h"
#include "ModelBase.h"
#include "Model.h"
#include "Transform.h"

#include "algorithm"
#include "externels/imgui/imgui.h"
#include "externels/imgui/imgui_impl_dx12.h"
#include "externels/imgui/imgui_impl_win32.h"
#include "externels/DirectXTex/DirectXTex.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Microsoft::WRL;

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

struct CameraForGPU {
	Vector3 worldPosition;
};

//struct Material {
//	Vector4 color;
//	int32_t enableLighting;
//	float pad[3];
//	Matrix4x4 uvTransform;
//	float shininess;
//	Vector3 specularColor;
//};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

//struct MaterialData
//{
//	std::string textureFilePath;
//};

//struct VertexData {
//	Vector4 position;
//	Vector2 texcoord;
//	Vector3 normal;
//};

//struct ModelData
//{
//	std::vector<VertexData> vertices;
//	MaterialData material;
//};

struct Particle {
	Transform transform;
	Vector3 velocity;
	Vector4 color;
	float lifeTime;
	float currentTime;
};

struct Emitter {
	Transform transform; //!< エミッタのTransform
	uint32_t count;      //!< 発生数
	float frequency;     //!< 発生頻度
	float frequencyTime; //!< 頻度用時刻
};

//struct DirectionalLight {
//	Vector4 color; //!< ライトの色
//	Vector3 direction; //!< ライトの向き
//	float intensity; //!< 輝度
//};
//
//struct PointLight {
//	Vector4 color; //!< ライトの色
//	Vector3 position; //!< ライトの位置
//	float intensity; //!< 輝度
//	float radius; //!< ライトの届く最大距離
//	float dacay; //!< 減衰率
//};
//
//struct SpotLight {
//	Vector4 color;//!< ライトの色
//	Vector3 position;//!< ライトの位置
//	float intensity;//!< 輝度
//	Vector3 direction;//!< ライトの向き
//	float distance; //!< ライトの届く最大距離
//	float dacay; //!< 減衰率
//	float cosAngle; //!< スポットライトの余弦
//	float cosFalloffStart; // falloffが開始される角度
//	float padding[2];
//};

//MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
//	// 1, 中で必要となる変数の宣言
//	MaterialData materialData; // 構築するMaterialData
//	std::string line; // ファイルから読んだ１行を格納するもの
//	// 2, ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
//	assert(file.is_open()); // とりあえず開けなかったら止める
//	// 3, 実際にファイルを読み、MaterialDataを構築していく
//	while (std::getline(file, line)){
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		// identifierに応じた処理
//		if (identifier == "map_Kd"){
//			std::string textureFilename;
//			s >> textureFilename;
//			// 連結してファイルパスにする
//			materialData.textureFilePath = directoryPath+"/"+textureFilename;
//		}
//	}
//	// 4, MaterialDataを返す
//	return materialData;
//}
//
//ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
//	// 1. 中で必要となる変数の宣言
//	ModelData modelData; // 構築するModelData
//	std::vector<Vector4> positions; // 位置
//	std::vector<Vector3> normals; // 法線
//	std::vector<Vector2> texcoords; // テクスチャ座標
//	std::string line; // ファイルから読んだ1行を格納するもの
//
//	// 2. ファイルを開く
//	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
//	assert(file.is_open()); // とりあえず開けなかったら止める
//	// 3. 実際にファイルを読み、ModelDataを構築していく
//	while (std::getline(file, line))
//	{
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier; // 先頭の識別子を読む
//
//		// identifierに応じた処理
//		if (identifier == "v"){
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			positions.push_back(position);
//		}
//		else if (identifier == "vt"){
//			Vector2 texcoord;
//			s >> texcoord.x >> texcoord.y;
//			texcoords.push_back(texcoord);
//		}
//		else if (identifier == "vn"){
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			normals.push_back(normal);
//		}
//		else if (identifier == "f"){
//			VertexData triangle[3];
//
//			// 面は三角形限定。その他は未対応
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex){
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3];
//				for (int32_t element = 0; element < 3; ++element){
//					std::string index;
//					std::getline(v, index, '/'); // /区切りでインデックスを読んでいく
//					elementIndices[element] = std::stoi(index);
//				}
//				// 要素へのIndexから、実際の要素を値を取得して、頂点を構築する
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texcoord = texcoords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//				//VertexData vertex = { position, texcoord, normal };
//				//modelData.vertices.push_back(vertex);
//				position.x *= -1.0f;
//				normal.x *= -1.0f;
//				texcoord.y = 1.0f - texcoord.y;
//
//				triangle[faceVertex] = {position, texcoord, normal};
//			}
//			// 頂点を逆順で登録することで、周り順を逆にする
//			modelData.vertices.push_back(triangle[2]);
//			modelData.vertices.push_back(triangle[1]);
//			modelData.vertices.push_back(triangle[0]);
//		}
//		else if (identifier == "mtllib") {
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


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	
	D3DResourceLeakChecker d3dResourceLeakChecker;
	
#pragma region 基盤システムの初期化

	WinApp* winApp = nullptr;
	winApp = new WinApp();
	winApp->Initialize();

	DirectXBase* directxBase = nullptr;
	directxBase = new DirectXBase();
	directxBase->Initialize(winApp);

	SpriteBase* spriteBase = nullptr;
	spriteBase = new SpriteBase();
	spriteBase->Initialize(directxBase);

	Object3dBase* object3dBase = nullptr;
	object3dBase = new Object3dBase();
	object3dBase->Initialize(directxBase);

	ModelBase* modelBase = nullptr;
	modelBase = new ModelBase();
	modelBase->Initialize(directxBase);

	TextureManager::GetInstance()->Initialize(directxBase);

#pragma endregion 基盤システムの初期化

	TextureManager::GetInstance()->LoadTexture("Resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("Resources/monsterBall.png");
	Sprite* sprite = nullptr;
	sprite = new Sprite();
	sprite->Initialize(spriteBase, "Resources/uvChecker.png");
	sprite->SetScale(Vector2{200.0f, 200.0f});

	uint32_t textureIndexSphere = TextureManager::GetInstance()->GetTextureIndexByFilePath("Resources/monsterBall.png");
	uint32_t textureIndexUv = TextureManager::GetInstance()->GetTextureIndexByFilePath("Resources/uvChecker.png");

	Object3d* object3d = nullptr;
	object3d = new Object3d();
	object3d->Initialize(object3dBase);

	Model* model = nullptr;
	model = new Model();
	model->Initialize(modelBase, "terrain.obj");

	object3d->SetModel(model);

	Input* input = nullptr;
	input = new Input();
	input->Initialize(winApp);

	// 出ウィンドウへの文字出力
	OutputDebugStringA("Hello,DirectX!\n");

	//// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース。テクスチャの場合また別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(VertexData) * 6;
	// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 分割数
	const uint32_t kSubdivision = 16;
	const uint32_t kVertexCount = kSubdivision * kSubdivision * 6;
	
	// 実際に頂点リソースを作る
	ComPtr<ID3D12Resource> vertexResourceSphere = directxBase->CreateBufferResource(sizeof(VertexData) * kVertexCount);
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere{};
	// リソースの先頭のアドレスから使う
	vertexBufferViewSphere.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferViewSphere.SizeInBytes = UINT(sizeof(VertexData) * kVertexCount);
	// 1頂点当たりのサイズ
	vertexBufferViewSphere.StrideInBytes = sizeof(VertexData); // 1頂点当たりのサイズ

	// 頂点リソースにデータを書き込む
	VertexData* vertexDataSphere = nullptr;
	// 書き込むためのアドレスを取得
	vertexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSphere));

	ComPtr<ID3D12Resource> directionalLightResource = directxBase->CreateBufferResource(sizeof(DirectionalLight));

	DirectionalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	directionalLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	directionalLightData->direction = {0.0f, -1.0f, 0.0f};
	directionalLightData->intensity = 1.0f;

	ComPtr<ID3D12Resource> pointLightResource = directxBase->CreateBufferResource(sizeof(PointLight));

	PointLight* pointLightData = nullptr;
	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));

	pointLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	pointLightData->position = {0.0f, 2.0f, 0.0f};
	pointLightData->intensity = 0.0f;
	pointLightData->radius = 5.0f;
	pointLightData->dacay = 5.0f;

	ComPtr<ID3D12Resource> spotLightResource = directxBase->CreateBufferResource(sizeof(SpotLight));

	SpotLight* spotLightData = nullptr;
	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));

	spotLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	spotLightData->position = {2.0f, 1.25f, 0.0f};
	spotLightData->distance = 7.0f;
	spotLightData->direction = Normalize({-1.0f, -1.0f, 0.0f});
	spotLightData->intensity = 0.0f;
	spotLightData->dacay = 2.0f;
	spotLightData->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 2.6f);

	// 経度分割1つ分の角度 φd
	const float kLonEvery = float(M_PI) * 2.0f / float(kSubdivision);
	// 緯度分割1つ分の角度 Θd
	const float kLatEvery = float(M_PI) / float(kSubdivision);
	// 緯度の方向に分割
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex; // Θ
		// 経度の方向に分割しながら線を描く
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
			float lon = lonIndex * kLonEvery; // φ
			// 頂点データを入力する。基準点a
			vertexDataSphere[start].position = {cos(lat) * cos(lon),
											  sin(lat),
											  cos(lat) * sin(lon),
											  1.0f};

				vertexDataSphere[start].texcoord = {float(lonIndex) / float(kSubdivision),
											  1.0f - float(latIndex) / float(kSubdivision)};

				vertexDataSphere[start].normal.x = vertexDataSphere[start].position.x;
			    vertexDataSphere[start].normal.y = vertexDataSphere[start].position.y;
			    vertexDataSphere[start].normal.z = vertexDataSphere[start].position.z;

				// b
			    vertexDataSphere[start + 1].position = {cos(lat + kLatEvery) * cos(lon),
												  sin(lat + kLatEvery), 
												  cos(lat + kLatEvery) * sin(lon),
												  1.0f};

				vertexDataSphere[start + 1].texcoord = {float(lonIndex) / float(kSubdivision),
												  1.0f - float(latIndex + 1) / float(kSubdivision)};

				vertexDataSphere[start + 1].normal.x = vertexDataSphere[start + 1].position.x;
			    vertexDataSphere[start + 1].normal.y = vertexDataSphere[start + 1].position.y;
			    vertexDataSphere[start + 1].normal.z = vertexDataSphere[start + 1].position.z;

				// c
			    vertexDataSphere[start + 2].position = {cos(lat) * cos(lon + kLonEvery), 
												  sin(lat), 
												  cos(lat) * sin(lon + kLonEvery),
												  1.0f};

				vertexDataSphere[start + 2].texcoord = {float(lonIndex + 1) / float(kSubdivision),
												  1.0f - float(latIndex) / float(kSubdivision)};

				vertexDataSphere[start + 2].normal.x = vertexDataSphere[start + 2].position.x;
			    vertexDataSphere[start + 2].normal.y = vertexDataSphere[start + 2].position.y;
			    vertexDataSphere[start + 2].normal.z = vertexDataSphere[start + 2].position.z;

				// c
			    vertexDataSphere[start + 3] = vertexDataSphere[start + 2];

				// b
			    vertexDataSphere[start + 4] = vertexDataSphere[start + 1];

				// d
			    vertexDataSphere[start + 5].position = {cos(lat + kLatEvery) * cos(lon + kLonEvery),
												  sin(lat + kLatEvery),
												  cos(lat + kLatEvery) * sin(lon + kLonEvery),
												  1.0f};
			    vertexDataSphere[start + 5].texcoord = {float(lonIndex + 1) / float(kSubdivision),
												  1.0f - float(latIndex + 1) / kSubdivision};

				vertexDataSphere[start + 5].normal.x = vertexDataSphere[start + 5].position.x;
			    vertexDataSphere[start + 5].normal.y = vertexDataSphere[start + 5].position.y;
			    vertexDataSphere[start + 5].normal.z = vertexDataSphere[start + 5].position.z;
		}
	}
	bool useMonsterBall = true;


	// モデル読み込み
	//ModelData modelDataTerrain = LoadObjFile("Resources", "terrain.obj");
	//DirectX::ScratchImage mipImages2 = directxBase->LoadTexture(modelDataTerrain.material.textureFilePath);
	//TextureManager::GetInstance()->LoadTexture(modelDataTerrain.material.textureFilePath);
	//uint32_t textureIndexModel = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelDataTerrain.material.textureFilePath);
	 //// 頂点リソースの作成
	 //ComPtr<ID3D12Resource> vertexResourceTerrain = directxBase->CreateBufferResource(sizeof(VertexData) * modelDataTerrain.vertices.size());
	 //// 頂点バッファビューを作成する
	 //D3D12_VERTEX_BUFFER_VIEW vertexBufferViewTerrain{};
	 //vertexBufferViewTerrain.BufferLocation = vertexResourceTerrain->GetGPUVirtualAddress();
	 //vertexBufferViewTerrain.SizeInBytes = UINT(sizeof(VertexData) * modelDataTerrain.vertices.size()); // 使用するリソースのサイズは頂点サイズ
	 //vertexBufferViewTerrain.StrideInBytes = sizeof(VertexData); // １頂点あたりのサイズ

	 // 頂点リソースにデータを書き込む
	 //VertexData* vertexDataTerrain = nullptr;
	 //vertexResourceTerrain->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataTerrain)); // 書き込むためのアドレス
	 //std::memcpy(vertexDataTerrain, modelDataTerrain.vertices.data(), sizeof(VertexData) * modelDataTerrain.vertices.size()); //頂点データをリソースにコピー

	// RootSinature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;  // 0から始まる
	descriptorRange[0].NumDescriptors = 1;    // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	// Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;  // バイナリフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; // レジスタ番号0
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	


	// RootParameter作成。複数設定できるので配列。今回は結果1つだけの長さ1の配列
	//D3D12_ROOT_PARAMETER rootParameter[1] = {};
	
	// Resource作る度に配列を増やしす
	// RootParameter作成、PixelShaderのMatrixShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[7] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;     // CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;  // PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;                     // レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;                    // レジスタ番号0を使う
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;            // Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderを使う
	rootParameters[3].Descriptor.ShaderRegister = 1; // レジスタ番号1を使う
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // ConstantBufferView
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShader
	rootParameters[4].Descriptor.ShaderRegister = 2;					// b2
	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // ConstantBufferView
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShader
	rootParameters[5].Descriptor.ShaderRegister = 3;                    // b3
	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;    // ConstantBufferView
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShader
	rootParameters[6].Descriptor.ShaderRegister = 4;                    // b4
	descriptionRootSignature.pParameters = rootParameters;              // ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);  // 配列の長さ



	// マテリアル用のリソースを作る。 今回はcolor1つ分のサイズを用意する
	ComPtr<ID3D12Resource> materialResource = directxBase->CreateBufferResource(sizeof(Material));
	// Sprite用のマテリアルリソースを作る
	//ComPtr<ID3D12Resource> materialResourceSprite = directxBase->CreateBufferResource(sizeof(Material));
	// マテリアルにデータを書き込む
	Material* materialData = nullptr;
	//Material* materialDataSprite = nullptr;
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	// データを書き込む
	//materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	// 今回は赤を書き込んでみる
	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f };

	materialData->uvTransform = MakeIdentity4x4();
	//materialDataSprite->uvTransform = MakeIdentity4x4();

	materialData->enableLighting = true;
	materialData->shininess = 70.0f;
	materialData->specularColor = {1.0f, 1.0f, 1.0f};

	//Lightingを有効にする
	/*materialDataSprite->enableLighting = false;
	materialDataSprite->shininess = 0.0f;
	materialDataSprite->specularColor = {1.0f, 1.0f, 1.0f};*/
	/*Transform uvTransformSprite{
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f},
	};*/
	

	//シリアライズしてバイナリにする
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリをもとに作成
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	hr = directxBase->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
	// InputLayout 
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);
	// BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	// すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// NomalBlendを行うための設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	// RasiterzerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	// Shaderをコンパイルする
	ComPtr<IDxcBlob> vertexShaderBlob = directxBase->CompileShader(L"Resources/shaders/Object3D.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	ComPtr<IDxcBlob> pixelShaderBlob = directxBase->CompileShader(L"Resources/shaders/Object3D.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
	// PSOを作成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get(); // RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;  // InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer() , vertexShaderBlob->GetBufferSize() }; // VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };    // PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc; // BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc; // RasterizerState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ、三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 実際に生成
	ComPtr<ID3D12PipelineState> graphicsPilelineState = nullptr;
	hr = directxBase->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPilelineState));
	assert(SUCCEEDED(hr));
	

	///TransformationMatrix用のResourceを作る
	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	ComPtr<ID3D12Resource> wvpResource = directxBase->CreateBufferResource(sizeof(TransformationMatrix));
	// データを書き込む
	TransformationMatrix* wvpData = nullptr;
	// 書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	// 単位行列を書き込んでおく
	wvpData->WVP = MakeIdentity4x4();


	// Transform変数を作る
	Transform transform{
	    {1.0f, 1.0f,   1.0f},
        {0.0f, -1.58f, 0.0f},
        {0.0f, 0.0f,   0.0f}
    };

	Transform cameraTransform{ {1.0f, 1.0f, 1.0f}, { 0.36f, 0.0f, 0.0f}, {0.0f, 6.0f, -19.0f} };

	//Matrix4x4 projectionMatrix = MakePrespectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.1f, 100.0f);

	//ComPtr<ID3D12Resource> indexResourceSprite = directxBase->CreateBufferResource(sizeof(uint32_t) * 6);

	//D3D12_INDEX_BUFFER_VIEW indexbufferViewSprite{};
	//// リソースの先頭のアドレスから使う
	//indexbufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	//// 使用するリソースのサイズはインデックス6つ分のサイズ
	//indexbufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//// インデックスはuint32_tとする
	//indexbufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	// インデックスリソースにデータを書き込む
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	/*indexDataSprite[0] = 0;  indexDataSprite[1] = 1;  indexDataSprite[2] = 3;
	indexDataSprite[3] = 3;  indexDataSprite[4] = 1;  indexDataSprite[5] = 2;*/
		
	// カメラ用のリソースを作成 Phong Reflection Model
	ComPtr<ID3D12Resource> cameraResource = directxBase->CreateBufferResource(sizeof(CameraForGPU));
	// マテリアルにデータを書き込む
	CameraForGPU* cameraData = nullptr;
	// 書き込むためのアドレスを取得
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	cameraData->worldPosition = cameraTransform.translate;

	Vector2 position = sprite->GetPosition();
	float rotation = sprite->GetRotation();
	Vector2 scale = sprite->GetScale();
	Vector4 color = sprite->GetColor();
	Vector2 anchorPoint = sprite->GetAnchorPoint();
	bool isFlipX = sprite->GetIsFlipX();
	bool isFlipY = sprite->GetIsFlipY();
	Vector2 textureLeftTop = sprite->GetTextureLeftTop();
	Vector2 textureSize = sprite->GetTextureSize();

	//ゲームループ
	/*MSG msg{};*/
	//ウィンドウの×ボタンが押されるまでループ
	while (true){

		if (winApp->ProcessMessage()) {
			// ループを抜ける
			break;
		}
		else
		{
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// Imguiの設定(Color&STR)
			ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.7f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.0f, 0.7f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(300, 400));
			ImGui::Begin("colorConfig");

			/*if (ImGui::TreeNode("ColorCode")){
				ImGui::DragFloat3("RGB", &materialData->color.x, 0.01f);
				ImGui::ColorEdit3("RGB", &materialData->color.x);
				ImGui::ColorEdit3("RGBSprite", &color.x);
				ImGui::SliderFloat("R", &materialData->color.x, 0.0f, 1.0f);
				ImGui::SliderFloat("G", &materialData->color.y, 0.0f, 1.0f);
				ImGui::SliderFloat("B", &materialData->color.z, 0.0f, 1.0f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("colorPreset")){
				if (ImGui::Button("RED")) {
					materialData->color.x = 1.0f;
					materialData->color.y = 0.0f;
					materialData->color.z = 0.0f;
				}
				if (ImGui::Button("GREEN")) {
					materialData->color.x = 0.0f;
					materialData->color.y = 1.0f;
					materialData->color.z = 0.0f;
				}
				if (ImGui::Button("BLUE")) {
					materialData->color.x = 0.0f;
					materialData->color.y = 0.0f;
					materialData->color.z = 1.0f;
				}
				if (ImGui::Button("YELLOW")) {
					materialData->color.x = 1.0f;
					materialData->color.y = 1.0f;
					materialData->color.z = 0.0f;
				}
				if (ImGui::Button("WHITE")) {
					materialData->color.x = 1.0f;
					materialData->color.y = 1.0f;
					materialData->color.z = 1.0f;
				}
				if (ImGui::Button("BLACK")) {
					materialData->color.x = 0.0f;
					materialData->color.y = 0.0f;
					materialData->color.z = 0.0f;
				}
				ImGui::TreePop();
			}*/
			if (ImGui::TreeNode("STR"))
			{
				ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f);
				ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f);
				ImGui::DragFloat3("Translate", &transform.translate.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SpriteSTR"))
			{
				ImGui::DragFloat2("Scale", &scale.x, 1.0f);
				ImGui::DragFloat("Rotate", &rotation, 0.01f);
				ImGui::DragFloat2("Translate", &position.x, 1.0f);
				ImGui::DragFloat2("AnchorPoint", &anchorPoint.x, 0.1f);
				ImGui::Checkbox("FlipX", &isFlipX);
				ImGui::Checkbox("FlipY", &isFlipY);
				ImGui::DragFloat2("TextureLeftTop", &textureLeftTop.x, 0.1f);
				ImGui::DragFloat2("TextureSize", &textureSize.x, 0.1f);
				//ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
				//ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
				//ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Lighting")) {
				//ImGui::ColorEdit3("SpecularColor", &materialData->specularColor.x);
				if (ImGui::TreeNode("DirectionalLight")) {
					ImGui::ColorEdit4("Color", &directionalLightData->color.x);
					ImGui::SliderFloat3("Direction", &directionalLightData->direction.x, -5.0f, 5.0f);
					ImGui::DragFloat("Insensity", &directionalLightData->intensity, 1.0f);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("PointLight")) {
					ImGui::ColorEdit4("Color", &pointLightData->color.x);
					ImGui::DragFloat3("Positoin", &pointLightData->position.x, 0.1f);
					ImGui::SliderFloat("radius", &pointLightData->radius, 0.0f, 10.0f);
					ImGui::SliderFloat("dacay", &pointLightData->dacay, 0.0f, 10.0f);
					ImGui::DragFloat("Insensity", &pointLightData->intensity, 1.0f);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("SpotLight")) {
					ImGui::ColorEdit4("Color", &spotLightData->color.x);
					ImGui::DragFloat3("Positoin", &spotLightData->position.x, 0.1f);
					ImGui::DragFloat3("Direction", &spotLightData->direction.x, 0.1f);
					ImGui::DragFloat("cosAngle", &spotLightData->cosAngle, 0.01f);
					ImGui::DragFloat("cosFalloffStart", &spotLightData->cosFalloffStart, 0.01f);
					ImGui::SliderFloat("distance", &spotLightData->distance, 0.0f, 10.0f);
					ImGui::SliderFloat("dacay", &spotLightData->dacay, 0.0f, 10.0f);
					ImGui::DragFloat("Insensity", &spotLightData->intensity, 1.0f);
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Texture")) {
				ImGui::Checkbox("useMonsterBall", &useMonsterBall);
				ImGui::TreePop();
			}
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(1080, 0));
			ImGui::SetNextWindowSize(ImVec2(200, 300));
			ImGui::Begin("Camera");

			ImGui::DragFloat3("Rotate", &cameraTransform.rotate.x, 0.01f);
			ImGui::DragFloat3("Translate", &cameraTransform.translate.x, 0.01f);

			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			if (input->PushKey(DIK_0)) {
				spotLightData->distance += 0.1f;
			}
			


			// ImGuiの内部コマンドを生成する
			ImGui::Render();

			sprite->SetStatus(position, rotation, scale, color);
			sprite->SetAnchorPoint(anchorPoint);
			sprite->SetIsFlip(isFlipX, isFlipY);
			sprite->SetTextureLeftTop(textureLeftTop);
			sprite->SetTextureSize(textureSize);
			sprite->Update();

			//object3d->SetDirectionalLight(directionalLightData);
			object3d->Update(cameraTransform);

			directxBase->PreDraw();

			//Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
			//uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransformSprite.rotate.z));
			//uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
			//materialDataSprite->uvTransform = uvTransformMatrix;

			////ゲームの処理
			//// Sprite用のWorldViewProjectionMatrixを作る
			//// SpriteのTransform処理
			//Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			//Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			//Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
			//Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
			//tranformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;

			
			//// 3DのTransform処理
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePrespectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
			Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
			wvpData->WVP = worldViewProjectionMatrix;
			wvpData->World = worldMatrix;



			
			// RootSignatureを設定。PSOに設定しているけど別途設定が必要
			//directxBase->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
			// PSOを設定
			//directxBase->GetCommandList()->SetPipelineState(graphicsPilelineState.Get()); 
			// 形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
			//directxBase->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			spriteBase->ShaderDraw();

			directxBase->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSphere); // VBVを設定
			// マテリアルCBufferの場所を設定
			directxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
			directxBase->GetCommandList()->SetGraphicsRootDescriptorTable(
			    2, useMonsterBall ? TextureManager::GetInstance()->GetSrvHandleGPU(textureIndexSphere) : TextureManager::GetInstance()->GetSrvHandleGPU(textureIndexUv));

			directxBase->GetCommandList()->SetGraphicsRootConstantBufferView(4, directionalLightResource->GetGPUVirtualAddress());

			directxBase->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource->GetGPUVirtualAddress());

			directxBase->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());

			// wvp用のCBufferの場所を設定
			directxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			// 描画！(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後(球)
			directxBase->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource->GetGPUVirtualAddress());
			directxBase->GetCommandList()->DrawInstanced(kVertexCount, 1, 0, 0);

			// 描画！(DrawCall/ドローコール) 6個のインデックスを使用し一つのインスタンスを描画。その他は当面0で良い

			//// ModelTerrain
			//directxBase->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewTerrain); // VBVを設定
			//directxBase->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndexModel));
			//directxBase->GetCommandList()->DrawInstanced(UINT(modelDataTerrain.vertices.size()), 1, 0, 0);

			////Spriteの描画。変更が必要なものだけ変更する
			//directxBase->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite); // VBVを設定

			//directxBase->GetCommandList()->IASetIndexBuffer(&indexbufferViewSprite); // IBVを設定

			//// マテリアルCBufferの場所を設定
			//directxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
			//// TransformationMatrixCBbufferの場所を設定
			//directxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			//directxBase->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			//// 描画
			//directxBase->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
			sprite->Draw();

			object3dBase->ShaderDraw();

			object3d->Draw();

			 
			// 実際のcommandListのImGuiの描画コマンドを積む
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), directxBase->GetCommandList().Get());
			
			input->Update();
			
			directxBase->PostDraw();
		}
	}
	
#ifdef DEBUG
	debugController->Release();
#endif // DEBUG


	winApp->Finalize();
	delete winApp;

	directxBase->Finalize();
	delete directxBase;

	delete spriteBase;

	delete object3dBase;

	delete modelBase;

	TextureManager::GetInstance()->Finalize();

	delete sprite;

	delete object3d;

	delete model;

	delete input;

	return 0;
}