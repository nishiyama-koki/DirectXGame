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
	delete attack_model_;
	delete player_model_;
	delete player_;
	delete enemy_model_;
	delete hitEffectModel_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	for (HitEffect* hitEffect : hiteEffects_) {
		delete hitEffect;
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
	delete deathParticles_;
	delete skydome_;
	delete boss_model_;      
	delete bossBulletModel_; 
	delete bubble_model_;
	for (Bubble* bubble : bubbles_) {
		delete bubble;
	}
	bubbles_.clear();
}

void GameScene::Initialize() {

	phase_ = Phase::kFadeIn;

	// ファイル名を指定してテクスチャを読み込む
	textureHandleMoveKeys_ = TextureManager::Load("./Resources/bg_keys/move_keys.png");
	textureHandleBlock_ = TextureManager::Load("./Resources/block/block.png");
	textureHandlePlayer_ = TextureManager::Load("./Resources/player/player.png");
	textureHandlePlayerHP_ = TextureManager::Load("./Resources/player/hp.png");
	textureHandleAttack_ = TextureManager::Load("./Resources/hit_effect/hit_effect.png");
	textureHandleEnemy_ = TextureManager::Load("./Resources/enemy/enemy.png");
	textureHandleHitEffect_ = TextureManager::Load("./Resources/hitEffect/hit_effect.png");
	textureHandleBoss_ = TextureManager::Load("./Resources/boss/boss.png");
	textureHandleBossBullet_ = TextureManager::Load("./Resources/bossBullet/bossBullet.png");
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
	spriteMoveKeys_ = Sprite::Create(textureHandleMoveKeys_, {80, 560});
	for (int i = 0; i < 3; ++i) { // プレイヤーの最大HP数（例: 3）
		KamataEngine::Sprite* hpSprite = Sprite::Create(textureHandlePlayerHP_, {0.0f, 0.0f});
		hpSprites_.push_back(hpSprite);
	}
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	player_model_ = Model::CreateFromOBJ("player", true);
	// プレイヤーの攻撃エフェクト
	attack_model_ = Model::CreateFromOBJ("hit_effect", true);
	enemy_model_ = Model::CreateFromOBJ("enemy", true);
	boss_model_ = Model::CreateFromOBJ("boss", true);
	block_model_ = Model::CreateFromOBJ("block", true);
	particleModel_ = Model::CreateFromOBJ("deathParticle", true);
	// 攻撃当てた時のモデル
	hitEffectModel_ = Model::CreateFromOBJ("hitEffect", true);

	bossBulletModel_ = Model::CreateFromOBJ("bossBullet", true);

	textureHandleBubble_ = TextureManager::Load("./Resources/bubble/bubble.png");
	bubble_model_ = Model::CreateFromOBJ("bubble", true);

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
	player_->InitializeAttackEffect(attack_model_, textureHandleAttack_);

	// 敵
	std::vector<EnemySpawnPoint> enemySpawnPoints = {
	    {5,  15}, // 1体目の位置
	    {16, 11}, // 2体目の位置
	    {18, 15}, // 3体目の位置
	    {25, 18}, // 4体目の位置
	    {30, 14}, // 5体目の位置
	    {40, 18}, // 6体目の位置
	    {48, 17}, // 7体目の位置
	    {55, 18}, // 8体目の位置
	    {62, 15}, // 9体目の位置
	    {68, 18}, // 10体目の位置
	};
	for (int i = 0; i < 10; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(enemySpawnPoints[i].x, enemySpawnPoints[i].y);
		newEnemy->Initialize(enemy_model_, textureHandleEnemy_, &camera_, enemyPosition);
		newEnemy->SetMapChipField(mapChipField_);
		enemies_.push_back(newEnemy);
	}

	//ボス
	boss_ = new Boss();
	Vector3 bossPosition = mapChipField_->GetMapChipPositionByIndex(90,18);
	boss_->Initialize(boss_model_, textureHandleBoss_, &camera_, bossPosition);
	boss_->SetPlayer(player_);
	boss_->SetBulletModel(bossBulletModel_, textureHandleBossBullet_);

	deathParticles_ = new DeathParticles();
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);



	// ヒットエフェクト
	HitEffect::SetModel(hitEffectModel_);
	HitEffect::SetCamera(&camera_);

	for (Enemy* enemy : enemies_) {
		enemy->SetGameScene(this);
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
void GameScene::CreateVerticalWall(uint32_t mapIndexX) {
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVirtical();

	// インデックスがマップの範囲外なら何もしない
	if (mapIndexX >= mapChipField_->GetNumBlockHorizontal()) {
		return;
	}

	for (uint32_t i = 0; i < numBlockVertical; ++i) {
		// すでにブロックが存在している場所はスキップ（既存ブロックの上書き防止）
		if (worldTransformBlocks_[i][mapIndexX] != nullptr) {
			continue;
		}

		// 1. マップチップデータ上もブロック扱い（kBlock）に変更
		// ※MapChipField側に Setter が無い場合は、MapChipFieldクラスに SetMapChipType(x, y, type) を追加してください
		mapChipField_->SetMapChipTypeByIndex(mapIndexX, i, MapChipType::kBlock);

		// 2. 描画用の WorldTransform を動的生成
		WorldTransform* worldTransformBlock = new WorldTransform();
		worldTransformBlock->Initialize();
		worldTransformBlock->translation_ = mapChipField_->GetMapChipPositionByIndex(mapIndexX, i);

		// 配列に格納
		worldTransformBlocks_[i][mapIndexX] = worldTransformBlock;
	}
}

void GameScene::SpawnBubble() {
	if (!player_)
		return;
	Vector3 spawnPos = player_->GetWorldPosition();
	spawnPos.x += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 3.0f; // 左右に少しバラけさせる
	spawnPos.y += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 1.0f;

	Bubble* newBubble = new Bubble();
	newBubble->Initialize(bubble_model_, textureHandleBubble_, &camera_, spawnPos);
	bubbles_.push_back(newBubble);
}


bool IsCollisionAABB(const Player::AABB& a, const BossBullet::AABB& b) {
	if (a.max.x < b.min.x || a.min.x > b.max.x)
		return false;
	if (a.max.y < b.min.y || a.min.y > b.max.y)
		return false;
	return true;
}

void GameScene::CheckAllCollisions() {
	if (!player_ || !boss_)
		return;
	if (player_->isDead())
		return;

	// プレイヤーのAABBを取得
	Player::AABB playerAABB = player_->GetAABB();


	// 2. 敵（Enemy）との当たり判定
	Player::AABB aabb1;
	Enemy::AABB aabb2;
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		if (enemy->IsCollisionDisabled())
			continue;
		aabb2 = enemy->GetAABB();
		if (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x && aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) {
			// hit!
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}

	if (boss_->IsAppeared() && !boss_->IsDead() && !boss_->IsDying()) {
		// 当たり判定処理

		// 1. ボスの弾リストを取得して判定
		const auto& bullets = boss_->GetBullets();
		for (const auto& bullet : bullets) {
			if (bullet->IsDead())
				continue;

			BossBullet::AABB bulletAABB = bullet->GetAABB();

			// AABB交差判定
			if (IsCollisionAABB(playerAABB, bulletAABB)) {
				// プレイヤーが無敵でない時だけ弾を消去
				if (!player_->IsInvincible()) {
					bullet->OnCollision(); // 弾を消す
				}
				player_->OnCollision(); // プレイヤーにダメージ
			}
		}
		// 3. ボス本体との当たり判定（通常接触 ＆ 突進攻撃判定）
		if (boss_->IsAppeared() && !boss_->IsDead()) { // ボスが出現中かつ生きている場合
			auto bossAABB = boss_->GetAABB();

			if (aabb1.min.x <= bossAABB.max.x && aabb1.max.x >= bossAABB.min.x && aabb1.min.y <= bossAABB.max.y && aabb1.max.y >= bossAABB.min.y) {

				// 【パターンA】 プレイヤーが突進攻撃中の場合 -> ボスにダメージ
				if (player_->IsAttacking()) { // ※突進判定用関数（環境に合わせて変更してください）
					if (!boss_->IsInvincible()) {
						boss_->TakeDamage(1);                         // ボスにダメージ
						CreateHitEffect(player_->GetWorldPosition()); // ヒットエフェクト生成
					}
				}
				// 【パターンB】 通常接触の場合 -> プレイヤーにダメージ
				else {
					player_->OnCollision();
				}
			}
		}
	}

}

void GameScene::CreateEffect(const KamataEngine::Vector3& position) {
	// HitEffect の生成
	HitEffect* effect = HitEffect::Create(position);
	hiteEffects_.push_back(effect);
}

void GameScene::CreateHitEffect(const KamataEngine::Vector3& position) {
	HitEffect* newHitEffect = HitEffect::Create(position);
	hiteEffects_.push_back(newHitEffect);
}



void GameScene::Update() {
	ChangePhase();
	if (fade_) {
		fade_->Update();
	}


	switch (phase_) {
	case Phase::kFadeIn:
#pragma region kFadeIn
		// 天球の更新
		skydome_->Update();
		player_->Update();
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		for (HitEffect* hitEffect : hiteEffects_) {
			hitEffect->Update();
		}

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
				worldTransformBlock->TransferMatrix();
			}
		}
		if (isDebugCameraActive_) {
			debugCamera_->Update();
			camera_.matView = debugCamera_->GetCamera().matView;
			camera_.matProjection = debugCamera_->GetCamera().matProjection;
		} else {
			cameraController_->Update();
			camera_.matView = cameraController_->GetCamera().matView;
			camera_.matProjection = cameraController_->GetCamera().matProjection;
		}
		camera_.TransferMatrix();
		break;
#pragma endregion

	case Phase::kPlay:
#pragma region kPlay
		// 天球の更新
		skydome_->Update();

		// 自キャラの更新
		player_->Update();

		for (HitEffect* hitEffect : hiteEffects_) {
			hitEffect->Update();
		}

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		CheckAllCollisions();
		enemies_.remove_if([](Enemy* enemy) {
			if (enemy->IsEnemyDead()) {
				delete enemy;
				return true;
			}
			return false;
		});

		

		// プレイヤーが特定位置に到達 & ボス未出現の場合
		if (player_->GetWorldPosition().x >= kBossTriggerX && !boss_->IsAppeared()) {
			// 1. ボスの登場開始
			boss_->StartAppearance();
			CameraController::Rect bossArea = {80.0f, 100.0f, 0.0f, 100.0f};
			cameraController_->SetMovablearea(bossArea);
			CreateVerticalWall(69);
		}

		// ボス・プレイヤー等の更新
		player_->Update();
		
			boss_->Update();
		

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
		// 泡の定期生成
		bubbleSpawnTimer_ += 1.0f / 60.0f;
		if (bubbleSpawnTimer_ >= kBubbleSpawnInterval) {
			bubbleSpawnTimer_ = 0.0f;
			SpawnBubble();
		}

		// 泡の更新処理
		for (Bubble* bubble : bubbles_) {
			bubble->Update();
		}

		// 寿命が尽きた泡の自動削除
		bubbles_.remove_if([](Bubble* bubble) {
			if (bubble->IsDead()) {
				delete bubble;
				return true;
			}
			return false;
		});

		// デバッグカメラの更新
		debugCamera_->Update();
#ifdef _DEBUG
		// デバッグカメラの切り替え Cキー
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
		break;
#pragma endregion

	case Phase::kDeath:
#pragma region kDeath
		// 天球の更新
		skydome_->Update();
		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		if (!deathParticles_->isFinished_) {
			deathParticles_->Update();
		}
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

		camera_.TransferMatrix();
		break;
#pragma endregion
	case Phase::kClear:
		skydome_->Update();
		camera_.TransferMatrix();
		break;
	case Phase::kFadeOut:
#pragma region kFadeOut
		skydome_->Update();
		camera_.TransferMatrix();
		break;
#pragma endregion
	}
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_ && fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;

	case Phase::kPlay:
		if (player_->isDead()) {
			phase_ = Phase::kDeath;
			const Vector3& playerPositionForParticles = player_->GetWorldPosition();
			deathParticles_->Initialize(particleModel_, &camera_, playerPositionForParticles);
		} else if (boss_->IsDead()) {
			phase_ = Phase::kClear;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;

	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->isFinished_) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}
		break;
	case Phase::kClear:
		if (fade_ && fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	case Phase::kFadeOut:
		if (fade_ && fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

void GameScene::Draw() {

	Model::PreDraw();
	skydome_->Draw(camera_);
	Model::PostDraw();

	// スプライト描画前処理
	Sprite::PreDraw();

	for (Bubble* bubble : bubbles_) {
		bubble->Draw();
	}

	if (phase_ == Phase::kPlay || phase_ == Phase::kFadeIn) {
		player_->Draw();
	}

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	 boss_->Draw();

	for (HitEffect* hitEffect : hiteEffects_) {
		hitEffect->Draw();
	}

	// スプライト描画後処理
	Sprite::PostDraw();

	// 3Dモデルの描画前処理
	Model::PreDraw();

	if (phase_ == Phase::kDeath || phase_ == Phase::kFadeOut) {
		if (!deathParticles_->isFinished_) {
			deathParticles_->Draw();
		}
	}

	// 天球の描画
	// skydome_->Draw(camera_);

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

	Sprite::PreDraw();
	if (phase_ == Phase::kPlay || phase_ == Phase::kFadeIn) {
		if (player_) {
			int currentHp = player_->GetHp();
			// 表示の起点となる座標とアイコンの間隔
			KamataEngine::Vector2 basePosition = {40.0f, 40.0f}; // 画面左上の基準位置
			float iconSpacing = 50.0f;                           // アイコンごとの横の間隔(px)
			for (int i = 0; i < currentHp; ++i) {
				if (i < hpSprites_.size() && hpSprites_[i]) {
					KamataEngine::Vector2 pos = {basePosition.x + (i * iconSpacing), basePosition.y};
					hpSprites_[i]->SetPosition(pos);
					hpSprites_[i]->Draw();
				}
			}
		}
	}
	spriteMoveKeys_->Draw();
	if (fade_) {
		fade_->Draw();
	}
	Sprite::PostDraw();
}