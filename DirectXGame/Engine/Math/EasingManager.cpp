#include "EasingManager.h"
#include <cmath>

EasingManager::EasingManager()
{
	startCount = std::chrono::steady_clock::now();	//開始時間
	nowCount = std::chrono::steady_clock::now();		//現在時間

	//右から左のサンプル
	this->start = (float)WinApp::GetInstance()->window_width;
	this->end = 0.0f;
	this->maxtime = 1.0f;

	//イージングプロパティ
	t = 0.0f;					//時間
	b = start;				//開始位置
	c = end - start;			//開始位置-終了位置の差
	d = maxtime;				//合計時間

}

EasingManager::EasingManager(float start, float end, float time)
{
	startCount = std::chrono::steady_clock::now();	//開始時間
	nowCount = std::chrono::steady_clock::now();		//現在時間

	this->start = start;
	this->end = end;
	this->maxtime = time;

	//イージングプロパティ
	t = 0.0f;					//時間
	b = start;				//開始位置
	c = end - start;			//開始位置-終了位置の差
	d = time;				//合計時間

}


float EasingManager::ease_in_cubic(float time, float startpos, float differencepos, float totaltime)
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	t = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(time / totaltime, 1.0f);

	float v = x * x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_cric(float time, float startpos, float differencepos, float totaltime)
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	t = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(time / totaltime, 1.0f);

	float v = x < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f :
		(sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}
