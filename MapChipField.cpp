#include "MapChipField.h"
#include <fstream>
#include <map>
#include <sstream>

namespace {

std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};

}

void MapChipField::ResetMapChipData() {

	// マップチップデータをリセット
	mapChipData_.data.clear();
	mapChipData_.data.resize(MapChipField::kNumBlockVertical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// マップチップデータをリセット
	ResetMapChipData();
	// ファイルを開く
	std::ifstream file;
	file.open(filePath);
	//assert(file.is_open());

	// マップチップcsv
	std::stringstream mapChipCsv;
	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();
	// ファイルを閉じる
	file.close();

	// CSVからマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVertical; ++i) {
		std::string line;
		getline(mapChipCsv, line);

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			std::getline(lineStream, word, ',');

			if (mapChipTable.contains(word)) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVertical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}

KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return KamataEngine::Vector3(xIndex * kBlockWidth + kBlockWidth / 2.0f, (kNumBlockVertical - 1 - yIndex) * kBlockHeight + kBlockHeight / 2.0f, 0.0f);
}

MapChipField::IndexSet MapChipField::GetMapIndexSetByPosition(const KamataEngine::Vector3& position) {
	MapChipField::IndexSet indexSet = {};
	int xIndex = static_cast<int>(position.x / kBlockWidth);
	int yIndex = static_cast<int>((kBlockHeight * kNumBlockVertical - position.y) / kBlockHeight);
	if (xIndex < 0)
		xIndex = 0;
	if (xIndex >= static_cast<int>(kNumBlockHorizontal))
		xIndex = kNumBlockHorizontal - 1;
	if (yIndex < 0)
		yIndex = 0;
	if (yIndex >= static_cast<int>(kNumBlockVertical))
		yIndex = kNumBlockVertical - 1;
	indexSet.xIndex = static_cast<uint32_t>(xIndex);
	indexSet.yIndex = static_cast<uint32_t>(yIndex);
	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex,uint32_t yIndex) { 
	KamataEngine::Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	MapChipField::Rect rect;

	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;

}