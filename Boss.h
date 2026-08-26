#pragma once
#include "BossBullet.h"
#include "KamataEngine.h"
#include <algorithm>
#include <memory>
#include <vector>

class Player;

class Boss {
public:
	enum class Phase {
		kWaiting,   // プレイヤー待機
		kAppearing, // 落下演出
		kBattle,    // 戦闘中
		kDeath,     // デス演出中（追加）
	};

	enum class Action {
		kDash,            // 突進
		kWaitBeforeShoot, // 発射前の溜め（停止）
		kShoot,           // 弾の発射（連続3〜5発）
		kCoolDown         // 攻撃後のスキ（停止）
	};

	enum class LRDirection {
		kRight,
		kLeft,
	};

	struct AABB {
		KamataEngine::Vector3 min;
		KamataEngine::Vector3 max;
	};

	Boss() = default;
	~Boss() = default;

	void Initialize(KamataEngine::Model* model, uint32_t textureHandleBoss, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	void StartAppearance();
	bool IsAppeared() const { return phase_ != Phase::kWaiting; }

	void SetPlayer(Player* player) { player_ = player; }
	void SetBulletModel(KamataEngine::Model* model, uint32_t textureHandle) {
		bulletModel_ = model;
		textureHandleBullet_ = textureHandle;
	}

	const KamataEngine::Vector3& GetTargetPosition() const { return targetPosition_; }

	// 弾コンテナのゲッター（プレイヤーとの当たり判定に使用可能）
	const std::vector<std::unique_ptr<BossBullet>>& GetBullets() const { return bullets_; }

	AABB GetAABB() const {
		AABB aabb;
		aabb.min = {worldTransform_.translation_.x - kWidth_ / 2.0f, worldTransform_.translation_.y - kHeight_ / 2.0f, worldTransform_.translation_.z};
		aabb.max = {worldTransform_.translation_.x + kWidth_ / 2.0f, worldTransform_.translation_.y + kHeight_ / 2.0f, worldTransform_.translation_.z};
		return aabb;
	}

	// --- ダメージ・無敵関連 ---
	void TakeDamage(int damage = 1);
	bool IsInvincible() const { return invincibleTimer_ > 0.0f; }

	// 修正: デス演出終了（isBossDead_）をもって完全に「死亡状態」と判定する
	bool IsDead() const { return isBossDead_; }
	bool IsDying() const { return phase_ == Phase::kDeath; } // デス演出中かどうかの判定
	int GetHp() const { return hp_; }

private:
	void UpdateBattle();
	void UpdateBullets();
	void ShootBullet();

	// --- 追加: デス演出 ---
	void BehaviorDeathInitialize();
	void BehaviorDeathUpdate();

private:
	KamataEngine::WorldTransform worldTransform_{};
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandleBoss_ = 0;
	KamataEngine::Camera* camera_ = nullptr;

	Player* player_ = nullptr;

	Phase phase_ = Phase::kWaiting;
	Action action_ = Action::kDash;

	float appearTimer_ = 0.0f;
	const float kAppearDuration_ = 1.5f;
	KamataEngine::Vector3 startPosition_{0.0f, 0.0f, 0.0f};
	KamataEngine::Vector3 targetPosition_{0.0f, 0.0f, 0.0f};

	float dashTimer_ = 0.0f;
	float dashDirection_ = -2.0f;
	const float kDashSpeed_ = 0.25f;
	const float kDashDuration_ = 1.2f;

	// 弾管理（分離されたクラスを保持）
	KamataEngine::Model* bulletModel_ = nullptr;
	uint32_t textureHandleBullet_ = 0;
	std::vector<std::unique_ptr<BossBullet>> bullets_;

	KamataEngine::Vector3 targetDirection_{0.0f, 0.0f, 0.0f};
	float shootTimer_ = 0.0f;
	int shotCount_ = 0;
	const int kMaxShots_ = 4;
	const float kShootInterval_ = 0.2f;

	float actionTimer_ = 0.0f;

	const float kWidth_ = 2.0f;
	const float kHeight_ = 2.0f;

	// --- ステータス変数 ---
	int hp_ = 10;                       // 体力（初期値10）
	float invincibleTimer_ = 0.0f;      // 無敵残り時間（秒）
	const float kInvincibleTime = 1.0f; // 被弾後の無敵時間（1秒）
	bool isVisible_ = true;             // 点滅描画用フラグ
	int flashFrame_ = 0;                // 点滅周期計算用カウンタ

	// --- 追加: デス演出用の変数 ---
	bool isBossDead_ = false;           // 完全に消滅したかどうかのフラグ
	float deathTimer_ = 0.0f;           // デス演出用タイマー
	const float kDeathDuration_ = 2.0f; // デス演出の長さ（2.0秒：Enemyの4倍）
	float deathStartRotationX_ = 0.0f;  // 演出開始時のX回転
	float deathStartRotationY_ = 0.0f;  // 演出開始時のY回転
	float deathStartPositionY_ = 0.0f;  // 演出開始時のY座標

	// --- 追加: 向き・回転制御用変数 ---
	LRDirection lrDirection_ = LRDirection::kLeft; // 現在の向き
	float turnFirstRotationY_ = 0.0f;              // 回転開始時のY角度
	float turnTimer_ = 0.0f;                       // 回転アニメーションタイマー
	const float kTimeTurn_ = 0.3f;                 // 振り向きにかける時間（0.3秒）
};