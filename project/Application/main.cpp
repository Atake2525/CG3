#include "DebugMode.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	DebugMode* debugMode = nullptr;
	debugMode = new DebugMode();
	debugMode->Initialize();

	// 出ウィンドウへの文字出力
	OutputDebugStringA("Hello,DirectX!\n");


	//ゲームループ
	/*MSG msg{};*/
	//ウィンドウの×ボタンが押されるまでループ
	while (true){

		if (debugMode->RoopOut()) {
			// ループを抜ける
			break;
		}
		else
		{
			debugMode->Update();

			debugMode->Draw();
		}
	}
	
#ifdef DEBUG
	debugController->Release();
#endif // DEBUG

	debugMode->Finalize();
	delete debugMode;

	return 0;
}