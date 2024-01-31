#include "Easing.h"
#include <cmath>
#include <cassert>
#include <sstream>
#include <fstream>
#include <Windows.h>

using namespace std;
using namespace IwasiEngine;

/*

*	Easing.cpp

*	イージング関係 Easing(float start, float end, float time);

*/

Easing::Easing()
{
	startCount_ = chrono::steady_clock::now();	//開始時間
	nowCount_ = chrono::steady_clock::now();		//現在時間

	struct DefaultNum
	{
		const float start = 0.0f;
		const float end = 1.0f;
		const float maxTime = 1.0f;
	};
	DefaultNum defaultNum;
	//0から1のサンプル
	this->start = defaultNum.start;
	this->end = defaultNum.end;
	this->maxtime = defaultNum.maxTime;

	//イージングプロパティ
	num_X = 0.0f;
	timeNow_ = 0.0f;					//時間
	startPos_ = start;				//開始位置
	differencePos_ = end - start;			//開始位置-終了位置の差
	totalTime_ = maxtime;				//合計時間
	isReverse_ = false;
}

Easing::Easing(const float start, const float end, const float time)
{
	startCount_ = chrono::steady_clock::now();	//開始時間
	nowCount_ = chrono::steady_clock::now();		//現在時間

	this->start = start;
	this->end = end;
	this->maxtime = time;

	//イージングプロパティ
	num_X = 0.0f;
	timeNow_ = 0.0f;					//時間
	totalTime_ = time;				//合計時間
	startPos_ = start;				//開始位置
	differencePos_ = end - start;	//開始位置-終了位置の差
	isReverse_ = false;
}

void Easing::Standby(bool reverse)
{
	startCount_ = chrono::steady_clock::now();
	isReverse_ = reverse;
	num_X = 0.0f;
	if (isReverse_)
	{
		startPos_ = end;				//開始位置
		differencePos_ = start - end;			//開始位置-終了位置の差
	}
	else
	{
		startPos_ = start;				//開始位置
		differencePos_ = end - start;			//開始位置-終了位置の差
	}
}


float Easing::ease_in_sine()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - cosf((x * PI) / 2.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_sine()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = sinf((x * PI) / 2.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_sine()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = -(cosf(PI * x) - 1.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_quad()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_quad()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - (1.0f - x) * (1.0f - x);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_quad()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? 2.0f * x * x : 1.0f - powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_cubic()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x * x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_cubic()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - powf(1.0f - x, 3.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_cubic()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? 4.0f * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_quart()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x * x * x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_quart()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - powf(1.0f - x, 4.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_quart()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? 8.0f * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 4.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_quint()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x * x * x * x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_quint()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - powf(1.0f - x, 5.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_quint()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? 16.0f * x * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 5.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_expo()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : powf(2.0f, 10.0f * x - 10.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_expo()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * x);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_expo()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : x < 0.5f ? powf(2.0f, 20.0f * x - 10.0f) / 2.0f
		: (2.0f - powf(2.0f, -20.0f * x + 10.0f)) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_circ()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - sqrtf(1.0f - powf(x, 2.0f));

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_circ()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = sqrtf(1.0f - powf(x - 1.0f, 2.0f));

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_circ()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f :
		(sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_back()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	const float v = c3 * x * x * x - c1 * x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;;
}

float Easing::ease_out_back()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	const float v = 1.0f + c3 * powf(x - 1.0f, 3.0f) + c1 * pow(x - 1.0f, 2.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;;
}

float Easing::ease_in_out_back()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;
	const float v = x < 0.5f
		? (powf(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f
		: (powf(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;;
}

float Easing::ease_in_elastic()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float c4 = (2.0f * PI) / 3.0f;

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f :
		-powf(2.0f, 10.0f * x - 10.0f) * sinf((x * 10.0f - 10.75f) * c4);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_elastic()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float c4 = (2.0f * PI) / 3.0f;

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f :
		powf(2.0f, -10.0f * x) * sinf((x * 10.0f - 0.75f) * c4) + 1.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_elastic()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float c5 = (2.0f * PI) / 4.5f;

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : x < 0.5f ?
		-(powf(2.0f, 20.0f * x - 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f :
		(powf(2.0f, -20.0f * x + 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_bounce()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - bounceCalculation(1.0f - x);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_bounce()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = bounceCalculation(x);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_bounce()
{
	//現在時間を取得する
	nowCount_ = chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f
		? (1.0f - bounceCalculation(1.0f - 2.0f * x)) / 2.0f
		: (1.0f + bounceCalculation(2.0f * x - 1.0f)) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::bounceCalculation(float x)
{
	float v;

	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	//計算パターン
	const float cal1 = 1.0f;
	const float cal2 = 2.0f;
	const float cal3 = 2.5f;

	if (x < cal1 / d1)
	{
		const float calculation = n1 * x * x;
		v = calculation;
	}
	else if (x < cal2 / d1)
	{
		const float calculation = n1 * (x -= 1.5f / d1) * x + 0.75f;
		v = calculation;
	}
	else if (x < cal3 / d1)
	{
		const float calculation = n1 * (x -= 2.25f / d1) * x + 0.9375f;
		v = calculation;
	}
	else
	{
		const float calculation = n1 * (x -= 2.625f / d1) * x + 0.984375f;
		v = calculation;
	}
	return v;
}

void Easing::SetEasing(const float s, const float e, const float t)
{

	this->start = s;
	this->end = e;
	this->maxtime = t;

	startCount_ = chrono::steady_clock::now();	//開始時間
	nowCount_ = chrono::steady_clock::now();		//現在時間
	//イージングプロパティ
	num_X = 0.0f;
	timeNow_ = 0.0f;					//時間
	totalTime_ = t;					//合計時間
	startPos_ = start;				//開始位置
	differencePos_ = end - start;	//開始位置-終了位置の差
	isReverse_ = false;

}

//イージングデータの読み込み

void Easing::LoadEasingData(const std::string& fileName, Easing& ease, const uint32_t easeArrayNum)
{
	//ストリームの初期化
	stringstream setEasingCommands;
	setEasingCommands.str("");//バッファのクリア
	setEasingCommands.clear(stringstream::goodbit);//状態クリア

	//ディレクトリパスとファイル名を連結してフルパスを得る
	const string defaultEasingPath = "Resources/csv/Easing/";
	string fullPath = defaultEasingPath + fileName;

	//ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

	//ワイド文字列に変換
	vector<wchar_t> wfilePath(filePathBufferSize);
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);

	//ファイルを開く
	ifstream file;
	file.open(wfilePath.data());
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	setEasingCommands << file.rdbuf();

	//ファイルを閉じる
	file.close();

	//SETNumのバッファ
	const char* set = SetNumStr(easeArrayNum);

	//1行分の文字列を入れる関数
	string line;
	//コマンド実行ループ
	while (getline(setEasingCommands, line))
	{
		//1行分の文字列をストリームに変換して解析しやすくする
		istringstream line_stream(line);

		string word;
		//,区切りで行の戦闘文字列を取得
		getline(line_stream, word, ',');

		// "//"=コメント
		if (word.find("//") == 0)continue;//コメント行を飛ばす
		//最大10種類のイージングに対応
		for (uint32_t i = 0; i < ESN_MAX; i++)SetEasingCommand(set, i, easeArrayNum, ease, line_stream, word);
	}
}

//イージングのコマンドから値のセット
void Easing::SetEasingCommand(const char* findWord, const uint32_t num, const uint32_t easeArrayNum, Easing& ease, std::istream& stream, std::string& word)
{
	//イージングのセット
	if (word.find(findWord) == 0 && num == easeArrayNum)
	{
		//x座標
		getline(stream, word, ',');
		float start = static_cast<float>(std::atof(word.c_str()));
		//y座標
		getline(stream, word, ',');
		float end = static_cast<float>(std::atof(word.c_str()));
		//z座標
		getline(stream, word, ',');
		float time = static_cast<float>(std::atof(word.c_str()));
		//イージングセット
		ease.SetEasing(start, end, time);
	}
}

//SETの文字を決める

const char* Easing::SetNumStr(const uint32_t easeArrayNum)
{
	//配列番号によって返す文字列は違う
	if (easeArrayNum == ESN_0)return "SET0";
	else if (easeArrayNum == ESN_1)return "SET1";
	else if (easeArrayNum == ESN_2)return "SET2";
	else if (easeArrayNum == ESN_3)return "SET3";
	else if (easeArrayNum == ESN_4)return "SET4";
	else if (easeArrayNum == ESN_5)return "SET5";
	else if (easeArrayNum == ESN_6)return "SET6";
	else if (easeArrayNum == ESN_7)return "SET7";
	else if (easeArrayNum == ESN_8)return "SET8";
	else if (easeArrayNum == ESN_9)return "SET9";
	else return nullptr;
}
