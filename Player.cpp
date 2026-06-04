#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "MyMath.h"
#include <algorithm>
#include <cassert>
#include <numbers>

using namespace KamataEngine;

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandlePlayer, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {

	// nullポインタチェック
	assert(model);
	assert(camera);

	// 引数で受け取ったデータをメンバ変数に記録
	model_ = model;
	textureHandlePlayer_ = textureHandlePlayer;
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::InputMove() {

	// 移動入力(左右)
	// 接地状態
	if (onGround_) {

		Vector3 acceleration = {};

		if (Input::GetInstance()->PushKey(DIK_D)) {
			if (velocity_.x < 0.0f) {
				// 反対方向に入力があった場合は減速する
				velocity_.x *= (1.0f - kAttenuation);
			}
			if (lrDirection_ != LRDirection::kRight) {
				lrDirection_ = LRDirection::kRight;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = 1.0f / 60.0f;
			}

			acceleration.x += kAcceleration;

			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;
		} else if (Input::GetInstance()->PushKey(DIK_A)) {
			if (velocity_.x > 0.0f) {
				// 反対方向に入力があった場合は減速する
				velocity_.x *= (1.0f - kAttenuation);
			}
			if (lrDirection_ != LRDirection::kLeft) {
				lrDirection_ = LRDirection::kLeft;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = 1.0f / 60.0f;
			}

			acceleration.x -= kAcceleration;

			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;
		} else {

			velocity_.x *= (1.0f - kAttenuation);
		}

		// ジャンプ初速
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			velocity_.y += kJumpAcceleration;
			onGround_ = false;
		}

		// 最大速度制限
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

	} else {

		// 落下速度,速度制限
		velocity_.y -= kGravityAcceleration;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

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

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
		positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftTop)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightTop)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(kLeftTop)]);
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.y = (rect.bottom - worldTransform_.translation_.y) - (kHeight / 2.0f + kBlank);
		info.moveAmount.y = std::max(0.0f, info.moveAmount.y);
		// 天井に当たったことを記録する
		info.isCeiling_ = true;
	}
}
void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	if (info.moveAmount.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
		positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定を行う
	bool hit = false;

	//左下点の判定
	MapChipField::IndexSet indexSetNow;
	indexSetNow = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftBottom)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSetNow.xIndex, indexSetNow.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSetNow.xIndex, indexSetNow.yIndex - 1); 
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	//右下点の判定 
	indexSetNow = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightBottom)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSetNow.xIndex, indexSetNow.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSetNow.xIndex, indexSetNow.yIndex - 1); // 💡右下側も同様に取得

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	if (hit) {
		indexSetNow = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(kLeftBottom)]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSetNow.xIndex, indexSetNow.yIndex);
		info.moveAmount.y = (rect.top - worldTransform_.translation_.y) + (kHeight / 2.0f + kBlank);
		info.moveAmount.y = std::min(0.0f, info.moveAmount.y);
		info.isFloorTouch_ = true;
	}
}
void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	if (info.moveAmount.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標を計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
		positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;
	// 右上点の判定
	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightTop)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightBottom)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kRightTop)]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.x = (rect.left - worldTransform_.translation_.x) - (kWidth / 2.0f + kBlank);
		info.moveAmount.x = std::max(0.0f, info.moveAmount.x);
	}
}
void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	if (info.moveAmount.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標を計算
	std::array<Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		Vector3 expectedPos = {worldTransform_.translation_.x + info.moveAmount.x, worldTransform_.translation_.y + info.moveAmount.y, worldTransform_.translation_.z + info.moveAmount.z};
		positionsNew[i] = CornerPosition(expectedPos, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;
	MapChipField::IndexSet indexSet;
	// 左上点の判定
	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftTop)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftBottom)]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		indexSet = mapChipField_->GetMapIndexSetByPosition(positionsNew[static_cast<uint32_t>(Corner::kLeftTop)]);
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.moveAmount.x = (rect.right - worldTransform_.translation_.x) + (kWidth / 2.0f + kBlank);
		info.moveAmount.x = std::min(0.0f, info.moveAmount.x);
	}
}

void Player::CheckCeilingCollision(const CollisionMapInfo& info) {
	if (info.isCeiling_) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
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
			// 真下の当たり判定を行う
			bool hit = false;
			MapChipField::IndexSet indexSet;
			float groundSearchHeight = 0.01f;
			Vector3 currentPos = worldTransform_.translation_;
			// 左下点の判定
			Vector3 leftBottomPos = CornerPosition(currentPos, Corner::kLeftBottom);
			leftBottomPos.y -= groundSearchHeight;

			indexSet = mapChipField_->GetMapIndexSetByPosition(leftBottomPos);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
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
			// X速度を減衰させる（エラーが出ないよう直接0.2fなどの数値を指定すると安全です）
			velocity_.x *= (1.0f - 0.2f);
			velocity_.y = 0.0f;
		}
	}
}


void Player::Update() {

	// 移動入力処理
	InputMove();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.moveAmount = velocity_;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);
	velocity_ = collisionMapInfo.moveAmount;
	CheckCeilingCollision(collisionMapInfo);

	SwitchGroundingState(collisionMapInfo);


	// 旋回制御
	if (turnTimer_ > 0.0f) {

		turnTimer_ += 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		float t = turnTimer_ / kTimeTurn;

		if (t >= 1.0f) {
			t = 1.0f;
			turnTimer_ = 0.0f;
		}

		// ease in out
		t = t * t * (3.0f - 2.0f * t);

		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = (1.0f - t) * turnFirstRotationY_ + t * destinationRotationY;
	}

	// 移動量の加算
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() {

	// 3Dモデル描画前処理
	KamataEngine::Model::PreDraw();

	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandlePlayer_);

	// 3Dモデル描画後処理
	KamataEngine::Model::PostDraw();
}
