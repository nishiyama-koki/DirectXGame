#define NOMINMAX
#include "Player.h"
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

void Player::Update() {

	// 着地フラグ
	bool landing = false;
	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

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

		// 着地
		if (landing) {
			worldTransform_.translation_.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		} else {

			// 落下速度,速度制限
			velocity_.y -= kGravityAcceleration;
			velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
		}
	}

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
