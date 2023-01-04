#include "Audio.h"

void Audio::Initialize()
{
	HRESULT result;

	//XAudioのインスタンスを生成
	result = XAudio2Create(&xaudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

	//マスターボイスを生成
	result = xaudio2->CreateMasteringVoice(&masterVoice);

}