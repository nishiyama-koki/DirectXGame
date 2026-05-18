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
	
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);

	MapChipData mapChipData_;

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	//1ブロックのサイズ
	static inline float kBlockWidth = 2.0f;
	static inline float kBlockHeight = 2.0f;

	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
		
	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

};
