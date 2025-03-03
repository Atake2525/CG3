#include "FrameWork.h"

void FrameWork::Initialize() {}

void FrameWork::Update() {}

void FrameWork::Finalize() {}

void FrameWork::Run() {
	// 初期化
	Initialize();

	while (true) {
		// 更新
		Update();
		// ループ脱出
		if (RoopOut()) {
			break;
		}
		// 描画
		Draw();
	}
	// 終了処理
	Finalize();
}