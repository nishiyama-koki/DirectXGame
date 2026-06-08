#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include "MyMath.h"
#include <algorithm>

using namespace KamataEngine;

void CameraController::Initialize() {

	//
	camera_.Initialize();
}

void CameraController::Update() {

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();
	// 追従対象とオフセットからカメラの座標を計算
	targetPosition_ = {
		targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias,
		targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias,
	    targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias};

	camera_.translation_ = Lerp(camera_.translation_, targetPosition_, kInterpolationRate);

	//追従対象が画面外に出ないように補正
	
	camera_.translation_.x = std::max(camera_.translation_.x, targetWorldTransform.translation_.x + margin_.left);
	camera_.translation_.x = std::min(camera_.translation_.x, targetWorldTransform.translation_.x + margin_.right);
	camera_.translation_.y = std::max(camera_.translation_.y, targetWorldTransform.translation_.y + margin_.bottom);
	camera_.translation_.y = std::min(camera_.translation_.y, targetWorldTransform.translation_.y + margin_.top);


	// 移動範囲制限
	camera_.translation_.x = std::clamp(camera_.translation_.x, movableArea_.left, movableArea_.right);
	camera_.translation_.y = std::clamp(camera_.translation_.y, movableArea_.bottom, movableArea_.top);

	// 行列の更新
	camera_.UpdateMatrix();
}

void CameraController::Reset() {

	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = {targetWorldTransform.translation_.x + targetOffset_.x, targetWorldTransform.translation_.y + targetOffset_.y, targetWorldTransform.translation_.z + targetOffset_.z};
}
