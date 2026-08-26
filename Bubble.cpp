#include "Bubble.h"
#include "MyMath.h"
#include <cmath>
#include <numbers>

using namespace KamataEngine;

void Bubble::Initialize(Model* model, uint32_t textureHandle, Camera* camera, const Vector3& position) {
	bubble_model_ = model;
	textureHandleBubble_ = textureHandle;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	

	// 泡のサイズをランダムにする（例: 0.2 ～ 0.5倍）
	float randomScale = 0.2f + static_cast<float>(rand()) / RAND_MAX * 0.3f;
	worldTransform_.scale_ = {randomScale, randomScale, randomScale};

	// 上昇スピードも少しランダムに
	moveSpeedY_ = 0.03f + static_cast<float>(rand()) / RAND_MAX * 0.04f;
}

void Bubble::Update() {
	if (isDead_)
		return;

	float deltaTime = 1.0f / 60.0f;
	currentLifeTime_ += deltaTime;
	timer_ += deltaTime;

	// 上昇移動
	worldTransform_.translation_.y += moveSpeedY_;

	// 左右にふわふわ揺らす（サイン波）
	worldTransform_.translation_.x += std::sin(timer_ * 4.0f) * 0.01f;

	// 寿命が来たらフラグを立てる
	if (currentLifeTime_ >= maxLifeTime_) {
		isDead_ = true;
	}

	// 行列更新
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Bubble::Draw() {
	Model::PreDraw();
	if (isDead_)
		return;
	bubble_model_->Draw(worldTransform_, *camera_, textureHandleBubble_);
	Model::PostDraw();
}