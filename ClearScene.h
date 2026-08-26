#pragma once
#include "Fade.h"
#include "KamataEngine.h"

class ClearScene {
public:
	void Initialize();
	void Update();
	void Draw();
	bool IsFinished() const { return finished_; }

	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

private:
	Phase phase_ = Phase::kFadeIn;
	bool finished_ = false;

	// アセットデータ
	uint32_t textureHandleClearBg_ = 0;
	uint32_t textureHandlePlayer_ = 0;
	uint32_t textureHandleReturn_ = 0;
	uint32_t textureHandleClear_ = 0;

	KamataEngine::Sprite* spriteClearBg_ = nullptr;
	KamataEngine::Sprite* spriteReturn_ = nullptr;
	KamataEngine::Sprite* spriteClear_ = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;
	KamataEngine::Camera camera_;

	// バウンド演出用
	float bounceTimer_ = 0.0f;
	float basePosY_ = -1.0f; 

	Fade* fade_ = nullptr;
};