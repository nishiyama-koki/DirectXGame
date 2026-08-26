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
		kDeath,     // デス演出
	};

	enum class Action {
		kDash,            // 突進
		kWaitBeforeShoot, // 発射前の溜め
		kShoot,           // 弾の発射
		kCoolDown         // 攻撃後のスキ
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
	const std::vector<std::unique_ptr<BossBullet>>& GetBullets() const { return bullets_; }

	AABB GetAABB() const {
		AABB aabb;
		aabb.min = {worldTransform_.translation_.x - kWidth_ / 2.0f, worldTransform_.translation_.y - kHeight_ / 2.0f, worldTransform_.translation_.z};
		aabb.max = {worldTransform_.translation_.x + kWidth_ / 2.0f, worldTransform_.translation_.y + kHeight_ / 2.0f, worldTransform_.translation_.z};
		return aabb;
	}
	void TakeDamage(int damage = 1);
	bool IsInvincible() const { return invincibleTimer_ > 0.0f; }
	bool IsDead() const { return isBossDead_; }
	bool IsDying() const { return phase_ == Phase::kDeath; } 
	int GetHp() const { return hp_; }

private:
	void UpdateBattle();
	void UpdateBullets();
	void ShootBullet();
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

	//ステータス
	int hp_ = 10;                      
	float invincibleTimer_ = 0.0f;     
	const float kInvincibleTime = 1.0f;
	bool isVisible_ = true;            
	int flashFrame_ = 0;               

	//デス演出用
	bool isBossDead_ = false;          
	float deathTimer_ = 0.0f;          
	const float kDeathDuration_ = 2.0f;
	float deathStartRotationX_ = 0.0f; 
	float deathStartRotationY_ = 0.0f; 
	float deathStartPositionY_ = 0.0f; 

	//向き・回転制御用
	LRDirection lrDirection_ = LRDirection::kLeft; 
	float turnFirstRotationY_ = 0.0f;              
	float turnTimer_ = 0.0f;                       
	const float kTimeTurn_ = 0.3f;                 
};