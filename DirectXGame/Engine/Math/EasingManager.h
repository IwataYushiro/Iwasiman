#pragma once
#include "WinApp.h"
#include <chrono>

class EasingManager
{
public:
	float start;
	float end;
	float maxtime;
public:
	EasingManager();
	EasingManager(float start, float end, float time);
	float num_X = 0.0f;//返す用

public://関数
	void Standby() { startCount = std::chrono::steady_clock::now(); }

	//イージング関数
	// sine
	float ease_in_sine(float time, float startpos, float differencepos, float totaltime);
	float ease_out_sine(float time, float startpos, float differencepos, float totaltime);
	float ease_in_out_sine(float time, float startpos, float differencepos, float totaltime);

	// quad
	
	// cubic
	float ease_in_cubic(float time, float startpos, float differencepos, float totaltime);
	float ease_out_cubic(float time, float startpos, float differencepos, float totaltime);
	float ease_in_out_cubic(float time, float startpos, float differencepos, float totaltime);
	
	// cric
	float ease_in_out_cric(float time, float startpos, float differencepos, float totaltime);

public:
	//時間計測
	std::chrono::steady_clock::time_point startCount;	//開始時間
	std::chrono::steady_clock::time_point nowCount;		//現在時間
	std::chrono::microseconds elapsedCount;				//経過時間 経過時間=現在時間-開始時間

	//イージングプロパティ
	float t;					//時間
	float b;					//開始位置
	float c;					//開始位置-終了位置の差
	float d;					//合計時間

public://定数
	const float PI = 3.141592f;

};

