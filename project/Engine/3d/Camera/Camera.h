#include "Transform.h"
#include "Matrix4x4.h"

struct CameraForGPU {
	Vector3 worldPosition;
};

#pragma once
class Camera {
public:
	Camera();

	void Update();

	// Getter
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix; }
	// Getter
	const Matrix4x4& GetViewMatrix() const { return viewMatrix; }
	// Getter
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix; }
	// Getter
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix; }
	// Getter(Rotate)
	const Vector3& GetRotate() const { return transform.rotate; }
	// Getter(Translate)
	const Vector3& GetTranslate() const { return transform.translate; }

	// Setter(Rotate)
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	// Setter(Translate)
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }

private:
	Transform transform;
	Matrix4x4 worldMatrix;
	Matrix4x4 viewMatrix;

	Matrix4x4 projectionMatrix;
	float fovY;
	float aspect;
	float nearClipDistance;
	float farClipDistance;

	Matrix4x4 viewProjectionMatrix;
};
