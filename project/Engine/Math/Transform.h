#include "Vector3.h"
#include "Vector4.h"

#pragma once
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct DirectionalLight {
	Vector4 color;     //!< ライトの色
	Vector3 direction; //!< ライトの向き
	float intensity;   //!< 輝度
	float padding[2];
};

struct PointLight {
	Vector4 color;    //!< ライトの色
	Vector3 position; //!< ライトの位置
	float intensity;  //!< 輝度
	float radius;     //!< ライトの届く最大距離
	float dacay;      //!< 減衰率
};

struct SpotLight {
	Vector4 color;         //!< ライトの色
	Vector3 position;      //!< ライトの位置
	float intensity;       //!< 輝度
	Vector3 direction;     //!< ライトの向き
	float distance;        //!< ライトの届く最大距離
	float dacay;           //!< 減衰率
	float cosAngle;        //!< スポットライトの余弦
	float cosFalloffStart; // falloffが開始される角度
	float padding[2];
};

//class Transform {};
