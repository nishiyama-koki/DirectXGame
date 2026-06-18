#include "Enemy.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include <cmath>

using namespace KamataEngine;

void Enemy::Initialize(KamataEngine::Model* model, uint32_t textureHandleEnemy, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	// nullポインタチェック
	assert(model);
	assert(camera);
	// 引数で受け取ったデータをメンバ変数に記録
	model_ = model;
	textureHandleEnemy_ = textureHandleEnemy;
	camera_ = camera;
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
	walkTimer_ = 0.0f;
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Enemy::OnCollision(const Player* player) { 
	(void)player;
}


KamataEngine::Vector3 Enemy::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;
	return worldPos;
}

Enemy::AABB Enemy::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f + kBlank, worldPos.y - kHeight / 2.0f + kBlank, worldPos.z};
	aabb.max = {worldPos.x + kWidth / 2.0f - kBlank, worldPos.y + kHeight / 2.0f - kBlank, worldPos.z};
	return aabb;
};



void Enemy::Update() {
	walkTimer_ += 1.0f / 60.0f;

	//回転アニメーション
	float omega = (2.0f * std::numbers::pi_v<float>) / kWalkMotionTime;
	float param = std::sin(omega * walkTimer_);
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * ((param + 1.0f) / 2.0f);
	worldTransform_.rotation_.x = degree * (std::numbers::pi_v<float> / 180.0f);

	// 移動量の加算
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}


void Enemy::Draw() {
	// 3Dモデル描画前処理
	KamataEngine::Model::PreDraw();
	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandleEnemy_);
	// 3Dモデル描画後処理
	KamataEngine::Model::PostDraw();
}