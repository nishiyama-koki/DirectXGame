#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
#include <numbers>

using namespace KamataEngine;

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandlePlayer, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	textureHandlePlayer_ = textureHandlePlayer;
	camera_ = camera;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::InitializeAttackEffect(KamataEngine::Model* modelAttack, uint32_t textureHandleAttack){
	modelAttack_ = modelAttack;
	textureHandleAttack_ = textureHandleAttack;

	worldTransformAttack_.Initialize();
	worldTransformAttack_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	isAttackEffectActive_ = false;
}



KamataEngine::Vector3 Player::GetWorldPosition() { return worldTransform_.translation_; }

Player::AABB Player::GetAABB() {
	Vector3 worldPos = GetWorldPosition();

	AABB aabb;
	aabb.min = {worldPos.x - kWidth / 2.0f + kBlank, worldPos.y - kHeight / 2.0f + kBlank, worldPos.z};
	aabb.max = {worldPos.x + kWidth / 2.0f - kBlank, worldPos.y + kHeight / 2.0f - kBlank, worldPos.z};
	return aabb;
}

void Player::OnCollision(const Enemy* enemy) {
	(void)enemy;
	isDead_ = true;
}

void Player::InputMove() {
	if (onGround_) {
		Vector3 acceleration = {};

		if (Input::GetInstance()->PushKey(DIK_D)) {
			if (velocity_.x < 0.0f) {
				velocity_.x *= (1.0f - kAttenuation);
			}
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = 1.0f / 60.0f;
			}

			acceleration.x += kAcceleration;
			velocity_.x += acceleration.x;
		} else if (Input::GetInstance()->PushKey(DIK_A)) {
			if (velocity_.x > 0.0f) {
				velocity_.x *= (1.0f - kAttenuation);
			}
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = 1.0f / 60.0f;
			}

			acceleration.x -= kAcceleration;
			velocity_.x += acceleration.x;
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			velocity_.y += kJumpAcceleration;
			onGround_ = false;
		}

		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	} else {
		velocity_.y -= kGravityAcceleration;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

#pragma region MapCollision
void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	if (info.moveAmount.y <= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
		positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	Corner hitCorner = Corner::kLeftTop;

	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftTop)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
		hitCorner = Corner::kLeftTop;
	}

	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightTop)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
		hitCorner = Corner::kRightTop;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(hitCorner)]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapIndexSetByPosition(CornerPosition(worldTransform_.translation_, hitCorner));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSetNow.xIndex, indexSetNow.yIndex);
			info.moveAmount.y = (rect.bottom - worldTransform_.translation_.y) - (kHeight / 2.0f + kBlank);
			info.moveAmount.y = std::max(0.0f, info.moveAmount.y);
			info.isCeiling_ = true;
		}
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.moveAmount.y >= 0) {
		return;
	}

	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
		positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;
	Corner hitCorner = Corner::kLeftBottom;

	MapChipField::IndexSet indexSet;

	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftBottom)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
		hitCorner = Corner::kLeftBottom;
	}

	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightBottom)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
		hitCorner = Corner::kRightBottom;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(hitCorner)]);
		MapChipField::IndexSet indexSetNow = mapChipField_->GetMapIndexSetByPosition(CornerPosition(worldTransform_.translation_, hitCorner));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSetNow.xIndex, indexSetNow.yIndex);
			info.moveAmount.y = (rect.top - worldTransform_.translation_.y) + (kHeight / 2.0f + kBlank);
			info.moveAmount.y = std::min(0.0f, info.moveAmount.y);
			info.isFloorTouch_ = true;
		}
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
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
		info.moveAmount.x = std::max(0.0f, info.moveAmount.x);
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
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
		info.moveAmount.x = std::min(0.0f, info.moveAmount.x);
	}
}

void Player::CheckCeilingCollision(const CollisionMapInfo& info) {
	if (info.isCeiling_) {
		velocity_.y = 0;
	}
}

Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Player::Corner corner) {
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

void Player::SwitchGroundingState(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			(void)info;

			MapChipType mapChipType;
			bool hit = false;
			MapChipField::IndexSet indexSet;
			float groundSearchHeight = 0.01f;
			Vector3 currentPos = worldTransform_.translation_;

			Vector3 leftBottomPos = CornerPosition(currentPos, Corner::kLeftBottom);
			leftBottomPos.y -= groundSearchHeight;

			indexSet = mapChipField_->GetMapIndexSetByPosition(leftBottomPos);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			Vector3 rightBottomPos = CornerPosition(currentPos, Corner::kRightBottom);
			rightBottomPos.y -= groundSearchHeight;
			indexSet = mapChipField_->GetMapIndexSetByPosition(rightBottomPos);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		if (info.isFloorTouch_) {
			onGround_ = true;
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
		}
	}
}
#pragma endregion

void Player::BehaviorRootInitialize() { worldTransform_.scale_ = {1.0f, 1.0f, 1.0f}; }

void Player::BehaviorRootUpdate() {
	InputMove();

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.moveAmount = velocity_;

	CheckMapCollision(collisionMapInfo);
	velocity_ = collisionMapInfo.moveAmount;
	CheckCeilingCollision(collisionMapInfo);
	SwitchGroundingState(collisionMapInfo);

	if (turnTimer_ > 0.0f) {
		turnTimer_ += 1.0f / 60.0f;

		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		float t = turnTimer_ / kTimeTurn;
		if (t >= 1.0f) {
			t = 1.0f;
			turnTimer_ = 0.0f;
		}

		t = t * t * (3.0f - 2.0f * t);
		worldTransform_.rotation_.y = (1.0f - t) * turnFirstRotationY_ + t * destinationRotationY;
	}

	if (Input::GetInstance()->TriggerKey(DIK_F)) {
		behaviorRequest_ = Behavior::kAttack;
		isAttackEffectActive_ = true;
		worldTransformAttack_.rotation_ = worldTransform_.rotation_;
	} else {
		isAttackEffectActive_ = false;
	}
}

void Player::BehaviorAttackInitialize() {
	attackCounter_ = 0;
	attackPhase_ = AttackPhase::charge;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
}

void Player::BehaviorAttackUpdate() {
	attackCounter_++;

	switch (attackPhase_) {
	case AttackPhase::charge: {
		float t = static_cast<float>(attackCounter_) / static_cast<float>(kChargeDuration);
		worldTransform_.scale_.z = EaseOut(1.0f, 0.3f, t);
		worldTransform_.scale_.y = EaseOut(1.0f, 1.6f, t);

		if (attackCounter_ >= kChargeDuration) {
			attackPhase_ = AttackPhase::dash;
			attackCounter_ = 0;
		}
		break;
	}

	case AttackPhase::dash: {
		float t = static_cast<float>(attackCounter_) / static_cast<float>(kDashDuration);
		worldTransform_.scale_.z = EaseOut(0.3f, 1.3f, t);
		worldTransform_.scale_.y = EaseIn(1.6f, 0.7f, t);

		float dir = (lrDirection_ == LRDirection::kRight) ? 1.0f : -1.0f;
		velocity_.x = dir * kDashSpeed;

		if (attackCounter_ >= kDashDuration) {
			attackPhase_ = AttackPhase::afterglow;
			attackCounter_ = 0;
		}
		break;
	}

	case AttackPhase::afterglow: {
		float t = static_cast<float>(attackCounter_) / static_cast<float>(kAfterglowDuration);
		worldTransform_.scale_.z = EaseOut(1.3f, 1.0f, t);
		worldTransform_.scale_.y = EaseOut(0.7f, 1.0f, t);

		velocity_.x *= (1.0f - kAttenuation);

		if (attackCounter_ >= kAfterglowDuration) {
			worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
			behaviorRequest_ = Behavior::kRoot;
		}
		break;
	}
	}

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.moveAmount = velocity_;
	CheckMapCollision(collisionMapInfo);
	velocity_ = collisionMapInfo.moveAmount;
	CheckCeilingCollision(collisionMapInfo);
	SwitchGroundingState(collisionMapInfo);
}

void Player::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;
		switch (behavior_) {
		case Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
	if (isAttackEffectActive_) {
		worldTransformAttack_.translation_ = worldTransform_.translation_;
		// 行列の更新
		worldTransformAttack_.matWorld_ = MakeAffineMatrix(worldTransformAttack_.scale_, worldTransformAttack_.rotation_, worldTransformAttack_.translation_);
		worldTransformAttack_.TransferMatrix();
	}
}

void Player::Draw() {
	KamataEngine::Model::PreDraw();
	model_->Draw(worldTransform_, *camera_, textureHandlePlayer_);
	if (isAttackEffectActive_ && modelAttack_) {
		modelAttack_->Draw(worldTransformAttack_, *camera_, textureHandleAttack_);
	}
	KamataEngine::Model::PostDraw();
}