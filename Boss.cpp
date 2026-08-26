#include "Boss.h"
#include "MyMath.h"
#include "Player.h"
#include <cassert>
#include <cmath>
#include <numbers>

using namespace KamataEngine;

void Boss::Initialize(Model* model, uint32_t textureHandleBoss, Camera* camera, const Vector3& position) {
	assert(model);
	assert(camera);
	model_ = model;
	textureHandleBoss_ = textureHandleBoss;
	camera_ = camera;

	targetPosition_ = position;
	startPosition_ = position;
	startPosition_.y += 20.0f;
	lrDirection_ = LRDirection::kLeft;
	turnTimer_ = 0.0f;

	worldTransform_.Initialize();
	worldTransform_.translation_ = startPosition_;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f + std::numbers::pi_v<float>;

	phase_ = Phase::kWaiting;
	action_ = Action::kDash;
	appearTimer_ = 0.0f;
	dashDirection_ = -1.0f;
	hp_ = 10;
	invincibleTimer_ = 0.0f;
	isVisible_ = true;
	isBossDead_ = false;
	bullets_.clear();
}

void Boss::StartAppearance() {
	if (phase_ == Phase::kWaiting) {
		phase_ = Phase::kAppearing;
		appearTimer_ = 0.0f;
	}
}

void Boss::Update() {
	// 無敵タイマーと点滅処理（死亡演出中は点滅させない）
	if (phase_ != Phase::kDeath) {
		if (invincibleTimer_ > 0.0f) {
			invincibleTimer_ -= 1.0f / 60.0f;

			flashFrame_++;
			if (flashFrame_ % 4 == 0) {
				isVisible_ = !isVisible_;
			}
		} else {
			invincibleTimer_ = 0.0f;
			isVisible_ = true;
		}
	} else {
		isVisible_ = true; // デス演出中は常に表示
	}

	switch (phase_) {
	case Phase::kWaiting:
		return;

	case Phase::kAppearing:
		appearTimer_ += 1.0f / 60.0f;
		{
			float t = appearTimer_ / kAppearDuration_;
			if (t > 1.0f)
				t = 1.0f;

			float easeT = t * t;
			worldTransform_.translation_.y = startPosition_.y + (targetPosition_.y - startPosition_.y) * easeT;

			if (t >= 1.0f) {
				worldTransform_.translation_ = targetPosition_;
				phase_ = Phase::kBattle;
				action_ = Action::kDash;
				dashTimer_ = 0.0f;
			}
		}
		break;

	case Phase::kBattle:
		UpdateBattle();
		break;

	case Phase::kDeath:
		BehaviorDeathUpdate();
		break;
	}
	// --- 追加: 向き変更（Y軸回転）のなめらかな補間処理 ---
	if (turnTimer_ > 0.0f) {
		turnTimer_ += 1.0f / 60.0f;

		// 右向き: π/2, 左向き: 3π/2
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		float t = turnTimer_ / kTimeTurn_;
		if (t >= 1.0f) {
			t = 1.0f;
			turnTimer_ = 0.0f;
		}

		// イージング（SmoothStep）
		t = t * t * (3.0f - 2.0f * t);
		worldTransform_.rotation_.y = (1.0f - t) * turnFirstRotationY_ + t * destinationRotationY;
	}
	UpdateBullets();

	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}

void Boss::UpdateBattle() {
	const float kDeltaTime = 1.0f / 60.0f;

	switch (action_) {
	case Action::kDash:
		dashTimer_ += kDeltaTime;
		worldTransform_.translation_.x += dashDirection_ * kDashSpeed_;

		if (dashTimer_ >= kDashDuration_) {
			action_ = Action::kWaitBeforeShoot;
			actionTimer_ = 0.0f;
			dashDirection_ *= -1.0f;
			// レイヤーの方向を見て振り向く ---
			if (player_) {
				LRDirection newDir = (player_->GetWorldPosition().x > worldTransform_.translation_.x) ? LRDirection::kRight : LRDirection::kLeft;
				if (newDir != lrDirection_) {
					lrDirection_ = newDir;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = 1.0f / 60.0f; // 振り向き開始
				}
			}
		}
		break;

	case Action::kWaitBeforeShoot:
		actionTimer_ += kDeltaTime;
		if (actionTimer_ >= 0.5f) {
			if (player_) {
				Vector3 pPos = player_->GetWorldPosition();
				Vector3 bPos = worldTransform_.translation_;
				Vector3 diff = {pPos.x - bPos.x, pPos.y - bPos.y, pPos.z - bPos.z};

				float length = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
				if (length != 0.0f) {
					targetDirection_ = {diff.x / length, diff.y / length, diff.z / length};
				} else {
					targetDirection_ = {-1.0f, 0.0f, 0.0f};
				}
			}

			action_ = Action::kShoot;
			shootTimer_ = 0.0f;
			shotCount_ = 0;
		}
		break;

	case Action::kShoot:
		shootTimer_ += kDeltaTime;
		if (shootTimer_ >= kShootInterval_) {
			shootTimer_ = 0.0f;
			ShootBullet();
			shotCount_++;

			if (shotCount_ >= kMaxShots_) {
				action_ = Action::kCoolDown;
				actionTimer_ = 0.0f;
			}
		}
		break;

	case Action::kCoolDown:
		actionTimer_ += kDeltaTime;
		if (actionTimer_ >= 1.0f) {
			action_ = Action::kDash;
			dashTimer_ = 0.0f;
			LRDirection dashDir = (dashDirection_ > 0.0f) ? LRDirection::kRight : LRDirection::kLeft;
			if (dashDir != lrDirection_) {
				lrDirection_ = dashDir;
				turnFirstRotationY_ = worldTransform_.rotation_.y;
				turnTimer_ = 1.0f / 60.0f; // 振り向き開始
			}
		}
		break;
	}
}

void Boss::ShootBullet() {
	if (!bulletModel_)
		return;

	const float kBulletSpeed = 0.3f;
	Vector3 velocity = {targetDirection_.x * kBulletSpeed, targetDirection_.y * kBulletSpeed, targetDirection_.z * kBulletSpeed};

	auto newBullet = std::make_unique<BossBullet>();
	newBullet->Initialize(bulletModel_, textureHandleBullet_, worldTransform_.translation_, velocity);
	bullets_.push_back(std::move(newBullet));
}

void Boss::UpdateBullets() {
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const std::unique_ptr<BossBullet>& bullet) { return bullet->IsDead(); }), bullets_.end());
}

void Boss::TakeDamage(int damage) {
	// すでに死亡中（デス演出中含む）なら何もしない
	if (IsInvincible() || phase_ == Phase::kDeath || isBossDead_)
		return;

	hp_ -= damage;
	if (hp_ <= 0) {
		hp_ = 0;
		// HPが0になったらデス演出フェーズへ移行
		phase_ = Phase::kDeath;
		BehaviorDeathInitialize();
		return;
	}

	// 無敵時間開始
	invincibleTimer_ = kInvincibleTime;
	flashFrame_ = 0;
}

// デス演出の初期化処理
void Boss::BehaviorDeathInitialize() {
	deathTimer_ = 0.0f;
	deathStartRotationX_ = worldTransform_.rotation_.x;
	deathStartRotationY_ = worldTransform_.rotation_.y;
	deathStartPositionY_ = worldTransform_.translation_.y;
}

// デス演出の更新処理（2.0秒かけて派手に倒れる）
void Boss::BehaviorDeathUpdate() {
	deathTimer_ += 1.0f / 60.0f;

	float t = std::clamp(deathTimer_ / kDeathDuration_, 0.0f, 1.0f);

	// EaseOutQuad (徐々に減速する回転)
	float easeT = 1.0f - (1.0f - t) * (1.0f - t);

	// 1. Y軸周りに3回転（6πラジアン）
	float targetRotationY = deathStartRotationY_ + std::numbers::pi_v<float> * 6.0f;
	worldTransform_.rotation_.y = (1.0f - easeT) * deathStartRotationY_ + easeT * targetRotationY;

	// 2. X軸周りに倒れる（90度）
	float targetRotationX = deathStartRotationX_ + (std::numbers::pi_v<float> / 2.0f);
	worldTransform_.rotation_.x = (1.0f - easeT) * deathStartRotationX_ + easeT * targetRotationX;

	// 3. 地下に少しずつ沈む
	worldTransform_.translation_.y = deathStartPositionY_ - (easeT * 2.0f);

	// タイマー完了時に完全消滅（isBossDead_ を true にする）
	if (deathTimer_ >= kDeathDuration_) {
		isBossDead_ = true;
	}
}

void Boss::Draw() {
	KamataEngine::Model::PreDraw();
	// 完全死亡時、または点滅非表示時、あるいは初期待機状態は描画しない
	if (isBossDead_ || !isVisible_ || phase_ == Phase::kWaiting || !camera_ || !model_) {
		return;
	}

	// ボス本体の描画
	model_->Draw(worldTransform_, *camera_, textureHandleBoss_);

	// 弾の描画
	for (auto& bullet : bullets_) {
		bullet->Draw(*camera_);
	}
	KamataEngine::Model::PostDraw();
}