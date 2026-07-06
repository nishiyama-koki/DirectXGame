#include "TitleScene.h"
#include "MyMath.h"

using namespace KamataEngine;

void TitleScene::Initialize() {

	textureHandlePlayer_ = TextureManager::Load("./Resources/player/player.png");
	textureHandleEnemy_ = TextureManager::Load("./Resources/enemy/enemy.png");
	playerTitleModel_ = Model::CreateFromOBJ("player", true);
	enemyTitleModel_ = Model::CreateFromOBJ("enemy", true);
	camera_.farZ = 5000.0f;
	camera_.Initialize();
	playerWorldTransform_.Initialize();
	playerWorldTransform_.translation_ = {-2.0f, 0.0f, -30.0f};
	enemyWorldTransform_.Initialize();
	enemyWorldTransform_.translation_ = {2.0f, 0.0f, -30.0f}; 
	enemyWorldTransform_.rotation_.y = 3.1415f;             
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	finished_ = false;
}

void TitleScene::Update() {
	fade_->Update();
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;

	case Phase::kMain:
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			fade_->Start(Fade::Status::FadeOut, 1.0f); 
			phase_ = Phase::kFadeOut;
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}

	playerWorldTransform_.rotation_.y += 0.01f;
	enemyWorldTransform_.rotation_.y -= 0.01f;
	playerWorldTransform_.matWorld_ = MakeAffineMatrix(playerWorldTransform_.scale_, playerWorldTransform_.rotation_, playerWorldTransform_.translation_);
	playerWorldTransform_.TransferMatrix();
	enemyWorldTransform_.matWorld_ = MakeAffineMatrix(enemyWorldTransform_.scale_, enemyWorldTransform_.rotation_, enemyWorldTransform_.translation_);
	enemyWorldTransform_.TransferMatrix();
}

void TitleScene::Draw() {

	Sprite::PreDraw();

	Sprite::PostDraw();

	Model::PreDraw();
	if (playerTitleModel_) {
		playerTitleModel_->Draw(playerWorldTransform_, camera_, textureHandlePlayer_);
	}
	if (enemyTitleModel_) {
		enemyTitleModel_->Draw(enemyWorldTransform_, camera_, textureHandleEnemy_);
	}
	Model::PostDraw(); 

	Sprite::PreDraw();
	if (fade_) {
		fade_->Draw();
	}
	Sprite::PostDraw();

}