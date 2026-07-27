#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "MapChipField.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "Fade.h"
#include <vector>



// ゲームシーン
class GameScene {
public:
	GameScene();
	~GameScene();

	enum class Phase { 
		kFadeIn,
		kPlay,
	    kDeath,
		kFadeOut,
	};

	Phase phase_;

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	bool isFinished() const { return finished_; }

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	uint32_t textureHandleBlock_ = 0;
	uint32_t textureHandlePlayer_ = 0;
	uint32_t textureHandleAttack_ = 0;
	uint32_t textureHandleEnemy_ = 0;
	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;
	// 3Dモデル
	KamataEngine::Model* player_model_ = nullptr;
	KamataEngine::Model* attack_model_ = nullptr;
	//敵モデル
	KamataEngine::Model* enemy_model_ = nullptr;
	std::list<Enemy*> enemies_;

		// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;

	//ブロック
	KamataEngine::Model* block_model_ = nullptr;

	//ワールドトランスフォーム
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;
	
	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	//デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	//天球
	KamataEngine::Model* modelSkydome_ = nullptr;

	KamataEngine::Model* particleModel_ = nullptr;

	bool finished_ = false;

	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	CameraController* cameraController_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;
	Fade* fade_ = nullptr;
};