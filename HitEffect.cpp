#include "HitEffect.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
#include <numbers>

// 静的メンバ変数の実態
KamataEngine::Model* HitEffect::model_ = nullptr;
KamataEngine::Camera* HitEffect::camera_ = nullptr;



HitEffect* HitEffect::Create(const KamataEngine::Vector3& position) {
	HitEffect* hitEffect = new HitEffect();
	assert(hitEffect);
	hitEffect->Initialize(position);
	return hitEffect;
}


void HitEffect::Initialize(const KamataEngine::Vector3& position) {
	circleWorldTransform_.Initialize();
	circleWorldTransform_.translation_ = position;
	circleWorldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	circleWorldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
}

void HitEffect::Update() {
	circleWorldTransform_.matWorld_ = MakeAffineMatrix(circleWorldTransform_.scale_, circleWorldTransform_.rotation_, circleWorldTransform_.translation_);
	circleWorldTransform_.TransferMatrix();
}

void HitEffect::Draw() { 
		// 3Dモデル描画前処理
	KamataEngine::Model::PreDraw();
	model_->Draw(circleWorldTransform_, *camera_); 
	// 3Dモデル描画後処理
	KamataEngine::Model::PostDraw();
}