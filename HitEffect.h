#pragma once

#include "KamataEngine.h"

class HitEffect {
public:
	
	static HitEffect* Create(const KamataEngine::Vector3& position);
	
	void Initialize(const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }


private:
	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;
	
	//円のワールドトランスフォーム
	KamataEngine::WorldTransform circleWorldTransform_;


};
