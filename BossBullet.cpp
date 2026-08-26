#include "BossBullet.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

using namespace KamataEngine;

void BossBullet::Initialize(Model* model, uint32_t textureHandle, const Vector3& position, const Vector3& velocity) {
	model_ = model;
	textureHandle_ = textureHandle;
	velocity_ = velocity;

	transform_.Initialize();
	transform_.translation_ = position;
	transform_.scale_ = {1.0f, 1.0f, 1.0f};
	if (velocity_.x > 0.0f) {
		transform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	} else {
		transform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;
	}

	isAlive_ = true;
	deathTimer_ = 0.0f;
}

void BossBullet::Update() {
	if (!isAlive_)
		return;

	// 位置の更新
	transform_.translation_.x += velocity_.x;
	transform_.translation_.y += velocity_.y;
	transform_.translation_.z += velocity_.z;

	// 行列更新
	transform_.matWorld_ = MakeAffineMatrix(transform_.scale_, transform_.rotation_, transform_.translation_);
	transform_.TransferMatrix();

	// 一定時間経過で消滅
	deathTimer_ += 1.0f / 60.0f;
	if (deathTimer_ >= kMaxLifeTime_) {
		isAlive_ = false;
	}
}

void BossBullet::Draw(const Camera& camera) {
	if (!isAlive_ || !model_)
		return;

	model_->Draw(transform_, camera, textureHandle_);
}