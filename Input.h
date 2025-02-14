#define DIRECTIONPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>
#include <wrl.h>
#include <Windows.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#pragma once

class WinApp;

class Input {
public:
	// namespcae省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	void Initialize(WinApp* winApp);
	void Update();

public:
	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber">キー番号 例(DIK_0)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	bool TriggerKey(BYTE keyNumber);

	bool ReturnKey(BYTE keyNumber);

private:
	// キーボードデバイス
	ComPtr<IDirectInputDevice8> keyboard;

	WinApp* winApp_ = nullptr;

	// 全キーの状態
	BYTE key[256] = {};
	// 前回の全キーの状態
	BYTE keyPre[256] = {};
};
