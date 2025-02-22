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
	



public:

	// Getter(Transform)
	const Transform& GetTransform() const { return transform; }
	// Getter(Translate)
	const Vector3& GetTranslate() const { return transform.translate; }
	// Getter(Scale)
	const Vector3& GetScale() const { return transform.scale; }
	// Getter(Rotate)
	const Vector3& GetRotate() const { return transform.rotate; }
	// Gettre(Color)
	const Vector4& GetColor() const;
	// Getter(EnableLighting)
	const bool& GetEnableLighting() const;

	// Setter(Transform)
	void SetTransform(const Transform& transform) { this->transform = transform; }
	// Setter(Translate)
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	// Setter(Scale)
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	// Setter(Rotate)
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	// Setter(Color)
	void SetColor(const Vector4& color);
	// Setter(EnableLighting)
	void SetEnableLighting(const bool& enableLighting);


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
