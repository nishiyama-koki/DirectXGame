#pragma once
#include "KamataEngine.h"
#include "Fade.h"

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
	KamataEngine::Model* playerTitleModel_ = nullptr;
	KamataEngine::Model* enemyTitleModel_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;
	KamataEngine::WorldTransform enemyWorldTransform_;
	KamataEngine::Camera camera_;

	Fade* fade_ = nullptr;

};
