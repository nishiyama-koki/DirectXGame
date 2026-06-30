#pragma once
#include "KamataEngine.h"

class MapChipField;
class Enemy;

class Player {
public:
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		// 要素数
		kNumCorner
	};

	struct AABB {
		KamataEngine::Vector3 min;
		KamataEngine::Vector3 max;
	};


	void Initialize(KamataEngine::Model* model, uint32_t textureHandlePlayer, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	void OnCollision(const Enemy* enemy);

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	AABB GetAABB();
	KamataEngine::Vector3 GetWorldPosition();

	enum class LRDirection {
		kRight,
		kLeft,
	};

	void InputMove();

	struct CollisionMapInfo {
		bool isCeiling_ = false;    // 天井
		bool isWallTouch_ = false;  // 壁
		bool isFloorTouch_ = false; // 床
		KamataEngine::Vector3 moveAmount = {};
	};

	bool isDead() const { return isDead_; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandlePlayer_ = 0;

	KamataEngine::Camera* camera_ = nullptr;

	static inline const float kAcceleration = 0.02f;
	static inline const float kAttenuation = 0.05f;
	static inline const float kLimitRunSpeed = 0.3f;
	KamataEngine::Vector3 velocity_ = {};

	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回制御
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	static inline const float kGravityAcceleration = 0.01f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kJumpAcceleration = 0.3f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定サイズ
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;

	// マップ全体の衝突判定
	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
      
	void CheckCeilingCollision(const CollisionMapInfo& info); 
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	//接地状態の切り替え処理
	void SwitchGroundingState(const CollisionMapInfo& info);
	static inline const float kAttenuationLanding = 0.2f;
	static inline const float kBlank = 0.01f;
	
	bool isDead_ = false;

};