#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>

#pragma comment(lib,"xaudio2.lib")

class Audio
{
private://エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private://サブクラス(カプセル化)
	//チャンクヘッダ
	struct ChunkHeader
	{
		char id[4];		//チャンク用のID
		int32_t size;	//チャンクサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[4];		//"WAVE"
	};

	//Formatチャンク
	struct FormatChunk
	{
		ChunkHeader chunk;	//"fmt"
		WAVEFORMATEX fmt;	//波形フォーマット
	};
public://サブクラス(共通)
	//サウンドデータ
	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		BYTE* pBuffer;
		//バッファのサイズ
		unsigned int bufferSize;
	};

public://メンバ関数
	
	//初期化
	void Initialize();

	//サウンド読み込み
	SoundData SoundLordWave(const char* filename);

	//終了処理(xAudio2の開放->各種音声データを開放するように！)
	void Finalize();

private://メンバ変数

	ComPtr<IXAudio2> xaudio2;

	IXAudio2MasteringVoice* masterVoice;
};
