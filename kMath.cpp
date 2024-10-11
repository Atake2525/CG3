#include "kMath.h"

//単位行列の作成
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 ans = { 0 };
	for (int a = 0; a < 4; a++) {
		for (int b = 0; b < 4; b++) {
			if (a == b)
			{
				ans.m[a][b] = 1;
			}
		}
	}
	return ans;
};

// 1, 2次元の軸回転行列
Matrix3x3 MakeRotateMatrix3x3(float radian) {
	Matrix3x3 ans = { 0 };
	ans.m[0][0] = std::cos(radian);
	ans.m[0][1] = std::sin(radian);
	ans.m[1][0] = -std::sin(radian);
	ans.m[1][1] = -std::cos(radian);
	ans.m[2][2] = 1;
	return ans;
};


//  行列の積
Matrix3x3 Multiply3x3(const Matrix3x3& m1, const Matrix3x3& m2) {
	Matrix3x3 ans;
	for (int a = 0; a < 2; a++) {
		for (int b = 0; b < 2; b++) {
			ans.m[a][b] = m1.m[a][0] * m2.m[0][b] + m1.m[a][1] * m2.m[1][b] + m1.m[a][2] * m2.m[2][b];
		}
	}
	return ans;
};

//2次元アフィン変換行列
Matrix3x3 MakeAffineMatrix3x3(const Vector2& scale, const Vector2& rotate, const Vector2& translate) {
	Matrix3x3 S = { 0 };
	Matrix3x3 R = { 0 };
	Matrix3x3 T = { 0 };
	Matrix3x3 ans = { 0 };

	S.m[0][0] = scale.x;
	S.m[1][1] = scale.y;
	S.m[2][2] = 1;

	R.m[0][0] = std::cos(rotate.x);
	R.m[1][0] = -std::sin(rotate.y);
	R.m[0][1] = std::sin(rotate.x);
	R.m[1][1] = std::cos(rotate.y);

	T.m[0][0] = 1;
	T.m[1][1] = 1;
	T.m[2][2] = 1;
	T.m[2][0] = translate.x;
	T.m[2][1] = translate.y;

	ans.m[0][0] = S.m[0][0] * R.m[0][0];
	ans.m[0][1] = S.m[0][1] * R.m[0][1];
	ans.m[1][0] = S.m[1][0] * R.m[1][0];
	ans.m[1][1] = S.m[1][1] * R.m[1][1];
	ans.m[2][0] = T.m[2][0];
	ans.m[2][1] = T.m[2][1];
	ans.m[2][2] = 1;

	return ans;
};

// 1, x軸回転行列
Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = 1;
	ans.m[1][1] = std::cos(radian);
	ans.m[1][2] = std::sin(radian);
	ans.m[2][1] = -std::sin(radian);
	ans.m[2][2] = std::cos(radian);
	ans.m[3][3] = 1;

	return ans;
};

// 2, y軸回転行列
Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = std::cos(radian);
	ans.m[0][2] = -std::sin(radian);
	ans.m[1][1] = 1;
	ans.m[2][0] = std::sin(radian);
	ans.m[2][2] = std::cos(radian);
	ans.m[3][3] = 1;

	return ans;
};

// 3, z軸回転行列
Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = std::cos(radian);
	ans.m[0][1] = std::sin(radian);
	ans.m[1][0] = -std::sin(radian);
	ans.m[1][1] = std::cos(radian);
	ans.m[2][2] = 1;
	ans.m[3][3] = 1;

	return ans;
};

//座標変換
Vector3 MatrixTransform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 ans;

	ans.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	ans.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	ans.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	ans.x /= w;
	ans.y /= w;
	ans.z /= w;

	return ans;
}

// Scale計算
Matrix4x4 MakeScaleMatrix(Vector3 scale) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = scale.x;
	ans.m[1][1] = scale.y;
	ans.m[2][2] = scale.z;
	ans.m[3][3] = 1;

	return ans;
};

// Translate計算
Matrix4x4 MakeTranslateMatrix(Vector3 translate) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = 1;
	ans.m[1][1] = 1;
	ans.m[2][2] = 1;
	ans.m[3][3] = 1;
	ans.m[3][0] = translate.x;
	ans.m[3][1] = translate.y;
	ans.m[3][2] = translate.z;

	return ans;
};

//  行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 ans;
	for (int a = 0; a < 4; a++) {
		for (int b = 0; b < 4; b++) {
			ans.m[a][b] = m1.m[a][0] * m2.m[0][b] + m1.m[a][1] * m2.m[1][b] + m1.m[a][2] * m2.m[2][b] + m1.m[a][3] * m2.m[3][b];
		}
	}
	return ans;
};

//３次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 S = { 0 };
	Matrix4x4 R = { 0 };
	Matrix4x4 rX = { 0 };
	Matrix4x4 rY = { 0 };
	Matrix4x4 rZ = { 0 };
	Matrix4x4 T = { 0 };
	Matrix4x4 ans = { 0 };

	S.m[0][0] = scale.x;
	S.m[1][1] = scale.y;
	S.m[2][2] = scale.z;
	S.m[3][3] = 1;

	rX = MakeRotateXMatrix(rotate.x);
	rY = MakeRotateYMatrix(rotate.y);
	rZ = MakeRotateZMatrix(rotate.z);

	R = Multiply(rX, Multiply(rY, rZ));

	T.m[0][0] = 1;
	T.m[1][1] = 1;
	T.m[2][2] = 1;
	T.m[3][3] = 1;
	T.m[3][0] = translate.x;
	T.m[3][1] = translate.y;
	T.m[3][2] = translate.z;

	ans.m[0][0] = S.m[0][0] * R.m[0][0];
	ans.m[0][1] = S.m[0][0] * R.m[0][1];
	ans.m[0][2] = S.m[0][0] * R.m[0][2];
	ans.m[1][0] = S.m[1][1] * R.m[1][0];
	ans.m[1][1] = S.m[1][1] * R.m[1][1];
	ans.m[1][2] = S.m[1][1] * R.m[1][2];
	ans.m[2][0] = S.m[2][2] * R.m[2][0];
	ans.m[2][1] = S.m[2][2] * R.m[2][1];
	ans.m[2][2] = S.m[2][2] * R.m[2][2];
	ans.m[3][0] = T.m[3][0];
	ans.m[3][1] = T.m[3][1];
	ans.m[3][2] = T.m[3][2];
	ans.m[3][3] = 1;


	return ans;
};

//逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 ans;
	float inverse;
	inverse = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]
		- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
		- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
		+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
		+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
		- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]
		- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
		+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	ans.m[0][0] = 1 / inverse * (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);
	ans.m[0][1] = 1 / inverse * (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);
	ans.m[0][2] = 1 / inverse * (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);
	ans.m[0][3] = 1 / inverse * (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	ans.m[1][0] = 1 / inverse * (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);
	ans.m[1][1] = 1 / inverse * (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);
	ans.m[1][2] = 1 / inverse * (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);
	ans.m[1][3] = 1 / inverse * (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);

	ans.m[2][0] = 1 / inverse * (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);
	ans.m[2][1] = 1 / inverse * (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);
	ans.m[2][2] = 1 / inverse * (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);
	ans.m[2][3] = 1 / inverse * (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

	ans.m[3][0] = 1 / inverse * (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);
	ans.m[3][1] = 1 / inverse * (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);
	ans.m[3][2] = 1 / inverse * (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);
	ans.m[3][3] = 1 / inverse * (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return ans;
};


// 1, 透視投影行列
Matrix4x4 MakePrespectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 ans = { 0 };

	float cot = 1 / std::tan(fovY / 2);
	ans.m[0][0] = (1 / aspectRatio) * cot;
	ans.m[1][1] = cot;
	ans.m[2][2] = farClip / (farClip - nearClip);
	ans.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	ans.m[2][3] = 1;


	return ans;
};

// 2, 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = 2 / (right - left);
	ans.m[1][1] = 2 / (top - bottom);
	ans.m[2][2] = 1 / (farClip - nearClip);
	ans.m[3][0] = (left + right) / (left - right);
	ans.m[3][1] = (top + bottom) / (bottom - top);
	ans.m[3][2] = nearClip / (nearClip - farClip);
	ans.m[3][3] = 1;

	return ans;
};

// 3, ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = width / 2;
	ans.m[1][1] = height / 2 * -1;
	ans.m[2][2] = maxDepth - minDepth;
	ans.m[3][0] = left + width / 2;
	ans.m[3][1] = top + height / 2;
	ans.m[3][2] = minDepth;
	ans.m[3][3] = 1;

	return ans;
};

// 球体描画
/*void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = float(M_PI) / float(kSubdivision);	//φd
	const float kLatEvery = 2 * float(M_PI) / float(kSubdivision);	//Θd

	//float centerx = sphere.radius * cos(kLatEvery) * cos(kLonEvery);
	//float centerz = sphere.radius * cos(kLatEvery) * sin(kLonEvery);
	// 緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; latIndex++)
	{
		float lat = -float(M_PI) / 2.0f + kLatEvery * latIndex;// 現在の緯度(Θ)
		// 経度の方向に分割 0 ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; lonIndex++)
		{
			float lon = lonIndex * kLonEvery; // 現在の経度(φ)
			// world座標系でのa,b,cを求める
			Vector3 a, b, c, d;
			a = { cos(lat) * cos(lon) * sphere.radius + sphere.center.x,
				  sin(lat) * sphere.radius + sphere.center.y,
				  cos(lat) * sin(lon) * sphere.radius + sphere.center.z };


			b = { cos(lat + kLatEvery) * cos(lon) * sphere.radius + sphere.center.x,
				  sin(lat + kLatEvery) * sphere.radius + sphere.center.y,
				  cos(lat + kLatEvery) * sin(lon) * sphere.radius + sphere.center.z };

			c = { cos(lat) * cos(lon + kLonEvery) * sphere.radius + sphere.center.x,
				  sin(lat) * sphere.radius + sphere.center.y,
				  cos(lat) * sin(lon + kLonEvery) * sphere.radius + sphere.center.z };

			d = {cos(lat + kLatEvery) * cos(lon +)}

			Vector3 screenA = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 screenB = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 screenC = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);
			// a,b,cをScreen座標系まで変換
			//ab,bcで線を引く
		}
	}
	uint32_t startIndex = (latIndex * kSubdivision + lonIndex) * 6;
};*/