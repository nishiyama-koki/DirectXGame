#pragma once
#include "KamataEngine.h"

class Player;

class Enemy {

public:
	struct AABB {
		KamataEngine::Vector3 min;
		KamataEngine::Vector3 max;
	};


	void Initialize(KamataEngine::Model* model, uint32_t textureHandleEnemy, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	KamataEngine::Vector3 GetWorldPosition();
	AABB GetAABB();
	void OnCollision(const Player* player);

private:
	// 歩行の速さ
	static inline const float kWalkSpeed = 0.02f;
	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 最初の角度
	static inline const float kWalkMotionAngleStart = 1.0f;
	static inline const float kWalkMotionAngleEnd = 10.0f;
	static inline const float kWalkMotionTime = 0.2f;
	float walkTimer_ = 0.0f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.01f;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandleEnemy_ = 0;
	KamataEngine::Camera* camera_ = nullptr;
};
