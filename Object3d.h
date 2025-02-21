#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Transform.h"

#pragma once

class Object3dBase;
class Model;

class Object3d {
public: // メンバ関数
	// 初期化
	void Initialize(Object3dBase* object3dBase);
	
	// 更新
	void Update(Transform& camera);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldPos">CameraPosition</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource, Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource);

	void SetModel(const std::string& filePath);

	void SetDirectionalLight(DirectionalLight* lightData);

	void SetPointLight(PointLight* lightData);

	void SetSpotLight(SpotLight* lightData);

private:

	Transform transform;
	Transform cameraTransform;

private:

	struct CameraForGPU {
		Vector3 worldPosition;
	};

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	//struct DirectionalLight {
	//	Vector4 color;     //!< ライトの色
	//	Vector3 direction; //!< ライトの向き
	//	float intensity;   //!< 輝度
	//};

	//struct PointLight {
	//	Vector4 color;    //!< ライトの色
	//	Vector3 position; //!< ライトの位置
	//	float intensity;  //!< 輝度
	//	float radius;     //!< ライトの届く最大距離
	//	float dacay;      //!< 減衰率
	//};

	//struct SpotLight {
	//	Vector4 color;         //!< ライトの色
	//	Vector3 position;      //!< ライトの位置
	//	float intensity;       //!< 輝度
	//	Vector3 direction;     //!< ライトの向き
	//	float distance;        //!< ライトの届く最大距離
	//	float dacay;           //!< 減衰率
	//	float cosAngle;        //!< スポットライトの余弦
	//	float cosFalloffStart; // falloffが開始される角度
	//	float padding[2];
	//};

	// 座標変換リソースのバッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	// 座標変換行列リソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrix = nullptr;

	// 平行光源リソースのバッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	// 平行光源リソース内のデータを指すポインタ
	DirectionalLight* directionalLightData = nullptr;

	// 点光源リソースのバッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource;
	// 点光源リソース内のデータを指すポインタ
	PointLight* pointLightData = nullptr;

	// スポットライトリソースのバッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource;
	// スポットライトリソース内のデータを指すポインタ
	SpotLight* spotLightData = nullptr;

	// PhongShading用カメラ
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	CameraForGPU* cameraData = nullptr;

	Object3dBase* object3dBase_;

	Model* model_ = nullptr;

private:
	Transform objectTransform;



public:

private:

	// TransformationMatrixResourceを作る
	void CreateTransformationMatrixResrouce();
	// LightResourceを作る
	void CreateLightResource();
	// DirectionalLightResourceを作る
	void CreateDirectionalLightResource();
	// PointLightResourceを作る
	void CreatePointLightResource();
	// SpotLightResourceを作る
	void CreateSpotLightResource();
	// CameraResourceを作る
	void CreateCameraResource();
};
