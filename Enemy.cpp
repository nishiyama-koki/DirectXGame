#include "Enemy.h"
#include "GameScene.h"
#include "HitEffect.h"
#include "MapChipField.h"
#include "MyMath.h"
#include "Player.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

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
	lrDirection_ = LRDirection::kLeft;
	walkTimer_ = 0.0f;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f + std::numbers::pi_v<float>;
}

void Enemy::OnCollision(const Player* player) {
	(void)player;

	// すでに死亡状態（デス演出中）の場合は何もしない
	if (behavior_ == Behavior::kDeath) {
		return;
	}

	// プレイヤーが攻撃状態である場合
	if (player->IsAttacking()) {

		isCollisionDisabled_ = true; // 衝突判定を無効化
		behaviorRequest_ = Behavior::kDeath;
		Vector3 enemyPos = worldTransform_.translation_;
		Vector3 playerPos = player->GetWorldPosition();
		Vector3 effectPos;
		effectPos.x = (enemyPos.x + playerPos.x) * 0.5f;
		effectPos.y = (enemyPos.y + playerPos.y) * 0.5f;
		effectPos.z = (enemyPos.z + playerPos.z) * 0.5f;

		gameScene_->CreateEffect(effectPos);
	}
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
}

#pragma region MapCollision
Vector3 Enemy::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0.0f}
    };
	Vector3 offset = offsetTable[static_cast<uint32_t>(corner)];

	Vector3 result;
	result.x = center.x + offset.x;
	result.y = center.y + offset.y;
	result.z = center.z + offset.z;

	return result;
}

void Enemy::CheckMapCollision(CollisionMapInfo& info) {
	if (!mapChipField_)
		return;
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Enemy::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.moveAmount.x <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
		positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	Corner hitCorner = Corner::kRightTop;
	MapChipField::IndexSet indexSet;
	MapChipField::IndexSet indexSetNow;

	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightTop)]);
	indexSetNow = mapChipField_->GetMapIndexSetByPosition(CornerPosition(worldTransform_.translation_, Corner::kRightTop));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (indexSetNow.xIndex != indexSet.xIndex && mapChipType == MapChipType::kBlock) {
		hit = true;
		hitCorner = Corner::kRightTop;
	}

	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightBottom)]);
	indexSetNow = mapChipField_->GetMapIndexSetByPosition(CornerPosition(worldTransform_.translation_, Corner::kRightBottom));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if ((indexSetNow.xIndex != indexSet.xIndex) && (mapChipType == MapChipType::kBlock)) {
		hit = true;
		hitCorner = Corner::kRightBottom;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(hitCorner)]);
		indexSetNow = mapChipField_->GetMapIndexSetByPosition(CornerPosition(worldTransform_.translation_, hitCorner));
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSetNow.xIndex, indexSetNow.yIndex);
		info.moveAmount.x = (rect.left - worldTransform_.translation_.x) - (kWidth / 2.0f + kBlank);
		info.moveAmount.x = (std::max)(0.0f, info.moveAmount.x);
		info.isWallTouch_ = true;
	}
}

void Enemy::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.moveAmount.x >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
		positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	Corner hitCorner = Corner::kLeftTop;
	MapChipField::IndexSet indexSet;
	MapChipField::IndexSet indexSetNow;

	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftTop)]);
	indexSetNow = mapChipField_->GetMapIndexSetByPosition(CornerPosition(worldTransform_.translation_, Corner::kLeftTop));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if ((indexSetNow.xIndex != indexSet.xIndex) && (mapChipType == MapChipType::kBlock)) {
		hit = true;
		hitCorner = Corner::kLeftTop;
	}

	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftBottom)]);
	indexSetNow = mapChipField_->GetMapIndexSetByPosition(CornerPosition(worldTransform_.translation_, Corner::kLeftBottom));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if ((indexSetNow.xIndex != indexSet.xIndex) && (mapChipType == MapChipType::kBlock)) {
		hit = true;
		hitCorner = Corner::kLeftBottom;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(hitCorner)]);
		indexSetNow = mapChipField_->GetMapIndexSetByPosition(CornerPosition(worldTransform_.translation_, hitCorner));

		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSetNow.xIndex, indexSetNow.yIndex);
		info.moveAmount.x = (rect.right - worldTransform_.translation_.x) + (kWidth / 2.0f + kBlank);
		info.moveAmount.x = (std::min)(0.0f, info.moveAmount.x);
		info.isWallTouch_ = true;
	}
}
#pragma endregion

void Enemy::BehaviorRootInitialize() {
	// 通常状態の初期化処理
}

void Enemy::BehaviorRootUpdate() {
	walkTimer_ += 1.0f / 60.0f;

	// マップ衝突判定と押し戻し
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.moveAmount = velocity_;
	CheckMapCollision(collisionMapInfo);

	// 移動量の更新
	velocity_ = collisionMapInfo.moveAmount;

	if (collisionMapInfo.isWallTouch_) {
		if (lrDirection_ == LRDirection::kLeft) {
			lrDirection_ = LRDirection::kRight;
			velocity_.x = kWalkSpeed;
			worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
		} else {
			lrDirection_ = LRDirection::kLeft;
			velocity_.x = -kWalkSpeed;
			worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f + std::numbers::pi_v<float>;
		}
	}

	// 回転アニメーション
	float omega = (2.0f * std::numbers::pi_v<float>) / kWalkMotionTime;
	float param = std::sin(omega * walkTimer_);
	float degree = kWalkMotionAngleStart + (kWalkMotionAngleEnd - kWalkMotionAngleStart) * ((param + 1.0f) / 2.0f);
	worldTransform_.rotation_.x = degree * (std::numbers::pi_v<float> / 180.0f);

	// 移動量の加算
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
}

void Enemy::BehaviorDeathInitialize() {
	// デスアニメーション用タイマーのリセット
	deathTimer_ = 0.0f;

	deathStartRotationX_ = worldTransform_.rotation_.x;
	deathStartRotationY_ = worldTransform_.rotation_.y;
}

void Enemy::BehaviorDeathUpdate() {
	// アニメーションタイマーの加算
	deathTimer_ += 1.0f / 60.0f;
	float t = std::clamp(deathTimer_ / kDeathDuration, 0.0f, 1.0f);
	float easeT = 1.0f - (1.0f - t) * (1.0f - t);
	float targetRotationY = deathStartRotationY_ + std::numbers::pi_v<float> * 2.0f;
	worldTransform_.rotation_.y = (1.0f - easeT) * deathStartRotationY_ + easeT * targetRotationY;
	float targetRotationX = deathStartRotationX_ + (std::numbers::pi_v<float> / 2.0f);
	worldTransform_.rotation_.x = (1.0f - easeT) * deathStartRotationX_ + easeT * targetRotationX;

	if (deathTimer_ >= kDeathDuration) {
		isEnemyDead_ = true;
	}
}

void Enemy::Update() {
	// ビヘイビアのリクエストによる切り替え
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	// ビヘイビアの更新
	switch (behavior_) {
	case Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}

	// 行列の更新と転送
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