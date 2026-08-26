#pragma once
#include "KamataEngine.h"

class Bubble {
public:
	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	bool IsDead() const { return isDead_; }

private:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* bubble_model_ = nullptr;
	uint32_t textureHandleBubble_ = 0;
	KamataEngine::Camera* camera_ = nullptr;

	float moveSpeedY_ = 0.05f;
	float timer_ = 0.0f;      
	float maxLifeTime_ = 3.0f;
	float currentLifeTime_ = 0.0f;
	bool isDead_ = false;
};