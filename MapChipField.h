#pragma once
#include "KamataEngine.h"

enum class MapChipType {
	kBlank, // 空
	kBlock, // ブロック
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下端
		float top;    // 上端
	};

	
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);

	MapChipData mapChipData_;

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	//1ブロックのサイズ
	static inline float kBlockWidth = 1.0f;
	static inline float kBlockHeight = 1.0f;

	static inline const uint32_t kNumBlockVertical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
		
	IndexSet GetMapIndexSetByPosition(const KamataEngine::Vector3& position);
	uint32_t GetNumBlockVirtical() const { return kNumBlockVertical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	public:
	void SetMapChipTypeByIndex(uint32_t x, uint32_t y, MapChipType type) {
		if (x < kNumBlockHorizontal && y < kNumBlockVertical) {
			mapChipData_.data[y][x] = type;
		}
	}
};
