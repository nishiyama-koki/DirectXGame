#include "Skydome.h"

void Skydome::Initialize(KamataEngine::Model* model) { 
	model_ = model;
	worldTransform_.Initialize();

	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

}

void Skydome::Update() {



}

void Skydome::Draw(KamataEngine::Camera& camera) { 
	model_->Draw(worldTransform_, camera);

}