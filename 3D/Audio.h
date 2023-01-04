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

public://メンバ関数
	
	//初期化
	void Initialize();

	//更新

private://メンバ変数

	ComPtr<IXAudio2> xaudio2;

	IXAudio2MasteringVoice* masterVoice;
};
