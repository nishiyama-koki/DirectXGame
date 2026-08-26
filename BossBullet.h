#pragma once
#include "KamataEngine.h"

class BossBullet {
public:

	// AABB構造体
	struct AABB {
		KamataEngine::Vector3 min;
		KamataEngine::Vector3 max;
	};
	BossBullet() = default;
	~BossBullet() = default;

	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);
	void Update();
	void Draw(const KamataEngine::Camera& camera);
	bool IsDead() const { return !isAlive_; }
	void OnCollision() { isAlive_ = false; }
	KamataEngine::Vector3 GetWorldPosition() const { return transform_.translation_; }

	// AABBの取得
	AABB GetAABB() const {
		AABB aabb;
		aabb.min = {transform_.translation_.x - kWidth_ / 2.0f, transform_.translation_.y - kHeight_ / 2.0f, transform_.translation_.z};
		aabb.max = {transform_.translation_.x + kWidth_ / 2.0f, transform_.translation_.y + kHeight_ / 2.0f, transform_.translation_.z};
		return aabb;
	}

private:
	KamataEngine::WorldTransform transform_{};
	KamataEngine::Vector3 velocity_{0.0f, 0.0f, 0.0f};
	KamataEngine::Model* model_ = nullptr;
	uint32_t textureHandle_ = 0;

	bool isAlive_ = false;
	float deathTimer_ = 0.0f;
	const float kMaxLifeTime_ = 5.0f; 
	const float kWidth_ = 0.5f;
	const float kHeight_ = 0.5f;

};