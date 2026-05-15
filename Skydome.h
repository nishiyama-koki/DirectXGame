#pragma once
#include "KamataEngine.h"


class Skydome {
public:

	void Initialize(KamataEngine::Model* model);
	void Update();
	void Draw(KamataEngine::Camera& camera);

private:

	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;

	uint32_t modelSkydome_ = 0;

	KamataEngine::Camera* camera_ = nullptr;
};