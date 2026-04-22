#include "Player.h"
#include <cassert>

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) {

	// nullポインタチェック
	assert(model);
	assert(textureHandle);
	assert(camera);

	// 引数で受け取ったデータをメンバ変数に記録
	model_ = model;
	textureHandle_ = textureHandle;
	camera_ = camera;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();



}

void Player::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

}

void Player::Draw() {

	//3Dモデル描画前処理
	KamataEngine::Model::PreDraw();

	// 3Dモデルの描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);

	// 3Dモデル描画後処理
	KamataEngine::Model::PostDraw();

}