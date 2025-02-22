#include "Input.h"
#include <cassert>
#include "WinApp.h"


void Input::Initialize(WinApp* winApp) {
	winApp_ = winApp;
	HRESULT result;
	// DirectInputの初期化
	// DirectInputのインスタンス生成
	ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));
	// キーボードデバイスの生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	// 入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	// 排他制御レベルセット
	result = keyboard->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update() {
	HRESULT result;

	// 前回のキー入力を保存
	memcpy(keyPre, key, sizeof(key));
	// キーボード情報の取得開始
	result = keyboard->Acquire();
	// 全キーの入力情報を取得する
	result = keyboard->GetDeviceState(sizeof(key), key);

}

bool Input::PushKey(BYTE keyNumber) { 
	if (key[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {
	if (key[keyNumber] && !keyPre[keyNumber]) {
		return true;
	}
	return false;
}

bool Input::ReturnKey(BYTE keyNumber) {
	if (!key[keyNumber] && keyPre[keyNumber]) {
		return true;
	}
	return false;
}