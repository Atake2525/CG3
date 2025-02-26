#include <xaudio2.h>
#include <fstream>
#include <wrl.h>

#pragma once

#pragma comment(lib, "xaudio2.lib")

// 音声データ
struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};

class Audio {
public:

	// 初期化
	void Initialize();

	// 終了処理
	void Finalize();
	
	// 音声読み込み
	SoundData SoundLoadWave(const char* filename);

	// 音声再生
	void SoundPlayWave(const SoundData& soundData);

	// 音声データ解放
	void SoundUnload(SoundData* soundData);

private:

	// audio test
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
};