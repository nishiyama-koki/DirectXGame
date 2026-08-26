#pragma once
#include "Boss.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "HitEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "Bubble.h"
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
		kClear,
	};

	struct EnemySpawnPoint {
		uint32_t x;
		uint32_t y;
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

	void CreateEffect(const KamataEngine::Vector3& position);
	void CreateHitEffect(const KamataEngine::Vector3& position);

	void CreateVerticalWall(uint32_t mapIndexX);
	bool IsClear() const { return boss_ && boss_->IsDead(); }
	bool IsFinished() const { return finished_; }

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	uint32_t textureHandleMoveKeys_ = 0;
	uint32_t textureHandleBlock_ = 0;
	uint32_t textureHandlePlayer_ = 0;
	uint32_t textureHandlePlayerHP_ = 0;
	uint32_t textureHandleAttack_ = 0;
	uint32_t textureHandleEnemy_ = 0;
	uint32_t textureHandleBoss_ = 0;
	uint32_t textureHandleHitEffect_ = 0;
	uint32_t textureHandleBossBullet_ = 0;
	// スプライト
	KamataEngine::Sprite* sprite_ = nullptr;
	KamataEngine::Sprite* spriteMoveKeys_ = nullptr;
	// 3Dモデル
	KamataEngine::Model* player_model_ = nullptr;
	KamataEngine::Model* attack_model_ = nullptr;
	std::vector<KamataEngine::Sprite*> hpSprites_; // HPアイコン用のスプライト配列
	// 敵モデル
	KamataEngine::Model* enemy_model_ = nullptr;
	KamataEngine::Model* hitEffectModel_ = nullptr;
	std::list<Enemy*> enemies_;
	std::list<HitEffect*> hiteEffects_;

	KamataEngine::Model* boss_model_ = nullptr;
	KamataEngine::Model* bossBulletModel_ = nullptr;

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;

	// ブロック
	KamataEngine::Model* block_model_ = nullptr;

	// ワールドトランスフォーム
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	// 天球
	KamataEngine::Model* modelSkydome_ = nullptr;

	KamataEngine::Model* particleModel_ = nullptr;

	bool finished_ = false;

	const float kBossTriggerX = 80.0f;

	// 泡関連
	KamataEngine::Model* bubble_model_ = nullptr;
	uint32_t textureHandleBubble_ = 0;
	std::list<Bubble*> bubbles_;
	float bubbleSpawnTimer_ = 0.0f;
	const float kBubbleSpawnInterval = 0.2f; // 泡が発生する間隔（秒）

	void SpawnBubble(); // 泡生成関数

	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	Boss* boss_ = nullptr;
	Skydome* skydome_ = nullptr;
	MapChipField* mapChipField_ = nullptr;
	CameraController* cameraController_ = nullptr;
	DeathParticles* deathParticles_ = nullptr;
	Fade* fade_ = nullptr;
};