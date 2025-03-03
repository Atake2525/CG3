#include "Object3d.h"
#include "Object3dBase.h"
#include "DirectXBase.h"
#include "TextureManager.h"
#include "kMath.h"
#include "WinApp.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include <fstream>
#include <sstream>
#include <cassert>

using namespace Microsoft::WRL;

void Object3d::Initialize() { 

	//// Resourceの作成
	CreateTransformationMatrixResrouce();
	CreateLightResource();
	CreateCameraResource();

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
	directionalLightData->intensity = 0.0f;

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
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };

	cameraData->worldPosition = {1.0f, 1.0f, 1.0f};

	camera = Object3dBase::GetInstance()->GetDefaultCamera();
}

void Object3d::Update() {

	//cameraTransform = camerad;

	// 3DのTransform処理
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 worldViewProjectionMatrix;
	if (camera) {
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	} else {
		worldViewProjectionMatrix = worldMatrix;
	}
	//Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	//Matrix4x4 projectionMatrix = MakePrespectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	//Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrix->WVP = worldViewProjectionMatrix;
	transformationMatrix->World = worldMatrix;
}

void Object3d::Draw(Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResourced, Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResourced) {
	
	if (model_) {
		model_->SetIA();
	}

	Object3dBase::GetInstance()->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(4, directionalLightResourced->GetGPUVirtualAddress());

	Object3dBase::GetInstance()->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResourced->GetGPUVirtualAddress());

	Object3dBase::GetInstance()->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource->GetGPUVirtualAddress());

	// wvp用のCBufferの場所を設定
	Object3dBase::GetInstance()->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	Object3dBase::GetInstance()->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource->GetGPUVirtualAddress());

	// 3Dモデルが割り当てられていれば描画する
	if (model_) {
		model_->Draw();
	}
}

void Object3d::CreateTransformationMatrixResrouce() { 
	transformationMatrixResource = Object3dBase::GetInstance()->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix)); 
}

void Object3d::CreateLightResource() {
	CreateDirectionalLightResource();
	CreatePointLightResource();
	CreateSpotLightResource();
}

void Object3d::CreateDirectionalLightResource() { 
	directionalLightResource = Object3dBase::GetInstance()->GetDxBase()->CreateBufferResource(sizeof(DirectionalLight)); 
}

void Object3d::CreatePointLightResource() { 
	pointLightResource = Object3dBase::GetInstance()->GetDxBase()->CreateBufferResource(sizeof(PointLight));
}

void Object3d::CreateSpotLightResource() { 
	spotLightResource = Object3dBase::GetInstance()->GetDxBase()->CreateBufferResource(sizeof(SpotLight)); 
}

void Object3d::CreateCameraResource() { 
	cameraResource = Object3dBase::GetInstance()->GetDxBase()->CreateBufferResource(sizeof(CameraForGPU));
}

void Object3d::SetDirectionalLight(DirectionalLight* lightData) {
	directionalLightData = lightData;
}

void Object3d::SetPointLight(PointLight* lightData) { 
	pointLightData = lightData;
}

void Object3d::SetSpotLight(SpotLight* lightData) { 
	spotLightData = lightData; 
}

void Object3d::SetModel(const std::string& filePath) {
	// モデルを検索してセットする
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}

void Object3d::SetColor(const Vector4& color) { 
	model_->SetColor(color); 
}

const Vector4& Object3d::GetColor() const { 
	return model_->GetColor(); 
}

const bool& Object3d::GetEnableLighting() const { 
	return model_->GetEnableLighting();
}

void Object3d::SetEnableLighting(const bool& enableLighting) { 
	model_->SetEnableLighting(enableLighting);
}

const Vector3& Object3d::GetRotateInDegree() const { 
	return SwapDegree(transform.rotate);
}

void Object3d::SetRotateInDegree(const Vector3& rotate) { 
	transform.rotate = SwapRadian(rotate);
}

void Object3d::SetTransform(const Vector3& translate, const Vector3& scale, const Vector3& rotate) {
	transform.translate = translate;
	transform.scale = scale;
	transform.rotate = rotate;
}

const Vector3& Object3d::GetSpecularColor() const { 
	return model_->GetSpecularColor();
}

const float& Object3d::GetShininess() const { 
	return model_->GetShininess();
}

void Object3d::SetSpecularColor(const Vector3& specularColor) {
	model_->SetSpecularColor(specularColor); 
}

void Object3d::SetShininess(const float& shininess) { 
	model_->SetShininess(shininess);
}