#include <xaudio2.h>
#include <fstream>
#include <wrl.h>

#pragma once

#pragma comment(lib, "xaudio2.lib")

// �����f�[�^
struct SoundData {
	// �g�`�t�H�[�}�b�g
	WAVEFORMATEX wfex;
	// �o�b�t�@�̐擪�A�h���X
	BYTE* pBuffer;
	// �o�b�t�@�̃T�C�Y
	unsigned int bufferSize;
};

class Audio {
public:

	// ������
	void Initialize();

	// �I������
	void Finalize();
	
	// �����ǂݍ���
	SoundData SoundLoadWave(const char* filename);

	// �����Đ�
	void SoundPlayWave(const SoundData& soundData);

	// �����f�[�^���
	void SoundUnload(SoundData* soundData);

private:

	// audio test
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
};