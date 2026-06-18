#include "GameScene.h"
#include "2d/ImGuiManager.h"
#include "MyMath.h"
using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete sprite_;
	delete modelSkydome_;
	delete player_model_;
	delete player_;
	delete enemy_model_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
	delete block_model_;
	delete mapChipField_;
	// ブロックのワールドトランスフォームの開放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	delete debugCamera_;
	delete cameraController_;
}

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandleBlock_ = TextureManager::Load("./Resources/block/block.png");
	textureHandlePlayer_ = TextureManager::Load("./Resources/player/player.png");
	textureHandleEnemy_ = TextureManager::Load("./Resources/enemy/enemy.png");
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	// player_model_ = Model::Create();
	// block_model_ = Model::Create();
	player_model_ = Model::CreateFromOBJ("player", true);
	enemy_model_ = Model::CreateFromOBJ("enemy", true);
	block_model_ = Model::CreateFromOBJ("block", true);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの生成
	camera_.farZ = 5000.0f;
	camera_.Initialize();

	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);

	// マップチップフィールド
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	// 自キャラ
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	player_->Initialize(player_model_, textureHandlePlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// 敵
	for (int i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(5 + i * 2, 18);
		newEnemy->Initialize(enemy_model_, textureHandleEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// カメラコントローラー
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
	CameraController::Rect movableCameraArea = {0.0f, 100.0f, 0.0f, 100.0f};
	cameraController_->SetMovablearea(movableCameraArea);
}

void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	//
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransformBlock = new WorldTransform();
				worldTransformBlock->Initialize();
				worldTransformBlocks_[i][j] = worldTransformBlock;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			} else {
				worldTransformBlocks_[i][j] = nullptr;
			}
		}
	}
}

void GameScene::CheckAllCollisions() {
	Player::AABB aabb1;
	Enemy::AABB aabb2;
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		aabb2 = enemy->GetAABB();
		if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x && aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) {
			// hit!
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
}

void GameScene::Update() {

	// 天球の更新
	skydome_->Update();

	// 自キャラの更新
	player_->Update();

	// 敵の更新
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	CheckAllCollisions();
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			// アフィン変換行列の生成

			worldTransformBlock->matWorld_ = worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

	// デバッグカメラの更新
	debugCamera_->Update();

#ifdef _DEBUG
	// デバッグカメラの有効/無効切り替え Cキーを押すたびに切り替える
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラが有効な場合
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロジェクション行列を転送
		camera_.TransferMatrix();
	} else {
		// ⬇️ 通常時：追従カメラコントローラーの更新
		cameraController_->Update();

		// コントローラー内のカメラ行列を、GameSceneのメインカメラに反映させる
		camera_.matView = cameraController_->GetCamera().matView;
		camera_.matProjection = cameraController_->GetCamera().matProjection;

		// 行列を定数バッファに転送
		camera_.TransferMatrix();
	}
}

void GameScene::Draw() {
	// スプライト描画前処理
	Sprite::PreDraw();

	//  自キャラの描画
	player_->Draw();

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// スプライト描画後処理
	Sprite::PostDraw();

	// 3Dモデルの描画前処理
	Model::PreDraw();

	// 天球の描画
	skydome_->Draw(camera_);

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			block_model_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 3Dモデル描画後処理
	Model::PostDraw();
}