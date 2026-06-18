#pragma once
#include "KamataEngine.h"


KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, float t);

KamataEngine::Matrix4x4 Multiply(const KamataEngine::Matrix4x4& m1, const KamataEngine::Matrix4x4& m2);
KamataEngine::Matrix4x4 MakeIdentityMatrix4x4();
KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate);
KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale);
KamataEngine::Matrix4x4 MakeRotateXMatrix(float radian);
KamataEngine::Matrix4x4 MakeRotateYMatrix(float radian);
KamataEngine::Matrix4x4 MakeRotateZMatrix(float radian);
KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);