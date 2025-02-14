#include <Windows.h>
#include <cstdint>

#pragma comment(lib, "winmm.lib")
#pragma once

class WinApp {
public:
	static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	void Update();

	// 終了
	void Finalize();

	// メッセージの処理
	bool ProcessMessage();

	// クライアント領域サイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	// getter
	HWND GetHwnd() const { return hwnd; }
	HINSTANCE GetHInstance() const { return wc.hInstance; }

private:
	// ウィンドウハンドル
	HWND hwnd = nullptr;

	// ウィンドウクラスの設定
	WNDCLASS wc{};
};
