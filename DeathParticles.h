#pragma once
#include "KamataEngine.h"
#include <array>
#include <cmath>
using namespace KamataEngine;

class DeathParticles {

public:
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();
	bool isFinished_ = false;
	bool isInitialized_ = false;
	float counter_ = 0.0f;

private:
	static inline const uint32_t kNumParticles = 8;
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransforms_;

	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;
	uint32_t textureHandle_ = 0;

	ObjectColor objectColor_;
	Vector4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};

	static inline const float kDuration = 1.5f;
	static inline const float kSpeed = 0.1f;
	static inline const float kAngleUnit = (2 * 3.14159265358979323846f) / kNumParticles;
};