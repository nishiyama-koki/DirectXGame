#pragma once
#include "Fade.h"
#include "KamataEngine.h"

class TitleScene {
public:
	void Initialize();
	void Update();
	void Draw();
	bool isFinished() const { return finished_; }

	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};
	Phase phase_ = Phase::kFadeIn;

private:
	bool finished_ = false;
	uint32_t textureHandlePlayer_ = 0;
	uint32_t textureHandleEnemy_ = 0;
	uint32_t textureHandleTitle_ = 0;
	uint32_t textureHandleGameTitle_ = 0;
	uint32_t textureHandleStart_ = 0;
	KamataEngine::Model* playerTitleModel_ = nullptr;
	KamataEngine::Model* enemyTitleModel_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;
	KamataEngine::WorldTransform enemyWorldTransform_;
	KamataEngine::Sprite* spriteTitle_ = nullptr;
	KamataEngine::Sprite* spriteGameTitle_ = nullptr;
	KamataEngine::Sprite* spriteStart_ = nullptr;
	KamataEngine::Camera camera_;

	Fade* fade_ = nullptr;
};
