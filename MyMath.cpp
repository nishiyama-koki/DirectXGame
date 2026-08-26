#include "MyMath.h"

using namespace KamataEngine;

KamataEngine::Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
	Vector3 result;
	result.x = start.x + (end.x - start.x) * t;
	result.y = start.y + (end.y - start.y) * t;
	result.z = start.z + (end.z - start.z) * t;
	return result;
}

#pragma region 行列の演算
// 行列の積
KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2) {
	KamataEngine::Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

// 単位行列の作成
KamataEngine::Matrix4x4 MakeIdentityMatrix4x4() {
	KamataEngine::Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		result.m[i][i] = 1.0f;
	}
	return result;
}

// 平行移動行列
KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate) {
	KamataEngine::Matrix4x4 result = MakeIdentityMatrix4x4();
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	return result;
}

// 拡大縮小行列
KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale) {
	KamataEngine::Matrix4x4 result = MakeIdentityMatrix4x4();
	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;
	return result;
}

// X軸回転行列
KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian) {
	KamataEngine::Matrix4x4 result = MakeIdentityMatrix4x4();
	float cosA = std::cos(radian);
	float sinA = std::sin(radian);
	result.m[1][1] = cosA;
	result.m[1][2] = sinA;
	result.m[2][1] = -sinA;
	result.m[2][2] = cosA;
	return result;
}

// Y軸回転行列
KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian) {
	KamataEngine::Matrix4x4 result = MakeIdentityMatrix4x4();
	float cosA = std::cos(radian);
	float sinA = std::sin(radian);
	result.m[0][0] = cosA;
	result.m[0][2] = -sinA;
	result.m[2][0] = sinA;
	result.m[2][2] = cosA;
	return result;
}

// Z軸回転行列
KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian) {
	KamataEngine::Matrix4x4 result = MakeIdentityMatrix4x4();
	float cosA = std::cos(radian);
	float sinA = std::sin(radian);
	result.m[0][0] = cosA;
	result.m[0][1] = sinA;
	result.m[1][0] = -sinA;
	result.m[1][1] = cosA;
	return result;
}

// 3次元アフィン変換行列
KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate) {
	KamataEngine::Matrix4x4 scaleMatrix = MakeScaleMatrix(scale);
	KamataEngine::Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	KamataEngine::Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	KamataEngine::Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	KamataEngine::Matrix4x4 translateMatrix = MakeTranslateMatrix(translate);
	return Multiply(Multiply(scaleMatrix, Multiply(Multiply(rotateXMatrix, rotateYMatrix), rotateZMatrix)), translateMatrix);
}
#pragma endregion

