#include "ClearScene.h"
#include "MyMath.h"
#include <cmath>
#include <numbers>

using namespace KamataEngine;

void ClearScene::Initialize() {
	textureHandleClearBg_ = TextureManager::Load("./Resources/bg_keys/bg_title.png");
	textureHandlePlayer_ = TextureManager::Load("./Resources/player/player.png");
	textureHandleReturn_ = TextureManager::Load("./Resources/bg_keys/return_title.png");
	textureHandleClear_ = TextureManager::Load("./Resources/bg_keys/clear.png");

	spriteClearBg_ = Sprite::Create(textureHandleClearBg_, {0.0f, 0.0f});
	spriteClear_ = Sprite::Create(textureHandleClear_, {0.0f, 0.0f});
	spriteReturn_ = Sprite::Create(textureHandleReturn_, {0.0f, 586.0f});
	playerModel_ = Model::CreateFromOBJ("player", true);

	// カメラ初期化
	camera_.farZ = 5000.0f;
	camera_.Initialize();

	// プレイヤーの配置
	playerWorldTransform_.Initialize();
	basePosY_ = -1.0f;
	playerWorldTransform_.translation_ = {0.0f, basePosY_,-40.0f};
	playerWorldTransform_.rotation_.y = 90.0f;

	// フェード初期化
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	finished_ = false;
	bounceTimer_ = 0.0f;
}

void ClearScene::Update() {
	if (fade_) {
		fade_->Update();
	}
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;

	case Phase::kMain:
		// スペースキー入力でフェードアウトへ
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f);
			phase_ = Phase::kFadeOut;
		}
		break;

	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true; // シーン終了
		}
		break;
	}
	bounceTimer_ += 1.0f / 60.0f;
	float bounceSpeed = 6.0f;  // 跳ねるスピード
	float bounceHeight = 1.2f; // 跳ねる高さ

	// absolute(sin) で地面で跳ね返る動きを作成
	float offsetY = std::abs(std::sin(bounceTimer_ * bounceSpeed)) * bounceHeight;
	playerWorldTransform_.translation_.y = basePosY_ + offsetY;

	// 行列更新
	playerWorldTransform_.matWorld_ = MakeAffineMatrix(playerWorldTransform_.scale_, playerWorldTransform_.rotation_, playerWorldTransform_.translation_);
	playerWorldTransform_.TransferMatrix();
}

void ClearScene::Draw() {
	// 1. 背景描画
	Sprite::PreDraw();
	if (spriteClearBg_) {
		spriteClearBg_->Draw();
	}
	spriteClear_->Draw();
	spriteReturn_->Draw();
	Sprite::PostDraw();

	// 深度クリア
	KamataEngine::DirectXCommon::GetInstance()->ClearDepthBuffer();

	// 跳ねるプレイヤー描画
	Model::PreDraw();
	if (playerModel_) {
		playerModel_->Draw(playerWorldTransform_, camera_, textureHandlePlayer_);
	}
	Model::PostDraw();

	// 3. フェード描画（最前面）
	Sprite::PreDraw();
	if (fade_) {
		fade_->Draw();
	}
	Sprite::PostDraw();
}