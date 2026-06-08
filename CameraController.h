#pragma once
#include <KamataEngine.h>

// 前方宣言
class Player;

class CameraController {

public:
	// CameraController() {}
	//~CameraController() {}
	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	void Initialize();

	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	void SetMovablearea(const Rect& area) { movableArea_ = area; }

	const KamataEngine::Camera& GetCamera() const { return camera_; }

	static inline const float kInterpolationRate = 0.1f;



private:
	KamataEngine::Camera camera_;

	// 追従対象
	Player* target_ = nullptr;

	// 追従対象との座標の差
	KamataEngine::Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f};

	KamataEngine::Vector3 targetPosition_ = {0,0,0};
	Rect movableArea_ = {0.0f, 100.0f, 0.0f, 100.0f};


	// 速度掛け率
	static inline const float kVelocityBias = 15.0f;
	static inline const Rect margin_ = {-15.0f, 15.0f, -10.0f, 10.0f};


};
