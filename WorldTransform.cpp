#include "WorldTransform.h"

using namespace KamataEngine;

static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {

	Matrix4x4 result = {};
	
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

void UpdateWorldTransform(WorldTransform& worldTransform) {

	Matrix4x4 matScale = MathUtility::MakeScaleMatrix(worldTransform.scale_);
	Matrix4x4 matRotateX = MathUtility::MakeRotateXMatrix(worldTransform.rotation_.x);
	Matrix4x4 matRotateY = MathUtility::MakeRotateYMatrix(worldTransform.rotation_.y);
	Matrix4x4 matRotateZ = MathUtility::MakeRotateZMatrix(worldTransform.rotation_.z);
	Matrix4x4 matTranslate = MathUtility::MakeTranslateMatrix(worldTransform.translation_);

	Matrix4x4 matRotate = Multiply(matRotateZ, Multiply(matRotateX, matRotateY));
	worldTransform.matWorld_ = Multiply(matScale, Multiply(matRotate, matTranslate));

	// 定数バッファへ転送
	worldTransform.TransferMatrix();
}
