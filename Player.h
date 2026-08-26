#pragma once
#include "KamataEngine.h"
#include <algorithm>

class MapChipField;
class Enemy;

class Player {
public:
	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

	enum class Behavior {
		kRoot,
		kAttack,
		kUnknown,
	};

	enum class AttackPhase {
		charge,
		dash,
		afterglow,
	};

	struct AABB {
		KamataEngine::Vector3 min;
		KamataEngine::Vector3 max;
	};

	enum class LRDirection {
		kRight,
		kLeft,
	};

	struct CollisionMapInfo {
		bool isCeiling_ = false;
		bool isWallTouch_ = false;
		bool isFloorTouch_ = false;
		KamataEngine::Vector3 moveAmount = {};
	};

	void Initialize(KamataEngine::Model* model, uint32_t textureHandlePlayer, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void InitializeAttackEffect(KamataEngine::Model* modelAttack, uint32_t textureHandleAttack);
	void Update();
	void Draw();

	void OnCollision();
	void OnCollision(const Enemy* enemy);

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	AABB GetAABB();
	const KamataEngine::Vector3& GetWorldPosition() const { return worldTransform_.translation_; };

	void InputMove();

	bool isDead() const { return isDead_; }
	bool IsAttacking() const { return isAttackEffectActive_ && modelAttack_ != nullptr; }

	void BehaviorRootInitialize();
	void BehaviorRootUpdate();
	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();


private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandlePlayer_ = 0;
	uint32_t textureHandleAttack_ = 0;
	bool isAttackEffectActive_ = false;
	KamataEngine::Camera* camera_ = nullptr;

	static inline const float kAcceleration = 0.02f;
	static inline const float kAttenuation = 0.05f;
	static inline const float kLimitRunSpeed = 0.17f;
	KamataEngine::Vector3 velocity_ = {};

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	static inline const float kGravityAcceleration = 0.01f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 0.3f;

	MapChipField* mapChipField_ = nullptr;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	void CheckCeilingCollision(const CollisionMapInfo& info);
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	void SwitchGroundingState(const CollisionMapInfo& info);
	static inline const float kAttenuationLanding = 0.2f;
	static inline const float kBlank = 0.01f;

	bool isDead_ = false;

	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	AttackPhase attackPhase_ = AttackPhase::charge;
	uint32_t attackCounter_ = 0;

	static inline const uint32_t kChargeDuration = 5;
	static inline const uint32_t kDashDuration = 10;
	static inline const uint32_t kAfterglowDuration = 10;
	static inline const float kDashSpeed = 0.3f;

	//攻撃エフェクト
	KamataEngine::Model* modelAttack_ = nullptr;
	KamataEngine::WorldTransform worldTransformAttack_;
	
	static float EaseIn(float start, float end, float t) {
		t = std::clamp(t, 0.0f, 1.0f);
		return start + (end - start) * (t * t);
	}

	static float EaseOut(float start, float end, float t) {
		t = std::clamp(t, 0.0f, 1.0f);
		return start + (end - start) * (1.0f - (1.0f - t) * (1.0f - t));
	}

	//体力無敵時間
	int hp_ = 3;                                          
	const int kMaxHp = 3;                                 
	float invincibleTimer_ = 0.0f;                        
	static inline const float kInvincibleDuration = 2.0f; 
public:
	int GetHp() const { return hp_; }
	bool IsInvincible() const { return invincibleTimer_ > 0.0f; }

};