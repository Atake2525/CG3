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

class Model;
class Camera;

class Object3d {
public: // メンバ関数
	// 初期化
	void Initialize();
	
	// 更新
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldPos">CameraPosition</param>
	void Draw(Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource, Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource);

	void SetModel(const std::string& filePath);

	void SetDirectionalLight(DirectionalLight* lightData);

	void SetPointLight(PointLight* lightData);

	void SetSpotLight(SpotLight* lightData);

	void SetCamera(Camera* camera) { this->camera = camera; }

private:

	Transform transform;

	Camera* camera = nullptr;

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

	Model* model_ = nullptr;

public:

	// Getter(Transform)
	const Transform& GetTransform() const { return transform; }
	// Getter(Translate)
	const Vector3& GetTranslate() const { return transform.translate; }
	// Getter(Scale)
	const Vector3& GetScale() const { return transform.scale; }
	// Getter(Rotate)
	const Vector3& GetRotate() const { return transform.rotate; }
	// Getter(Rotate Degree)
	const Vector3& GetRotateInDegree() const;
	// Gettre(Color)
	const Vector4& GetColor() const;
	// Getter(EnableLighting)
	const bool& GetEnableLighting() const;
	// Getter(specularColor)
	const Vector3& GetSpecularColor() const;
	// Getter(shininess)
	const float& GetShininess() const;

	// Setter(Transform)
	void SetTransform(const Transform& transform) { this->transform = transform; }
	// Setter(Transform, pos,scale,rotate)
	void SetTransform(const Vector3& translate, const Vector3& scale, const Vector3& rotate);
	// Setter(Translate)
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	// Setter(Scale)
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	// Setter(Rotate)
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	// Setter(Rotate Degree)
	void SetRotateInDegree(const Vector3& rotate);
	// Setter(Color)
	void SetColor(const Vector4& color);
	// Setter(EnableLighting)
	void SetEnableLighting(const bool& enableLighting);
	// Setter(specularColor)
	void SetSpecularColor(const Vector3& specularColor);
	// Setter(shininess)
	void SetShininess(const float& shininess);


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
