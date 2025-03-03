#include "Camera.h"
#include "kMath.h"
#include "WinApp.h"

Camera::Camera() 
	: transform({ {1.0f, 1.0f, 1.0f}, {0.3f, 0.0f, 0.0f  }, {0.0f, 6.0f, -19.0f} })
	, fovY(0.45f)
	, aspect(float(WinApp::kClientWidth) / float(WinApp::kClientHeight))
	, nearClipDistance(0.1f)
	, farClipDistance(100.0f)
	, worldMatrix(MakeAffineMatrix(transform.scale, transform.rotate, transform.translate))
	, viewMatrix(Inverse(worldMatrix))
	, projectionMatrix(MakePrespectiveFovMatrix(fovY, aspect, nearClipDistance, farClipDistance))
	, viewProjectionMatrix(Multiply(viewMatrix, projectionMatrix)) 
{}

void Camera::Update() {

	worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	viewMatrix = Inverse(worldMatrix);
	projectionMatrix = MakePrespectiveFovMatrix(fovY, aspect, nearClipDistance, farClipDistance);
	// ここがエラーの可能性あり
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
}