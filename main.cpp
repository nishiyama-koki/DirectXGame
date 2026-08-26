#include "ClearScene.h"
#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;
ClearScene* clearScene = nullptr;

enum class Scene {

	kUnknown = 0,

	kTitle,
	kGame,
	kClear,
};

Scene scene = Scene::kUnknown;

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->isFinished()) {
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// ボスを撃破してクリアした場合
			if (gameScene->IsClear()) {
				scene = Scene::kClear;
				delete gameScene;
				gameScene = nullptr;
				clearScene = new ClearScene();
				clearScene->Initialize();
			}
			// ゲームオーバーの場合タイトルへ戻る
			else {
				scene = Scene::kTitle;
				delete gameScene;
				gameScene = nullptr;
				titleScene = new TitleScene();
				titleScene->Initialize();
			}
		}
		break;
	case Scene::kClear:
		if (clearScene->IsFinished()) {
			scene = Scene::kTitle;
			delete clearScene;
			clearScene = nullptr;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	case Scene::kClear:
		clearScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	case Scene::kClear:
		clearScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"GC2A_03_ニシヤマ_コウキ_GYOGYOGYO!!");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	scene = Scene::kTitle;

	titleScene = new TitleScene();
	// タイトルシーンの初期化
	titleScene->Initialize();

	// ImGuiManagerインスタンスの取得
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// ImGui受付開始---------------------------
		imguiManager->Begin();

		// タイトルシーンの更新=====================

		ChangeScene();

		UpdateScene();

		// ImGui受付終了--------------------------
		imguiManager->End();

		// 描画開始______________________________
		dxCommon->PreDraw();

		// タイトルシーンの描画===================

		DrawScene();

		// 軸方向の表示
		AxisIndicator::GetInstance()->Draw();

		// ImGuiの描画---------------------------
		imguiManager->Draw();

		// 描画終了____________________________
		dxCommon->PostDraw();
	}

	// タイトルシーンの開放
	delete titleScene;
	delete gameScene;
	// nullptrの代入
	titleScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
