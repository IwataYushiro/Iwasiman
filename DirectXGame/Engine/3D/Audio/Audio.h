#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>

//音声
class Audio
{
private://エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://サブクラス(カプセル化)
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
		ChunkHeader chunk;	//"fmt "
		WAVEFORMATEX fmt;	//波形フォーマット
	};

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
public://シングルトンインスタンス
	static Audio* GetInstance();

public://メンバ関数
	
	//初期化
	void Initialize();

	//サウンド読み込み
	SoundData SoundLoadWave(const char* filename);
	
	//音声再生
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);
	
	//各種音声データの開放
	void SoundUnLoad(SoundData* soundData);
	
	//終了処理(xAudio2の開放->各種音声データを開放するように！)
	void Finalize();

private://メンバ変数

	ComPtr<IXAudio2> xaudio2;

public://アクセッサ置き場
	//xAudio2
	IXAudio2* GetXAudio2() { return xaudio2.Get(); }
};
