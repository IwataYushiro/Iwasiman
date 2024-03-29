#include "Audio.h"
#include <cassert>
#include <string.h>

#pragma comment(lib,"xaudio2.lib")
using namespace IwasiEngine;
/*

*	Audio.cpp

*	音声(オーディオ)

*/

Audio* Audio::GetInstance()
{
	//インスタンス取得
	static Audio instance;
	return &instance;
}

//初期化
void Audio::Initialize()
{
	HRESULT result;

	//XAudioのインスタンスを生成
	result = XAudio2Create(&xaudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);

	//マスターボイスを生成

	IXAudio2MasteringVoice* masterVoice;     

	result = xaudio2_->CreateMasteringVoice(&masterVoice);

}

//サウンド読み込み
Audio::SoundData Audio::SoundLoadWave(const char* filename)
{
	//ファイルオープン
	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗の検出
	assert(file.is_open());

	//.wavデータ読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", INW_Num) != 0)
	{
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", INW_Num) != 0)
	{
		assert(0);
	}
	
	//formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", INW_Num) != 0)
	{
		assert(0);
	}
	//チャンク本体の読み込み 
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	
	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", INW_Num) == 0)
	{
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", INW_Num) != 0)
	{
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	//ファイルクローズ
	file.close();

	//読み込んだ音声データをreturn
	//returnする為の音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

//音声再生
void Audio::SoundPlayWave(IXAudio2* xAudio2, const Audio::SoundData& soundData, const bool loop)
{
	HRESULT result;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの生成
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (loop)
	{
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

//各種音声データの開放
void Audio::SoundUnLoad(SoundData* soundData)
{
	//バッファのメモリを開放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

//終了処理(xAudio2の開放→各種音声データを開放するように！)
void Audio::Finalize()
{
	xaudio2_.Reset();
}
