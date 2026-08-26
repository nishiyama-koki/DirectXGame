#include "DeathParticles.h"
#include "MyMath.h"
#include <algorithm>

using namespace KamataEngine;

void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	//assert(model);
	//assert(camera);
	model_ = model;
	camera_ = camera;
	for (auto& worldTransform : worldTransforms_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
		//worldTransform.scale_ = {0.5f, 0.5f, 0.5f};
	}
	objectColor_.Initialize();
	color_ = {1.0f, 1.0f, 1.0f, 1.0f};
}

void DeathParticles::Update() {

	if (isFinished_)
		return;

	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}

	for (auto& worldTransform : worldTransforms_) {
		worldTransform.translation_.y += 0.02f;
	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		float speed = 0.05f;
		float angle = kAngleUnit * i;
		worldTransforms_[i].translation_.x += std::cos(angle) * speed;
		worldTransforms_[i].translation_.y += std::sin(angle) * speed;
		worldTransforms_[i].matWorld_ = MakeAffineMatrix(worldTransforms_[i].scale_, worldTransforms_[i].rotation_, worldTransforms_[i].translation_);
		worldTransforms_[i].TransferMatrix();
	}

	color_.w = std::clamp(1.0f - (counter_ / kDuration), 0.0f, 1.0f);
	objectColor_.SetColor(color_);
}

void DeathParticles::Draw() {
	if (isFinished_)
		return;

	for (int i = 0; i < kNumParticles; i++) {
		model_->Draw(worldTransforms_[i], *camera_, &objectColor_);
	}
}