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
}

void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");
	sprite_ = Sprite::Create(textureHandle_, {100, 50});	
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	player_model_ = Model::Create();
	block_model_ = Model::Create();	

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの生成
	camera_.farZ = 5000.0f;
	camera_.Initialize();


	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);



	// 自キャラ
	player_ = new Player();
	player_->Initialize(player_model_, textureHandle_, &camera_);

	// マップチップフィールド
	mapChipField_ = new MapChipField();
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();
	

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
	
}

void GameScene::GenerateBlocks() {
	//要素数
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

void GameScene::Update() {

	// 天球の更新
	skydome_->Update();

	// 自キャラの更新
	player_->Update();

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

	//カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// 通常カメラのビュー行列をカメラに転送する
		camera_.TransferMatrix();
	} else {
		//ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

}

void GameScene::Draw() {
	// スプライト描画前処理
	Sprite::PreDraw();

	// sprite_->Draw();
	//  自キャラの描画
	player_->Draw();

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