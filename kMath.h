#pragma once
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <cmath>
#include <cassert>
#define _USE_MATH_DEFINES
#include <math.h>

/*struct Sphere
{
	Vector3 center; //!< 中心点
	float radius;   //!< 半径
};*/

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

//単位行列の作成
Matrix4x4 MakeIdentity4x4();

// 1, x軸回転行列
Matrix3x3 MakeRotateMatrix3x3(float radian);

//  行列の積
Matrix3x3 Multiply3x3(const Matrix3x3& m1, const Matrix3x3& m2);

//1次元アフィン変換行列
Matrix3x3 MakeAffineMatrix3x3(const Vector2& scale, const Vector2& rotate, const Vector2& translate);

// 1, x軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian);

// 2, y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian);

// 3, z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian);

//座標変換
Vector3 MatrixTransform(const Vector3& vector, const Matrix4x4& matrix);

// Scale計算
Matrix4x4 MakeScaleMatrix(Vector3 scale);

// Translate計算
Matrix4x4 MakeTranslateMatrix(Vector3 translate);

//  行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

//３次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);


// 1, 透視投影行列
Matrix4x4 MakePrespectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// 2, 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);

// 3, ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// 球体描画
//void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);