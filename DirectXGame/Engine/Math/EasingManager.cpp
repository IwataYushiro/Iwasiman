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
	timeNow = 0.0f;					//時間
	startpos = start;				//開始位置
	differencepos = end - start;			//開始位置-終了位置の差
	totaltime = maxtime;				//合計時間
	isReverse = false;
}

EasingManager::EasingManager(float start, float end, float time)
{
	startCount = std::chrono::steady_clock::now();	//開始時間
	nowCount = std::chrono::steady_clock::now();		//現在時間

	this->start = start;
	this->end = end;
	this->maxtime = time;

	//イージングプロパティ
	timeNow = 0.0f;					//時間
	totaltime = time;				//合計時間
	startpos = start;				//開始位置
	differencepos = end - start;	//開始位置-終了位置の差
	isReverse = false;
}

void EasingManager::Standby(bool reverse)
{
	startCount = std::chrono::steady_clock::now();
	isReverse = reverse;
	if (isReverse)
	{
		startpos = end;				//開始位置
		differencepos = start - end;			//開始位置-終了位置の差
	}
	else
	{
		startpos = start;				//開始位置
		differencepos = end - start;			//開始位置-終了位置の差
	}
}


float EasingManager::ease_in_sine()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - cosf((x * PI) / 2.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_sine()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = sinf((x * PI) / 2.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_sine()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = -(cosf(PI * x) - 1.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_quad()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_quad()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - (1.0f - x) * (1.0f - x);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_quad()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? 2.0f * x * x : 1.0f - powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_cubic()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x * x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_cubic()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - powf(1.0f - x, 3.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_cubic()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? 4.0f * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_quart()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x * x * x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_quart()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - powf(1.0f - x, 4.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_quart()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? 8.0f * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 4.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_quint()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x * x * x * x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_quint()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - powf(1.0f - x, 5.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_quint()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? 16.0f * x * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 5.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_expo()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x == 0.0f ? 0.0f : powf(2.0f, 10.0f * x - 10.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_expo()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * x);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_expo()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : x < 0.5f ? powf(2.0f, 20.0f * x - 10.0f) / 2.0f
		: (2.0f - powf(2.0f, -20.0f * x + 10.0f)) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_circ()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - sqrtf(1.0f - powf(x, 2.0f));

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_circ()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = sqrtf(1.0f - powf(x - 1.0f, 2.0f));

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_circ()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f :
		(sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_back()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	float v = c3 * x * x * x - c1 * x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;;
}

float EasingManager::ease_out_back()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	float v = 1.0f + c3 * powf(x - 1.0f, 3.0f) + c1 * pow(x - 1.0f, 2.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;;
}

float EasingManager::ease_in_out_back()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;
	float v = x < 0.5f
		? (powf(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f
		: (powf(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;;
}

float EasingManager::ease_in_elastic()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float c4 = (2.0f * PI) / 3.0f;

	float x = min(timeNow / totaltime, 1.0f);

	float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f :
		-powf(2.0f, 10.0f * x - 10.0f) * sinf((x * 10.0f - 10.75f) * c4);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_elastic()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float c4 = (2.0f * PI) / 3.0f;

	float x = min(timeNow / totaltime, 1.0f);

	float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f :
		powf(2.0f, -10.0f * x) * sinf((x * 10.0f - 0.75f) * c4) + 1.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_elastic()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	const float c5 = (2.0f * PI) / 4.5f;

	float x = min(timeNow / totaltime, 1.0f);

	float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : x < 0.5f ?
		-(powf(2.0f, 20.0f * x - 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f :
		(powf(2.0f, -20.0f * x + 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_bounce()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - bounceCalculation(1.0f - x);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_bounce()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = bounceCalculation(x);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_bounce()
{
	//現在時間を取得する
	nowCount = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//マイクロ秒を秒に単位変換

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f
		? (1.0f - bounceCalculation(1.0f - 2.0f * x)) / 2.0f
		: (1.0f + bounceCalculation(2.0f * x - 1.0f)) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::bounceCalculation(float x)
{
	float v;

	const float n1 = 7.5625f;
	const float d1 = 2.75f;
	if (x < 1.0f / d1)v = n1 * x * x;
	else if (x < 2.0f / d1)v = n1 * (x -= 1.5f / d1) * x + 0.75f;
	else if (x < 2.5f / d1)v = n1 * (x -= 2.25f / d1) * x + 0.9375f;
	else v = n1 * (x -= 2.625f / d1) * x + 0.984375f;

	return v;
}
