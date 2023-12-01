#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>

/*

*	Audio.h

*	音声(オーディオ)

*/
class Audio final
{
private://エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	//チャンクIDの数
	enum IdNumWav
	{
		INW_Num = 4
	};

public://サブクラス
	
	//チャンクヘッダ
	struct ChunkHeader
	{
		char id[INW_Num];		//チャンク用のID
		int32_t size;	//チャンクサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[INW_Num];		//"WAVE"
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

	//サウンド読み込み(使用音声.wav)
	SoundData SoundLoadWave(const char* filename);

	//音声再生(Xaudio2、サウンドデータ、true＝ループ再生)
	void SoundPlayWave(const IXAudio2* xAudio2, const SoundData& soundData, const bool loop);

	//各種音声データの開放(サウンドデータ)
	void SoundUnLoad(SoundData* soundData);

	//終了処理(xAudio2の開放->各種音声データを開放するように！)
	void Finalize();

private://メンバ変数

	ComPtr<IXAudio2> xaudio2_;//Xaudio2
	
public://アクセッサ置き場
	//xAudio2ゲット
	IXAudio2* GetXAudio2() { return xaudio2_.Get(); }
private:
	//コンストラクタ（シングルトンパターン）
	Audio() = default;
	//デストラクタ（シングルトンパターン）
	~Audio() = default;
public:
	//コピーコンストラクタの防止（シングルトンパターン）
	Audio(const Audio& obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	Audio& operator=(const Audio& obj) = delete;
};
