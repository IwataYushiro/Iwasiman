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
	void Standby(bool reverse);

	//イージング関数
	// sine
	float ease_in_sine();
	float ease_out_sine();
	float ease_in_out_sine();
	// quad
	float ease_in_quad();
	float ease_out_quad();
	float ease_in_out_quad();
	// cubic
	float ease_in_cubic();
	float ease_out_cubic();
	float ease_in_out_cubic();
	// quart
	float ease_in_quart();
	float ease_out_quart();
	float ease_in_out_quart();
	// quint
	float ease_in_quint();
	float ease_out_quint();
	float ease_in_out_quint();
	// expo
	float ease_in_expo();
	float ease_out_expo();
	float ease_in_out_expo();
	// circ
	float ease_in_circ();
	float ease_out_circ();
	float ease_in_out_circ();
	// back
	float ease_in_back();
	float ease_out_back();
	float ease_in_out_back();
	// elastic
	float ease_in_elastic();
	float ease_out_elastic();
	float ease_in_out_elastic();
	// bounce
	float ease_in_bounce();
	float ease_out_bounce();
	float ease_in_out_bounce();

public:
	//時間計測
	std::chrono::steady_clock::time_point startCount;	//開始時間
	std::chrono::steady_clock::time_point nowCount;		//現在時間
	std::chrono::microseconds elapsedCount;				//経過時間 経過時間=現在時間-開始時間

	//イージングプロパティ
	float timeNow;					//時間
	float startpos;					//開始位置
	float differencepos;					//開始位置-終了位置の差
	float totaltime;					//合計時間

	bool isReverse = false;
public://定数
	const float PI = 3.141592f;

};

