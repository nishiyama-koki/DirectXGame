#pragma once
#include "KamataEngine.h"
#include <array>

class Player;
class GameScene;
class MapChipField; // 前方宣言

class Enemy {
public:
	// ビヘイビア（行動状態）
	enum class Behavior {
		kRoot,    // 通常状態
		kDeath,   // デス演出状態
		kUnknown, // 不明
	};

	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

	enum class LRDirection {
		kRight,
		kLeft,
	};

	struct CollisionMapInfo {
		bool isWallTouch_ = false;
		KamataEngine::Vector3 moveAmount = {};
	};

	struct AABB {
		KamataEngine::Vector3 min;
		KamataEngine::Vector3 max;
	};

	void Initialize(KamataEngine::Model* model, uint32_t textureHandleEnemy, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	bool IsEnemyDead() const { return isEnemyDead_; }
	bool IsCollisionDisabled() const { return isCollisionDisabled_; }

	KamataEngine::Vector3 GetWorldPosition();
	AABB GetAABB();
	void OnCollision(const Player* player);

	// 各ビヘイビアの初期化・更新
	void BehaviorRootInitialize();
	void BehaviorRootUpdate();
	void BehaviorDeathInitialize();
	void BehaviorDeathUpdate();

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	// マップチップフィールドのセット
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:
	// 歩行の速さ
	static inline const float kWalkSpeed = 0.02f;
	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 向き
	LRDirection lrDirection_ = LRDirection::kLeft;

	// 死亡フラグ
	bool isEnemyDead_ = false;

	// 最初の角度
	static inline const float kWalkMotionAngleStart = 1.0f;
	static inline const float kWalkMotionAngleEnd = 10.0f;
	static inline const float kWalkMotionTime = 0.2f;
	float walkTimer_ = 0.0f;

	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.01f;

	// デス演出用の変数
	float deathTimer_ = 0.0f;
	static inline const float kDeathDuration = 0.5f; // デス演出の長さ（秒単位）
	bool isCollisionDisabled_ = false;               // デス演出中は衝突判定を無効化するフラグ

	// イージング用の初期回転角保存用
	float deathStartRotationX_ = 0.0f;
	float deathStartRotationY_ = 0.0f;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandleEnemy_ = 0;
	KamataEngine::Camera* camera_ = nullptr;

	// ビヘイビア関連
	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	GameScene* gameScene_ = nullptr;

	// マップ当たり判定関連
	MapChipField* mapChipField_ = nullptr;
	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);
};