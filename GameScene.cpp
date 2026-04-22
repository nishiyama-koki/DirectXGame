#include "GameScene.h"
#include "2d/ImGuiManager.h"

using namespace KamataEngine;
GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite_;
	delete model_;
	delete player_;
}

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");
	sprite_ = Sprite::Create(textureHandle_, {100, 50});

	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの生成
	camera_.Initialize();


	//自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &camera_);

}

void GameScene::Update() {

	// 自キャラの更新
	player_->Update();

}

void GameScene::Draw() {
	// スプライト描画前処理
	Sprite::PreDraw();

	//sprite_->Draw();
	// 自キャラの描画
	player_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();



}