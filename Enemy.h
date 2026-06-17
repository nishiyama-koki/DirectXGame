#pragma once
#include "KamataEngine.h"

class Enemy {

public:
	void Initialize(KamataEngine::Model* model, uint32_t textureHandleEnemy, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();



private:

	//歩行の速さ
	static inline const float kWalkSpeed = 0.02f;
	//速度
	KamataEngine::Vector3 velocity_ = {};


	//最初の角度
	static inline const float kWalkMotionAngleStart = 1.0f;
	static inline const float kWalkMotionAngleEnd = 10.0f;
	static inline const float kWalkMotionTime = 0.2f;
	float walkTimer_ = 0.0f;





	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandleEnemy_ = 0;
	KamataEngine::Camera* camera_ = nullptr;

};
